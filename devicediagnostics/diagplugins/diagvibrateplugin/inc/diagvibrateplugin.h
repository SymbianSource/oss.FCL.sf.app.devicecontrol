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
* Description:  CDiagVibratePugin derives from CDiagTestPluginBase and 
*                  is the access point to run the vibrate test.
*
*/


#ifndef __DIAGVIBRATEPLUGIN_H__
#define __DIAGVIBRATEPLUGIN_H__

// INCLUDES
#include <DiagTestPluginBase.h>         //CDiagTestPluginBase
#include <AknProgressDialog.h>          //MProgressDialogCallback

#include "diagvibratepluginobserver.h"  //MDiagVibratePluginObserver

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CDiagVibratePluginEngine;

// CLASS DEFINITION
class CDiagVibratePlugin : public CDiagTestPluginBase, 
                           public MDiagVibratePluginObserver,
                           public MProgressDialogCallback

    {
    public: // Constructors and destructor

        /**
        * Symbian OS two-phased constructor
        * @return MDiagPlugin*
        */
        static MDiagPlugin* NewL( TAny* aInitParams );

        /**
        * Destructor
        */
        virtual ~CDiagVibratePlugin();

    public: // From CDiagTestPluginBase

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

    public: // from MDiagVibratePluginObserver

        /**
         * Inform vibrate status changes
         *
         * @param aStatus Status of the vibrate engine
         */
        virtual void VibraStatusChanged( TVibrateStatus aStatus );

    private: // from CDiagTestPluginBase

        /**
        * Handle plug-in execution.
        * @see CDiagTestPluginBase::DoRunTestL
        */
        void DoRunTestL();

        /**
        * Clean up
        * @see CDiagTestPluginBase::DoStopAndCleanupL
        */
        void DoStopAndCleanupL();

    private: // from CActive
    
        /**
        * Handle active object completion event
        */
        virtual void RunL();
        
        /**
        * Handle RunL leaves
        */
        virtual TInt RunError( TInt aError );
    
        /**
        * Handle cancel
        */
        virtual void DoCancel();
    
    private: // from MProgressDialogCallback
        
        /**
        * Called when the progress dialog is dismissed.
        *
        * @param aButtonId - indicate which button made dialog to dismiss
        */
        virtual void DialogDismissedL( TInt aButtonId );
    
    private: 

        /**
        * C++ default constructor.
        */
        CDiagVibratePlugin( CDiagPluginConstructionParam* aInitParams  );

        /**
        * Symbian OS default constructor.
        *
        */
        void ConstructL();

   private:    // data
    
        /**  enum for the vibrate plugin internal state */
        enum TState
            {
            EStateUnknown = 0,
            EStateFailed,
            EStateCancelled,
            EStateSkipped,
            EStateIdle,
            EStateVibrating,
            EStateUserQuery,
            EStateBlocked,
            EStateRestart
            };

    private:    // private functions
    
        /**
        * Makes sure vibrate is enabled in the profile
        */
        TInt SetVibrateSettingOnL();

        /**
        * Sets the vibration setting to its original value
        */
        void SetVibrateSettingToOriginalValueL();

        /**
        * Checks if the charger is connected
        */
        TBool IsChargerConnected() const;

        /**
        * Sets the state
        * @param aState State to be set
        */
        void SetState( CDiagVibratePlugin::TState aState );
        
        /**
        * Displays disconnect charger dlg
        * @return TBool
        */
        TBool DisplayDisconnectChargerQueryL();
        
        /**
        * Displays vibrating progress dialog
        */
        void ShowVibratingNoteL();
        
        /**
        * Gets vibration time in seconds from CR
        */
        void ReadVibrationTimeL();
        
        /*
        * confirmation query for activating the vibration in profile.
        */
        TInt ShowActivateConfNoteL();
       
    private: // data

        /**
        * Vibrate plugin engine
        * own
        */
        CDiagVibratePluginEngine* iEngine;

        /**
        * Progress Dialog
        * own
        */
        CAknProgressDialog* iProgressDialog;

        /**
        * State of the vibrate plugin
        */
        TState iState;
        
        /**
        * Indicates if the profile vibrate setting was 
        * temporalily set active 
        */
        TBool iVibrateSettingUpdated;
        
        /**
        * Original profile vibrate setting
        */
        TBool iOrigVibrateSetting;
        
        /**
        * Vibration time in seconds
        */
        TInt iVibrationTime;        
        
    };

#endif // __DIAGVIBRATEPLUGIN_H__

// End of File
