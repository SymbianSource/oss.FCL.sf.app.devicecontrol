/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Contains the implementation of dialog monitor
*
*/



// INCLUDE FILES
//#include    <basched.h>
#include    "PnpProvisioningSaver.h"
#include    "cwaitdialogmonitor.h"
#include    "mwaitdialogstatusnotifier.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// PhoneModule name


// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS ===============================
// None

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWaitDialogMonitor::CWaitDialogMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//

CWaitDialogMonitor::CWaitDialogMonitor( MWaitDialogStatusNotifier& aNotifier ) :
    CActive( CActive::EPriorityStandard ),
    iNotifier( aNotifier )
    {
    }


// -----------------------------------------------------------------------------
// CWaitDialogMonitor::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWaitDialogMonitor::ConstructL()
    {
    CActiveScheduler::Add(this); 
    }

// -----------------------------------------------------------------------------
// CWaitDialogMonitor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWaitDialogMonitor* CWaitDialogMonitor::NewL( MWaitDialogStatusNotifier& aNotifier )
    {
    CWaitDialogMonitor* self = new( ELeave ) CWaitDialogMonitor( aNotifier );    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CWaitDialogMonitor::~CWaitDialogMonitor()
    {
    if( IsActive() )
        {
        Cancel();
        }
    }

void CWaitDialogMonitor::Start()
    {
    if( IsActive() )
        {
        Cancel();
        }
    SetActive();
    }

void CWaitDialogMonitor::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CWaitDialogMonitor::RunL()
// Handles object’s request completion event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CWaitDialogMonitor::RunL()
    {
    //LOGSTRING("Enter to CWaitDialogMonitor::RunL() ");
    //LOGSTRING2( "CWaitDialogMonitor status %i" , iStatus.Int() );

    iNotifier.WaitDialogDismissedL( iStatus.Int() );
    }

// ---------------------------------------------------------------------------
// CWaitDialogMonitor::RunError
// ---------------------------------------------------------------------------
//
TInt CWaitDialogMonitor::RunError( TInt /*aError*/ )
	{
//		LOGSTRING2( "CWaitDialogMonitor::RunError( %d )", aError )    
		return KErrNone;
	}
	
// ========================== OTHER EXPORTED FUNCTIONS =========================

// None

//  End of File  
