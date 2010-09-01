/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Multiquery for user name and password
*
*/


// INCLUDE FILES
#include "NSmlDMAuthQueryDlg.h"
#include "NSmlDMSyncProfile.h"
#include "NSmlDMSyncDebug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlDMMultiQuery::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNSmlDMMultiQuery* CNSmlDMMultiQuery::NewL( CNSmlDMSyncAppEngine* aAppEngine,
                                            TInt aProfileId,
                                            TBool aIsHttp )
	{
	CNSmlDMMultiQuery* self = new( ELeave ) CNSmlDMMultiQuery( aAppEngine,
	                                                           aProfileId,
	                                                           aIsHttp );
	CleanupStack::PushL( self );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CNSmlDMMultiQuery::CNSmlDMMultiQuery
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNSmlDMMultiQuery::CNSmlDMMultiQuery( CNSmlDMSyncAppEngine* aAppEngine,
                                      TInt aProfileId,
                                      TBool aIsHttp,
                                      TTone aTone /*= ENoTone*/ )
                                    : CAknMultiLineDataQueryDialog( aTone ),
                                      iAppEngine( aAppEngine ),
                                      iProfileId( aProfileId ),
                                      iIsHttp( aIsHttp )
	{
	}

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CNSmlDMMultiQuery::~CNSmlDMMultiQuery() 
	{ 
	delete iTimer;
	}	

// -----------------------------------------------------------------------------
// CNSmlDMMultiQuery::OkToExitL
// -----------------------------------------------------------------------------
//
TBool CNSmlDMMultiQuery::OkToExitL( TInt aButtonId )
	{
	FLOG( "[OMADM] CNSmlDMMultiQuery::OkToExitL" );

	if ( iTimer )
		{
		iTimer->Cancel();
		}

	TBool retValue = CAknMultiLineDataQueryDialog::OkToExitL( aButtonId );
	
	if ( aButtonId == EAknSoftkeySave )
		{
		CAknMultilineQueryControl* firstControl = FirstControl();
		firstControl->GetText(iUsername);

		CAknMultilineQueryControl* secondControl = SecondControl();
		secondControl->GetText(iPassword);
		
		CNSmlDMSyncProfile* profile = iAppEngine->OpenProfileL( iProfileId );
        if ( iIsHttp )
            {
            profile->SetHttpUserNameL( iUsername );
            profile->SetHttpPasswordL( iPassword );
            }
        else
            {   
		    profile->SetUserNameL( iUsername );
            profile->SetPasswordL( iPassword );
            }
		profile->SaveL();
        iAppEngine->CloseProfile();
		}
	return retValue;
	}

// -----------------------------------------------------------------------------
// CNSmlDMMultiQuery::PostLayoutDynInitL
// -----------------------------------------------------------------------------
//
void CNSmlDMMultiQuery::PostLayoutDynInitL()
	{
    FLOG( "[OMADM] CNSmlDMMultiQuery:" );

	iTimer = CNSmlDMQueryTimer::NewL( this );
	iTimer->After( NSmlDMAuthQueryShowTime * KMultiplier );	// iTime == microseconds
	}

// -----------------------------------------------------------------------------
// CNSmlDMMultiQuery::HandleTimerEndL
// -----------------------------------------------------------------------------
//
void CNSmlDMMultiQuery::HandleTimerEndL()
	{
    FLOG( "[OMADM] CNSmlDMMultiQuery:" );

	delete iTimer;
	iTimer = 0;
	TryExitL( EEikBidCancel );
	}

// -----------------------------------------------------------------------------
// CNSmlDMAlertTimer implementation
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CNSmlDMQueryTimer::CNSmlDMQueryTimer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNSmlDMQueryTimer::CNSmlDMQueryTimer() : CTimer( EPriorityStandard )
	{
	}

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CNSmlDMQueryTimer::~CNSmlDMQueryTimer()
	{
	Cancel();
	}

// -----------------------------------------------------------------------------
// CNSmlDMQueryTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNSmlDMQueryTimer* CNSmlDMQueryTimer::NewL( CNSmlDMMultiQuery* aAlertDialog )
	{
	CNSmlDMQueryTimer* self = new (ELeave) CNSmlDMQueryTimer();
	CleanupStack::PushL(self);
	self->ConstructL( aAlertDialog );
	CleanupStack::Pop(); // self
	return self;
	}

// -----------------------------------------------------------------------------
// CNSmlDMQueryTimer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNSmlDMQueryTimer::ConstructL( CNSmlDMMultiQuery* aAlertDialog )
	{
    FLOG( "[OMADM] CNSmlDMQueryTimer: ConstructL" );

	iAlertDialog = aAlertDialog;
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMQueryTimer::RunL
// -----------------------------------------------------------------------------
//
void CNSmlDMQueryTimer::RunL()
	{
    FLOG( "[OMADM] CNSmlDMQueryTimer:" );

	iAlertDialog->HandleTimerEndL();
	}

//  End of File
