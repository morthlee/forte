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

#ifndef SRC_MODULES_EMBRICK_TYPES_SLAVE2181_H_
#define SRC_MODULES_EMBRICK_TYPES_SLAVE2181_H_

#include <funcbloc.h>
#include <forte_bool.h>
#include <forte_wstring.h>
#include "BusAdapter.h"
#include "Slave.h"

namespace EmBrick {
namespace FunctionBlocks {

class Slave2181: public Slave {
DECLARE_FIRMWARE_FB(Slave2181)

private:
  static const CStringDictionary::TStringId scm_anDataInputNames[];
  static const CStringDictionary::TStringId scm_anDataInputTypeIds[];

  CIEC_WSTRING &DigitalInput_1() {
    return *static_cast<CIEC_WSTRING*>(getDI(1));
  }

  CIEC_WSTRING &DigitalInput_2() {
    return *static_cast<CIEC_WSTRING*>(getDI(2));
  }

  CIEC_WSTRING &DigitalInput_3() {
    return *static_cast<CIEC_WSTRING*>(getDI(3));
  }

  CIEC_WSTRING &DigitalInput_4() {
    return *static_cast<CIEC_WSTRING*>(getDI(4));
  }

  CIEC_WSTRING &DigitalInput_5() {
    return *static_cast<CIEC_WSTRING*>(getDI(5));
  }

  CIEC_WSTRING &DigitalInput_6() {
    return *static_cast<CIEC_WSTRING*>(getDI(6));
  }

  CIEC_WSTRING &DigitalInput_7() {
    return *static_cast<CIEC_WSTRING*>(getDI(7));
  }

  CIEC_WSTRING &DigitalInput_8() {
    return *static_cast<CIEC_WSTRING*>(getDI(8));
  }

  CIEC_WSTRING &DigitalOutput_1() {
    return *static_cast<CIEC_WSTRING*>(getDI(9));
  }

  CIEC_WSTRING &DigitalOutput_2() {
    return *static_cast<CIEC_WSTRING*>(getDI(10));
  }

  CIEC_WSTRING &DigitalOutput_3() {
    return *static_cast<CIEC_WSTRING*>(getDI(11));
  }

  CIEC_WSTRING &DigitalOutput_4() {
    return *static_cast<CIEC_WSTRING*>(getDI(12));
  }

  CIEC_WSTRING &DigitalOutput_5() {
    return *static_cast<CIEC_WSTRING*>(getDI(13));
  }

  CIEC_WSTRING &DigitalOutput_6() {
    return *static_cast<CIEC_WSTRING*>(getDI(14));
  }

  CIEC_WSTRING &DigitalOutput_7() {
    return *static_cast<CIEC_WSTRING*>(getDI(15));
  }

  CIEC_WSTRING &DigitalOutput_8() {
    return *static_cast<CIEC_WSTRING*>(getDI(16));
  }

  CIEC_UINT &UpdateInterval() {
    return *static_cast<CIEC_UINT*>(getDI(17));
  }

  static const CStringDictionary::TStringId scm_anDataOutputNames[];
  static const CStringDictionary::TStringId scm_anDataOutputTypeIds[];

  static const TForteInt16 scm_anEIWithIndexes[];
  static const TDataIOID scm_anEIWith[];
  static const CStringDictionary::TStringId scm_anEventInputNames[];

  static const TForteInt16 scm_anEOWithIndexes[];
  static const TDataIOID scm_anEOWith[];
  static const CStringDictionary::TStringId scm_anEventOutputNames[];

  static const SAdapterInstanceDef scm_astAdapterInstances[];

  static const SFBInterfaceSpec scm_stFBInterfaceSpec;

  FORTE_FB_DATA_ARRAY(2, 18, 2, 2)

  static const TForteUInt8 scm_slaveConfigurationIO[];
  static const TForteUInt8 scm_slaveConfigurationIO_num;

  virtual void initHandles();

public:
  FUNCTION_BLOCK_CTOR_FOR_IO_MULTI_SLAVE(Slave2181, Slave, Handlers::G_8Di8Do){
};

virtual ~Slave2181() {};

};

}
/* namespace FunctionsBlocks */
} /* namespace EmBrick */

#endif //close the ifdef sequence from the beginning of the file

