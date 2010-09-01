/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of customization components
*
*/
#ifndef  EMBEDDEDLINKAPP_H
#define  EMBEDDEDLINKAPP_H
#include "EmbeddedLinkHandler.h"
#include <f32file.h>
#include <favouritesitem.h>
#include <favouritesitemlist.h>

class CEmbeddedLinkApp : public CBase
{
	public:		// public standard methods
		static CEmbeddedLinkApp* NewL( TInt aId );
		static CEmbeddedLinkApp* NewLC( TInt aId );
		virtual ~CEmbeddedLinkApp();	
	private:	// private standard methods
		void ConstructL( TInt aId );
		
	
	public:		// public methods
		TInt AddLinkL( TDesC& aName, TDesC& aUrl, const TDesC& aUsername=KNullDesC, 
			const TDesC& aPassword=KNullDesC, TInt aApId=KErrNotFound );

		TInt GetLinkL( TInt aUid, CFavouritesItem& aFavItem );
		TInt GetLinksL( CFavouritesItemList& aFavItemList );
		TPtrC GetURI();
		TInt SetURI( const TDesC& aURI );
		TInt RemoveLinkL( TInt aUid );
		TInt UpdateNameL( const TDesC& aName, TInt aUid );
		TInt UpdateUrlL( const TDesC& aUrl, TInt aUid );
		TInt UpdateUserNameL( const TDesC& aUserName, const TInt aUid );
		TInt UpdatePasswordL( const TDesC& aPassword, const TInt aUid );
		TInt UpdateWapApL( const TDesC& aWapAp, const TInt aUid );
		TInt GetLinkUidL( TDesC& aName, TDesC& aAddress, TInt folderUid, TInt& aUid );

		TInt AppId() const;
		TPtrC AppName() const;

	private:	// private methods
		TInt GetAppName( TDes& aName );
		TInt FolderUidL( );
	
	public:		// public class variables
		TBuf<64> iURI;
	
	private:	// private class variables

		RFavouritesSession iFavouritesSess;

		TBuf<32> iAppName;
		TInt iId;
};

#endif




