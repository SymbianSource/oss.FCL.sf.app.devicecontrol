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
* Description:  Methods for CNSmlDMProgressDialog. 
*
*/


// INCLUDE FILES
#include "NSmlDMSyncAppUi.h"
#include "NSmlDMProgress.h"
#include "NSmlDMSyncUtil.h"
#include "NSmlDMSyncDebug.h"
#include <NSmlDMSync.rsg>
#include <StringLoader.h>
#include <eikprogi.h>
#include <notecontrol.h>
#include <AknsUtils.h>
#include <aknappui.h>
#include "NSmlDMSyncUi.hrh"
// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CNSmlDMProgressDialog::CNSmlDMProgressDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNSmlDMProgressDialog::CNSmlDMProgressDialog(
                                    MNSmlDMProgressDialogObserver* aObserver )
                                   : iObserver( aObserver )
    {
    FLOG( "CNSmlDMProgressDialog: CNSmlDMProgressDialog" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMProgressDialog::NewL
// -----------------------------------------------------------------------------
//
CNSmlDMProgressDialog* CNSmlDMProgressDialog::NewL(
                                      MNSmlDMProgressDialogObserver* aObserver )
    {
    FLOG( "CNSmlDMProgressDialog::NewL" );

    CNSmlDMProgressDialog* self = 
                               new ( ELeave ) CNSmlDMProgressDialog( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    FLOG( "CNSmlDMProgressDialog::NewL completed" );
    return(self);
    }

// -----------------------------------------------------------------------------
// CNSmlDMProgressDialog::ConstructL
//
// Symbian OS default constructor can leave.
// -----------------------------------------------------------------------------
//
void CNSmlDMProgressDialog::ConstructL()
    {
    FLOG( "CNSmlDMProgressDialog::ConstructL" );

	iLastText = KNullDesC;
	iAnimation = EAnimationNone;
	iEventCount = 0;
    
	FLOG( "CNSmlDMProgressDialog::ConstructL completed" );
    } 

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CNSmlDMProgressDialog::~CNSmlDMProgressDialog()
    {
    FLOG( "CNSmlDMProgressDialog::~CNSmlDMProgressDialog" );

    delete iProgressDialog;

	FLOG( "CNSmlDMProgressDialog::~CNSmlDMProgressDialog completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMProgressDialog::LaunchProgressDialogL
// -----------------------------------------------------------------------------
//
void CNSmlDMProgressDialog::LaunchProgressDialogL( const TDesC& aLabel)
    {
    FLOG( "CNSmlDMProgressDialog::LaunchProgressDialogL" );

	iProgressDialog = new( ELeave ) CNSmlDMProgressDialog2(
	                          reinterpret_cast<CEikDialog**>(&iProgressDialog));
    
	iProgressDialog->PrepareLC( R_PROGRESS_NOTE );
    iProgressDialog->SetTextL(aLabel);

    // get reference to progress info bar
    iProgressInfo = iProgressDialog->GetProgressInfoL();
    iProgressDialog->SetCallback(this);
    
    iProgressDialog->RunLD();
    
	FLOG( "CNSmlDMProgressDialog::LaunchProgressDialogL completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMProgressDialog::LaunchProgressDialogL
// -----------------------------------------------------------------------------
//
void CNSmlDMProgressDialog::LaunchProgressDialogL( const TDesC& aLabel,
                                                   TAknsItemID aId,
                                                   const TDesC& aFileName,
                                                   TInt aFileBitmapId,
                                                   TInt aFileMaskId )
	{
    FLOG( "CNSmlDMProgressDialog::LaunchProgressDialogL" );

	iProgressDialog = new( ELeave ) CNSmlDMProgressDialog2(
	                        reinterpret_cast<CEikDialog**>(&iProgressDialog));
    
	iProgressDialog->PrepareLC( R_PROGRESS_NOTE );
    iProgressDialog->SetTextL(aLabel);
	
	CEikImage* image = CreateImageLC( aId, 
	                                  aFileName, 
	                                  aFileBitmapId, 
	                                  aFileMaskId );
    iProgressDialog->SetImageL(image);
	CleanupStack::Pop(image);

    // get reference to progress info bar
    iProgressInfo = iProgressDialog->GetProgressInfoL();
    iProgressDialog->SetCallback(this);
    
    iProgressDialog->RunLD();

	FLOG( "CNSmlDMProgressDialog::LaunchProgressDialogL completed" );
	}

// -----------------------------------------------------------------------------
// CNSmlDMProgressDialog::CancelProgressDialogL
// -----------------------------------------------------------------------------
//
void CNSmlDMProgressDialog::CancelProgressDialogL()
    {
    FLOG( "CNSmlDMProgressDialog::CancelProgressDialogL" );

	if( iProgressDialog )
        {        
        iProgressDialog->ProcessFinishedL();
        }    

	FLOG( "CNSmlDMProgressDialog::CancelProgressDialogL completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMProgressDialog::UpdateTextL
// -----------------------------------------------------------------------------
//
void CNSmlDMProgressDialog::UpdateTextL(TInt /*aResId*/)
    {
   	FLOG( "CNSmlDMProgressDialog::UpdateTextL " );

    }

// -----------------------------------------------------------------------------
// CNSmlDMProgressDialog::UpdateTextL
// -----------------------------------------------------------------------------
//
void CNSmlDMProgressDialog::UpdateTextL(const TDesC& aText)
    {
    FLOG( "CNSmlDMProgressDialog::UpdateTextL ( const TDesC& aText )" );

	__ASSERT_DEBUG(iProgressDialog, TUtil::Panic(KErrGeneral));

	if (iLastText.Compare(aText) == 0)
		{
		return;  // text has not changed
		}
    
    iProgressDialog->SetTextL( aText );
	TUtil::StrCopy(iLastText, aText);

	FLOG(  "CNSmlDMProgressDialog::UpdateTextL ( const TDesC& aText )completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMProgressDialog::SetProgress
// -----------------------------------------------------------------------------
//
void CNSmlDMProgressDialog::SetProgress(TInt aValue)
    {
    FLOG(  "CNSmlDMProgressDialog::SetProgress" );
    
	__ASSERT_DEBUG(iProgressDialog  && iProgressInfo, TUtil::Panic(KErrGeneral));

	CEikProgressInfo::SInfo info = iProgressInfo->Info();

	TInt val = aValue;
	if (val > info.iFinalValue)
		{
		val = info.iFinalValue;
		}
	iProgressInfo->SetAndDraw(val);
    
	FLOG( "CNSmlDMProgressDialog::SetProgress completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMProgressDialog::SetFinalProgress
// -----------------------------------------------------------------------------
//
void CNSmlDMProgressDialog::SetFinalProgress(TInt aFinalValue)
	{
    FLOG( "CNSmlDMProgressDialog::SetFinalProgress" );
    
    __ASSERT_DEBUG(iProgressInfo, TUtil::Panic(KErrGeneral));

    iProgressInfo->SetFinalValue(aFinalValue);
	iProgressInfo->SetAndDraw(0);

	FLOG( "CNSmlDMProgressDialog::SetFinalProgress completed" );
	}

// -----------------------------------------------------------------------------
// CNSmlDMProgressDialog::NoteControl
// -----------------------------------------------------------------------------
//
CAknNoteControl* CNSmlDMProgressDialog::NoteControl()
	{
	FLOG( "CNSmlDMProgressDialog::NoteControl" );
	
    CAknNoteControl* note = STATIC_CAST( 
                           CAknNoteControl*, 
                           iProgressDialog->ControlOrNull(ENSmlProgressNote) );
	__ASSERT_DEBUG( note, TUtil::Panic( KErrGeneral ) );

	return note;
	}

// -----------------------------------------------------------------------------
// CNSmlDMProgressDialog::CancelAnimation
// -----------------------------------------------------------------------------
//
void CNSmlDMProgressDialog::CancelAnimation()
	{
	NoteControl()->CancelAnimation();
	}

// ----------------------------------------------------------------------------
// CNSmlDMProgressDialog::StartAnimationL
// ----------------------------------------------------------------------------
//
void CNSmlDMProgressDialog::StartAnimationL()
	{
	NoteControl()->StartAnimationL();
	}

// -----------------------------------------------------------------------------
// CNSmlDMProgressDialog::DialogDismissedL
// -----------------------------------------------------------------------------
//
void CNSmlDMProgressDialog::DialogDismissedL( TInt aButtonId )
    {
    FLOG( "CNSmlDMProgressDialog::DialogDismissedL " );
  
    if ( iObserver )
		{
        // this dialog only sends one cancel event - other events are ignored 
    	if ( aButtonId == EEikBidCancel && iEventCount == 0 )
    		{
    		iEventCount++;
            TRAP_IGNORE( iObserver->HandleDialogEventL( aButtonId ) );
            }
		}
    FLOG( "CNSmlDMProgressDialog::DialogDismissedL Completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMProgressDialog::ReadL
// -----------------------------------------------------------------------------
//	
void CNSmlDMProgressDialog::ReadL( TDes& aText, TInt aResourceId )
    {
    HBufC* buf = StringLoader::LoadLC( aResourceId );
    TUtil::StrCopy( aText, *buf );
    CleanupStack::PopAndDestroy( buf );
    }

// -----------------------------------------------------------------------------
// CNSmlDMProgressDialog::CreateImageLC
// -----------------------------------------------------------------------------
//
CEikImage* CNSmlDMProgressDialog::CreateImageLC( TAknsItemID aId,
                                                 const TDesC& aFileName,
                                                 TInt aFileBitmapId,
                                                 TInt aFileMaskId )
	{
	CFbsBitmap* bitmap1 = NULL;
	CFbsBitmap* bitmap2 = NULL;
	
	CEikImage* image = new (ELeave) CEikImage;
	CleanupStack::PushL(image);

	AknsUtils::CreateIconLC( AknsUtils::SkinInstance(),
	                         aId,
	                         bitmap1,
	                         bitmap2,
	                         aFileName,
	                         aFileBitmapId,
	                         aFileMaskId );
	image->SetPicture( bitmap1, bitmap2 );
	CleanupStack::Pop(2); // bitmap1 bitmap2

	return image;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMProgressDialog::CNSmlDMProgressDialog2
// -----------------------------------------------------------------------------
//
void CNSmlDMProgressDialog::SetImageL(CEikImage* aimage)
    {
    NoteControl()->SetImageL(aimage);	
    }
    
/******************************************************************************
 * class CAknProgressDialog2
 ******************************************************************************/

// -----------------------------------------------------------------------------
// CNSmlDMProgressDialog2::CNSmlDMProgressDialog2
// -----------------------------------------------------------------------------
//
CNSmlDMProgressDialog2::CNSmlDMProgressDialog2( CEikDialog** aSelfPtr ) 
                                              : CAknProgressDialog( aSelfPtr )
	{
	iCloseEventReceived = EFalse;
	iClosed = EFalse;
	}

// -----------------------------------------------------------------------------
// CNSmlDMProgressDialog2::OkToExitL
// -----------------------------------------------------------------------------
//
TBool CNSmlDMProgressDialog2::OkToExitL(TInt aButtonId)
    {
    if ( iCloseEventReceived )
    {
    	if (iCallback && !iClosed)
   	    	{
   	    	TRAP_IGNORE(iCallback->DialogDismissedL(EKeyPhoneEnd));
   	    	STATIC_CAST( CNSmlDMSyncAppUi*, iEikonEnv->EikAppUi() )->ExitCallL();	
   	    	FLOG("CNSmlDMProgressDialog2::OkToExitL: close handling done");
  	    	}
   		    
        if (iAvkonAppUi)
            {
            // prevent further close events
            iAvkonAppUi->RemoveFromStack(this);
            }
            
        FLOG( "CNSmlDMProgressDialog2::OkToExitL END" );

        iClosed = ETrue;
        return ETrue;
    }
    
    if (aButtonId == EEikBidCancel && iCallback)
        {
        iCallback->DialogDismissedL(aButtonId);
		return EFalse; // leave dialog open - let observer close it
		}

    return CAknProgressDialog::OkToExitL(aButtonId);
    }
    
// -----------------------------------------------------------------------------
// CNSmlDMProgressDialog2::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CNSmlDMProgressDialog2::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
	{
	FLOG("CNSmlDMProgressDialog2::OfferKeyEventL START");
		
	if (aType == EEventKey)
		{
		switch (aKeyEvent.iCode)
			{

            case EKeyEscape:  // iCode 27 (0x1b)
            	{
            	iCloseEventReceived = ETrue;
            	FLOG( "CNSmlDMProgressDialog2::OfferKeyEventL: EKeyEscape received" );
            	break;
            	}
            	
            case EKeyPhoneEnd:
             	{
             	iCloseEventReceived = ETrue;
                FLOG( "CNSmlDMProgressDialog2::OfferKeyEventL: EKeyPhoneEnd received" );
                break;
               	}

            default:
                FLOG( "event key code is %d" );        
			    break;
			}
		}


    FLOG( "CNSmlDMProgressDialog2::OfferKeyEventL END" );
    
	return CAknProgressDialog::OfferKeyEventL(aKeyEvent, aType);
	}

// -----------------------------------------------------------------------------
// CNSmlDMProgressDialog2::RunLD
// -----------------------------------------------------------------------------
//
TInt CNSmlDMProgressDialog2::RunLD()
    {
    CNSmlDMSyncAppUi* appUi = STATIC_CAST( CNSmlDMSyncAppUi*, iEikonEnv->EikAppUi());
	CNSmlDMSyncDocument* doc = STATIC_CAST( CNSmlDMSyncDocument*, appUi->Document() );
    if ( doc->SANSupport() && doc->SilentSession() ) //to suppress backlight
        {
        return CEikDialog::RunLD();	
    	}
    
    else //for User initated and all alerts except silent 
    	{
        return CAknNoteDialog::RunLD();		
    	}
    
    }
    
// -----------------------------------------------------------------------------
// CNSmlDMProgressDialog2::ProcessFinishedL
// -----------------------------------------------------------------------------
//    
 void CNSmlDMProgressDialog2::ProcessFinishedL()
    { 
    CNSmlDMSyncAppUi* appUi = STATIC_CAST( CNSmlDMSyncAppUi*, iEikonEnv->EikAppUi());
	CNSmlDMSyncDocument* doc = STATIC_CAST( CNSmlDMSyncDocument*, appUi->Document() );	       
     if ( doc->SANSupport() && doc->SilentSession() ) //to suppress backlight
      {
        switch ( iState )
        {
        case EProcessOnDisplayOff:
        case EProcessOnDisplayOnCanBeDismissed:
            iState = EProcessOffDisplayOff;
            delete iProgressDialogTimer;
            iProgressDialogTimer = NULL;
            //ReportUserActivity();
            //PlayTone();
            TryExitL( EAknSoftkeyDone );
            break;
        case EProcessOnDisplayOn:
            iState = EProcessOffDisplayOn;
            break;
		default:
			break;
        }
      }
     else //for User initated and all alerts except silent 
      {
      CAknProgressDialog::ProcessFinishedL();
      } 
    }
//  End of File  
