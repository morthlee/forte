/*******************************************************************************
 * Copyright (c) 2006 - 2011 ACIN, Profactor GmbH
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *   Alois Zoitl, Gerhard Ebenhofer
 *     - initial API and implementation and/or initial documentation
 *******************************************************************************/
#ifndef _E_REND_H_
#define _E_REND_H_

#ifndef FMU

#include <funcbloc.h>

// cppcheck-suppress noConstructor
class E_REND: public CFunctionBlock{
  DECLARE_FIRMWARE_FB(E_REND)

private:

  static const TEventID scm_nEventEI1ID = 0;
  static const TEventID scm_nEventEI2ID = 1;
  static const TEventID scm_nEventRID = 2;
  static const CStringDictionary::TStringId scm_anEventInputNames[];

  static const TEventID scm_nEventEOID = 0;
  static const CStringDictionary::TStringId scm_anEventOutputNames[];

  static const int scm_nStateSTART = 0;
  static const int scm_nStateR = 1;
  static const int scm_nStateR1 = 2;
  static const int scm_nStateEI1 = 3;
  static const int scm_nStateEO = 4;
  static const int scm_nStateEI2 = 5;
  static const int scm_nStateR2 = 6;

  bool m_bE1Occ;
  bool m_bE2Occ;


  virtual void executeEvent(int pa_nEIID);

  static const SFBInterfaceSpec scm_stFBInterfaceSpec;

  FORTE_FB_DATA_ARRAY(1,0,0, 0);

public:
  FUNCTION_BLOCK_CTOR(E_REND),
    m_bE1Occ(false),
    m_bE2Occ(false){
  };
  virtual ~E_REND() {};

};

#else

#include <basicfb.h>

class E_REND: public CBasicFB{
  DECLARE_FIRMWARE_FB(E_REND)

private:
  static const TEventID scm_nEventEI1ID = 0;
  static const TEventID scm_nEventEI2ID = 1;
  static const TEventID scm_nEventRID = 2;
  static const TForteInt16 scm_anEIWithIndexes[];
  static const CStringDictionary::TStringId scm_anEventInputNames[];

  static const TEventID scm_nEventEOID = 0;
  static const TForteInt16 scm_anEOWithIndexes[];
  static const CStringDictionary::TStringId scm_anEventOutputNames[];

  static const SFBInterfaceSpec scm_stFBInterfaceSpec;

   FORTE_BASIC_FB_DATA_ARRAY(1, 0, 0, 0, 0);
  static const TForteInt16 scm_nStateSTART = 0;
  static const TForteInt16 scm_nStateR = 1;
  static const TForteInt16 scm_nStateR1 = 2;
  static const TForteInt16 scm_nStateEI1 = 3;
  static const TForteInt16 scm_nStateEO = 4;
  static const TForteInt16 scm_nStateEI2 = 5;
  static const TForteInt16 scm_nStateR2 = 6;

  void enterStateSTART(void);
  void enterStateR(void);
  void enterStateR1(void);
  void enterStateEI1(void);
  void enterStateEO(void);
  void enterStateEI2(void);
  void enterStateR2(void);

  virtual void executeEvent(int pa_nEIID);

public:
  E_REND(CStringDictionary::TStringId pa_nInstanceNameId, CResource *pa_poSrcRes) :
       CBasicFB(pa_poSrcRes, &scm_stFBInterfaceSpec, pa_nInstanceNameId,
              0, m_anFBConnData, m_anFBVarsData){
  };

  virtual ~E_REND(){};

};

#endif

#endif //close the ifdef sequence from the beginning of the file

