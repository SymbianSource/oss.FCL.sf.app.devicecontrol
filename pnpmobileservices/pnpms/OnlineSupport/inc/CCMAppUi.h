/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file for Ui class
*
*/



#ifndef CCMAPPUI_H
#define CCMAPPUI_H

//  INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknappui.h>
#include <FavouritesLimits.h>   // for KFavouritesMaxUrl
#include <sysutil.h>
#include <ApDataHandler.h>      // for creating new Init Access
#include <ApAccessPointItem.h>  // for creating new Init Access

#include "MCMSocketsEngineNotifier.h"
#include "MCMRegisterationStatusNotifier.h"
#include "MiniBrowserObserver.h"
#include "mcmdialogstatusnotifier.h"
#include "ccmdialogmonitor.h"


// CONSTANTS
//Maximun length of WAP address
const TInt KMaximumLengthOfWapAddress = KFavouritesMaxUrl;
const TInt KMaxServerString = 20;
const TInt KMaxPageString = 20;
const TInt KMaxQueryString = 256;

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CRepository;
class CCMRegistrationMonitor;
class CCMSocketsEngine;
class CMiniBrowser;
class CCMDialogMonitor;
class CAknGlobalConfirmationQuery;
class CAknGlobalNote;
// CLASS DECLARATION

/**
*  CCMAppUi application class.
*
*  @lib ConfManager.app
*  @since 2.0
*/
class CCMAppUi : public CAknAppUi,
                    /*public MProgressDialogCallback,*/ 
                    public MCMSocketsEngineNotifier,
                    public MCMRegisterationStatusNotifier,
                    public MCoeForegroundObserver,
                    public MMiniBrowserObserver,
                    public MCMDialogStatusNotifier
    {
    public:  // Constructors and destructor
        CCMAppUi();
             
        /**
        * Destructor.
        */      
        ~CCMAppUi();

    public: // New methods

        /**
        * ApplicationStatus used when the timer expires
        */
        enum TApplicationStatus 
            {
            EAppStatusMonitoring,
            EAppStatusConnecting,
            EAppStatusWaitingDisconnect,
            EAppStatusInactive,
            EAppStatusStartingUp,
            /*(if the app is closed immediately, it crashes after calling the destructor.
            Seems like CAknMessageQueryDialog releases some of it's resources after some
            kind of delay and this causes the crash)
            */
            EAppStatusClosing
            };

        /**
        * ApplicationStatus used when the timer expires
        */
        enum TConnectingVia 
            {
            EConnectingViaNull, // Initial state, no connection has been tried yet
            EConnectingViaInit, // Trying to connect via Init Access
            EConnectingViaDefault, // Trying to connect via Browser default AP
            EConnectingViaUser // Trying to connect via AP asked from user
            };


        /**
        * ConnectReason used to inform help-portal
        */
        enum TConnectReason
            {
            EConnectReasonUnknown               = 0,
            EConnectReasonFirstBoot             = 1,
            EConnectReasonNewSim                = 2,
            EConnectReasonUserTriggered         = 3,
            EConnectReasonApplicationTriggered  = 4,
            EConnectReasonRemoteTriggered       = 5
            };
        
        /**
        * Activates confirmation dialog
        * @since 2.0
        */
        void ConfirmationDialogL();

        /**
        * Activates error note
        * @since 2.0
        * @return void
        */
        void ErrorNoteConnectionL();

        /**
        * Activates error note, displaying the error value
        * @since 2.0
        * @param aError errorvalue, will displayed in the note
        * @return void
        */
        void ErrorNoteUnknownL( TInt aError );

        /**
        * StartUp starting sequence.
        * @since 2.0
        * @return errorcode.
        */
        TInt StartUpStartL();

        /**
        * Checks if there is accesspoint available
        * @since 2.0
        * @return errorcode
        */ 
        TInt CheckSettingsL();

        /**
        * opens wap browser
        * @since 2.0
        * @return void
        */ 
        void OpenWapBrowserL( );

        /**
        * Checks if there is defaul accesspoint available
        * @since 2.0
        * @returns ID of the accesspoint or KErrNotFound
        */ 
        TUint32 CheckDefaultAccessPointL();

        /**
        * Checks if there is init accesspoint available
        * @since 2.0
        * @returns ID of the accesspoint or KErrNotFound
        */ 
        TUint32 InitAccessPointL();

        /**
        * Checks the settings of init access point
        * @since 2.0
        * @param aDb Commsdb connection
        * @param aAccessPoint id of the AP to be checked
        * @returns ETrue if the AP was ok, EFalse if not
        */ 
        TBool CheckInitAccessPointL( CCommsDatabase& aDb, TUint32& aAccessPoint );
        
        /**
        * Creates the init accesspoint
        * @since 2.0
        * @param aDb Commsdb connection
        * @returns ID of the accesspoint or KErrNotFound
        */ 
        TUint32 CreateInitAccessPointL( CCommsDatabase& aDb );

        /**
        * Initiates next test
        * @since 2.0
        * @returns void
        */ 
        void InitiateNextConnectionTestL();

        /**
        * Checks if init access is ok in the comms db
        *   if not creates one and cheks again
        *   if init access AP is ok initiates the init access test
        *   otherwise returns EFalse
        * @since 2.0
        * @returns ETrue if connection test was initiated, EFalse if not
        */ 
        TBool TryInitAccessL();

        /**
        * Checks if browser default access is ok in the comms db
        *   if so initiates the default access test
        *   otherwise returns EFalse
        * @since 2.0
        * @returns ETrue if connection test was initiated or EFalse
        */ 
        TBool TryDefaultAccessL();

        /**
        * Prompts the user to select an ap to test
        * @since 2.0
        * @returns ETrue if connection test was initiated or EFalse
        */ 
        TBool TryUserAccessL();

        /**
        * Reads the accesspoint id from the preferences table and changes
        * iAccessPoint to refer to the id in the WAP_ACCESS_POINT table.
        * @since 2.0
        * @returns ID of the accesspoint or KErrNotFound
        */ 
        TInt ReadAccessPointFromPreferencesL();

        /**
        * function checks the settings in the ini file
        * @since 2.0
        * @return void
        */ 
        void CheckIniFileSettingsL();

        /**
        * Shows wait Dialog
        * @since 2.0
        * @return void
        */ 
        void ShowWaitDialogL();

        /**
        * Starts the real functionality of the configuration Manager
        * @since 2.0
        * @return void
        */ 
        void StartUpOperationL();

        /**
        * Sets server string (http://<server>.help-portal.com)
        * @param aServer the server string, max length KMaxServerString
        * @since 2.0
        * @return void
        */ 
        void SetServerString(const TDesC& aServer);

        /**
        * Sets page string (http://server.help-portal.com/<page>)
        * @param aPage the page string, max length KMaxPageString
        * @since 2.0
        * @return void
        */ 
        void SetPageString(const TDesC& aPage);

        /**
        * Sets connect reason 
        * @param aReason the connect reason
        * @since 2.0
        * @return void
        */ 
        void SetConnectReason(TConnectReason aReason);

        /**
        * Sets query string 
        * @param aQuery the query string, max length KMaxQueryString
        * @since 2.0
        * @return void
        */ 
        void SetQueryString(const TDesC& aQuery);

        /**
        * Sets iAtBoot
        * @param aAtBoot if ETrue indicates that Support is started as part
        *                of boot process
        * @return voi
        */
        void SetAtBoot( TBool aAtBoot);

        /**
        * Fetch product model and sw version
        * @since 2.0
        * @return void
        */ 
        void FetchProductModelAndSwVersionL();


        /**
        * Exit application. The implementation depends on the platform.
        * On 3.0 a short delay is needed on exit because dialogs do not release
        * all resources when ExecuteLD function completes. The delay is needed
        * since the app panics on exit if all resources has not been freed.
        */ 
        void DelayedExit();

    public: //Functions from base classes

        /**
        * TimerExpired function was replaced by TimeOutCallBack
        * after replacing class CCMTimer with CPeriodic. Calls DoTimeOutCallBack.
        * @return TInt KErrNone in case everything was Ok.
        */
        static TInt TimeOutCallBack( TAny* aCCMAppUi );

        /**
        * The functionality from TimerExpired function was moved here
        */
        void DoTimeOutCallBackL();

        /**
        Starts a time-out period of aTimeOut micro seconds. Calls TimeOutCallBack
        after that.
        */
        void StartTimeOut( const TInt aTimeOut );

        /**
        * From MCMSocketsEngineNotifier notifies the connection result
        * @since 2.0
        * @param aErrorCode error code of the connection test
        * @return void
        */    
        void ConnectionResultL( TInt aErrorCode );
        
        /**
        * A method for CCMSocketEngine to tell that a unhandled
        * leave has occurred during it's RunL handling.
        * Displays a Unknown Error note
        * @param aErrorCode the errorcode of the unhandled leave
        * @return voi
        */
        void UnhandledLeave( TInt aErrorCode );

        /**
        * From MCMRegisterationStatusNotifier
        * @param aNetworkStatus
        */
        void RegistrationMonitoringDoneL( CCMRegistrationMonitor::TNetworkStatus aNetworkStatus );

        /**
        * From MCMRegisterationStatusNotifier
        * @param aStatus 
        * @param aError
        */
        void RegistrationReportErrorL( const CCMRegistrationMonitor::TMonitoringStatus aStatus, const TInt aError );

        /**
        * From CEikAppui
        * Handles commandline params
        * @since 2.0
        * @param aCommand see sdk help
        * @param aDocumentName see sdk help
        * @return void
        */    
        TBool ProcessCommandParametersL( TApaCommand aCommand, TFileName& aDocumentName );

        /**
        * From CEikAppui
        * Handles commandline params
        * @since 2.0
        * @param aCommand see sdk help
        * @param aDocumentName see sdk help
        * @param aTail see sdk help
        * @return void
        */    
        TBool ProcessCommandParametersL( TApaCommand aCommand, TFileName& aDocumentName, const TDesC8& aTail );

        /**
        * takes care of wait dialog command handling.
        * @since 2.0
        * @param aCommand command
        * @return void
        */
        void WaitDialogDismissedL( TInt aButtonId );

    public: // from MMiniBrowserObserver

        /**
        Called after MiniBrowser has handled a request for an URL
        (a call to HttpGetL successfully done)
        */      
        void MiniBrowserRequestDoneL();

        /**
        Called if RunL of CMiniBrowser leaves.
        @param aError The leave code.
        */      
        void MiniBrowserErrorL( const TInt aError );

        /**
        Called if a call to MiniBrowserRequestDoneL or MiniBrowserErrorL leaves
        @param aError The leave code
        */      
        void MiniBrowserUnhandledLeave( const TInt aError );

    public: // from MCoeForegroundObserver
        void HandleGainingForeground();
        void HandleLosingForeground();
        
    public: // from MCMDialogStatusNotifier
        void CommonDialogDismissedL( const CCMDialogMonitor::TDialogMode aDialog, const TInt aStatusCode );

    private: // Functions from base classes
        

        /**
        * Checks if network connections are allowed. The current implementation
        * checks the value of KCoreAppUIsNetworkConnectionAllowed central 
        * repository key in KCRUidCoreApplicationUIs.
        * @return TBool ETrue if network connections are allowed
        */    
        TBool IsNetworkConnectionsAllowedL();

        /**
        * From MEikMenuObserver takes care of menu handling.
        * @since 2.0
        * @param aResourceId resourceid
        * @param aMenuPane menu
        * @return void
        */    
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
        * From CEikAppUi takes care of command handling.
        * @since 2.0
        * @param aCommand command
        * @return void
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From CEikAppUi handles key events.
        * @since 1.0
        * @param aKeyEvent key event
        * @param aType event type code
        * @return Response code (EKeyWasConsumed, EKeyWasNotConsumed).
        */
        virtual TKeyResponse HandleKeyEventL
            (
            const TKeyEvent& aKeyEvent, TEventCode aType 
            );

        /**
        For storing an integer value.
        */
        TInt StoreIntValue( const TUint32 aCrKey, const TInt aValue );

        /**
        For storing a string value.
        */
        TInt StoreStringValue( const TUint32 aCrKey, const TDesC& aValue );

        /**
        For restoring an integer value that was stored by using StoreIntValue.
        */
        TInt GetIntValue( const TUint32 aCrKey, TInt& aValue );

        /**
        For restoring a string value that was stored by using StoreStringValue.
        */
        TInt GetStringValue( const TUint32 aCrKey, TDes& aValue );

        /**
        Appends the domain to be used for redirecting browser (for example
        www.help-portal.com)
        @param aUrl The descriptor the domain is appended to.
        */
        void AppendDomainL( TDes& aUrl );
        
        /**
        Clear Service Activation Information if there are any, online support should not read
        */
        void DeleteServiceActivationInfoL();
        
        /**
        Append Token And Nonce Value to WAPPage
        */
        void AppendTokenAndNonceValL();
        
        /**
        Append Device Information to URL
        */
        void AppendDeviceInfoL();
        
        /**
        Create Minibrowser and Launch
        */
        void LaunchMiniBrowserL();
        
        /*
        Launch s60 Web browser using command line parameters
        */
        void LaunchS60WebBrowserL();
        
        /*
        Append parameters to URL required from Server
        */
        void AppendWebUtilParamsL(); 
        
        /*
        Create Online Help for settings based on Autostart value
        Can be either Silent or Non Silent.
        In case of Silent settings are downloaded settings silently
        connecting to browser in background
        In case of Non-Silent Browser will be opened with display of
        webpage where user can select settings based on his wish
        */
        void OnlineHelpL();

    private:
        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();

    private: //Data
        
        // RegistrarionMonitor
        CCMRegistrationMonitor* iRegistrationMonitor;
        // Sockets engine
        CCMSocketsEngine* iEngine;
        // Timer
        CPeriodic* iTimer;
        // WAP page address
        TBuf <KMaximumLengthOfWapAddress> iWapPage;
        // if this is true the application has changed the preferences
        TBool iPreferencesChanged;
        // If this is true the phone is in homenetwork
        TBool iNetworkStatusOk;
        // This is set to true if the phone has been started before while it 
        //hasn't been registered to it's homenetwork and now the phone has 
        //been started in the homenetework.
        TBool iFirstStartInHomeNetwork;
        // if this is true the application knows that the confirmation dialog will be showed
        TBool iShowConfirmation;
        /** Id for the current AccessPoint this id refers to the WAP_ACCESS_POINT table. */
        TUint32 iAccessPoint;
        // Application status
        TApplicationStatus iApplicationStatus;
        // This is for the initAccess
        TBool iInitAccessPointUsed;

        // Connection reason (EConnectReasonUnknown if not started with parameters)
        TConnectReason iConnectReason;
        // Page string (http://<server>.help-portal.com/)
        TBuf<KMaxServerString> iServer;
        // Page string (http://server.help-portal.com/<page>)
        TBuf<KMaxPageString> iPage;
        // SW version
        TBuf<KSysUtilVersionTextLength>    iSwVersion;
        // Product model
        TBuf<KSysUtilVersionTextLength>    iProductModel;
        // Query string
        TBuf<KMaxQueryString> iQuery;        

        // Tells by which AP the connection is tried, so that the state machine in
        //   TimerExpired can try the next AP if the current did not work
        TConnectingVia iConnectingVia;
        // The number to show in wait dialog
        TInt iWaitDialogProgressVal;
        
        // Last configured IMSI from ini file
        RMobilePhone::TMobilePhoneSubscriberId iLastConfIMSI;

        CCMRegistrationMonitor::TNetworkStatus iNetworkStatus;

        /** Tells if the application was started in auto start (quiet) mode */
        TBool iAutoStart;
        /** Tells if we should override need for initAccess in order to use TOKEN (TokenOverride).
        set to ETrue to append TOKEN and tValid parameters even if not using initAccess */
        TBool iTokenOverride;
        /** Tells if we started Browser (needed in considering whether a long exit delay is needed) */
        TBool iStartingBrowser;

        CRepository* iRepository;
        
        /*!
        Note that iMiniBrowser must have an opened RSocketServ handle and an
        opened and started RConnection handle until it is destroyed. This means
        that it has to be deleted after iEngine (iMiniBrowser uses the RSocketServ
        and RConnection handles of iEngine).
        */
        CMiniBrowser* iMiniBrowser;

        CCMDialogMonitor* iCommonDialogMonitor;
        /*!
        To prevent infinite redirects in silent mode
        */
        TInt iRedirectsLeft;
        
        CAknGlobalConfirmationQuery* iGlobalConfirmationQuery;
        CAknGlobalNote* iGlobalWaitNote;
        TInt iWaitNoteId;
        TInt iInitAccessOn;
    };

#endif  // CCMAPPUI_H

// End of File
