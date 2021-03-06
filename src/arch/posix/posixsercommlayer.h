/*******************************************************************************
 * Copyright (c) 2012, 2017 ACIN, fortiss GmbH
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *  Alois Zoitl - initial API and implementation and/or initial documentation
 *******************************************************************************/
#ifndef _SERCOMMLAYER_H_
#define _SERCOMMLAYER_H_

#include "../../core/cominfra/serialcomlayerbase.h"
#include "../fdselecthand.h"
#include <termios.h>

class CPosixSerCommLayer : public CSerialComLayerBase<CFDSelectHandler::TFileDescriptor, CFDSelectHandler::scm_nInvalidFileDescriptor>{
  public:
    CPosixSerCommLayer(forte::com_infra::CComLayer* paUpperLayer, forte::com_infra::CBaseCommFB * paFB);
    virtual ~CPosixSerCommLayer();

    virtual forte::com_infra::EComResponse sendData(void *paData, unsigned int paSize);
    virtual forte::com_infra::EComResponse recvData(const void *paData, unsigned int paSize);

  protected:
  private:
    virtual forte::com_infra::EComResponse openSerialConnection(const SSerialParameters& paSerialParameters, CSerialComLayerBase<CFDSelectHandler::TFileDescriptor, CFDSelectHandler::scm_nInvalidFileDescriptor>::TSerialHandleType* paHandleResult);
    virtual void closeConnection();

    struct termios mOldTIO;    //!< buffer for the existing sercom settings
};

#endif /* CSERCOMMLAYER_H_ */
