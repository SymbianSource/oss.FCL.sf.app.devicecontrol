/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Network coverage plug-in.
*
*/


#ifndef DIAGNETCOVERAGEPLUGIN_H
#define DIAGNETCOVERAGEPLUGIN_H

// INCLUDES
#include <etel3rdparty.h>            // CTelephony

#include <DiagTestPluginBase.h>      // CDiagTestPluginBase
#include <DiagResultsDatabaseItem.h> // CDiagResultsDatabaseItem::TResult
#include "diagnetcoverageplugin.hrh" // DIAG_NET_COVERAGE_PLUGIN_UID
#include <AknWaitDialog.h> 
//CONSTANTS

const TUid KDiagNetCoveragePluginUid = { DIAG_NET_COVERAGE_PLUGIN_UID };

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CPeriodic;
class CDiagResultsDbItemBuilder;


// CLASS DEFINITION
class CDiagNetCoveragePlugin : public CDiagTestPluginBase,
                               public MProgressDialogCallback
    {
    public: // Constructors and destructor
        /**
        * Symbian OS two-phased constructor
        * @return Net Coverage Plugin
        */
        static MDiagPlugin* NewL( TAny* aInitParams );

        /**
        * Destructor
        */
        virtual ~CDiagNetCoveragePlugin();
        
    private: // Constructors

        /**
        * C++ default constructor.
        */
        CDiagNetCoveragePlugin( CDiagPluginConstructionParam* aInitParams );

        /**
        * Symbian OS default constructor.
        *
        */
        void ConstructL();        

    private: // from CActive
        /**
        * Handle active object completion event
        * @see CActive
        */
        void RunL();
        
        /**
        * Handle RunL leaves
        */
        TInt RunError( TInt aError );
    
        /**
        * Handle cancel
        * @see CActive
        */
        void DoCancel();

    private: // From CDiagTestPluginBase
        
        /**
        * @see CDiagTestPluginBase::Visible()
        */
        virtual TBool IsVisible() const;

        /**
        * @see CDiagTestPluginBase::RunMode()
        */
        virtual TRunMode RunMode() const;

        /**
        * @see CDiagTestPluginBase::TotalSteps()
        */
        virtual TUint TotalSteps() const;

        /**
        * @see CDiagTestPluginBase::GetPluginNameL
        */
        virtual HBufC* GetPluginNameL( TNameLayoutType aLayoutType ) const;

        /**
        * @see CDiagTestPluginBase::Uid
        */
        virtual TUid Uid() const;                   

        /**
        * Handle plug-in execution.
        * @see CDiagTestPluginBase::DoRunTestL
        */
        void DoRunTestL();

        /**
        * Clean up after test is completed or stopped.
        * @see CDiagTestPluginBase::DoStopAndCleanupL
        */
        virtual void DoStopAndCleanupL();
        
        // ADO & Platformization Changes
        TBool ShowMessageQueryL( TInt aResourceId, TInt &aButtonId  );

    private:    // private data types
    
        // Internal asynchronous state
        enum TState
            {
            EStateUninitialized = 0,
            EStateOpeningIsc,
            EStateWaitForSendBandInfoRequest,
            EStateWaitForReceivebandInfoResponse,
            EStateWaitForBandType,
            EStateWaitForSignalStrength,
            EStateTestCancel,
            EStateTestCompleted
            };
            
        // Possible network types for signal strength measurement
        enum TNetworkType
            {
            //Type
            ETypeUnknown = -1,
            ETypeGSM,
            ETypeUMTS,
            ETypeBandCount
            };
            
        // Possible network bands for signal strength measurement            
        enum TNetworkBand
            {
            EBandUnknown = -1,
            EBand900_850,
            EBand1800_1900_2100
            }; 
            
        enum TNetworkCoveragePanicCodes
            {
            EPanicUnhandledLeave = 0
            };                                    

    private:    // internal API
        /**
        * Initiate asynchronous request to obtain band type
        */        
        void ObtainBandTypeAsyncL();
        
        /**
        * Read and store threshold values
        */              
        void ReadThresholdValuesL();  
                
        /**
        * Get network access from ETel 3rd party API
        */                              
        void CheckNetworkAccessL();
        
        /**
        * Get signal strength from ETel 3rd party API
        */                   
        void CheckSignalStrengthL();
        
        /**
        * Panic with string indicating net coverage plug-in and <aPanicCode> as
        * the panic code
        */                   
        void Panic(TInt aPanicCode);

		/**
		/* Wait Note
		/*
		*/
		void ShowProgressNoteL();
		
		void DialogDismissedL( TInt aButtonId );
		
    private:    // data
        // Current state of plug-in execution
        TState                              iState;
        
        // Interface to ETel 3rd Party Telephony lib
        CTelephony*                         iTelephony;        	
        
        // Network information from ETel (contains network access type)
		CTelephony::TNetworkInfoV1          iNetworkInfoV1;
		
		// Signal strength information from ETel
		CTelephony::TSignalStrengthV1       iSignalStrengthV1;
		
		// TPckg object for iNetworkInfoV1
		CTelephony::TNetworkInfoV1Pckg      iNetworkInfoV1Pckg;		
		
		// TPckg object for iSignalStrengthV1
		CTelephony::TSignalStrengthV1Pckg   iSignalStrengthV1Pckg;		
		
		// Network type (UMTS vs. GSM)
		TNetworkType                        iNetworkType;
		
		// Network band class
		TNetworkBand                        iNetworkBand;
		
		// Threshold values for network coverage are read from CR key
		TReal                               iThresholdValues[2][2];		
		
		// Counter to keep track of test progress
		TInt                                iProgressCounter;
		
		//Wait Dialog
		CAknWaitDialog*      iWaitDialog;
		
		TBool idialogOn;
    };

#endif //DIAGNETCOVERAGEPLUGIN_H

// End of File

