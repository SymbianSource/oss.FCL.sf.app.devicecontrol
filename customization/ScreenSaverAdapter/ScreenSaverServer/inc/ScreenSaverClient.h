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
* Description:  DM Screensaver Adapter
*
*/



#ifndef __SCREENSAVER_CLIENT_H__
#define __SCREENSAVER_CLIENT_H__



#include <e32std.h>
#include <e32base.h>
#include <e32cmn.h>
#include <ecom/implementationinformation.h>
#include <badesca.h>
#include <s32mem.h>





class CScreenSaverListInfo : public CBase
{

	public:
	static 	  CScreenSaverListInfo* NewLC( TUid aUid, 
										   TInt aVersion, 
										   TDesC*  aName, 
										   TDesC8* aOpaqueData);
										   
	static 	  CScreenSaverListInfo* NewL( TUid aUid, 
										   TInt aVersion, 
										   TDesC*  aName, 
										   TDesC8* aOpaqueData);	
										   
	~CScreenSaverListInfo();									   								   
	
	IMPORT_C const TDesC& DisplayName() const;
	
	IMPORT_C const TDesC8& OpaqueData() const;
	
	IMPORT_C TUid ImplementationUid() const;
	
	IMPORT_C TInt Version() const;
	
	private:
	
	CScreenSaverListInfo(TUid aUid, 
						TInt aVersion);
	void ConstructL(TDesC*  aName, 
					TDesC8* aOpaqueData);
	
	
	TUid 	iUid;
	TInt 	iVersion;
	HBufC*  iName;
	HBufC8* iOpaqueData;
};


typedef RPointerArray<CScreenSaverListInfo> RSSListInfoPtrArray;


class RScreenSaverClient : public RSessionBase
{
	public:
	    /**
		* Connect Connects client side to Screen saver server
	    * @return KErrNone Symbian error code
        */
		IMPORT_C TInt Connect();

	    /**
		* Close Close server connection
        */
		IMPORT_C void Close();
		
		/**
		* Use this method to get screen saver list information like name,version,display name
		*/
		IMPORT_C void GetScreenSaverListInfoL(RSSListInfoPtrArray &aScreenSaverList);
		/**
		* Use this method to get screen saver list UIDs
		*/
		//IMPORT_C void GetScreenSaverListL(RArray< TUid >& aUids,CDesCArray& aIds);
		IMPORT_C void GetScreenSaverListL(CDesCArray& aIds);
	};




#endif
