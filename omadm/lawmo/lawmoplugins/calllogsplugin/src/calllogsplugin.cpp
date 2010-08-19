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
* Description: Call logs plug-in for LAWMO
*
*/

#include <e32base.h>
#include "lawmocalllogsasyncwipe.h"
#include "calllogsplugin.h"
#include "fdebug.h"

// Construction and destruction functions

// ----------------------------------------------------------------------------------------
// CLAWMOCallLogsPlugin::NewL
// ----------------------------------------------------------------------------------------
CLAWMOCallLogsPlugin* CLAWMOCallLogsPlugin::NewL(MLawmoPluginWipeObserver* aObserver)
	{
        FLOG(_L( "CLAWMOCallLogsPlugin::NewL(MLawmoPluginWipeObserver* aObserver) >>"));
        CLAWMOCallLogsPlugin* self = new(ELeave) CLAWMOCallLogsPlugin();  
		CleanupStack::PushL(self);
		self->ConstructL(aObserver); 
		CleanupStack::Pop();
        FLOG(_L( "CLAWMOCallLogsPlugin::NewL(MLawmoPluginWipeObserver* aObserver) <<"));
		return self;
	}

// ----------------------------------------------------------------------------------------
// CLAWMOCallLogsPlugin::~CLAWMOCallLogsPlugin
// ----------------------------------------------------------------------------------------
CLAWMOCallLogsPlugin::~CLAWMOCallLogsPlugin()
	{
        FLOG(_L( "CLAWMOCallLogsPlugin::~CLAWMOCallLogsPlugin() >>"));
        if(iAsyncOp)
        	delete iAsyncOp;
        FLOG(_L( "CLAWMOCallLogsPlugin::~CLAWMOCallLogsPlugin() <<"));
	}

// ----------------------------------------------------------------------------------------
// CLAWMOCallLogsPlugin::CLAWMOCallLogsPlugin
// ----------------------------------------------------------------------------------------
CLAWMOCallLogsPlugin::CLAWMOCallLogsPlugin() 
	{
        FLOG(_L( "CLAWMOCallLogsPlugin  >>"));
        // See ConstructL() for initialisation completion.
        FLOG(_L( "CLAWMOCallLogsPlugin <<"));
	}

// ----------------------------------------------------------------------------------------
// CLAWMOCallLogsPlugin::ConstructL
// ----------------------------------------------------------------------------------------
void CLAWMOCallLogsPlugin::ConstructL(MLawmoPluginWipeObserver* aObserver)
    {
        FLOG(_L( "CLAWMOCallLogsPlugin ConstructL >>"));
        iAsyncOp = CActiveWipeObject::NewL(aObserver);
        FLOG(_L( "CLAWMOCallLogsPlugin ConstructL <<"));	
    }

// Implementation of CLAWMOPluginInterface

// ----------------------------------------------------------------------------------------
// CLAWMOCallLogsPlugin::WipeL
// Wipes the Contacts.
// ----------------------------------------------------------------------------------------

void CLAWMOCallLogsPlugin::WipeL ()
    {
        FLOG(_L(" void CLAWMOCallLogsPlugin::WipeL (TInt aCategory) >>"));
        //FLOG(_L("Calling async wipe"))
        iAsyncOp->WipeAsyncL();    
        FLOG(_L(" void CLAWMOCallLogsPlugin::WipeL (TInt aCategory) <<"));
    }
