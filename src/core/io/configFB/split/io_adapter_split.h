/*******************************************************************************
 * Copyright (c) 2017 fortiss GmbH
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *   Johannes Messmer - initial API and implementation and/or initial documentation
 *******************************************************************************/

#ifndef SRC_CORE_IO_CONFIGFB_SPLIT_ADAPTER_H_
#define SRC_CORE_IO_CONFIGFB_SPLIT_ADAPTER_H_

#include <adapter.h>

namespace IO {
namespace ConfigurationFB {

class SplitAdapter: public CAdapter {
public:
  SplitAdapter(CResource *pa_poSrcRes,
      const SFBInterfaceSpec *pa_pstInterfaceSpecSocket,
      const CStringDictionary::TStringId pa_nInstanceNameId,
      const SFBInterfaceSpec *pa_pstInterfaceSpecPlug, bool pa_bIsPlug,
      TForteByte *pa_acFBConnData, TForteByte *pa_acFBVarsData);
  virtual ~SplitAdapter();

  CIEC_BOOL &QO() {
    return *static_cast<CIEC_BOOL*>((isSocket()) ? getDI(0) : getDO(0));
  }

  CIEC_BOOL &QI() {
    return *static_cast<CIEC_BOOL*>((isSocket()) ? getDO(0) : getDI(0));
  }

  CIEC_UINT &MasterId() {
    return *static_cast<CIEC_UINT*>((isSocket()) ? getDO(1) : getDI(1));
  }

  static const TEventID scm_nEventINITID = 0;
  int INIT() {
    return m_nParentAdapterListEventID + scm_nEventINITID;
  }

  static const TEventID scm_nEventINITOID = 0;
  int INITO() {
    return m_nParentAdapterListEventID + scm_nEventINITOID;
  }
};

} /* namespace ConfigurationFB */
} /* namespace IO */

#endif /* SRC_CORE_IO_CONFIGFB_SPLIT_ADAPTER_H_ */
