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
* Description: Active object for wiping messages, called by lawmomessagingplugin.
*
*/

#include <msvids.h>
#include <f32file.h>
#include <e32base.h>
#include "lawmomessagingasyncwipe.h"
#include "fdebug.h"

// ----------------------------------------------------------------------------- 
// CActiveWipeObject::CActiveWipeObject
// Description: CActiveWipeObject constructer . Initializes datamember of MLawmoPluginWipeObserver 
// ----------------------------------------------------------------------------- 
//
CActiveWipeObject::CActiveWipeObject(MLawmoPluginWipeObserver* aObserver) :
    CActive(EPriorityStandard), iObserver(aObserver), iSession(NULL)
    {
    FLOG(
            _L(" Messaging: CActiveWipeObject::CActiveWipeObject(MLawmoPluginWipeObserver* aObserver) >>"));
    // See ConstructL() for initialisation completion.
    FLOG(
            _L(" Messaging: CActiveWipeObject::CActiveWipeObject(MLawmoPluginWipeObserver* aObserver) <<"));
    }

// ----------------------------------------------------------------------------- 
// CActiveWipeObject::NewL
// Description: CActiveWipeObject constructer 
// ----------------------------------------------------------------------------- 
//
CActiveWipeObject* CActiveWipeObject::NewL(MLawmoPluginWipeObserver* aObserver)
    {
    FLOG(_L(" Messaging: CActiveWipeObject* CActiveWipeObject::NewL(MLawmoPluginWipeObserver* aObserver) >>"));
    CActiveWipeObject* self=new(ELeave) CActiveWipeObject(aObserver);  
    CleanupStack::PushL(self);
    self->ConstructL(); 
    CleanupStack::Pop();
    FLOG(_L(" Messaging: CActiveWipeObject* CActiveWipeObject::NewL(MLawmoPluginWipeObserver* aObserver) <<"));
    return self;
    }

// ----------------------------------------------------------------------------------------
// CActiveWipeObject::ConstructL
// ----------------------------------------------------------------------------------------
void CActiveWipeObject::ConstructL()
    {
    FLOG(_L(" Messaging: CActiveWipeObject::ConstructL() >>"));

    iTimer.CreateLocal();
    User::LeaveIfError(iWs.Connect());
    CActiveScheduler::Add(this); // Add to scheduler
    iDummyObserver = new CDummyObserver();
    FLOG(_L(" Messaging: CActiveWipeObject::ConstructL() <<"));
    }

void CActiveWipeObject::Close()
    {
    FLOG(_L(" Messaging: void CActiveWipeObject::Close() >>"));
    if (iSession)
        {
        FLOG(_L("  delete iSession;"));
        delete iSession;
        iSession = NULL;
        }
    FLOG(_L(" Messaging: void CActiveWipeObject::Close() <<"));
    }

//
//----------------------------------------------------------------------------- 
// CActiveWipeObject:: CActiveWipeObject
// Description: CActiveWipeObject Destructer
// ----------------------------------------------------------------------------- 
// 
CActiveWipeObject::~CActiveWipeObject()
    {
    FLOG(_L(" Messaging: CActiveWipeObject::~CActiveWipeObject() >>"));
    Close();
    if (iDummyObserver)
        delete iDummyObserver;
    iWs.Close();
    iTimer.Close();
    FLOG(_L(" Messaging: CActiveWipeObject::~CActiveWipeObject() <<"));
    }

//
// Description: Deletes message entries one by one. If used by message application, force deletes.
//
void CActiveWipeObject::DoJobL(CMsvEntry* aContext)
    {
    FLOG(_L(" Messaging: void CLAWMOMessagingplugin::doJobL() >>"));

    TMsvId entryID;
    CMsvEntrySelection* entries;

    entries = aContext->ChildrenL();
    CleanupStack::PushL(entries);

    TInt msgCount= entries->Count();
    FLOG(_L("  msgCount= %d "), msgCount );
    
    for (TInt i=msgCount-1; i>=0; i--)
    { 
         entryID = entries->At(i);
         CMsvEntry* entry = iSession->GetEntryL(entryID);
         CleanupStack::PushL(entry);
         TRAPD(err, entry->DeleteL(entryID));                                                                            
         CleanupStack::PopAndDestroy(entry);
         if(err)
         {
             FLOG(_L("  Deleteing entry failed with err= %d "), err);
             // Force removal of the entry. If no any other way only.  
             iSession->RemoveEntry(entryID);             
         }
    }                

    CleanupStack::PopAndDestroy(entries);
    FLOG(_L(" Messaging: void CLAWMOMessagingplugin::doJobL() <<"));
    }

void CActiveWipeObject::InitializeL()
    {
    FLOG(_L(" Messaging: void CActiveWipeObject::InitializeL() >>"));
    iSession = CMsvSession::OpenSyncL(*iDummyObserver);
    FLOG(_L(" Messaging: void CActiveWipeObject::InitializeL() <<"));
    }

//
//----------------------------------------------------------------------------- 
// CActiveWipeObject::WipeAsyncL
// Description: Wipes the logs 
// ---------------------------------------------------------------------------- 
//

void CActiveWipeObject::WipeAsyncL()
    {
    FLOG(_L(" Messaging: void CActiveWipeObject::WipeAsyncL () >>"));
        
    if ( !WaitTillMessageAppKilled()) //Application is closed already
        {
        TRequestStatus* status = &iStatus;
        FLOG(_L("  SetActive()"));
        SetActive();
        User::RequestComplete(status, KErrNone);
        }
    FLOG(_L(" Messaging: void CActiveWipeObject::WipeAsyncL () <<"));
    }

//
// Description: Wipes Inbox, Sent Items, Drafts, Outbox messages.
//
//
void CActiveWipeObject::WipeAllL()
    {
    FLOG(_L(" Messaging: void  CActiveWipeObject::wipeAll() >>"));

    CMsvEntry* context = NULL;
    TMsvSelectionOrdering sort;

    sort.SetShowInvisibleEntries(ETrue); // we want to handle also the invisible entries
    context = CMsvEntry::NewL(*iSession, KMsvGlobalInBoxIndexEntryId, sort); // Reading Messages from Inbox Folder
    DoJobL(context);
    delete context;

    context = CMsvEntry::NewL(*iSession, KMsvGlobalOutBoxIndexEntryId, sort); // Reading Messages from Outbox Folderif(KErrNone)
    DoJobL(context);
    delete context;

    context = CMsvEntry::NewL(*iSession, KMsvSentEntryId, sort); // Reading Messages from Sent Folder
    DoJobL(context);
    delete context;

    context = CMsvEntry::NewL(*iSession, KMsvDraftEntryId, sort); // Reading Messages from Draft Folder
    DoJobL(context);
    delete context;
    FLOG(_L(" Messaging: void  CActiveWipeObject::wipeAll() <<"));
    }

//
//----------------------------------------------------------------------------- 
// CActiveWipeObject::RunError
// Description: This returns error 
//----------------------------------------------------------------------------- 
//

TInt CActiveWipeObject::RunError(TInt aError)
    {
    FLOG(_L(" Messaging: TInt CActiveWipeObject::RunError(TInt aError) >>"));

    FLOG(_L(" Messaging: RunError error %d"), aError);
    iObserver->HandleWipeCompleted(aError);

    FLOG(_L(" Messaging: TInt CActiveWipeObject::RunError(TInt aError) <<"));
    return KErrNone;
    }

//
//----------------------------------------------------------------------------- 
// CActiveWipeObject::DoCancel
// Description: This calls CActive's Cancel 
//----------------------------------------------------------------------------- 
//
void CActiveWipeObject::DoCancel()
    {
    FLOG(_L(" Messaging: void CActiveWipeObject::DoCancel() >>"));
    iTimer.Cancel();
    FLOG(_L(" Messaging: void CActiveWipeObject::DoCancel() <<"));
    }

//
//-----------------------------------------------------------------------------
// CActiveWipeObject::WaitTillMessageAppKilled
// Description: This function checks for the Messaging applicaiton being run. If running then kill it.
//-----------------------------------------------------------------------------

TBool CActiveWipeObject::WaitTillMessageAppKilled()
    {
    FLOG(_L(" void CTTPAPITestAppUi::WaitTillMessageAppKilled() >>"));
    TBool ret(EFalse);
    TBool flag(ETrue);

    TApaTaskList list(iWs);
    TUid uid =
        {
        KMessagingAppUid
        };

    while (flag)
        {
        TApaTask task( list.FindApp( uid ) );
        if (task.Exists())
            {
            FLOG(_L("Killing one instance..."));
            task.EndTask();
            User::After(500000);
            ret = ETrue;
            }
        else
            {
            flag = EFalse;
            }
        }
    
    if (ret)
        {
        FLOG(_L(" WaitTillMessageAppKilled: iTimer.After(iStatus,2000); "));  
        iTimer.After(iStatus,2000); // 0.002 sec
        SetActive();
        }
    
     FLOG(_L(" void CTTPAPITestAppUi::WaitTillMessageAppKilled(), ret = %d <<"), ret);
     return ret;
    }



//
//----------------------------------------------------------------------------- 
// CActiveWipeObject::RunL 
// Description: CActive::RunL implementation which will either stop the
// Scheduler or increment the count
//----------------------------------------------------------------------------- 
//
void CActiveWipeObject::RunL()
    {
    FLOG(_L(" Messaging: void CActiveWipeObject::RunL() >>"));
    
    FLOG(_L("void CActiveWipeObject::RunL(): iStatus= %d "), iStatus.Int());
    InitializeL();    
    WipeAllL();
    iObserver->HandleWipeCompleted(KErrNone);
    FLOG(_L(" Messaging: void CActiveWipeObject::RunL() <<"));
    }
