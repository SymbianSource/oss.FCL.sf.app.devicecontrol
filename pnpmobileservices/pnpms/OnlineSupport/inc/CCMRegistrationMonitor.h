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
* Description:  Header file for the registeration monitor class, This class is 
*                is used to monitor the registeration status between the phone 
*                and network.
*
*/



#ifndef CCMREGISTRATIONMONITOR_H
#define CCMREGISTRATIONMONITOR_H

//  INCLUDES
#include <e32base.h>
#include <etelmm.h>

// CONSTANTS
//None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class MCMRegisterationStatusNotifier;

// CLASS DECLARATION

/**
*  CCMRegistrationMonitor.
*  Monitors the registeration status of the phone
*
*  @lib Confmanager.app
*  @since 2.0
*/
class CCMRegistrationMonitor : public CActive
    {
    public:  // Constructors and destructor
        /**
        * Registeration status.
        */
        enum TNetworkStatus 
            {
            EStatusUnknown = 0,
            EHomenetwork,
            ERoaming,
            ERequestCancelled
            };

        enum TMonitoringStatus
            {
            EInactive = 0,
            ERequestingNetworkStatus,
            EWaitingForChangesInNetworkStatus,
            ERequestingServiceProviderName,
            ERequestingHomeNetworkInfo,
            ERequestingCurrentNetworkInfo,
            ERequestingOwnNumberInfo,
            ERequestingPhoneIdentity,
            ERequestingIMSI,
            EDone
            };

        /**
        * Two-phased constructor.
        */
        static CCMRegistrationMonitor* NewL( MCMRegisterationStatusNotifier& aNotifier );
        
        /**
        * Destructor.
        */
        ~CCMRegistrationMonitor();

    public: // New functions
            

        /**
        * StartMonitoring.
        * @since 2.0
        * @return void
        */
        void StartMonitoring( );
        
        /**
        * StopMonitoring.
        * @since 2.0
        * @return void
        */
        void StopMonitoring( );

    public: // Functions from base classes
        
    protected:  // New functions

    protected:  // Functions from base classes

    private: // Functions from base classes
        
        /**
        * From CActive Handles an active object’s request completion event.
        * @since 2.0
        * @return void
        */
        void RunL();
        
        /**
        * From CActive Cancels any outstanding request.
        * @since 2.0
        * @return void
        */
        void DoCancel();

        /**
        * From CActive Handles a leave occurring in the request completion event handler RunL().
        */
        TInt RunError( TInt aError );
    private:

        /**
        * C++ default constructor.
        */
        CCMRegistrationMonitor( MCMRegisterationStatusNotifier& aNotifier );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    
    public:     // Data
        TBool IsReady(){ return iMonitoringStatus == EDone; };
        RMobilePhone::TMobilePhoneNetworkInfoV1 GetHomeNetworkInfo(){ return iHomeNetworkInfo; };
        RMobilePhone::TMobilePhoneNetworkInfoV1 GetCurrentNetworkInfo(){ return iCurrentNetworkInfo; };
        RMobileONStore::TMobileONEntryV1 GetOwnNumberInfo(){ return iOwnNumberInfo; };
        RMobilePhone::TMobilePhoneSubscriberId GetImsi(){ return iIMSI; };
        RMobilePhone::TMobilePhoneServiceProviderNameV2 GetServiceProviderName(){ return iServiceProviderName; };
        RMobilePhone::TMobilePhoneIdentityV1 GetPhoneIdentity(){ return iPhoneIdentity; };
    protected:  // Data        

    private:    // Data
        TMonitoringStatus iMonitoringStatus;
        MCMRegisterationStatusNotifier& iNotifier;

        // Telephony Server
        RTelServer iServer;
        // Phone Object
        RMobilePhone iPhone;
        // Network registeration status
        RMobilePhone::TMobilePhoneRegistrationStatus iRegisterationStatus;
        // Phone info object
        RTelServer::TPhoneInfo iPhoneInfo;
        // Time when the monitor is started
        TTime iStartTime;
        // Current network info packaged
        RMobilePhone::TMobilePhoneNetworkInfoV1 iCurrentNetworkInfo;
        RMobilePhone::TMobilePhoneNetworkInfoV1Pckg iCurrentNetworkInfoPckg;
        // Home network info packaged
        RMobilePhone::TMobilePhoneNetworkInfoV1 iHomeNetworkInfo;
        RMobilePhone::TMobilePhoneNetworkInfoV1Pckg iHomeNetworkInfoPckg;
        RMobileONStore iONStore;
        // Own number 
        RMobileONStore::TMobileONEntryV1 iOwnNumberInfo;
        RMobileONStore::TMobileONEntryV1Pckg iOwnNumberInfoPckg;
        // IMSI
        RMobilePhone::TMobilePhoneSubscriberId iIMSI;
        // sp name
        RMobilePhone::TMobilePhoneServiceProviderNameV2 iServiceProviderName;
        RMobilePhone::TMobilePhoneServiceProviderNameV2Pckg iServiceProviderNamePckg;

        // to get IMEI
        RMobilePhone::TMobilePhoneIdentityV1 iPhoneIdentity;

    public:     // Friend classes
        
    protected:  // Friend classes
        
    private:    // Friend classes

    };

#endif      // CCMREGISTRATIONMONITOR_H   
            
// End of File
