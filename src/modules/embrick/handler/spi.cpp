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

#include "spi.h"
#include <sstream>
#include <string>

namespace EmBrick {
namespace Handlers {

unsigned long const SPI::DefaultSpiSpeed = 300000;
unsigned long const SPI::MaxSpiSpeed = 700000;
char const SPI::spiMode = SPI_CPHA;
char const SPI::spiBitOrder = 0; // MSB first

const char * const SPI::scmFailedToInitHandler =
    "Failed to init spidev handler. Check if spi is enabled.";
const char * const SPI::scmFailedToConfigMode =
    "Failed to config spi write mode.";
const char * const SPI::scmFailedToConfigBitOrder =
    "Failed to config spi bit order.";
const char * const SPI::scmFailedToConfigSpeed =
    "Failed to config spi speed.";
const char * const SPI::scmFailedToTestBus =
    "Failed to send test byte to spi.";
const char * const SPI::scmFailedToTransferBuffer =
    "Failed to transfer buffer via spi.";

SPI::SPI(unsigned int interface) :
    error(0) {
  // Convert int to string
  std::ostringstream interfaceStream;
  interfaceStream << interface;
  std::string interfaceStr = interfaceStream.str();

  std::string spidev = "/dev/spidev" + interfaceStr + ".0";
  init(spidev.c_str());
}

SPI::~SPI() {
  deInit();
}

void SPI::init(const char* spidev) {
  // Init spidev
  DEVLOG_DEBUG("emBrick[SPIHandler]: Open spidev '%s'\n", spidev);
  fd = open(spidev, O_RDWR);

  if (fd < 0)
    return fail(scmFailedToInitHandler);

  // Set mode to SPI_CPOL and SPI_CPHA
  if (!config(SPI_IOC_WR_MODE, SPI_IOC_RD_MODE, spiMode))
    return fail(scmFailedToConfigMode);

  // Set MSB (Most Significant Bit First)
  if (!config(SPI_IOC_WR_LSB_FIRST, SPI_IOC_RD_LSB_FIRST, spiBitOrder))
    return fail(scmFailedToConfigBitOrder);

  // Set speed
  if (!config(SPI_IOC_WR_MAX_SPEED_HZ, SPI_IOC_RD_MAX_SPEED_HZ, MaxSpiSpeed))
    return fail(scmFailedToConfigSpeed);
  spiSpeed = DefaultSpiSpeed;

  // Test
  unsigned char testByte[1] = { 0 };
  if (!transfer(testByte, testByte, 1))
    return fail(scmFailedToTestBus);

  DEVLOG_INFO("emBrick[SPIHandler]: Ready.\n");
}

void SPI::deInit() {
  // Close handler if open
  if (fd >= 0)
    close(fd);
}

template<typename T> bool SPI::config(unsigned int config,
    unsigned int configVerify, T value) {
  int status;

  // Set config via ioctl
  status = ioctl(fd, config, &value);
  if (status < 0)
    return false;

  // Verify config
  T valueCheck;
  status = ioctl(fd, configVerify, &valueCheck);
  if (status < 0)
    return false;

  return valueCheck == value;
}

void SPI::fail(const char* reason) {
  error = reason;
  DEVLOG_ERROR("emBrick[SPIHandler]: %s\n", error);
}

bool SPI::transfer(unsigned char* sendBuffer,
    unsigned char* receiveBuffer, int length) {

  struct spi_ioc_transfer msg;

  msg.tx_buf = (unsigned long) sendBuffer;
  msg.rx_buf = (unsigned long) receiveBuffer;
  msg.len = length;
  msg.delay_usecs = 0;
  msg.speed_hz = spiSpeed;
  msg.bits_per_word = 8;
  msg.cs_change = 0;
  msg.pad = 0;

  // Send data to spi bus
  int status = ioctl(fd, SPI_IOC_MESSAGE(1), &msg);
  if (status < 0) {
    fail(scmFailedToTransferBuffer);
    return false;
  }

  return true;
}

void SPI::setSpeed(const unsigned long speed) {
  spiSpeed = speed;
}

} /* namespace Handlers */
} /* namespace EmBrick */
