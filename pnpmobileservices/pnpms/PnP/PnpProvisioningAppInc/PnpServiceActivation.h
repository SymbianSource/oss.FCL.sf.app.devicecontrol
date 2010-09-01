/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  PnpServiceActivation header file
*
*/


#ifndef PNP_SERVICE_ACTIVATION_H
#define PNP_SERVICE_ACTIVATION_H

#include <e32base.h>

class CPnpServiceActivation: public CBase
{

public:  // Constructors and destructor
        
       /**
        * Two-phased constructor.
        */
        static CPnpServiceActivation* NewL();

       /**
        * Two-phased constructor.
        */
        static CPnpServiceActivation* NewLC();

       /**
        * Destructor.
        */
        ~CPnpServiceActivation();
private:
        /**
         * C++ default constructor.
         */
         CPnpServiceActivation();

        /** 
         *By default Symbian 2nd phase constructor is private.
         */
         void ConstructL();
private:

  	   /** 
  	     * Mapping Application ID launched PNPMS client with that of 
  	     * provisioning setting application ID. 
  	     * This function validates the application ID with provisioning 
  	     * settings ID 
  	     */ 
  	     TBool ValidateAppUidWithProvappIDL();
  	     TUint32 iLaunchUid;
  	     TUint32 iPosition;

public:

	  /** 	    
	    * Launch Application using command line Symbian Fw 
	    */ 
	    void LaunchApplicationL();

	  /** 
	    * kill browser after settings are downloded and launching application 
	    */ 
	    void KillBrowserL();

	  /** 	      	
	    *Display note specific to Service 
	    */ 
	    void DisplayAppSpecNoteL();
	// Additional  functions can be added if new functions are required
        // for service activation

		TBool isPresent(TDesC& p );

		void ParseValues(RPointerArray<HBufC>& array, TDesC& aValue);
		void GetCenrepValueL(const TUid aRepositoryUid,TUint32 aKey, TDes16& aValue);
		TBool Search(RPointerArray<HBufC>& array,TDesC& p);
		TBool isFound(TDesC& p,TUint32 position);
};


#endif PNP_SERVICE_ACTIVATION_H