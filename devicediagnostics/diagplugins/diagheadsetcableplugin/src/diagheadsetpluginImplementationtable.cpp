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
* Description:  ECOM proxy table for DiagHeadsetCableTestPlugin
*
*/


// INCLUDES
#include <e32std.h>
#include <implementationproxy.h>

#include "diagheadsetcabletestplugin.hrh"   // UIDs
#include "diagheadsetcableplugin.h"         // CDiagHeadsetCableTestPlugin

// CONSTANTS 
const TImplementationProxy KDiagHeadsetCableTestPluginImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( _IMPLEMENTATION_UID, 
                                CDiagHeadsetCableTestPlugin::NewL )
    };


// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// 
// Gate/factory function
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount )
    {
    aTableCount = sizeof( KDiagHeadsetCableTestPluginImplementationTable ) 
        / sizeof( TImplementationProxy );

    return KDiagHeadsetCableTestPluginImplementationTable;
    }
    
// End of File
