/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Service Activation this feature provided if service 
*                to be enabled from PNPMS application
*
*/


#include <PnpProvUtil.h>
#include "PnpServiceActivation.h"
#include <apacmdln.h> 
#include <apgcli.h>
#include <w32std.h>
#include <apgtask.h>
#include <w32std.h>
#include <StringLoader.h>
#include <pnpprovisioning.rsg>
#include <AknGlobalNote.h>
#include <centralrepository.h>

#include <e32svr.h>
#include <centralrepository.h>
#include "PnpLogger.h"


const TUid    KCRUidPnPProvisioning = {0x10204338};
// Browser UID
const TUid KWAPBrowserUID = { 0x10008d39 };
const TInt KMaxLength = 255;
// POP3 and IMAP4 provisioning settings application ID

// Literal constants

const TUint32 KServiceAct1  = 0x00000100;

// INCLUDE FILES

// -----------------------------------------------------------------------------
// CPnpServiceActivation::CPnpServiceActivation
// C++ default constructor can NOT contain any code, that
//  -----------------------------------------------------------------------------
//
CPnpServiceActivation::CPnpServiceActivation()
    {
    }

// -----------------------------------------------------------------------------
// CPnpProvUtil::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPnpServiceActivation::ConstructL()
    {
     //No values to assign	
    }  
 
// -----------------------------------------------------------------------------
// CPnpProvUtil::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPnpServiceActivation* CPnpServiceActivation::NewL()
    {
    CPnpServiceActivation* self = NewLC();
    CleanupStack::Pop();
    
    return self; 
    }

// -----------------------------------------------------------------------------
// CPnpServiceActivation::NewLC
// -----------------------------------------------------------------------------
//
CPnpServiceActivation* CPnpServiceActivation::NewLC()
    {
    CPnpServiceActivation* self = new( ELeave ) CPnpServiceActivation;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    
    return self; 
    } 

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CPnpServiceActivation::~CPnpServiceActivation()
    {   
    }
// -----------------------------------------------------------------------------
// CPnpServiceActivation::LaunchApplicationL
// -----------------------------------------------------------------------------
//
void CPnpServiceActivation::LaunchApplicationL()
    {
    
    // For email service activation Messaging launcher is activated
    TBool validApp = ValidateAppUidWithProvappIDL();
    
    if(validApp) 
    {
    	
    
     
     
     RApaLsSession appArcSession;
	
     User::LeaveIfError( appArcSession.Connect() );
       CleanupClosePushL( appArcSession );
       TApaAppInfo appInfo; 
       User::LeaveIfError(appArcSession.GetAppInfo( appInfo, TUid::Uid( iLaunchUid) ) );        
       CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
       cmdLine->SetCommandL( EApaCommandRun );
       cmdLine->SetExecutableNameL( appInfo.iFullName );
       User::LeaveIfError( appArcSession.StartApp( *cmdLine ) );
       CleanupStack::PopAndDestroy( 2 ); // cmdLine, appArcSession 
    }

    }


// -----------------------------------------------------------------------------
// CPnpServiceActivation::ValidateAppUidWithProvappIDL()
// -----------------------------------------------------------------------------
//
TBool CPnpServiceActivation::ValidateAppUidWithProvappIDL()
    {
    
    // Get application ID and Provisioning settings application
    // ID and validate the application launcher	
   
    RPointerArray<HBufC> appID;
    TBuf<16> pLaunchUid;
    TBool returnFlag (EFalse);
    TBool FoundUid(EFalse);
    TBool FoundAppid(EFalse);
	
    CPnpProvUtil *provutil = CPnpProvUtil::NewLC();
    
    iLaunchUid = provutil->GetApplicationUidL();
 
	provutil->GetProvAdapterAppIdsL(appID);
    pLaunchUid.Num(iLaunchUid);
    
    FoundUid = isPresent(pLaunchUid);
    TUint32 secondPosition = iPosition+200;
    if(FoundUid)
        {
    	for(TInt i=0; i<appID.Count(); i++)
    	{
		TPtrC ptr(*appID[i]);
    	    FoundAppid = isFound(ptr,secondPosition);
            if(FoundAppid)
                {
                if(iPosition == KServiceAct1){
                DisplayAppSpecNoteL();
                }
                returnFlag = ETrue;
                }
    	}
     }
    
    
    CleanupStack::PopAndDestroy();
     
    appID.ResetAndDestroy();
    appID.Close();
    return returnFlag; 
    }


// -----------------------------------------------------------------------------
// CPnpServiceActivation::KillBrowserL()
// -----------------------------------------------------------------------------
//
void CPnpServiceActivation::KillBrowserL()
    {
     
    // Get the correct application data 
    RWsSession ws;
    User::LeaveIfError(ws.Connect()); 
    CleanupClosePushL(ws);

    // Find the task with uid of browser. If task exists then 
    // kill browser
    TApaTaskList taskList(ws);
    TApaTask task = taskList.FindApp( KWAPBrowserUID );
    if(task.Exists())
    task.EndTask();

    CleanupStack::PopAndDestroy(); // ws, lsSession

    
    }
    
// -----------------------------------------------------------------------------
// CPnpServiceActivation::DisplayAppSpecNote()
// -----------------------------------------------------------------------------
//

void CPnpServiceActivation::DisplayAppSpecNoteL()
{
    HBufC* text = StringLoader::LoadLC(R_TEXT_EMAIL_SERVICE_NOTE);

    CAknGlobalNote* globalNote = CAknGlobalNote::NewL();
    CleanupStack::PushL( globalNote );
    globalNote->ShowNoteL( EAknGlobalInformationNote , *text );
    CleanupStack::PopAndDestroy( 2 );
}

TBool CPnpServiceActivation::isPresent(TDesC& p)
{
   TBool flag(EFalse);
   TUint32 position = KServiceAct1;
   for(TInt i=0;i<=299;i++)
        {
        TBool val = isFound(p,position);
        if(val)
            {
            iPosition = position;
            flag = ETrue;
            break;
            }   
        position++;
        }
      return flag;
     }
TBool CPnpServiceActivation::isFound(TDesC& p,TUint32 position)
{
    TBool val(EFalse);     
    TBuf16<KMaxLength> keys;
    HBufC* heapBuf = HBufC::NewLC(20);
    TPtr ptr(heapBuf->Des());
    ptr = keys;
    RPointerArray<HBufC> array;    
    TRAPD( errVal,GetCenrepValueL(KCRUidPnPProvisioning,position,keys));
//  End of File
    if(errVal != KErrNone)
            {
            CleanupStack::PopAndDestroy();
            return val;
            }
    ParseValues(array, keys);
    val = Search(array,p);
    CleanupStack::PopAndDestroy(heapBuf);
    array.ResetAndDestroy();
    array.Close();
    return val;    
}

TBool CPnpServiceActivation::Search(RPointerArray<HBufC>& array,TDesC& p)
{
    TBool flag(EFalse);
    for(TInt j=0; j<array.Count(); j++)
        {
        TPtrC ptr(*array[j]);
	if(!ptr.Compare(p))
            {
            flag = ETrue;
            }
        }
    return flag;
}
void CPnpServiceActivation::ParseValues(RPointerArray<HBufC>& array, TDesC& aValue)
{
     TLex aLex(aValue);
     while(aLex.Peek() != '\x00')
        {
        aLex.Mark();
        while(aLex.Peek()!=',' && aLex.Peek()!='\x00')
            aLex.Inc();
        TPtrC aPtr = aLex.MarkedToken(); 
        HBufC *buf18 = aPtr.AllocL();
        array.Append(buf18); 
        if(aLex.Peek()=='\x00') // end of string
            break; 
        aLex.Inc();
    }
}
void CPnpServiceActivation::GetCenrepValueL(const TUid aRepositoryUid,TUint32 aKey, TDes16& aValue)
    {
    CRepository * rep = NULL;
    TInt errorStatus = KErrNone;
    TRAPD( errVal, rep = CRepository::NewL(aRepositoryUid));
    if(errVal == KErrNone)
        {
        errorStatus =  rep->Get( aKey , aValue );
        }
        else
        {
        errorStatus =  errVal;
        }
    
    if(rep)     
        {
        delete rep;
        }
    if(errorStatus != KErrNone )
        {
        User::Leave(errorStatus);
        }
    }
