/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Sync Log handler class
*
*/


#ifndef NSMLDMSYNCDIALOG_H
#define NSMLDMSYNCDIALOG_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>

#include "NSmlDMdef.h"

// FORWARD DECLARATIONS
class RSyncMLSession;
class RSyncMLHistoryLog;
class CSyncMLHistoryJob;
class CNSmlDMSyncProfile;

// CLASS DECLARATION

/**
*  CNSmlDMSyncDialog
*  
*/
NONSHARABLE_CLASS (CNSmlDMSyncDialog) : public CBase
    {
    
	// this must match with resource r_sml_log_items
	enum TLogItem
		{
	    EHeadingProfile = 0,
	    EHeadingServer = 1,
	    EHeadingDate = 2,
		EHeadingTime = 3,
		EHeadingStatus = 4
		};

	public:
	    /**
        * Launches log dialog.
		* @param aProfileId Profile id
		* @param aSyncSession Server session
        * @return Completion code.
        */
		static TBool ShowDialogL( TInt aProfileId,
		                          RSyncMLSession* aSyncSession );

    
	public:// Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CNSmlDMSyncDialog* NewL( TInt aProfileId,
                                        RSyncMLSession* aSyncSession );
        
        /**
        * Destructor.
        */
        virtual ~CNSmlDMSyncDialog();
    
    private:

        /**
        * C++ default constructor.
        */
        CNSmlDMSyncDialog( TInt aProfileId, RSyncMLSession* aSyncSession );
	
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

	private:
	    /**
        * Shows popup-style log view.
		* @param None.
        * @return None.
        */
		void PopupLogViewL();

		/**
        * Adds general log data into listbox array.
		* @param aArray Listbox array.
        * @return None.
        */
		void FormatLogViewListL( CDesCArray* aArray );
      
		/**
        * Gets latest history (sync) job.
		* @param aHistoryLog.
        * @return History job.
        */
        CSyncMLHistoryJob* LatestHistoryJob( RSyncMLHistoryLog& aHistoryLog );
		
		/**
        * Reads resource text with the given resource id.
		* @param aText Associated resource text.
		* @param aResourceId Given resource id.
        * @return None.
        */
        void ReadL( TDes& aText, TInt aResourceId );

	private:
        // profile id
		TInt                        iProfileId;
		
		// sync profile
		CNSmlDMSyncProfile*         iProfile;

		// session with sync server
		RSyncMLSession*             iSyncSession;
		
		// history job (contains sync information)
		const CSyncMLHistoryJob*    iHistoryJob;

		// log item headers (from resource file)
		CDesCArrayFlat*             iLogItems;
	};

#endif      // NSMLDMSYNCDIALOG_H
            
// End of File
