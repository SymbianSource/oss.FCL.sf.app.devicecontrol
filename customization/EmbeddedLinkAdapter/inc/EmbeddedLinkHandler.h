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

#ifndef  EMBEDDEDLINKHANDLER_H
#define  EMBEDDEDLINKHANDLER_H

#include <e32base.h>
#include <favouriteslimits.h>
#include <e32std.h>

#include <FavouritesDb.h>


class CEmbeddedLinkApp;

class CEmbeddedLinkHandler : public CBase
{
	public:
		static CEmbeddedLinkHandler* NewL();
		static CEmbeddedLinkHandler* NewLC();
		~CEmbeddedLinkHandler();
	private:
		void ConstructL();
			
	public:
		enum TLinkAppType
		{
			EImageLinkApp = 0,
			ERingtoneLinkApp,
			EVideoLinkApp,
			EAudioLinkApp,
			ESkinLinkApp,
			EGameLinkApp,
			EApplicationLinkApp
		};
		typedef TBuf<10> TEmbeddedLinkAppName;
		void SupportingApplicationsL( RArray<TEmbeddedLinkAppName>& aArray );
		TInt GetLinkAppL( TInt aLinkType, CEmbeddedLinkApp*& aLinkApp );
		TInt RemoveLinkL( TInt aLinkUid );
		TInt UpdateLinkUrlL( TInt aLinkUid, const TDesC& aURL );
		TInt UpdateLinkNameL( TInt aLinkUid, const TDesC& aName );
		TInt UpdateUserNameL( TInt aLinkUid, const TDesC& aUserName );
		TInt UpdatePasswordL( TInt aLinkUid, const TDesC& aPassword );
		TInt UpdateWapApL( TInt aLinkUid, const TDesC& aWapAp );

	private:
	
	public:
		CEmbeddedLinkApp* iEmbeddedLinkImage;
		CEmbeddedLinkApp* iEmbeddedLinkRingtone;
		CEmbeddedLinkApp* iEmbeddedLinkVideo;
		CEmbeddedLinkApp* iEmbeddedLinkAudio;
		CEmbeddedLinkApp* iEmbeddedLinkSkin;
		CEmbeddedLinkApp* iEmbeddedLinkGame;
		CEmbeddedLinkApp* iEmbeddedLinkApplication;	
		CEmbeddedLinkApp* iEmbeddedLinkMusic;	
	private:

		RFavouritesSession iFavouritesSess;


};

#endif