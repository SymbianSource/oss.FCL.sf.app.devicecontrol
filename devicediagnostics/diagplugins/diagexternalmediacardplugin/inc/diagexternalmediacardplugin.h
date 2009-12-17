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
* Description:  External Media Card Plugin
*
*/


#ifndef DIAGEXTERNALMEDIACARDPLUGIN_H
#define DIAGEXTERNALMEDIACARDPLUGIN_H

// INCLUDES
#include <DiagTestPluginBase.h>         // CDiagTestPluginBase
#include <DiagResultsDatabaseItem.h>    // DiagResultsDatabaseItem::TResult
#include <f32file.h>                    // RFs, RFile
#include <AknWaitDialog.h> 
//CONSTANTS
const TUid KDiagExternalMediaCardPluginUid = { 0x2000E58B };
_LIT( KDiagExternalMediaCardPluginResourceFileName, "z:DevDiagExternalMediaCardPluginRsc.rsc" );
const TInt KDiagExternalMediaCardBufferSize = 256;

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CDiagResultsDbItemBuilder;

// CLASS DEFINITION
class CDiagExternalMediaCardPlugin : public CDiagTestPluginBase,
									 public MProgressDialogCallback
    {
    public: // Constructors and destructor
		
        enum TStates
            {
            EIdle,
            EWriting,
            EReading
            };
		
        /**
        * Symbian OS two-phased constructor
        * @return Timer Test Plugin
        */
        static MDiagPlugin* NewL( TAny* aInitParams );

        /**
        * Destructor
        */
        virtual ~CDiagExternalMediaCardPlugin();

    public: // From CDiagTestPluginBase

        /**
        * @see CDiagTestPluginBase::IsVisible()
        */
        virtual TBool IsVisible() const;
        
        /**
        * @see CDiagTestPluginBase::IsSupported()
        */
        virtual TBool IsSupported() const;

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
 
        
    private: // from CDiagTestPluginBase
    
        /**
        * Handle plug-in execution.
        * @see CDiagTestPluginBaseExtended::DoRunTestL
        */
        virtual void DoRunTestL();

        /**
        * Stop and clean up
        * @see CDiagTestPluginBaseExtended::DoStopAndCleanupL
        */
        virtual void DoStopAndCleanupL();
                        
    private:    // from CActive
    
        /**
        * Handle active object completion event
        */
        virtual void RunL();
    
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
    
	void ShowProgressNoteL();

    private:    // private functions
        
        /**
        * C++ default constructor.
        */
        CDiagExternalMediaCardPlugin( CDiagPluginConstructionParam* aInitParams );

        /**
        * Symbian OS default constructor.
        *
        */
        void ConstructL();
        
        /*
        * Set the states
        */
        void SetState( TStates aState );
        
        // ADO & Platformization Changes
        TBool ShowMessageQueryL( TInt aResourceId, TInt &aButtonId  );
        
        void ReportResultToPluginL(CDiagResultsDatabaseItem::TResult aResult);
        
        TBool ShowTitleMessageL();

    private:    // data
        TInt                                        iCounter;
        TStates                                     iState;  //rw states
        RFs                                         iFs;
        RFile                                       iFile;
        RFile                                       iFile2;
        TBuf8<KDiagExternalMediaCardBufferSize>     iBufw;
        TBuf8<KDiagExternalMediaCardBufferSize>     iBufr;
        TFileName                                   iFileName;
        TBool									    idialogOn;
	    TBool										idialogDismissed;
	    CAknWaitDialog*      						iWaitDialog;

    };

#endif //DIAGEXTERNALMEDIACARDPLUGIN_H

// End of File
