/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Contacts plug-in for LAWMO.
*
*/ 

#include <f32file.h>
#include <data_caging_path_literals.hrh>
#include <centralrepository.h>
#include "contactsplugin.h"
#include "fdebug.h"
#include "lawmocontactsasyncwipe.h"
// Construction and destruction functions

// ----------------------------------------------------------------------------------------
// CLAWMOContactsPlugin::NewL
// ----------------------------------------------------------------------------------------
CLAWMOContactsPlugin* CLAWMOContactsPlugin::NewL(MLawmoPluginWipeObserver* aObserver)
	{
        FLOG(_L(" CLAWMOContactsPlugin* CLAWMOContactsPlugin::NewL(MLawmoPluginWipeObserver* aObserver) >>"));
 
        CLAWMOContactsPlugin* self=new(ELeave) CLAWMOContactsPlugin();  
		CleanupStack::PushL(self);
		self->ConstructL(aObserver); 
		CleanupStack::Pop();
	    FLOG(_L(" CLAWMOContactsPlugin* CLAWMOContactsPlugin::NewL(MLawmoPluginWipeObserver* aObserver) <<"));	 
		return self;
	}

// ----------------------------------------------------------------------------------------
// CLAWMOContactsPlugin::~CLAWMOContactsPlugin
// ----------------------------------------------------------------------------------------
CLAWMOContactsPlugin::~CLAWMOContactsPlugin()
	{
        FLOG(_L(" CLAWMOContactsPlugin::~CLAWMOContactsPlugin() >>"));
        if(asyncOp)
        {
            FLOG(_L("  delete asyncOp; "));
            delete asyncOp;
        }
        FLOG(_L(" CLAWMOContactsPlugin::~CLAWMOContactsPlugin() <<"));
	}

// ----------------------------------------------------------------------------------------
// CLAWMOContactsPlugin::CLAWMOContactsPlugin
// ----------------------------------------------------------------------------------------
CLAWMOContactsPlugin::CLAWMOContactsPlugin() 
	{
        FLOG(_L(" CLAWMOContactsPlugin::CLAWMOContactsPlugin() >>"));
        // See ConstructL() for initialisation completion.
        FLOG(_L(" CLAWMOContactsPlugin::CLAWMOContactsPlugin() <<"));
	}

// ----------------------------------------------------------------------------------------
// CLAWMOContactsPlugin::ConstructL
// ----------------------------------------------------------------------------------------
void CLAWMOContactsPlugin::ConstructL(MLawmoPluginWipeObserver* aObserver)
{	
    FLOG(_L(" void CLAWMOContactsPlugin::ConstructL(MLawmoPluginWipeObserver* aObserver) >>"));
    FLOG(_L("  asyncOp = CActiveWipeObject::NewL(aObserver);"));
    asyncOp = CActiveWipeObject::NewL(aObserver);
    FLOG(_L(" void CLAWMOContactsPlugin::ConstructL(MLawmoPluginWipeObserver* aObserver) <<"));
}

// Implementation of CLAWMOPluginInterface

// ----------------------------------------------------------------------------------------
// CLAWMOContactsPlugin::WipeL
// Wipes the Contacts.
// ----------------------------------------------------------------------------------------

void CLAWMOContactsPlugin::WipeL ()
{
    FLOG(_L(" void CLAWMOContactsPlugin::WipeL () >>"));

    FLOG(_L("  asyncOp->WipeAsyncL();   "));
    asyncOp->WipeAsyncL();    
    /*
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    */
    FLOG(_L(" void CLAWMOContactsPlugin::WipeL () <<"));
}

