/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     Holds Email setting item
*
*/


#ifndef CWPEmailItem_H
#define CWPEmailItem_H

// INCLUDES
#include <MWPVisitor.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CWPCharacteristic;

//E-mail Notification and retrieval parameters
_LIT( KParamEMN, "EMN" );
_LIT( KParamCS, "CS" );
_LIT( KParamLMS, "LMS" );
_LIT( KParamRETRMODE, "RETR-MODE" );
_LIT( KParamRETRSIZE, "RETR-SIZE" );
_LIT( KParamPOLL, "POLL" );
_LIT( KParamONLINE, "ONLINE" );


// CLASS DECLARATION

/**
 * CWPEmailItem handles Email settings.
 * @since 2.0
 */ 
class CWPEmailItem : public CBase, public MWPVisitor
	{
	public:  // Constructors and destructor

        /**
        * Creates Email setting item
		* @since 2.0
        * @return self
        */
		static CWPEmailItem* NewL();
        static CWPEmailItem* NewLC();

        /**
        * Destructor.
        */
		~CWPEmailItem();

    public: // New functions
		
		/**
        * Get parameter value. Must be one of enumeration
		* TWPParameterID.
		* @since 2.0
		* @return param value or KNullDesC if not found
		*/
		const TDesC& ParameterValue(TInt aParamID) const;
		       
        /**
        * Get associated IAP name for email settings.
		* @since 2.0
        * @return Internet Access Point name.
        */
        HBufC* AssociatedIapNameL();
        
        /**
        * Get associated IAP id
        * @since 2.0
        * @return IAP id or Zero if not found
        */
		TUint32 AssociatedIapId();

        /**
        * Check that current settings are valid
        * since 2.0
        * @return ETrue if settings are valid
        */
        TBool IsValid() const;
        
        /**
        * Get parameter value. Must be one of enumeration
        * TWPParameterID.
        * @since 2.0
        * @return param value or KNullDesC if not found
        */
        const TDesC& ParameterValue(const TDesC& aParamName);


	public: // From MWPVisitor

		void VisitL(CWPCharacteristic& aCharacteristic);
		void VisitL(CWPParameter& aParameter);
		void VisitLinkL(CWPCharacteristic& aCharacteristic );


	private:  // constructor 

        /**
        * C++ default constructor.
        */
        CWPEmailItem();

        /**
        * 2nd phase constructor
        */
		void ConstructL();
		
	private:  // Data
        
        /**
		* Ref: Array to Mail setting parameters
		**/ 
		CArrayPtr<CWPParameter>* iEmailSettingParams;

        ///Ref:to the IAP settings
        CWPCharacteristic* iNapDef;

    
    private: // for testing use
        friend class T_CWPEmailItem;
		
	};

#endif	// CWPEmailItem_H
            
// End of File
