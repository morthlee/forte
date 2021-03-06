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

#ifndef SRC_MODULES_EMBRICK_TYPES_BUSCONFIGADAPTER_H_
#define SRC_MODULES_EMBRICK_TYPES_BUSCONFIGADAPTER_H_

#include <io/configFB/multi/io_adapter.h>
#include <typelib.h>
#include <forte_bool.h>
#include <forte_uint.h>

namespace EmBrick {
namespace FunctionBlocks {

class BusAdapter: public IO::ConfigurationFB::Multi::Adapter {
DECLARE_ADAPTER_TYPE(BusAdapter)

private:
private:
  static const CStringDictionary::TStringId scm_anDataInputNames[];
  static const CStringDictionary::TStringId scm_anDataInputTypeIds[];

  static const CStringDictionary::TStringId scm_anDataOutputNames[];
  static const CStringDictionary::TStringId scm_anDataOutputTypeIds[];
public:

  CIEC_UINT &UpdateInterval() {
    return *static_cast<CIEC_UINT*>((isSocket()) ? getDO(3) : getDI(3));
  }

private:
  static const TForteInt16 scm_anEIWithIndexes[];
  static const TDataIOID scm_anEIWith[];
  static const CStringDictionary::TStringId scm_anEventInputNames[];

  static const TForteInt16 scm_anEOWithIndexes[];
  static const TDataIOID scm_anEOWith[];
  static const CStringDictionary::TStringId scm_anEventOutputNames[];

  static const SFBInterfaceSpec scm_stFBInterfaceSpecSocket;

  static const SFBInterfaceSpec scm_stFBInterfaceSpecPlug;

  FORTE_ADAPTER_DATA_ARRAY(1, 1, 1, 4, 0)

public:
  ADAPTER_CTOR_FOR_IO_MULTI(BusAdapter){
};

private:
  static const TForteUInt8 scm_slaveConfigurationIO[];
  static const TForteUInt8 scm_slaveConfigurationIO_num;

  virtual ~BusAdapter() {};

};

}
/* namespace FunctionsBlocks */
} /* namespace EmBrick */

#endif //close the ifdef sequence from the beginning of the file

