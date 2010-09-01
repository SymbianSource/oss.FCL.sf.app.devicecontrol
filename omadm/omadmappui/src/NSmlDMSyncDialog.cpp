/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Methods for Sync Log handler class
*
*/


// INCLUDE FILES

#include "NSmlDMSyncDialog.h"
#include "NSmlDMSyncUtil.h"
#include <nsmldmsync.rsg>
#include "NSmlDMSyncProfile.h"
#include "NSmlDMSyncDebug.h"

#include <aknPopup.h>
#include <aknlists.h>
#include <StringLoader.h>

// -----------------------------------------------------------------------------
// ShowDialogL
// -----------------------------------------------------------------------------
//
TBool CNSmlDMSyncDialog::ShowDialogL( TInt aProfileId,
                                      RSyncMLSession* aSyncSession )
	{
	CNSmlDMSyncDialog* dialog = CNSmlDMSyncDialog::NewL( aProfileId,
	                                                     aSyncSession );
	CleanupStack::PushL( dialog );
	dialog->PopupLogViewL();
	CleanupStack::PopAndDestroy( dialog );
    return ETrue;
	}

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
CNSmlDMSyncDialog* CNSmlDMSyncDialog::NewL( TInt aProfileId,
                                            RSyncMLSession* aSyncSession )
    {
    CNSmlDMSyncDialog* self = new ( ELeave ) CNSmlDMSyncDialog( aProfileId,
                                                                aSyncSession );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return(self);
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncDialog
// -----------------------------------------------------------------------------
//
CNSmlDMSyncDialog::CNSmlDMSyncDialog( TInt aProfileId,
                                      RSyncMLSession* aSyncSession )
	{
    iProfileId = aProfileId;
	iSyncSession = aSyncSession;

	__ASSERT_DEBUG(iSyncSession, TUtil::Panic(KErrArgument));
    }

// -----------------------------------------------------------------------------
// Symbian OS default constructor can leave.
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncDialog::ConstructL()
    {
    FLOG( "CNSmlDMSyncDialog::ConstructL()" );
    
    iLogItems = CEikonEnv::Static()->ReadDesCArrayResourceL( R_SML_LOG_ITEMS );

    iProfile = CNSmlDMSyncProfile::NewL( KUidSmlSyncApp.iUid, iSyncSession );
   	iProfile->OpenL( iProfileId, ESmlOpenReadWrite );
   	
	iHistoryJob = iProfile->LatestHistoryJob();
   	if ( !iHistoryJob )
   		{
   		FLOG( "CNSmlDMSyncDialog::ConstructL() KErrNotFound" );
   		User::Leave( KErrNotFound );
   		}
    } 

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CNSmlDMSyncDialog::~CNSmlDMSyncDialog()
    {
	delete iLogItems;
	delete iProfile;
    }

// -----------------------------------------------------------------------------
// PopupLogViewL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncDialog::PopupLogViewL()
	{
	FLOG( "CNSmlDMSyncDialog::PopupLogViewL()" );
	
	CAknSingleHeadingPopupMenuStyleListBox* listBox = 
	                       new (ELeave) CAknSingleHeadingPopupMenuStyleListBox;
	CleanupStack::PushL( listBox );

	CAknPopupList* popupList = CAknPopupList::NewL( 
	                                            listBox,
	                                            R_AVKON_SOFTKEYS_OK_EMPTY__OK,
	                                            AknPopupLayouts::EMenuWindow );
	CleanupStack::PushL( popupList );

	listBox->ConstructL( popupList, EAknListBoxViewerFlags );
	
	CDesCArray* array = ( CDesCArray* )listBox->Model()->ItemTextArray();
	listBox->CreateScrollBarFrameL( ETrue );
	listBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
	                                               CEikScrollBarFrame::EOff,
	                                               CEikScrollBarFrame::EAuto );

	// read synclog and trap error if there aren't any content types
	TRAP_IGNORE( FormatLogViewListL( array ) );

	FLOG( "CNSmlDMSyncDialog::PopupLogViewL() set title" );
	
    HBufC* titleText = StringLoader::LoadLC( R_SML_LOG_SYNCLOG_TITLE );
    popupList->SetTitleL( titleText->Des() );
    CleanupStack::PopAndDestroy( titleText );
    popupList->ExecuteLD();
	
	CleanupStack::Pop( popupList );
	CleanupStack::PopAndDestroy( listBox );
	
	FLOG( "CNSmlDMSyncDialog::PopupLogViewL() Done" );
	}

// -----------------------------------------------------------------------------
// FormatLogViewListL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncDialog::FormatLogViewListL( CDesCArray* aArray )
	{
	FLOG( "CNSmlDMSyncDialog::FormatLogViewListL()" );
	
	TBuf<KBufSize> buf( KNullDesC );
	TInt num = KErrNotFound;

	aArray->Reset();
	TBuf<KBufSize256> tempBuf;
	
	tempBuf.Append( (*iLogItems)[EHeadingProfile] );	//profile name heading
	tempBuf.Append( KNSmlTab );
	FLOG( "CNSmlDMSyncDialog::FormatLogViewListL() get name" );
	iProfile->GetName( buf );
	tempBuf.Append( buf );
	AknTextUtils::DisplayTextLanguageSpecificNumberConversion( tempBuf );
	aArray->AppendL( tempBuf );
    
	tempBuf.Zero();
	tempBuf.Append( (*iLogItems)[EHeadingServer] );  // server name heading
	tempBuf.Append( KNSmlTab );

	iProfile->GetHostAddress( buf, num );
	tempBuf.Append( buf );
	aArray->AppendL( tempBuf );
	
	TTime time = iHistoryJob->TimeStamp();
	
	TUtil::GetDateTextL(buf, time);
	tempBuf.Zero();
	tempBuf.Append( (*iLogItems)[EHeadingDate] );	// synchronisation date heading
	tempBuf.Append( KNSmlTab );
	tempBuf.Append( buf );
	AknTextUtils::DisplayTextLanguageSpecificNumberConversion(tempBuf);
	aArray->AppendL(tempBuf);

    TUtil::GetTimeTextL(buf, time);

	tempBuf.Zero();
	tempBuf.Append((*iLogItems)[EHeadingTime]);	// synchronisation time heading
	tempBuf.Append(KNSmlTab);
	tempBuf.Append(buf);

	AknTextUtils::DisplayTextLanguageSpecificNumberConversion( tempBuf );
	aArray->AppendL(tempBuf);

	// check sync error
	TInt status = iHistoryJob->ResultCode();
	TInt syncTaskError = KErrNone;
	TInt count = iHistoryJob->TaskCount();
	for (TInt i=0; i<count; i++)
		{
		const CSyncMLHistoryJob::TTaskInfo& taskInfo = iHistoryJob->TaskAt(i);
		if (taskInfo.iError != KErrNone)
			{
			syncTaskError = taskInfo.iError;
		    }
		}
    iProfile->CloseHistoryLog();
    
	tempBuf.Zero();
	tempBuf.Append((*iLogItems)[EHeadingStatus]); // status heading
	tempBuf.Append(KNSmlTab);	
		
	if ( status != KErrNone )
		{
		if ( status == KErrCancel )
			{
			ReadL( buf, R_QTN_SML_LOG_CANCELLED );
			}
		else
			{
			ReadL( buf, R_QTN_SML_LOG_ERR );
			}
		tempBuf.Append( buf );
		aArray->AppendL( tempBuf );
		return;
		}
	else if ( syncTaskError != KErrNone )
		{
        ReadL( buf, R_QTN_SML_LOG_ERR );
		tempBuf.Append( buf);
		aArray->AppendL( tempBuf );
		}
    else
    	{
        ReadL( buf, R_QTN_SML_LOG_OK );
	    tempBuf.Append( buf );
	    aArray->AppendL( tempBuf );
    	}
	
	FLOG( "CNSmlDMSyncDialog::FormatLogViewListL() Done" );
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMSyncDialog::ReadL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncDialog::ReadL( TDes& aText, TInt aResourceId )
    {
    FLOG( "CNSmlDMSyncDialog::ReadL()" );
    
    HBufC* buf = StringLoader::LoadLC( aResourceId );
    TUtil::StrCopy( aText, *buf );
    CleanupStack::PopAndDestroy( buf );
    }

//  End of File  
