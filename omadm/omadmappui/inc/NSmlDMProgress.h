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
* Description:  Header file for CNSmlDMProgressDialog
*
*/


#ifndef NSMLDMPROGRESSDIALOG_H
#define NSMLDMPROGRESSDIALOG_H


//  INCLUDES
#include    <e32base.h>
#include    <AknProgressDialog.h>
#include    <AknsItemID.h>

#include "NSmlDMSyncDocument.h"
#include    "NSmlDMdef.h"

// CLASS DECLARATIONS

/**
*  MNSmlDMProgressDialogObserver
* 
*/
class MNSmlDMProgressDialogObserver
    {
    public:
        /**
        * Callback method
        * Get's called when a dialog is dismissed.
        */
        virtual void HandleDialogEventL(TInt aButtonId) = 0;
    };

/**
* CNSmlDMProgressDialog2
*
* CNSmlDMProgressDialog2 is needed to modify CAknProgressDialog::OkToExitL. 
*/
NONSHARABLE_CLASS (CNSmlDMProgressDialog2) : public CAknProgressDialog
    {
	public:

        /**
        * C++ default constructor.
        */
		CNSmlDMProgressDialog2(CEikDialog** aSelfPtr);

        /**
        * From the base class.
		* Called by framework before exiting the dialog.
        * @param Button id.
		* @return ETrue to exit\ EFalse to not to exit.
        */
        TBool OkToExitL(TInt aButtonId);

        /**
        * From the base class.
		* Called to launch the dialog.
        * @param None.
		* @return the Button Id which exits the dialog
        */
        TInt RunLD();

        /**
        * From the base class.
		* Called to stop the timer and delete the dialog.
        * @param None.
		* @return None.
        */
        void ProcessFinishedL();
        
  private:
  
        /**
        * From the base class.
		* Called by framework for key event handling.
        * @param aKeyEvent.
		* @param TEventCode.
		* @return Return code.
        */
		TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

        
  private:
        // has dialog received close event (eg. end key) 
		TBool iCloseEventReceived;
		
		// has DM Connection been closed
        TBool iClosed;
        //Documnet class reference
		CNSmlDMSyncDocument*        iDoc;       
    };

/**
*  CNSmlDMProgressDialog
*  
*/
NONSHARABLE_CLASS (CNSmlDMProgressDialog) : public CBase,
                                            public MProgressDialogCallback
    {
	enum TAnimationState
        {
        EAnimationOn,
    	EAnimationOff,
    	EAnimationNone
        };

    public:// Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CNSmlDMProgressDialog* NewL(MNSmlDMProgressDialogObserver* aObserver);
        
        /**
        * Destructor.
        */
        virtual ~CNSmlDMProgressDialog();
    
    private:

        /**
        * C++ default constructor.
        */
        CNSmlDMProgressDialog( MNSmlDMProgressDialogObserver* aObserverPtr );
	
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();
        
		/**
		* From MProgressDialogCallback.
        * Called by the framework when dialog is dismissed.
        * @param aButtonId
        * @return None
        */
        void DialogDismissedL( TInt aButtonId );

    public:        
        
        /**
        * Launches progress dialog.
        * @param aLabel.
        * @return None.
        */
        void LaunchProgressDialogL( const TDesC& aLabel );
        
        /**
        * Launches progress dialog with custom bitmap.
        * @param aLabel Dialog label
        * @param aId Item identifier used to identify items
        * @param aFileName Name of the file
        * @param aFileBitmapId Bitmap id
        * @param aFileMaskId Mask id
        * @return None.
        */
        void LaunchProgressDialogL( const TDesC& aLabel,
                                    TAknsItemID aId,
                                    const TDesC& aFileName,
                                    TInt aFileBitmapId,
                                    TInt aFileMaskId );
        
        /**
        * Closes progress dialog.
        * @param None
        * @return None
        */
        void CancelProgressDialogL();
        
        /**
        * Update dialog label text.
        * @param aText.
        * @return None.
        */
        void UpdateTextL( const TDesC& aText );

        /**
        * Update dialog label text.
        * @param aResId.
        * @return None.
        */
        void UpdateTextL( TInt aResId );


        /**
        * Set progress bar state.
        * @param aValue.
        * @return None
        */
        void SetProgress( TInt aValue );

        /**
        * Set progress bar state.
        * @param aFinalValue.
        * @return None
        */
        void SetFinalProgress( TInt aFinalValue );

        /**
        * Gets CAknNoteControl from CAknProgressDialog.
        * @param None
        * @return CAknNoteControl.
        */
		CAknNoteControl* NoteControl();

        /**
        * Cancels animation.
        * @param None.
        * @return None.
        */
		void CancelAnimation();
        
		/**
        * Starts animation.
        * @param None
        * @return None.
        */
		void StartAnimationL();
    private:
        /**
        * Read resource
        * @param aText Resource text
        * @param aResourceId Resource id
        * @return None.
        */
        void ReadL( TDes& aText, TInt aResourceId );

    public:       
        /**
        * Create image
        * @param aId
        * @param aFileName
        * @param aFileBitmapId
        * @param aFileMaskId
        * @return CEikImage*
        */        
        CEikImage* CreateImageLC( TAknsItemID aId,
                                  const TDesC& aFileName,
                                  TInt aFileBitmapId,
                                  TInt aFileMaskId );
       /**
        * Set's the image
        * @param CEikImage*
        * @return None
        */                            
       void SetImageL(CEikImage* aimage);

    private:
        // progress dialog
		CNSmlDMProgressDialog2*	iProgressDialog;

        // progress dialog info
        CEikProgressInfo* iProgressInfo;

		// last label text
		TBuf<KBufSize> iLastText;

		// animation state
		TBool iAnimation;

		// dialog observer
        MNSmlDMProgressDialogObserver*	iObserver;

		// count of received events from CAknProgressDialog
		TInt iEventCount;
    };

#endif // NSMLDMPROGRESSDIALOG_H
            
// End of File
