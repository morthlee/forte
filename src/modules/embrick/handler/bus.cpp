/*******************************************************************************
 * Copyright (c) 2016 Johannes Messmer (admin@jomess.com)
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Johannes Messmer - initial API and implementation and/or initial documentation
 *******************************************************************************/

#include "bus.h"

#include <algorithm>
#include <slave/slave.h>
#include <slave/packages.h>

#include <slave/handles/bit.h>
#include <slave/handles/analog.h>
#include <slave/handles/analog10.h>

namespace EmBrick {
namespace Handlers {

const char * const Bus::scmSlaveUpdateFailed = "Update of slave failed.";
const char * const Bus::scmNoSlavesFound = "No slave modules found.";

Bus::Bus(CDeviceExecution& paDeviceExecution) :
    spi(0), slaveSelect(0), slaves(0), slaveCount(
        0), sList(0), MultiController(paDeviceExecution) {
  // Set init time
  struct timespec ts;
  // TODO Check compile error. Had to to add rt libary to c++ make flags
  clock_gettime(CLOCK_MONOTONIC, &ts);

  initTime = ts.tv_sec;
  lastTransfer = micros();

  // Default config
  config.BusInterface = 1;
  config.BusSelectPin = 49;
  config.BusInitSpeed = SPI::DefaultSpiSpeed;
  config.BusLoopSpeed = SPI::MaxSpiSpeed;
}

void Bus::setConfig(struct IO::Device::Controller::Config* config) {
  // Check if BusHandler is active -> configuration changes are not allowed
  if (isAlive()) {
    DEVLOG_ERROR(
        "emBrick[BusHandler]: Cannot change configuration while running.\n");
    return;
  }

  this->config = *static_cast<Config*>(config);
}

const char* Bus::init() {
  // Start handlers
  spi = new SPI(config.BusInterface);
  slaveSelect = new Pin(config.BusSelectPin);
  slaves = new TSlaveList();

  // Check handlers
  if (checkHandlerError()) {
    return error;
  }

  // Set SPI sped for initialization
  spi->setSpeed(config.BusInitSpeed);

  // Disable slave select -> reset all slaves
  slaveSelect->disable();

  // Wait for reset
  sleep(1);

  // Enable slave select -> the first slave waits for initialization
  slaveSelect->enable();

  // Wait for init
  microsleep(SyncGapDuration * 2);

  // Init the slaves sequentially. Abort if the init package is ignored -> no further slaves found.
  int slaveCounter = 1;
  int attempts = 0;

  do {
    Slave *slave = Slave::sendInit(this, slaveCounter);

    if (slave != 0) {
      slaves->push_back(slave);

      // Activate next slave by sending the 'SelectNextSlave' command to the current slave
      // It enables the slave select pin for the next slave on the bus
      transfer(slave->address, SelectNextSlave);

      slaveCounter++;
      attempts = 0;
    }

    microsleep(SyncGapDuration * 2);
  } while (++attempts < 3);

  slaveCount = slaveCounter - 1;

  if (slaveCount == 0) {
    return scmNoSlavesFound;
  }

  // Increase the speed of the bus for data transfers
  spi->setSpeed(config.BusLoopSpeed);

  return 0;
}

void Bus::deInit() {
  // Free memory -> delete all slave instances and hardware handlers
  TSlaveList::Iterator itEnd(slaves->end());
  for (TSlaveList::Iterator it = slaves->begin(); it != itEnd; ++it)
    delete *it;
  delete slaves;
  slaves = 0;

  delete spi;
  delete slaveSelect;
}

Handle* Bus::initHandle(
    IO::Device::MultiController::HandleDescriptor *handleDescriptor) {
  HandleDescriptor desc = *static_cast<HandleDescriptor*>(handleDescriptor);

  Slave *slave = getSlave(desc.slaveIndex);
  if (slave == 0)
    return 0;

  switch (desc.type) {
  case Bit:
    return new BitSlaveHandle(this, desc.direction, desc.offset, desc.position,
        slave);
  case Analog:
    return new AnalogSlaveHandle(this, desc.direction, desc.offset, slave);
  case Analog10:
    return new Analog10SlaveHandle(this, desc.direction, desc.offset, slave);
  }

  return 0;
}

void Bus::prepareLoop() {
  // Get current time
  clock_gettime(CLOCK_MONOTONIC, &nextLoop);

  // Scheduling
  // TODO Combine slaves list and scheduling information
  // DISCUSS Speed of array and forte_list?
  TSlaveList::Iterator it = slaves->begin();
  sList = (struct SEntry**) forte_malloc(sizeof(struct SEntry*) * slaveCount);
  for (int i = 0; i < slaveCount; i++) {
    sList[i] = (struct SEntry*) forte_malloc(sizeof(struct SEntry));
    sList[i]->slave = *it;
    sList[i]->nextDeadline = nextLoop;
    sList[i]->lastDuration = 0;
    sList[i]->forced = false;
    sList[i]->delayed = false;

    ++it;
  }
  sNextIndex = 0;
  sNext = sList[sNextIndex];
  loopActive = false;
}

void Bus::runLoop() {
  prepareLoop();

  // Init loop variables
  SEntry *sCur = 0;

  int i;

  while (isAlive()) {
    // Sleep till next deadline is reached or loop is waked up
    loopSync.waitUntil(sNext->nextDeadline);

    // Set current slave
    loopActive = true;
    sCur = sNext;

    // Set next deadline of current slave
    clock_gettime(CLOCK_MONOTONIC, &sCur->nextDeadline);
    addTime(sCur->nextDeadline, 1000000 / sCur->slave->config.UpdateInterval);

    // Remove delayed and forced flag
    sCur->forced = false;
    sCur->delayed = false;

    // Remove lock during blocking operation -> allows forced update interrupts
    loopSync.unlock();

    uint64_t ms = micros();

    // Perform update on current slave
    if (-1 == sCur->slave->update()) {
      error = scmSlaveUpdateFailed;
      // Check for critical bus errors
      if (checkHandlerError() || hasError())
        break;
    }

    // Search for next deadline -> set lock to avoid changes of list
    loopSync.lock();
    loopActive = false;

    // Store update duration
    sCur->lastDuration = (uint16_t) (micros() - ms);

    // If current slave is forced again -> add update duration to deadline
    if (sCur->forced)
      addTime(sCur->nextDeadline, sCur->lastDuration);

    for (i = 0; i < slaveCount; i++) {
      if (cmpTime(sList[i]->nextDeadline, sNext->nextDeadline)) {
        sNext = sList[i];
        sNextIndex = i;
      }
    }
  }

  // Clean loop
  cleanLoop();
}

void Bus::cleanLoop() {
  // Free memory of list
  for (int i = 0; i < slaveCount; i++)
    forte_free(sList[i]);
  forte_free(sList);
  sList = 0;
}

bool Bus::checkHandlerError() {
  if (spi->hasError()) {
    error = spi->error;
    return true;
  }

  if (slaveSelect->hasError()) {
    error = slaveSelect->error;
    return true;
  }

  return false;
}

Slave* Bus::getSlave(int index) {
  if (slaves == 0) {
    return 0;
  }

  TSlaveList::Iterator itEnd = slaves->end();
  int i = 0;
  for (TSlaveList::Iterator it = slaves->begin(); it != itEnd; ++it, i++)
    if (index == i) {
      return *it;
    }

  return 0;
}

void Bus::forceUpdate(int index) {
  loopSync.lock();

  if (sList == 0 || slaveCount <= index || sList[index]->forced) {
    loopSync.unlock();
    return;
  }

  SEntry *e = sList[index];

  e->forced = true;
  clock_gettime(CLOCK_MONOTONIC, &e->nextDeadline);
  if (!loopActive) {
    if(!sNext->delayed && !sNext->forced) {
      struct timespec ts = e->nextDeadline;
      addTime(ts, e->lastDuration * 2);

      if(!cmpTime(ts, sNext->nextDeadline)) {
        sNext->delayed = true;
      }

      sNext = e;
      sNextIndex = index;
    }

    // Force next loop
    loopSync.wakeUp();
  }

  loopSync.unlock();
}

void Bus::addSlaveHandle(int index, Handle* handle) {
  Slave* slave = getSlave(index);
  if (slave == 0)
    return;

  slave->addHandle((SlaveHandle*) handle);
}

void Bus::dropSlaveHandles(int index) {
  Slave* slave = getSlave(index);
  if (slave == 0)
    return;

  slave->dropHandles();
}

bool Bus::isSlaveAvailable(int index) {
  return getSlave(index) != 0;
}

bool Bus::checkSlaveType(int index, int type) {
  Slave* slave = getSlave(index);
  if (slave == 0)
    return false;

  return slave->type == type;
}

bool Bus::transfer(unsigned int target, Command cmd,
    unsigned char* dataSend, int dataSendLength, unsigned char* dataReceive,
    int dataReceiveLength, SlaveStatus* status, CSyncObject *syncMutex) {
  unsigned int dataLength = std::max(dataSendLength, dataReceiveLength + 1); // + 1 status byte

  unsigned int bufferLength = sizeof(Packages::Header) + dataLength + 1; // + 1 checksum byte
  if (bufferLength > TransferBufferLength) {
    DEVLOG_ERROR("emBrick[BusHandler]: Transfer buffer size exceeded.\n");
    return false;
  }

  memset(sendBuffer, 0, bufferLength);
  memset(recvBuffer, 0, bufferLength);

  // Prepare header
  Packages::Header* header = (Packages::Header*) sendBuffer;

  header->address = (char) target;
  header->command = cmd;
  header->checksum = calcChecksum((unsigned char*) header, 2);

  if (syncMutex)
    syncMutex->lock();
  memcpy(sendBuffer + sizeof(Packages::Header), dataSend, dataSendLength);
  if (syncMutex)
    syncMutex->unlock();
  sendBuffer[sizeof(Packages::Header) + dataSendLength] = calcChecksum(
      sendBuffer + sizeof(Packages::Header), dataSendLength);

  // Invert data of master
  for (unsigned int i = 0; i < bufferLength; i++)
    sendBuffer[i] = (unsigned char) ~sendBuffer[i];

  // Send and receive buffer via SPI
  int attempts = 3;
  int fails = 0;
  bool ok;
  do {
    // Wait required microseconds between messages
    uint64_t microTime = micros();
    if (lastTransfer + SyncGapDuration > microTime)
      microsleep(lastTransfer + (uint64_t) SyncGapDuration - microTime);

//    // Send header
//    ok = spi->transfer(sendBuffer, recvBuffer, sizeof(Packages::Header));
//    lastTransfer = micros();
//    if (!ok) {
//      DEVLOG_ERROR("emBrick[BusHandler]: Failed to transfer header buffer.\n");
//      break;
//    }
//
//    // Wait required microseconds between messages
//    microTime = micros();
//    if (lastTransfer + 4 > microTime)
//      microsleep(lastTransfer + 4 - microTime);
//
//    // Send data
//    ok = spi->transfer(sendBuffer + sizeof(Packages::Header),
//        recvBuffer + sizeof(Packages::Header),
//        bufferLength - sizeof(Packages::Header));
//    lastTransfer = micros();
//    if (!ok) {
//      DEVLOG_ERROR("emBrick[BusHandler]: Failed to transfer data buffer.\n");
//      break;
//    }
    ok = spi->transfer(sendBuffer, recvBuffer, bufferLength);
    lastTransfer = micros();

    // Critical error of bus -> break immediately
    if (!ok)
      break;

    // Validate checksum
    ok = calcChecksum(recvBuffer + sizeof(Packages::Header),
        bufferLength - sizeof(Packages::Header)) == 0;
    if (!ok) {
//      DEVLOG_DEBUG("emBrick[BusHandler]: Transfer - Invalid checksum\n");
    }
  } while (!ok && ++fails < attempts);

  // Check if command was transmitted successfully
  if (!ok) {
    if (target != 0 && cmd != Data) {
      DEVLOG_DEBUG(
          "emBrick[BusHandler]: Failed to send command %d to slave %d.\n", cmd,
          target);
    }
    return false;
  }

  // Copy result
  if (syncMutex)
    syncMutex->lock();
  if (status)
    *status = (SlaveStatus) recvBuffer[sizeof(Packages::Header)];

  memcpy(dataReceive, recvBuffer + sizeof(Packages::Header) + 1,
      dataReceiveLength);
  if (syncMutex)
    syncMutex->unlock();

  return true;
}

unsigned char Bus::calcChecksum(unsigned char * data, int dataLen) {
  unsigned char c = 0;
  for (int i = 0; i < dataLen; i++)
    c += data[i];

  return ChecksumConstant - c;
}

uint64_t Bus::micros() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);

  return round(ts.tv_nsec / 1.0e3) + (ts.tv_sec - initTime) * 1E6;
}

unsigned long Bus::millis() {
  // TODO Improve timing func. Maybe replace with existing forte implementation.
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);

  return round(ts.tv_nsec / 1.0e6) + (ts.tv_sec - initTime) * 1000;
}

void Bus::microsleep(uint64_t microseconds) {
  struct timespec ts;
  ts.tv_sec = (long) (microseconds / (uint64_t) 1E6);
  ts.tv_nsec = (long) microseconds * (long) 1E3 - ts.tv_sec * (long) 1E9;
  nanosleep(&ts, 0);
}

void Bus::addTime(struct timespec& ts, unsigned long microseconds) {
  ts.tv_sec += microseconds / (unsigned long) 1E6;
  unsigned long t = ts.tv_nsec + microseconds * (unsigned long) 1E3
      - (microseconds / (unsigned long) 1E6) * (unsigned long) 1E9;
  if (t >= (unsigned long) 1E9) {
    t -= (unsigned long) 1E9;
    ts.tv_sec++;
  }
  ts.tv_nsec = t;
}

bool Bus::cmpTime(struct timespec& t1, struct timespec& t2) {
  return (t1.tv_nsec < t2.tv_nsec && t1.tv_sec == t2.tv_sec)
      || t1.tv_sec < t2.tv_sec;
}

} /* namespace Handlers */
} /* namespace EmBrick */

