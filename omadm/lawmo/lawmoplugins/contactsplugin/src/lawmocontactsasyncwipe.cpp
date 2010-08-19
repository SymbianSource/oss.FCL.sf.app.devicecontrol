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
* Description:  Active Object Source for wiping contacts, used by lawmocontactsplugin.
*
*/

#include <f32file.h>
#include <CPbkContactEngine.h>
#include <CPbkSingleEntryFetchDlg.h>
#include <rpbkviewresourcefile.h>
#include <cntdb.h>
#include <etelmm.h>

// PhoneBook2 uid 
 #include <Pbk2UID.h>

// Application Closing
#include <apgtask.h>
#include <w32std.h>
#include <APGTASK.H>

#include "fdebug.h"
#include "lawmocontactsasyncwipe.h"

// ----------------------------------------------------------------------------- 
// CActiveWipeObject::CActiveWipeObject
// Description: CActiveWipeObject constructer . Initializes datamember of MLawmoPluginWipeObserver 
// ----------------------------------------------------------------------------- 
//
CActiveWipeObject::CActiveWipeObject(MLawmoPluginWipeObserver* aObserver) : CActive( EPriorityStandard ) , 
        iObserver(aObserver), iContactsDb (NULL), iContacts (NULL)
{
    FLOG(_L( " Contacts: CActiveWipeObject::CActiveWipeObject(MLawmoPluginWipeObserver* aObserver) >> "));
    // See ConstructL() for initialisation completion.
    FLOG(_L( " Contacts: CActiveWipeObject::CActiveWipeObject(MLawmoPluginWipeObserver* aObserver) << "));
}

// ----------------------------------------------------------------------------- 
// CActiveWipeObject::NewL
// Description: CActiveWipeObject constructer 
// ----------------------------------------------------------------------------- 
//
CActiveWipeObject* CActiveWipeObject::NewL(MLawmoPluginWipeObserver* aObserver)
{
    FLOG(_L( " Contacts: CActiveWipeObject::NewL(MLawmoPluginWipeObserver* aObserver) >> "));
    CActiveWipeObject* self=new(ELeave) CActiveWipeObject(aObserver);  
    CleanupStack::PushL(self);
    self->ConstructL(); 
    CleanupStack::Pop();
    FLOG(_L( " Contacts: CActiveWipeObject::NewL(MLawmoPluginWipeObserver* aObserver) << "));
    return self;
}

// ----------------------------------------------------------------------------------------
// CActiveWipeObject::ConstructL
// ----------------------------------------------------------------------------------------
void CActiveWipeObject::ConstructL()
{   
    FLOG(_L( " Contacts: CActiveWipeObject::ConstructL >> "));
    iTimer.CreateLocal();
    User::LeaveIfError( iWsSession.Connect() );
    CActiveScheduler::Add(this); // Add to scheduler
    FLOG(_L( " Contacts: CActiveWipeObject::ConstructL << "));
}

//
//----------------------------------------------------------------------------- 
// CActiveWipeObject:: CActiveWipeObject
// Description: CActiveWipeObject Destructer
// ----------------------------------------------------------------------------- 
// 
CActiveWipeObject::~CActiveWipeObject()
{
    FLOG(_L( " Contacts: CActiveWipeObject::~CActiveWipeObject() >> "));
    Cancel();

    if(iContactsDb)
    {
        FLOG(_L("  delete iContactsDb;"));
        delete iContactsDb;
    }
        
    iTimer.Close();
    
    iWsSession.Close();

    FLOG(_L( " Contacts: CActiveWipeObject::~CActiveWipeObject() << "));
}

//
//----------------------------------------------------------------------------- 
// CActiveWipeObject::WipeAsyncL
// Description: Wipes the Contacts 
// ---------------------------------------------------------------------------- 
//

void CActiveWipeObject::WipeAsyncL ()
{    
    FLOG(_L(" Contacts: void CActiveWipeObject::WipeAsyncL () >>"));

    if ( !WaitTillPhoneBookKilled()) //Application is closed already
        {
        TRequestStatus* status = &iStatus;
        FLOG(_L("  SetActive()"));
        SetActive();
		User::RequestComplete(status,KErrNone);
        }
      
    FLOG(_L(" Contacts: void CActiveWipeObject::WipeAsyncL () <<"));
}

//
//----------------------------------------------------------------------------- 
// CActiveWipeObject::RunError
// Description: This returns error 
//----------------------------------------------------------------------------- 
//

TInt CActiveWipeObject::RunError(TInt aError)
{
    FLOG(_L(" Contacts: TInt CActiveWipeObject::RunError(TInt aError) >>"));
    FLOG(_L("  iObserver->HandleWipeCompleted(aError);"));
    iObserver->HandleWipeCompleted(aError);
    FLOG(_L(" Contacts: TInt CActiveWipeObject::RunError(TInt aError) <<"));
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
    FLOG(_L(" Contacts: void CActiveWipeObject::DoCancel() >>"));
    iTimer.Cancel();
    FLOG(_L(" Contacts: void CActiveWipeObject::DoCancel() <<"));
}

void CActiveWipeObject::DeletePhoneContactsL()
{
    FLOG(_L(" Contacts: void CActiveWipeObject::DeletePhoneContactsL() >>"));

    iContactsDb = CContactDatabase::OpenL();
    iContactsDb->SetDbViewContactType( KUidContactCard );
    iContacts= iContactsDb->SortedItemsL();
    FLOG(_L("Count = %d "), iContacts->Count());
    iContactsDb->DeleteContactsL(*iContacts);
    FLOG(_L(" Contacts: void CActiveWipeObject::DeletePhoneContactsL() <<"));
}

void CActiveWipeObject::DeleteSIMContactsL()
{
    FLOG(_L("void CActiveWipeObject::DeleteSIMContactsL() >> "));
    RTelServer telServer;
    RMobilePhone phone;
    RTelServer::TPhoneInfo info;
    User::LeaveIfError(telServer.Connect());
    CleanupClosePushL(telServer);
    
    User::LeaveIfError(telServer.GetPhoneInfo(0,info));
    User::LeaveIfError(phone.Open(telServer,info.iName) );
    CleanupClosePushL(phone);
    
    RMobilePhoneBookStore simPhBk;
    User::LeaveIfError(simPhBk.Open(phone, KETelIccAdnPhoneBook)) ;
    CleanupClosePushL(simPhBk); 

    TRequestStatus aStatus;
    FLOG(_L("  Contacts: phonebookstore open success"));
    simPhBk.DeleteAll(aStatus);
    FLOG(_L("  Contacts: issued delete"));
    User::WaitForRequest(aStatus);
    FLOG(_L("  Contacts: waiting done"));

 		User::LeaveIfError(aStatus.Int());
    CleanupStack::PopAndDestroy(&simPhBk);
    CleanupStack::PopAndDestroy(&phone);
    CleanupStack::PopAndDestroy(&telServer);
      
    FLOG(_L("void CActiveWipeObject::DeleteSIMContactsL() << "));
}

TBool CActiveWipeObject::WaitTillPhoneBookKilled()
    {
    FLOG(_L(" void CTTPAPITestAppUi::WaitTillPhonebookKilled() >>"));
    TBool ret(EFalse);
    TBool flag(ETrue);

    TApaTaskList list(iWsSession);
    TUid uid =
        {
        KPbk2UID3
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
        iTimer.After(iStatus, 2000); // 0.002 sec
        SetActive();
        }
    
     FLOG(_L(" void CTTPAPITestAppUi::WaitTillPhonebookKilled(), ret = %d <<"), ret);
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
    FLOG(_L(" Contacts: void CActiveWipeObject::RunL() >>"));

    DeletePhoneContactsL();
    DeleteSIMContactsL();
    iObserver->HandleWipeCompleted(KErrNone);
    FLOG(_L(" Contacts: void CActiveWipeObject::RunL() <<"));
    }
