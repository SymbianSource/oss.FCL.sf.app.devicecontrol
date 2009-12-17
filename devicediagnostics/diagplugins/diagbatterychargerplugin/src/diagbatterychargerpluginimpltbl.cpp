/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ECOM proxy table for DiagDataNetworkTestPlugin
*
*/


// INCLUDES

#include <e32std.h>
#include <implementationproxy.h>

#include "diagbatterychargercabletestplugin.hrh"    // UIDs
#include "diagbatterychargercableplugin.h"          //CDiagBatteryChargerCableTestPlugin

// CONSTANTS 
const TImplementationProxy KDiagBatteryChargerCableTestPluginImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY( _IMPLEMENTATION_UID,	CDiagBatteryChargerCableTestPlugin::NewL ),
	};


// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// 
// Gate/factory function
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount )
	{
	aTableCount = sizeof( KDiagBatteryChargerCableTestPluginImplementationTable ) 
        / sizeof( TImplementationProxy );
	return KDiagBatteryChargerCableTestPluginImplementationTable;
	}



// End of File
