/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     Bio control for Provisioning documents.
*
*/

// INCLUDE FILES
#include "CWPBioControl.h"
#include <msvapi.h>
#include <bldvariant.hrh>
#include <msvids.h>
#include <featmgr.h>
#include <sysutil.h>
#include "CWPEngine.h"
#include "CWPAdapter.h"
#include "MWPPhone.h"
#include "WPPhoneFactory.h"
#include "CWPPushMessage.h"
#include "ProvisioningUIDs.h"
#include "MWPContextManager.h"
#include "ProvisioningDebug.h"
#include "CWPNameValue.h"
#include <aknmessagequerydialog.h> 
#include "ProvisioningInternalCRKeys.h"

// CONSTANTS
/// Maximum length of PIN code
const TInt KMaxPinSize = 20;
/// Number of retries for PIN
// const TInt KPinRetries = 3; // Removed, because iMtmData1 data member of TMsvEntry in CWPMessage.cpp contains Max tries, default=3.
///NONE
_LIT(KNone,"");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Constructor.
// -----------------------------------------------------------------------------
//
CWPBioControl::CWPBioControl(CMsvSession* aSession,TMsvId aId)
                          
    {
    }

// -----------------------------------------------------------------------------
// CWPBioControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPBioControl::ConstructL(CMsvSession* aSession,TMsvId aId)
    {
    FLOG( _L( "[ProvisioningBC] CWPBioControl::ConstructL:" ) );  
    FeatureManager::InitializeLibL();
    iSession = aSession;
    iId = aId;
    iEngine = CWPEngine::NewL();
    RestoreMsgL();
    FLOG( _L( "[ProvisioningBC] CWPBioControl::ConstructL: done" ) );
    }

// -----------------------------------------------------------------------------
// CWPBioControl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CWPBioControl* CWPBioControl::NewL(CMsvSession* aSession,TMsvId aId)
     {
    CWPBioControl* self = new( ELeave ) CWPBioControl(aSession,aId);                                                       
    CleanupStack::PushL( self );
    self->ConstructL(aSession,aId);
    CleanupStack::Pop();
    return self;
    }
// -----------------------------------------------------------------------------
// CWPBioControl::returnEngine
// Return CWPEngine pointer
// -----------------------------------------------------------------------------
//    
EXPORT_C CWPEngine* CWPBioControl::returnEngine()
    {
    return iEngine;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CWPBioControl::~CWPBioControl()
    {
    delete iEngine;
    delete iMessage;
    FeatureManager::UnInitializeLib();
    }
// ----------------------------------------------------------------------------
// CWPBioControl ::RestoreMsgL
// ----------------------------------------------------------------------------
//
void CWPBioControl::RestoreMsgL()
    {                    
    CMsvEntry* entry = iSession->GetEntryL(iId);   
    CleanupStack::PushL( entry );
    TMsvEntry tentry( entry->Entry() );
    iMessage = CWPPushMessage::NewL();
    CMsvStore* readStore = entry->ReadStoreL();
    CleanupStack::PushL( readStore );
    iMessage->RestoreL( *readStore );
    CleanupStack::PopAndDestroy(); // readStore
 
    TRAPD( result, iEngine->ImportDocumentL( iMessage->Body() ) );
    if( result == KErrCorrupt )
        {
        //result = KErrMsgBioMessageNotValid;
        }
    FTRACE(RDebug::Print(_L("[ProvisioningBC] CWPBioControl::RestoreMsgL result (%d)"), result));
    User::LeaveIfError( result );
    
    TBool preAuthenticated( iMessage->Authenticated() );
    // If the message was not authenticated, give a warning
    AuthenticateL( *iMessage );
    if( !tentry.ReadOnly() && preAuthenticated != iMessage->Authenticated() )
        {
        // If the entry can be written to, get its edit store and save
        // authentication flag. Failing is not dangerous, as the only
        // effect is that the user has to re-enter the PIN.
        // FLOG( _L( "[ProvisioningBC] CWPBioControl::RestoreMsgL 4 " ) );
        TRAPD( ignoreError, SaveMessageL() );
        if ( ignoreError ) ignoreError = 0; // prevent compiler warning 
        }
    
    iEngine->PopulateL();
    if( iEngine->ItemCount() == 0 )
        {
        FLOG( _L( "[ProvisioningBC] CWPBioControl::RestoreMsgL 7 " ) );
        }

    TPtrC8 orig8( iMessage->Originator() );
    HBufC* orig16 = HBufC::NewLC( orig8.Length() );
    orig16->Des().Copy( orig8 );
    FLOG( _L( "[ProvisioningBC] CWPBioControl::RestoreMsgL 8 " ) );
    if( iMessage->Authenticated() 
        && iEngine->ContextExistsL( *orig16 ) )
        {
        TUint32 context( iEngine->ContextL( *orig16 ) );
        iEngine->SetCurrentContextL( context );
        }
    else
        {
        iEngine->SetCurrentContextL( KWPMgrUidNoContext );
        }
        
    
    CleanupStack::PopAndDestroy(2);
    FLOG( _L( "[ProvisioningBC] CWPBioControl::RestoreMsgL done" ) );
    }

// ----------------------------------------------------------------------------
// CWPBioControl ::AuthenticateL
// ----------------------------------------------------------------------------
//
void CWPBioControl::AuthenticateL( CWPPushMessage& aMessage )
    {
    if( !aMessage.Authenticated() )
        {
         MWPPhone* phone = NULL;
         TBuf<KMaxPinSize> imsi;
         TRAPD(err, phone = WPPhoneFactory::CreateL());
         if(err!=KErrNone)
             {
             imsi = KNone;
             }
         else
             {
             imsi = phone->SubscriberId();
             }
        CWPBootstrap* bootstrap = CWPBootstrap::NewL( imsi );
        if(phone)
        delete phone;

        CleanupStack::PushL( bootstrap );
        CWPBootstrap::TBootstrapResult result( bootstrap->BootstrapL(aMessage,*iEngine,KNullDesC ) );
        if( result == CWPBootstrap::EPinRequired )
         {
            CMsvEntry* entry = iSession->GetEntryL(iId);
            TInt count = (entry->Entry()).MtmData1();
            TInt ipin = 1234;
            TBuf<KMaxPinSize> pin;
            pin.AppendNum(ipin);
            result = bootstrap->BootstrapL( aMessage, *iEngine, pin );
            CleanupStack::PopAndDestroy( bootstrap );
          }
      }
    if (aMessage.Authenticated() )
        {
        //update Cenrep key
        TBuf8<100> orig;
        orig.Copy(aMessage.Originator());
        TRAPD(err, SetCenrepKeyL(orig));
        User::LeaveIfError(err);

        }
    }
// ----------------------------------------------------------------------------
// CWPBioControl ::SaveMessageL
// ----------------------------------------------------------------------------
//
void CWPBioControl::SaveMessageL()
    {
    CMsvEntry* entry = iSession->GetEntryL(iId);
    CleanupStack::PushL( entry );

    CMsvStore* editStore = entry->EditStoreL();
    CleanupStack::PushL( editStore );
    iMessage->StoreL( *editStore );
    editStore->CommitL();
    CleanupStack::PopAndDestroy(2); // editStore, entry
    }

// ----------------------------------------------------------------------------
// CWPBioControl ::CollectItemsL
// ----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CWPNameValue>* CWPBioControl::CollectItemsLC() const
    {
    RPointerArray<CWPNameValue>* array = new(ELeave) RPointerArray<CWPNameValue>;
    CleanupStack::PushL( TCleanupItem( Cleanup, array ) );

    TInt count( iEngine->ItemCount() );
    FLOG( _L( "[ProvisioningBC] CWPBioControl::RestoreMsgL 83 " ) );
    for( TInt index = 0; index < count; index++ )
        {
        const TDesC& text = iEngine->SummaryText( index );
        const TDesC& title = iEngine->SummaryTitle( index );

        CWPNameValue* pair = CWPNameValue::NewLC( title, text );
        User::LeaveIfError( array->Append( pair ) );
        CleanupStack::Pop( pair );
        }
    array->Sort( TLinearOrder<CWPNameValue>( CWPBioControl::Compare ) );
    return array;
    }

// -----------------------------------------------------------------------------
// CWPBioControl::Cleanup
// -----------------------------------------------------------------------------
//
void CWPBioControl::Cleanup( TAny* aAny )
    {
    RPointerArray<CWPNameValue>* array = reinterpret_cast<RPointerArray<CWPNameValue>*>( aAny );
    array->ResetAndDestroy();
    array->Close();
    delete array;
    }

// -----------------------------------------------------------------------------
// CWPBioControl::Compare
// -----------------------------------------------------------------------------
//
TInt CWPBioControl::Compare( const CWPNameValue& aItem1,
                             const CWPNameValue& aItem2 )
    {
    return aItem1.Name().Compare( aItem2.Name() );
    }

// ----------------------------------------------------------------------------
// CWPBioControl ::SetCenrepKey
// ----------------------------------------------------------------------------
//
void CWPBioControl::SetCenrepKeyL(const TDesC8& aValue)
    {

    FLOG( _L( "[ProvisioningBC] CWPBioControl::SetCenrepKeyL" ) );

    CRepository* rep= NULL;
    TInt errorStatus = KErrNone;

    TRAPD( errVal, rep = CRepository::NewL( KCRUidOMAProvisioningLV ))
    ;

    if (errVal == KErrNone)
        {
        errorStatus = rep->Set(KOMAProvOriginatorContent, aValue);
        }
    else
        {
        errorStatus = errVal;
        }

    if (rep)
        {
        delete rep;
        }

    if (errorStatus != KErrNone)
        {
        User::Leave(errorStatus);
        }

    FLOG( _L( "[ProvisioningBC] CWPBioControl::SetCenrepKeyL done" ) );

    }
//  End of File
