/*******************************************************************************
  * Copyright (c) 2006-2014 ACIN, Profactor GmbH, fortiss GmbH
  * All rights reserved. This program and the accompanying materials
  * are made available under the terms of the Eclipse Public License v1.0
  * which accompanies this distribution, and is available at
  * http://www.eclipse.org/legal/epl-v10.html
  *
  * Contributors:
  *    Rene Smodic, Alois Zoitl, Michael Hofmann, Martin Melik Merkumians,
  *    Patrick Smejkal
  *      - initial implementation and rework communication infrastructure
  *******************************************************************************/

#include "basecommfb.h"
#include "comlayer.h"
#include "comlayersmanager.h"
#include "../resource.h"
#include "../../arch/fortenew.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

using namespace forte::com_infra;

const char * const CBaseCommFB::scm_sResponseTexts[] = { "OK", "INVALID_ID", "TERMINATED", "INVALID_OBJECT", "DATA_TYPE_ERROR", "INHIBITED", "NO_SOCKET", "SEND_FAILED", "RECV_FAILED" };

CBaseCommFB::CBaseCommFB(const CStringDictionary::TStringId pa_nInstanceNameId, CResource *pa_poSrcRes, forte::com_infra::EComServiceType pa_eCommServiceType) :
	CEventSourceFB(pa_poSrcRes, 0, pa_nInstanceNameId, 0, 0), m_eCommServiceType(pa_eCommServiceType), m_poTopOfComStack(0) {
	memset(m_apoInterruptQueue, 0, sizeof(m_apoInterruptQueue)); //TODO change this to  m_apoInterruptQueue{0} in the extended list when fully switching to C++11
	setEventChainExecutor(getResource().getResourceEventExecution());
	m_unComInterruptQueueCount = 0;
	m_nConfiguredFBTypeNameId = CStringDictionary::scm_nInvalidStringId;
}

CBaseCommFB::CBaseCommFB(const CStringDictionary::TStringId pa_nInstanceNameId, CResource *pa_poSrcRes, const SFBInterfaceSpec *pa_pstInterfaceSpec,
	TForteByte *pa_acFBConnData, TForteByte *pa_acFBVarsData, forte::com_infra::EComServiceType pa_eCommServiceType) :
	CEventSourceFB(pa_poSrcRes, pa_pstInterfaceSpec, pa_nInstanceNameId, pa_acFBConnData, pa_acFBVarsData), m_eCommServiceType(pa_eCommServiceType), m_poTopOfComStack(0) {
	memset(m_apoInterruptQueue, 0, sizeof(m_apoInterruptQueue)); //TODO change this to  m_apoInterruptQueue{0} in the extended list when fully switching to C++11
	setEventChainExecutor(getResource().getResourceEventExecution());
	m_unComInterruptQueueCount = 0;
	m_nConfiguredFBTypeNameId = CStringDictionary::scm_nInvalidStringId;
}

CBaseCommFB::~CBaseCommFB() {
	closeConnection();

	if ((getManagesFBData()) && (0 != m_pstInterfaceSpec)) {
		//Free the memory allocated for the interface, only do this if we dynamically created it (i.e., getManagesFBData is true)
		delete[](m_pstInterfaceSpec->m_anEIWith);
		delete[](m_pstInterfaceSpec->m_anEOWith);
		delete[](m_pstInterfaceSpec->m_aunDINames);
		delete[](m_pstInterfaceSpec->m_aunDIDataTypeNames);
		delete[](m_pstInterfaceSpec->m_aunDONames);
		delete[](m_pstInterfaceSpec->m_aunDODataTypeNames);
	}
}

EMGMResponse CBaseCommFB::changeFBExecutionState(EMGMCommandType pa_unCommand) {
	EMGMResponse retVal = CEventSourceFB::changeFBExecutionState(pa_unCommand);
	if ((e_RDY == retVal) && (cg_nMGM_CMD_Kill == pa_unCommand)) {
		//when we are killed we'll close the connection so that it can safely be opened again after an reset
		closeConnection();
	}
	return retVal;
}

EComResponse CBaseCommFB::openConnection() {
	EComResponse retVal;
	if (0 == m_poTopOfComStack) {
		// Get the ID
		char *commID;
		if (0 == strchr(ID().getValue(), ']')) {
			commID = getDefaultIDString(ID().getValue());
		}
		else {
			commID = new char[strlen(ID().getValue()) + 1];
			strcpy(commID, ID().getValue());
		}

		// If the ID is empty return an error
		if ('\0' == *commID) {
			retVal = e_InitInvalidId;
		}
		else {
			retVal = createComstack(commID);
			// If any error is going to be returned, delete the layers that were created
			if (e_InitOk != retVal) {
				closeConnection();
			}
		}
		delete[] commID;
	}
	else {
		// If the connection was already opened return ok
		retVal = e_InitOk;
	}
	return retVal;
}

EComResponse CBaseCommFB::createComstack(char *commID) {
	EComResponse retVal = e_InitInvalidId;
	CComLayer *newLayer = 0;
	CComLayer *previousLayer = 0; // Reference to the previous layer as it needs to set the bottom layer
	char *layerParams = 0;
	// Loop until reaching the end of the ID
	while ('\0' != *commID) {
		// Get the next layer's ID and parameters
		char * layerID = extractLayerIdAndParams(&commID, &layerParams);
		// If not well formated ID return an error
		if ((0 == commID) && ('\0' != *layerID)) {
			retVal = e_InitInvalidId;
			break;
		}

		// Create the new layer
		newLayer = CComLayersManager::createCommunicationLayer(layerID, previousLayer, this);
		if (0 == newLayer) {
			// If the layer can't be created return an error
			retVal = e_InitInvalidId;
			break;
		}
		else if (0 == m_poTopOfComStack) {
			// Assign the newly created layer to the FB
			m_poTopOfComStack = newLayer;
		}

		// Update the previous layer reference
		previousLayer = newLayer;

		// Open the layer connection
		retVal = newLayer->openConnection(layerParams);
		if (e_InitOk != retVal) {
			// If it was not opened correctly return the error
			break;
		}
	}
	return retVal;
}


void CBaseCommFB::closeConnection() {
	if (m_poTopOfComStack != 0) {
		m_poTopOfComStack->closeConnection();
		delete m_poTopOfComStack; // this will close the whole communication stack
		m_poTopOfComStack = 0;
	}
}

void CBaseCommFB::interruptCommFB(CComLayer *pa_poComLayer) {
	if (cg_unCommunicationInterruptQueueSize > m_unComInterruptQueueCount) {
		m_apoInterruptQueue[m_unComInterruptQueueCount] = pa_poComLayer;
		m_unComInterruptQueueCount++;
	}
	else {
		//TODO to many interrupts received issue error msg
	}
}

char *CBaseCommFB::extractLayerIdAndParams(char **paRemainingID, char **paLayerParams) {
	char *LayerID = *paRemainingID;
	if ('\0' != **paRemainingID) {
		*paRemainingID = strchr(*paRemainingID, '[');
		if (0 != *paRemainingID) {
			**paRemainingID = '\0';
			++*paRemainingID;
			*paLayerParams = *paRemainingID;
			*paRemainingID = strchr(*paRemainingID, ']');
			if (0 != *paRemainingID) {
				**paRemainingID = '\0';
				++*paRemainingID;
				if ('\0' != **paRemainingID) {
					++*paRemainingID;
				}
			}
		}
	}
	return LayerID;
}

char *CBaseCommFB::buildIDString(const char *paPrefix, const char *paIDRoot, const char *paSuffix) {
	char * RetVal = new char[strlen(paPrefix) + strlen(paIDRoot) + strlen(paSuffix) + 1];
	strcpy(RetVal, paPrefix);
	strcat(RetVal, paIDRoot);
	strcat(RetVal, paSuffix);
	return RetVal;
}
