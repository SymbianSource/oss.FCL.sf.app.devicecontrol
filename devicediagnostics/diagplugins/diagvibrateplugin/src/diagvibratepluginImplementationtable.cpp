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
* Description:  ECOM proxy table for DiagVibratePlugin
*
*/


// INCLUDES
#include <e32std.h>
#include <implementationproxy.h>

#include "diagvibrateplugin.h"

// CONSTANTS
const TImplementationProxy KDiagVibratePluginImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( 0x2000E587, CDiagVibratePlugin::NewL )
    };


// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// 
// Gate/factory function
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
                                                  TInt& aTableCount )
    {
    aTableCount = sizeof( KDiagVibratePluginImplementationTable ) 
        / sizeof( TImplementationProxy );
        
    return KDiagVibratePluginImplementationTable;
    }

// End of File
