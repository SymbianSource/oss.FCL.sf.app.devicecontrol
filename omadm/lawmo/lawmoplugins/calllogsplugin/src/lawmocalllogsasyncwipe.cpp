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
* Description: Active Object for wiping Call-logs, called by lawmocalllogsplugin.
*
*/
#include <logcli.h>
#include <f32file.h>
#include <logwrap.h>
#include <logview.h>
#include <e32base.h>
#include <logwraplimits.h>
#include <logwrapconst.h>
#include "lawmocalllogsasyncwipe.h"
#include "fdebug.h"

// Application Closing
#include <apgtask.h>
#include <w32std.h>

// ----------------------------------------------------------------------------- 
// CActiveWipeObject::CActiveWipeObject
// Description: CActiveWipeObject constructer . Initializes datamember of MLawmoPluginWipeObserver 
// ----------------------------------------------------------------------------- 
//
CActiveWipeObject::CActiveWipeObject(MLawmoPluginWipeObserver* aObserver) :
    CActive(EPriorityStandard), iObserver(aObserver), ilogClient(NULL), iTimerStarted (EFalse)
    {
    FLOG(
            _L( " CAllLogs: CActiveWipeObject::CActiveWipeObject(MLawmoPluginWipeObserver* aObserver) >> "));
    // See ConstructL() for initialisation completion.
    FLOG(
            _L( " CAllLogs: CActiveWipeObject::CActiveWipeObject(MLawmoPluginWipeObserver* aObserver) << "));
    }

// ----------------------------------------------------------------------------- 
// CActiveWipeObject::NewL
// Description: CActiveWipeObject constructer 
// ----------------------------------------------------------------------------- 
//
CActiveWipeObject* CActiveWipeObject::NewL(
        MLawmoPluginWipeObserver* aObserver)
    {
    FLOG(_L( " CAllLogs: CActiveWipeObject::NewL >> "));
    CActiveWipeObject* self = new (ELeave) CActiveWipeObject(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    FLOG(_L( " CAllLogs: CActiveWipeObject::NewL << "));
    return self;
    }

// ----------------------------------------------------------------------------------------
// CActiveWipeObject::ConstructL
// ----------------------------------------------------------------------------------------
void CActiveWipeObject::ConstructL()
    {
    FLOG(_L( " CAllLogs: CActiveWipeObject::ConstructL >> "));
    iTimer.CreateLocal();
    User::LeaveIfError(iWsSession.Connect());
    CActiveScheduler::Add(this); // Add to scheduler    
    FLOG(_L( " CAllLogs: CActiveWipeObject::ConstructL << "));
    }

//
//----------------------------------------------------------------------------- 
// CActiveWipeObject:: CActiveWipeObject
// Description: CActiveWipeObject Destructer
// ----------------------------------------------------------------------------- 
// 
CActiveWipeObject::~CActiveWipeObject()
    {
    FLOG(_L( " CAllLogs: CActiveWipeObject::~CActiveWipeObject() >> "));
    Cancel();
    if (ilogClient)
    	{
        delete ilogClient;
      }
      
    if (ifs.Handle())
      	{
      		ifs.Close();
      	}

    iTimer.Close();
    
    iWsSession.Close();
        
    FLOG(_L( " CAllLogs: CActiveWipeObject::~CActiveWipeObject() << "));
    }

//
//----------------------------------------------------------------------------- 
// CActiveWipeObject::WipeAsyncL
// Description: Wipes the logs 
// ---------------------------------------------------------------------------- 
//

void CActiveWipeObject::WipeAsyncL()
    {
    FLOG(_L(" CAllLogs: void CActiveWipeObject::WipeAsyncL () >>"));
    if (!WaitTillLogsAppKilled()) //Application is closed already
        {
        ClearLogsL();
        }

    FLOG(_L(" CAllLogs: void CActiveWipeObject::WipeAsyncL () <<"));
    }

//
//----------------------------------------------------------------------------- 
// CActiveWipeObject::RunError
// Description: This returns error 
//----------------------------------------------------------------------------- 
//

TInt CActiveWipeObject::RunError(TInt aError)
    {
    FLOG(_L(" CAllLogs: TInt CActiveWipeObject::RunError(TInt aError) >>"));
    FLOG(_L(" CAllLogs: RunError error %d"), aError);
    iObserver->HandleWipeCompleted(aError);
    FLOG(_L(" CAllLogs: TInt CActiveWipeObject::RunError(TInt aError) <<"));
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
    FLOG(_L(" CAllLogs: void CActiveWipeObject::DoCancel() >>"));
    if (iTimerStarted)
    	{
    		iTimer.Cancel();
    	}
    else
    	{
    	ilogClient->Cancel();
      }
    FLOG(_L(" CAllLogs: void CActiveWipeObject::DoCancel() <<"));
    }

TBool CActiveWipeObject::WaitTillLogsAppKilled()
    {
    FLOG(_L(" WaitTillLogsAppKilled() >>"));

    TBool ret(EFalse);
    TBool flag(ETrue);

    TApaTaskList list(iWsSession);
    TUid uid =
        {
        KCallLogsAppUid
        };

    while (flag)
        {
        TApaTask task(list.FindApp(uid));
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
        FLOG(_L(" WaitTillPhonebookKilled: iTimer.After(iStatus,1000); "));
        iTimerStarted = ETrue; // If time is started, this is true
        iTimer.After(iStatus, 2000); // 0.002 sec
        SetActive();
        }

    FLOG(_L(" WaitTillLogsAppKilled(), ret = %d <<"), ret);
    return ret;
    }


//
//----------------------------------------------------------------------------- 
// CActiveWipeObject::RunL 
// Description: Clears the logs
//----------------------------------------------------------------------------- 
//
void CActiveWipeObject::ClearLogsL()
    {
    FLOG(_L("void CActiveWipeObject::ClearLogs() >> "));
    TTime time(KMaxTime);
    User::LeaveIfError(ifs.Connect());
    ilogClient = CLogClient::NewL(ifs);
    FLOG(_L("iStatus.Int() = %d "), iStatus.Int());
    ilogClient->ClearLog(time, iStatus);
    SetActive();

    FLOG(_L("void CActiveWipeObject::ClearLogs() << "));
    }

//
//----------------------------------------------------------------------------- 
// CActiveWipeObject::RunL 
// Description: Called when either timer expires of logs are cleared
//----------------------------------------------------------------------------- 
//
void CActiveWipeObject::RunL()
    {
    FLOG(_L(" CAllLogs: void CActiveWipeObject::RunL() >>"));
    FLOG(_L("void CActiveWipeObject::RunL(): iStatus= %d "), iStatus.Int());

    if (iTimerStarted)
        {
        iTimerStarted = EFalse;
        ClearLogsL();
        }
    else
        iObserver->HandleWipeCompleted(iStatus.Int());
    FLOG(_L(" CAllLogs: void CActiveWipeObject::RunL() <<"));
    }
