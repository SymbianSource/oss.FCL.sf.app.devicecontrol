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
* Description:  Messaging plug-in for LAWMO.
*
*/

#include <f32file.h>
#include <data_caging_path_literals.hrh>
#include <centralrepository.h>

#include "messagingplugin.h"
#include "fdebug.h"
#include "lawmomessagingasyncwipe.h"

// Construction and destruction functions

// ----------------------------------------------------------------------------------------
// CLAWMOMessagingplugin::NewL
// ----------------------------------------------------------------------------------------
CLAWMOMessagingplugin* CLAWMOMessagingplugin::NewL(MLawmoPluginWipeObserver* aObserver)
	{
        FLOG(_L(" CLAWMOMessagingplugin* CLAWMOMessagingplugin::NewL(MLawmoPluginWipeObserver* aObserver) >>"));
        CLAWMOMessagingplugin* self=new(ELeave) CLAWMOMessagingplugin();  
		CleanupStack::PushL(self);
		self->ConstructL(aObserver); 
		CleanupStack::Pop();
        FLOG(_L(" CLAWMOMessagingplugin* CLAWMOMessagingplugin::NewL(MLawmoPluginWipeObserver* aObserver) <<"));
		return self;
	}

// ----------------------------------------------------------------------------------------
// CLAWMOMessagingplugin::~CLAWMOMessagingplugin
// ----------------------------------------------------------------------------------------
CLAWMOMessagingplugin::~CLAWMOMessagingplugin()
	{
        FLOG(_L(" CLAWMOMessagingplugin::~CLAWMOMessagingplugin() >>"));
        if(iAsyncOp)
        {
            FLOG(_L(" delete iAsyncOp; "));
            delete iAsyncOp;
        }
        FLOG(_L(" CLAWMOMessagingplugin::~CLAWMOMessagingplugin() <<"));
	}

// ----------------------------------------------------------------------------------------
// CLAWMOMessagingplugin::CLAWMOMessagingplugin
// ----------------------------------------------------------------------------------------
CLAWMOMessagingplugin::CLAWMOMessagingplugin() 
	{
        FLOG(_L(" CLAWMOMessagingplugin::CLAWMOMessagingplugin() >>"));
	// See ConstructL() for initialisation completion.
        FLOG(_L(" CLAWMOMessagingplugin::CLAWMOMessagingplugin() <<"));
	}

// ----------------------------------------------------------------------------------------
// CLAWMOMessagingplugin::ConstructL
// ----------------------------------------------------------------------------------------
void CLAWMOMessagingplugin::ConstructL(MLawmoPluginWipeObserver* aObserver)
{	
    FLOG(_L(" void CLAWMOMessagingplugin::ConstructL(MLawmoPluginWipeObserver* aObserver) >>"));
    FLOG(_L("  iAsyncOp = CActiveWipeObject::NewL(aObserver);"));
	iAsyncOp = CActiveWipeObject::NewL(aObserver);
	
    FLOG(_L(" void CLAWMOMessagingplugin::ConstructL(MLawmoPluginWipeObserver* aObserver) <<"));
}


// Implementation of CLAWMOPluginInterface

// ----------------------------------------------------------------------------------------
// CLAWMOMessagingplugin::WipeL
// Wipes the Contacts.
// ----------------------------------------------------------------------------------------

void CLAWMOMessagingplugin::WipeL ()
{
    FLOG(_L(" void CLAWMOMessagingplugin::WipeL (TInt aCategory) >>"));
    FLOG(_L("  iAsyncOp->WipeAsyncL();"));
    iAsyncOp->WipeAsyncL();
    
    FLOG(_L(" void CLAWMOMessagingplugin::WipeL (TInt aCategory) <<"));
}
