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
* Description:  Sync app server
*
*/


// INCLUDE FILES
#include    "NSmlDMSyncAppServer.h"
#include    "NSmlDMSyncDebug.h"

#include    <SyncServiceSession.h>
#include    <SyncServiceConst.h>


// ------------------------------------------------------------------------------
// CNSmlDMSyncAppServer::CreateServiceL
// ------------------------------------------------------------------------------
//
CApaAppServiceBase* CNSmlDMSyncAppServer::CreateServiceL( 
                                                      TUid aServiceType ) const
	{
	FLOG( "[OMADM] CNSmlDMSyncAppServer::CreateServiceL" );
	
	if ( aServiceType == TUid::Uid( KSyncServiceUid ) )   
		{
		return new (ELeave) CSyncServiceSession;
		}
	else
		{
		return CAknAppServer::CreateServiceL( aServiceType );
		}
	}

// ------------------------------------------------------------------------------
// CNSmlDMSyncAppServer::CreateServiceL
// ------------------------------------------------------------------------------
//
void CNSmlDMSyncAppServer::ConstructL( const TDesC& aFixedServerName )
    {
    FTRACE(RDebug::Print(_L("[OMADM] CNSmlDMSyncAppServer::ConstructL %S"),
                                                           &aFixedServerName));

    CAknAppServer::ConstructL( aFixedServerName );
    }

// ------------------------------------------------------------------------------
// CNSmlDMSyncAppServer::HandleAllClientsClosed
// ------------------------------------------------------------------------------
//
void CNSmlDMSyncAppServer::HandleAllClientsClosed()
    {
    FLOG( "[OMADM] CNSmlDMSyncAppServer::HandleAllClientsClosed" );
    }

// End of File
