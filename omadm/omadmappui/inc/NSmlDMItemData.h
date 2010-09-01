/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Contains profile item data
*
*/


#ifndef NSMLDMITEMDATA_H
#define NSMLDMITEMDATA_H

// INCLUDES

#include <e32base.h>
#include "NSmlDMdef.h"

// CLASS DECLARATION

/**
*  CNSmlDMItemData
*
*  Base class for holding Setting data
*/
NONSHARABLE_CLASS (CNSmlDMItemData) : public CBase
	{
	public:

		/**
		* Constructor
		*/
		CNSmlDMItemData() : iHeader( 0 ), iValue( 0 ) { }
		
		/**
		* Destructor
		*/
		virtual ~CNSmlDMItemData();
		
		/**
		* Set header value of the settings item
		* @param const TDesC& aData - Setting item header value
		* @return -
		*/
		void SetHeader( const TDesC& aData );
		
		/**
		* Set value of the settings item
		* @param const TDesC& aData - Setting item value
		* @return -
		*/
		void SetValue( const TDesC& aData );
		/**
		* Set value of the port item
		* @param const TInt aPort - Port number
		* @return -
		*/
		void SetValue( TInt aPort );

		/**
		* Return header text
		* @param -
		* @return - String pointer to the setting item header value
		*/
		HBufC* Header() const;
		
		/**
		* Get setting item value text
		* @param -
		* @return - String pointer to the setting item value
		*/
		HBufC* Value() const;
		
		/**
		* Format Setting item to listbox
		* @param aConvert ETrue creates language specific conversion.
		* @return -
		*/
		virtual HBufC* ItemToListLC( TBool aConvert );

	public:
		
		TBool iMandatory;

	protected:

		/**
		* Private second constructor
		*/
		void ConstructL();

	protected:
		HBufC* iHeader;
		HBufC* iValue;
	};

/**
*  CNSmlDMItemData
*
*  Base class for holding profile data
*/
NONSHARABLE_CLASS (CNSmlDMProfileData) : public CNSmlDMItemData
	{
	public:

		enum TNSmlItemType
			{
			ENSmlTypeIAP,
			ENSmlTypeText,
			ENSmlTypeList,
			ENSmlTypeListYesNo,
			ENSmlTypeTime,
			ENSmlTypeNumber,
			ENSmlTypeSecret,
			ENSmlTypehttpListYesNo
			};
		/**
		* Create object
		* @param -
		* @return -
		*/
		static CNSmlDMProfileData* NewLC();

		/**
		* Destructor
		*/
	    virtual	~CNSmlDMProfileData() { }
	
		TInt                        iLength;
		TInt                        iResource;
		TNSmlItemType               iItemType;
		TBuf<KNSmlMaxItemLength>    iValue;
		TInt                        iNumberData;
		TBool                       iLatinInput;
		TInt                        iIndex;
		TBool                       iHidden;
		TBool                       iWapAPInUse;
		TInt                        iItemId;
	};

#endif // NSMLDMITEMDATA_H


// End of File
