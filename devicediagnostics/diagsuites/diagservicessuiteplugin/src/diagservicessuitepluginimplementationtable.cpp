/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  This is the ECOM implementation table for the Services Suite
*                Plugin.
*
*/


// System Include Files
#include <implementationproxy.h>            // TImplementationProxy

// User Include Files
#include "diagservicessuiteplugin.h"        // CDiagServicesSuitePlugin
#include "diagservicessuiteplugin.hrh"      // UID Defintions

// Local Constants
const TImplementationProxy KDiagServicesSuitePluginImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(
	    _IMPLEMENTATION_UID,
	    CDiagServicesSuitePlugin::NewL )
	};


// ============================ GLOBAL FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Returns the implementation table.
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount )
	{
	aTableCount = sizeof( KDiagServicesSuitePluginImplementationTable ) /
	    sizeof( TImplementationProxy );
	return KDiagServicesSuitePluginImplementationTable;
	}

// End of File

