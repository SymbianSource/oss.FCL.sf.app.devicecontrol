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
* Description:  ECOM proxy table for DiagNetCoveragePlugin
*
*/


// INCLUDES
#include "diagnetcoverageplugin.h"

#include <e32std.h>
#include <implementationproxy.h>


// CONSTANTS
const TImplementationProxy KDiagNetCoveragePluginImplementationTable[] = 
	{	
	IMPLEMENTATION_PROXY_ENTRY( DIAG_NET_COVERAGE_PLUGIN_UID,	CDiagNetCoveragePlugin::NewL ),
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
	aTableCount = sizeof( KDiagNetCoveragePluginImplementationTable ) 
        / sizeof( TImplementationProxy );
	return KDiagNetCoveragePluginImplementationTable;
	}



// End of File
