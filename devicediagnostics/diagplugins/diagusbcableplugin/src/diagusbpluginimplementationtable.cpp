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
* Description:  ECOM proxy table for DiagUSBCableTestPlugin
*
*/


// INCLUDES
#include <e32std.h>
#include <implementationproxy.h>

#include "diagusbcabletestplugin.hrh"   // UIDs
#include "diagusbcabletestplugin.h"     // CDiagUSBCableTestPlugin

// CONSTANTS 
const TImplementationProxy KDiagUSBCableTestPluginImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( _IMPLEMENTATION_UID, 
                                CDiagUSBCableTestPlugin::NewL )
    };


// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// 
// Gate/factory function
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount )
    {
    aTableCount = sizeof( KDiagUSBCableTestPluginImplementationTable ) 
        / sizeof( TImplementationProxy );
    return KDiagUSBCableTestPluginImplementationTable;
    }

// End of File
