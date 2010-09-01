/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Settings saver with progress note.
*
*/



#ifndef CPNPPROVISIONINGSAVER_H
#define CPNPPROVISIONINGSAVER_H

// INCLUDES
#include <msgbiocontrol.h>                // for CMsgBioControl
#include <ActiveApDb.h>
#include <favouritesdbobserver.h>
#include "mwaitdialogstatusnotifier.h"  // for MWaitDialogStatusNotifier

// FORWARD DECLARATIONS
class CWPEngine;
class CActiveFavouritesDbNotifier;
class CBookmarkDb;
class CWaitDialogMonitor;
// CLASS DECLARATION

/**
 * Helper class for saving Provisioning settings. Provides a progress note.
 */
class CPnpProvisioningSaver :
    public CActive,
    private MActiveApDbObserver,
    private MFavouritesDbObserver,
    private MWaitDialogStatusNotifier
    {
    public:
        /**
        * C++ default constructor.
        * @param aEngine Engine to be used for saving
        */
        CPnpProvisioningSaver( CWPEngine& aEngine, TBool aSetAsDefault, TBool aWaitNote );

        /**
        * Destructor.
        */
        ~CPnpProvisioningSaver();

    public:
        /**
        * Prepares the object for saving.
        */
        void PrepareLC();

        /**
        * Executes save with a progress note. Ownership of the
        * CPnpProvisioningSaver object is transferred.
        * @param aNumSaved When returns, contains number of settings saved.
        * @return Status code. >= 0 if saving was completed
        */
        TInt ExecuteLD( TInt& aNumSaved );

    protected: // From CActive

        void DoCancel();
        void RunL();
        TInt RunError( TInt aError );

    private: // from MActiveApDbObserver

        void HandleApDbEventL( TEvent anEvent );

    private: // from MFavouritesDbObserver

        void HandleFavouritesDbEventL( RDbNotifier::TEvent aEvent );
        
    private: // from MCMDialogStatusNotifier
        void WaitDialogDismissedL( const TInt aStatusCode );
    private:
        /**
        * Complete the request so that RunL() gets called.
        */
        void CompleteRequest();

        /**
        * Complete the request so that Timeout() gets called after
        * a delay.
        */
        void DelayedCompleteRequest();

        /**
        * Retry save now.
        */
        void Retry();

        /**
        * Timer timed-out.
        */
        static TInt Timeout(TAny* aSelf);

        /*
        * Process is finished. Deletes wait dialog.
        */
        void ProcessFinishedL();
        
        /*
        * Show wait note dialog
        */
        void ShowWaitNoteL();

    private:
        // The engine used for performing the save. Refs.
        CWPEngine& iEngine;

        // ETrue if setting as default
        TBool iSetAsDefault;

        // The item to be saved next
        TInt iCurrentItem;

        // Contains result to be passed to the called of ExecuteLD
        TInt iResult;

        // Active AP database
        CActiveApDb* iApDbNotifier;

        // Active Favourites Database
        CActiveFavouritesDbNotifier* iFavouritesNotifier;

        // Bookmarks database
        CBookmarkDb* iBookmarkDb;

        // Contains ETrue if commsdb is being waited on
        TBool iWaitCommsDb;

        // Contains ETrue if favourites db is being waited on
        TBool iWaitFavourites;

        // Active scheduler.
        CActiveSchedulerWait iWait;

        // Timer for retry
        CPeriodic* iRetryTimer;
        
        // wait note dialog monitor
        CWaitDialogMonitor* iWaitDialogMonitor;
        // wait note dialog
        CAknGlobalNote* iGlobalWaitNote;
        
        //End key pressed or not
        TBool iEndKeyPressed;
        
        TBool iShowWaitNote;
    };


#endif // CPNPPROVISIONINGSAVER_H

// End of File
