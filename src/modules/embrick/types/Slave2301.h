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

#ifndef SRC_MODULES_EMBRICK_TYPES_SLAVE2301_H_
#define SRC_MODULES_EMBRICK_TYPES_SLAVE2301_H_

#include <funcbloc.h>
#include <forte_bool.h>
#include <forte_wstring.h>
#include "BusAdapter.h"
#include "Slave.h"

namespace EmBrick {
namespace FunctionBlocks {

class Slave2301: public Slave {
DECLARE_FIRMWARE_FB(Slave2301)

private:
  static const CStringDictionary::TStringId scm_anDataInputNames[];
  static const CStringDictionary::TStringId scm_anDataInputTypeIds[];

  CIEC_WSTRING &Relay_1() {
    return *static_cast<CIEC_WSTRING*>(getDI(1));
  }

  CIEC_WSTRING &Relay_2() {
    return *static_cast<CIEC_WSTRING*>(getDI(2));
  }

  CIEC_WSTRING &Relay_3() {
    return *static_cast<CIEC_WSTRING*>(getDI(3));
  }

  CIEC_WSTRING &Relay_4() {
    return *static_cast<CIEC_WSTRING*>(getDI(4));
  }

  CIEC_WSTRING &Relay_5() {
    return *static_cast<CIEC_WSTRING*>(getDI(5));
  }

  CIEC_WSTRING &Relay_6() {
    return *static_cast<CIEC_WSTRING*>(getDI(6));
  }

  CIEC_UINT &UpdateInterval() {
    return *static_cast<CIEC_UINT*>(getDI(7));
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

  FORTE_FB_DATA_ARRAY(2, 8, 2, 2)

  static const TForteUInt8 scm_slaveConfigurationIO[];
  static const TForteUInt8 scm_slaveConfigurationIO_num;

  virtual void initHandles();

public:
  FUNCTION_BLOCK_CTOR_FOR_IO_MULTI_SLAVE(Slave2301, Slave, Handlers::G_2RelNo4RelCo){
};

virtual ~Slave2301() {};

};

}
/* namespace FunctionsBlocks */
} /* namespace EmBrick */

#endif //close the ifdef sequence from the beginning of the file

