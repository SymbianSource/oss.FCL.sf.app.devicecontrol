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
* Description:  Header file for CNSmlDMSyncAppServer
*
*/


#ifndef CNSMLDMSYNCAPPSERVER_H
#define CNSMLDMSYNCAPPSERVER_H

// INCLUDES

#include <AknDoc.h>
#include <aknapp.h>
#include <AknServerApp.h>

// CLASS DECLARATION

/**
* Device Management Aplication Server
*
*/
NONSHARABLE_CLASS (CNSmlDMSyncAppServer) :  public CAknAppServer
	{
		
    public: //from CAknServerApp
        /**
        * CreateServiceL.
        * @param    aServiceType
        * @return   CApaAppServiceBase*
        */
        CApaAppServiceBase* CreateServiceL( TUid aServiceType ) const;
        
        /**
        * ConstructL.
        * @param    aFixedServerName Server name
        * @return   None
        */        
        void ConstructL(const TDesC& aFixedServerName);

        /**
        * Implementing this prevents framework for closing application
        * after last client is destroyed.
        * @param    None
        * @return   None
        */               
        void HandleAllClientsClosed();
	};

#endif  //CNSMLDMSYNCAPPSERVER_H

// End of File
