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
* Description:  Header file for CNSmlDMMultiQuery
*
*/


#ifndef NSMLDMAUTHQUERYDLG_H
#define NSMLDMAUTHQUERYDLG_H

#include <AknQueryDialog.h>

#include "NSmlDMdef.h"
#include "NSmlDMSyncAppEngine.h"

class CNSmlDMQueryTimer;

/**
* CNSmlDMMultiQuery
*  
* Multi query
*/
NONSHARABLE_CLASS (CNSmlDMMultiQuery) : public CAknMultiLineDataQueryDialog
	{
	public:

        /**
        * Two-phased constructor.
        *
        * @param aAppEngine pointer to app engine
		* @param aProfileId profile id
		* @param aText1     text for username
		* @param aText2     text for password
        * @return CNSmlDMMultiQuery* new CNSmlDMMultiQuery object
        */
		static CNSmlDMMultiQuery* NewL( CNSmlDMSyncAppEngine* aAppEngine,
		                                TInt aProfileId, 
		                                TBool aIsHttp );
		
		/**
		* Destructor
		*/
		virtual ~CNSmlDMMultiQuery();

		/**
        * Called during initialisation just before dialog is shown
		* @param None
        * @return None
        */
		void PostLayoutDynInitL();
		
		/**
		* called when dialog button is clicked or timer has ended
		* @param aButtonId  Id of button that was clicked
        * @return ETrue to close dialog
        */
		virtual TBool OkToExitL( TInt aButtonId );
		
		/**
		* Called whe timer has ended, calls TryExitL()
		* @param None
        * @return None
        */
		void HandleTimerEndL();
	private:

		/**
		* Constructor
        * @param aAppEngine  pointer to app engine
		* @param aProfileId  profile id
		* @param aTone       sound when launching the dialog
        * @return None
        */
		CNSmlDMMultiQuery( CNSmlDMSyncAppEngine* aAppEngine,
		                   TInt aProfileId,
		                   TBool aIsHttp,
		                   TTone aTone = ENoTone );

	private:
	    // Application engine
		CNSmlDMSyncAppEngine*           iAppEngine;
		// Profile id
		TInt                            iProfileId;
		// user name
		TBuf<KNSmlMaxUsernameLength>    iUsername;
		// Password
		TBuf<KNSmlMaxPasswordLength>    iPassword;
		// Query timer
		CNSmlDMQueryTimer*              iTimer;
		TBool                           iIsHttp;
	};

/**
* CNSmlDMQueryTimer
*
* Query timer  
*/
NONSHARABLE_CLASS (CNSmlDMQueryTimer) : public CTimer
	{
	public:

        /**
        * Two-phased constructor.
        *
        * @param aAlertDialog  dialog to be closed after timer has ended
        * @return new CNSmlDMQueryTimer object
        */
		static CNSmlDMQueryTimer* NewL( CNSmlDMMultiQuery* aAlertDialog );
		
		/**
		* Method is called when time has passed
		* @param None
        * @return None
        */
		void RunL();

		/**
		* Destructor
        */
		virtual ~CNSmlDMQueryTimer();

	private:
        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aAlertDialog  dialog to be closed after timer has ended
        * @return None
        */
		void ConstructL( CNSmlDMMultiQuery* aAlertDialog );

		/**
		* Constructor
        * @param None
        * @return None
        */
		CNSmlDMQueryTimer();

	private:
	    // multi query
		CNSmlDMMultiQuery* iAlertDialog;
	};

#endif  // NSMLDMAUTHQUERYDLG_H

// End of File
