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
#include    "OnlineSupportLogger.h"
#include    "ccmdialogmonitor.h"
#include    "mcmdialogstatusnotifier.h"

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
// CCMDialogMonitor::CCMDialogMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//

CCMDialogMonitor::CCMDialogMonitor( MCMDialogStatusNotifier& aNotifier ) :
    CActive( CActive::EPriorityStandard ),
    iNotifier( aNotifier )
    {
    }


// -----------------------------------------------------------------------------
// CCMDialogMonitor::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCMDialogMonitor::ConstructL()
    {
    CActiveScheduler::Add(this); 
    }

// -----------------------------------------------------------------------------
// CCMDialogMonitor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCMDialogMonitor* CCMDialogMonitor::NewL( MCMDialogStatusNotifier& aNotifier )
    {
    CCMDialogMonitor* self = new( ELeave ) CCMDialogMonitor( aNotifier );    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CCMDialogMonitor::~CCMDialogMonitor()
    {
    if( IsActive() )
        {
        Cancel();
        }
    }
    
void CCMDialogMonitor::SetDialogMode(TDialogMode aMode)
    {
    iDialogMode = aMode;
    }

void CCMDialogMonitor::Start()
    {
    if( IsActive() )
        {
        Cancel();
        }
    SetActive();
    }

void CCMDialogMonitor::DoCancel()
    {
    }
    
//TInt CCMDialogMonitor::RunError( TInt aError )
//    {
//    return aError;
//    }

// -----------------------------------------------------------------------------
// CCMDialogMonitor::StartMonitoring()
// Starts monitoring of the network registeration status
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
void CCMDialogMonitor::StartMonitoring()
    {
    if( IsActive() )
        {
        Cancel();
        }
    iStartTime.HomeTime();
#ifdef __WINS__ // do not try to connect on the emulator
    iRegisterationStatus = RMobilePhone::ERegisteredOnHomeNetwork;
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
#else
    iPhone.GetNetworkRegistrationStatus( iStatus, iRegisterationStatus );
#endif
    iMonitoringStatus = ERequestingNetworkStatus;
    SetActive();
    }
*/
// -----------------------------------------------------------------------------
// CCMDialogMonitor::StopMonitoring()
// Stops the registeration monitor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
void CCMDialogMonitor::StopMonitoring()
    {
    LOGSTRING( "Enter to CCMDialogMonitor::StopMonitoring " );
    if( IsActive() )
        {
        Cancel();
        }
    LOGSTRING( "Exit from CCMDialogMonitor::StopMonitoring " );
    }
*/
// -----------------------------------------------------------------------------
// CCMDialogMonitor::DoCancel()
// Cancels the monitoring
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
void CCMDialogMonitor::DoCancel()
    {
    LOGSTRING("CCMDialogMonitor::DoCancel");
    switch( iMonitoringStatus )
        {
        case EInactive:
            User::Leave( KErrUnderflow );
            break;
        case ERequestingNetworkStatus:
            iPhone.CancelAsyncRequest( EMobilePhoneGetNetworkRegistrationStatus );
            iMonitoringStatus = EInactive;
            break;
        case EWaitingForChangesInNetworkStatus:
            iPhone.CancelAsyncRequest( EMobilePhoneNotifyNetworkRegistrationStatusChange );
            iMonitoringStatus = EInactive;
            break;
        case ERequestingServiceProviderName:
            iPhone.CancelAsyncRequest( EMobilePhoneGetServiceProviderName );
            iMonitoringStatus = EInactive;
            break;
        case ERequestingHomeNetworkInfo:
            iPhone.CancelAsyncRequest( EMobilePhoneGetHomeNetwork );
            iMonitoringStatus = EInactive;
            break;
        case ERequestingCurrentNetworkInfo:
            iPhone.CancelAsyncRequest( EMobilePhoneGetCurrentNetworkNoLocation );
            iMonitoringStatus = EInactive;
            break;
        case ERequestingOwnNumberInfo:
            iMonitoringStatus = EInactive;
            break;
        case ERequestingPhoneIdentity:
            iPhone.CancelAsyncRequest( EMobilePhoneGetPhoneId );
            iMonitoringStatus = EInactive;
            break;
        case ERequestingIMSI:
            iPhone.CancelAsyncRequest( EMobilePhoneGetSubscriberId );
            iMonitoringStatus = EInactive;
            break;
        default:
            User::Leave( KErrOverflow );
            break;
        }

    TInt err( KErrNone );
    TRAP( err, iNotifier.RegistrationReportErrorL( iMonitoringStatus, KErrCancel ) );
    LOGSTRING("Monitoring Cancelled");
    }
*/
/*
TInt CCMDialogMonitor::RunError( TInt aError )
    {
    LOGSTRING2( "CCMDialogMonitor::RunError %i", aError );
    TInt err( KErrNone );
    TRAP( err, iNotifier.RegistrationReportErrorL( iMonitoringStatus, aError ) );
    if( err == KLeaveExit )
        {
        User::Leave( KLeaveExit );
        }
    if( aError == KLeaveExit )
        {
        User::Leave( KLeaveExit );
        }
    LOGSTRING( "CCMDialogMonitor::RunError - done" );
    return KErrNone;
    }
*/
// -----------------------------------------------------------------------------
// CCMDialogMonitor::RunL()
// Handles object’s request completion event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCMDialogMonitor::RunL()
    {
    LOGSTRING("Enter to CCMDialogMonitor::RunL() ");
    LOGSTRING2( "CCMDialogMonitor status %i" , iStatus.Int() );

    iNotifier.CommonDialogDismissedL( iDialogMode, iStatus.Int() );
    /*
    switch( iMonitoringStatus )
        {
        case EInactive:
            User::Leave( KErrUnderflow );
            break;
        case ERequestingNetworkStatus:
        case EWaitingForChangesInNetworkStatus:
            {
            LOGSTRING( "ERequestingNetworkStatus | EWaitingForChangesInNetworkStatus" );
            switch( iRegisterationStatus )
                {
                // Not Ok cases
                case RMobilePhone::ERegistrationUnknown:
                case RMobilePhone::ENotRegisteredNoService:
                case RMobilePhone::ENotRegisteredEmergencyOnly:
                case RMobilePhone::ENotRegisteredSearching:
                case RMobilePhone::ERegisteredBusy:
                case RMobilePhone::ERegistrationDenied:
                    currentTime.HomeTime();
                    currentTime.SecondsFrom( iStartTime, interval );
                    seconds = interval.Int();
                    if ( seconds > KMaxInterval )
                        {
                        iMonitoringStatus = EInactive;
                        // Report error
                        iNotifier.RegistrationMonitoringDoneL( EStatusUnknown );
                        }
                    else
                        {
                        iMonitoringStatus = EWaitingForChangesInNetworkStatus;
                        iPhone.NotifyNetworkRegistrationStatusChange( iStatus, iRegisterationStatus );
                        LOGSTRING( "Network registration status change notification started " );
                        SetActive();
                        }
                    break;
                // These are Ok
                case RMobilePhone::ERegisteredOnHomeNetwork:
                case RMobilePhone::ERegisteredRoaming:
                    iMonitoringStatus = ERequestingServiceProviderName;
                    iPhone.GetServiceProviderName( iStatus, iServiceProviderNamePckg );
                    SetActive();
                    break;
                // Some mysterious error
                default:
                    LOGSTRING2( "Unknown network status code! %i", iRegisterationStatus );
                    LOGSTRING2( "ERegisteredOnHomeNetwork: %i", RMobilePhone::ERegisteredOnHomeNetwork );
                    User::Leave( KErrOverflow );
                    break;
                }
            break;
            }
        case ERequestingServiceProviderName:
            LOGSTRING( "ERequestingServiceProviderName" );
            iMonitoringStatus = ERequestingHomeNetworkInfo;
            iPhone.GetHomeNetwork( iStatus, iHomeNetworkInfoPckg );
            SetActive();
            break;
        case ERequestingHomeNetworkInfo:
            LOGSTRING( "ERequestingHomeNetworkInfo" );
            iMonitoringStatus = ERequestingCurrentNetworkInfo;

            // Use the override that does not need Location capability
            iPhone.GetCurrentNetwork(
                iStatus,
                iCurrentNetworkInfoPckg );
                
            SetActive();
            break;
        case ERequestingCurrentNetworkInfo:
            {
            LOGSTRING( "ERequestingCurrentNetworkInfo" );
            iMonitoringStatus = ERequestingOwnNumberInfo;

#ifdef __WINS__
            TRequestStatus* status = &iStatus;
            User::RequestComplete( status, KErrNone );
#else
            TInt err;
            err = iONStore.Open( iPhone );
            if (err != KErrNone) // error occured
                {
                LOGSTRING2( "iONStore.Open err: %i", err );
                User::Leave( err );
                }
            iOwnNumberInfo.iIndex = 0;
            iONStore.Read( iStatus, iOwnNumberInfoPckg );
#endif
            SetActive();
            }
            break;
        case ERequestingOwnNumberInfo:
            {
            LOGSTRING( "ERequestingOwnNumberInfo" );
            iMonitoringStatus = ERequestingPhoneIdentity;
#ifdef __WINS__
            TRequestStatus* status = &iStatus;
            User::RequestComplete( status, KErrNone );
#else
            iPhone.GetPhoneId( iStatus, iPhoneIdentity );
#endif
            SetActive();
            }
            break;
        case ERequestingPhoneIdentity:
            LOGSTRING( "ERequestingPhoneIdentity" );
            iMonitoringStatus = ERequestingIMSI;
            iPhone.GetSubscriberId( iStatus, iIMSI );
            SetActive();
            break;
        case ERequestingIMSI:
            LOGSTRING( "ERequestingIMSI" );
            iMonitoringStatus = EDone;
            switch( iRegisterationStatus )
                {
                case RMobilePhone::ERegisteredOnHomeNetwork:
                    iNotifier.RegistrationMonitoringDoneL( EHomenetwork );
                    break;
                case RMobilePhone::ERegisteredRoaming:
                    iNotifier.RegistrationMonitoringDoneL( ERoaming );
                    break;
                default:
                    iNotifier.RegistrationMonitoringDoneL( EStatusUnknown );
                    break;
                }
            break;
        default:
            LOGSTRING( "default" );
            User::Leave( KErrOverflow );
            break;
        }
    */
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// None

//  End of File  
