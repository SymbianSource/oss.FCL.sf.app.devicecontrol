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
* Description:  Active object for the wiping messages, called by lawmomessagingplugin.
*
*/
#ifndef ACTIVEWIPEOBJECT_H 
#define ACTIVEWIPEOBJECT_H

#include <e32base.h>
#include <f32file.h>
#include <msvstd.h>
#include <msvapi.h>
#include <msvids.h>
#include <lawmocallbackinterface.h>
#include "fdebug.h"
 
// Application Closing
#include <apgtask.h>
#include <w32std.h>

#define KMessagingAppUid 0x100058C5  // Messgaing App Uid

class CDummyObserver : public CBase, public MMsvSessionObserver
{
public:
        void HandleSessionEventL(TMsvSessionEvent aEvent, TAny*, TAny*, TAny*){}
};

class CActiveWipeObject : public CActive
{
public:
     static CActiveWipeObject* NewL(MLawmoPluginWipeObserver* aObserver);

     ~CActiveWipeObject();
     void WipeAsyncL();
       
private:
     // Over-riding the Base class functions.
     void DoCancel();
     TInt RunError(TInt aError);
     void RunL();
     
private:
     /**
      * second phase constructor
      */
     void ConstructL();
     
     // Construction
     CActiveWipeObject(MLawmoPluginWipeObserver* aObserver);
     void DoJobL(CMsvEntry* aContext);
     void InitializeL();
     void Close();
     void  WipeAllL();
     TBool WaitTillMessageAppKilled();
    
     MLawmoPluginWipeObserver* iObserver;
     RTimer iTimer; // Provides async timing service
     RWsSession iWs;
     CDummyObserver* iDummyObserver;
     CMsvSession* iSession;
} ;

#endif //ACTIVEWIPEOBJECT_H
