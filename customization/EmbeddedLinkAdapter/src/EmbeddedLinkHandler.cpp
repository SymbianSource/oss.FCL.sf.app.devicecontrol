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

#include "EmbeddedLinkHandler.h"
#include "EmbeddedLinkApp.h"

// ---------------------------------------------------------
// CEmbeddedLinkHandler::NewL
//
//
// ---------------------------------------------------------
CEmbeddedLinkHandler* CEmbeddedLinkHandler::NewL()
{
	CEmbeddedLinkHandler* self = CEmbeddedLinkHandler::NewLC();
    CleanupStack::Pop( self );
    return self;
}

// ---------------------------------------------------------
// CEmbeddedLinkHandler::NewLC
//
//
// ---------------------------------------------------------
CEmbeddedLinkHandler* CEmbeddedLinkHandler::NewLC()
{
	CEmbeddedLinkHandler* self = new ( ELeave ) CEmbeddedLinkHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
}

// ---------------------------------------------------------
// CEmbeddedLinkHandler::ConstructL
//
//
// ---------------------------------------------------------
void CEmbeddedLinkHandler::ConstructL()
{
	iEmbeddedLinkImage = CEmbeddedLinkApp::NewL( KFavouritesImageContextId );
	iEmbeddedLinkRingtone = CEmbeddedLinkApp::NewL( KFavouritesRingingToneContextId );
	iEmbeddedLinkVideo = CEmbeddedLinkApp::NewL( KFavouritesVideoContextId );
	iEmbeddedLinkAudio = CEmbeddedLinkApp::NewL( KFavouritesAudioContextId );
	iEmbeddedLinkSkin = CEmbeddedLinkApp::NewL( KFavouritesSkinContextId );
	iEmbeddedLinkGame = CEmbeddedLinkApp::NewL( KFavouritesGameDataContextId );
	iEmbeddedLinkApplication = CEmbeddedLinkApp::NewL( KFavouritesApplicationContextId );
	iEmbeddedLinkMusic = CEmbeddedLinkApp::NewL( KFavouritesMusicContextId );

	User::LeaveIfError( iFavouritesSess.Connect() );
}

// ---------------------------------------------------------
// CEmbeddedLinkHandler::~CEmbeddedLinkHandler
//
//
// ---------------------------------------------------------
CEmbeddedLinkHandler::~CEmbeddedLinkHandler()
	{
	delete iEmbeddedLinkImage;
	delete iEmbeddedLinkRingtone;
	delete iEmbeddedLinkVideo;
	delete iEmbeddedLinkAudio;
	delete iEmbeddedLinkSkin;
	delete iEmbeddedLinkGame;
	delete iEmbeddedLinkApplication;
	delete iEmbeddedLinkMusic;

	iFavouritesSess.Close();
	}

// ---------------------------------------------------------
// CEmbeddedLinkHandler::SupportingApplicationsL
//
//
// ---------------------------------------------------------
void CEmbeddedLinkHandler::SupportingApplicationsL( RArray<TEmbeddedLinkAppName>& aArray )
	{
	TEmbeddedLinkAppName appName;
	
	appName.Zero();
	appName.AppendNum( KFavouritesApplicationContextId, EHex );
	aArray.Append( appName )	;

	appName.Zero();
	appName.AppendNum( KFavouritesImageContextId, EHex );
	aArray.Append( appName )	;

	appName.Zero();
	appName.AppendNum( KFavouritesAudioContextId, EHex );
	aArray.Append( appName );

	appName.Zero();
	appName.AppendNum( KFavouritesVideoContextId, EHex );
	aArray.Append( appName );

	appName.Zero();
	appName.AppendNum( KFavouritesGameDataContextId, EHex );
	aArray.Append( appName );
	
	appName.Zero();
	appName.AppendNum( KFavouritesRingingToneContextId, EHex );
	aArray.Append( appName );
	
	appName.Zero();
	appName.AppendNum( KFavouritesSkinContextId, EHex );
	aArray.Append( appName );	

	appName.Zero();
	appName.AppendNum( KFavouritesMusicContextId, EHex );
	aArray.Append( appName );
	}

// ---------------------------------------------------------
// CEmbeddedLinkHandler::GetLinkAppL
//
//
// ---------------------------------------------------------
TInt CEmbeddedLinkHandler::GetLinkAppL( TInt aLinkType, CEmbeddedLinkApp*& aLinkApp )
	{
	TInt err( KErrNone );
	
	switch( aLinkType )
		{
		//Lis‰‰ browseri! KFavouritesRootUid
		
		case KFavouritesImageContextId:
		aLinkApp = iEmbeddedLinkImage;
		break;
		
		case KFavouritesRingingToneContextId:
		aLinkApp = iEmbeddedLinkRingtone;
		break;
		
		case KFavouritesVideoContextId:
		aLinkApp = iEmbeddedLinkVideo;
		break;
		
		case KFavouritesAudioContextId:
		aLinkApp = iEmbeddedLinkAudio;
		break;
		
		case KFavouritesSkinContextId:
		aLinkApp = iEmbeddedLinkSkin;
		break;
		
		case KFavouritesGameDataContextId:
		aLinkApp = iEmbeddedLinkGame;
		break;
		
		case KFavouritesApplicationContextId:
		aLinkApp = iEmbeddedLinkApplication;
		break;

		case KFavouritesMusicContextId:
		aLinkApp = iEmbeddedLinkMusic;
		break;
		
		default:
		err = KErrNotFound;
		break;
		}	
	
	return err;
	}


TInt CEmbeddedLinkHandler::RemoveLinkL( TInt aLinkUid )
	{
	TInt ret = KErrNone;

  RFavouritesDb db;
  User::LeaveIfError( db.Open( iFavouritesSess, KBrowserBookmarks ) );
  CleanupClosePushL<RFavouritesDb>( db );

	ret=db.Delete( aLinkUid );
	db.Close();


	CleanupStack::PopAndDestroy(); //db
	return ret;
	}


TInt CEmbeddedLinkHandler::UpdateLinkUrlL( TInt aLinkUid, const TDesC& aURL )
	{
	TInt err( KErrNone );

  RFavouritesDb db;
  User::LeaveIfError( db.Open( iFavouritesSess, KBrowserBookmarks ) );
  CleanupClosePushL<RFavouritesDb>( db );

	CFavouritesItem* favItem = CFavouritesItem::NewL();
	CleanupStack::PushL( favItem );
	
	err = db.Get( aLinkUid, *favItem );
	
	if( err==KErrNone )
		{
		favItem->SetUrlL( aURL );
		}

	err = db.Update( *favItem, aLinkUid, ETrue );
	db.Close();

	CleanupStack::PopAndDestroy( 2 ); //db, favItem
	
	return err;
	}


TInt CEmbeddedLinkHandler::UpdateLinkNameL( TInt aLinkUid, const TDesC& aName )
	{
	TInt err( KErrNone );

  RFavouritesDb db;
  User::LeaveIfError( db.Open( iFavouritesSess, KBrowserBookmarks ) );
  CleanupClosePushL<RFavouritesDb>( db );

	CFavouritesItem* favItem = CFavouritesItem::NewL();
	CleanupStack::PushL( favItem );

	err = db.Get( aLinkUid, *favItem );
	
	if( err == KErrNone )
		{
		favItem->SetNameL( aName );
		}

	err=db.Update( *favItem, aLinkUid, ETrue );
	db.Close();

	CleanupStack::PopAndDestroy( 2 ); //db
	
	return err;
	}


TInt CEmbeddedLinkHandler::UpdateUserNameL( TInt aLinkUid, const TDesC& aUserName )
	{
	TInt err( KErrNone );

  RFavouritesDb db;
  User::LeaveIfError( db.Open( iFavouritesSess, KBrowserBookmarks ) );
  CleanupClosePushL<RFavouritesDb>( db );

	CFavouritesItem* favItem = CFavouritesItem::NewL();
	CleanupStack::PushL( favItem );

	err = db.Get( aLinkUid, *favItem );

	if( err==KErrNone )
		{
		favItem->SetUserNameL( aUserName );
		}

	err = db.Update( *favItem, aLinkUid, ETrue );
	db.Close();

	CleanupStack::PopAndDestroy( 2 ); //db
	
	return err;
	}


TInt CEmbeddedLinkHandler::UpdatePasswordL( TInt aLinkUid, const TDesC& aPassword )
	{
	TInt err( KErrNone );

  RFavouritesDb db;
  User::LeaveIfError( db.Open( iFavouritesSess, KBrowserBookmarks ) );
  CleanupClosePushL<RFavouritesDb>( db );

	CFavouritesItem* favItem = CFavouritesItem::NewL();
	CleanupStack::PushL( favItem );
	
	err = db.Get( aLinkUid, *favItem );

	if ( err == KErrNone ) 
		{
		favItem->SetPasswordL( aPassword );
		}

	err = db.Update( *favItem, aLinkUid, ETrue );
	db.Close();

	CleanupStack::PopAndDestroy( 2 ); //db
	
	return err;
	}

TInt CEmbeddedLinkHandler::UpdateWapApL( TInt aLinkUid, const TDesC& aWapAp )
	{
	TInt err( KErrNone );

  RFavouritesDb db;
  User::LeaveIfError( db.Open( iFavouritesSess, KBrowserBookmarks ) );
  CleanupClosePushL<RFavouritesDb>( db );

	CFavouritesItem* favItem = CFavouritesItem::NewL();
	CleanupStack::PushL( favItem );
	
	err = db.Get( aLinkUid, *favItem );

	if ( err == KErrNone ) 
		{
        TFavouritesWapAp wapAp;
        wapAp.SetDefault();
        if (aWapAp.Length() > 0)
        	{
        	TUint value = 0;
			TLex lex( aWapAp );
			TInt lexErr = lex.Val( value );
			if (lexErr == KErrNone)
				{
				wapAp.SetApId( value );
				}
			else
				{
				wapAp.SetNull();
				}
        	}

		favItem->SetWapAp( wapAp );	
		}

	err = db.Update( *favItem, aLinkUid, ETrue );
	db.Close();

	CleanupStack::PopAndDestroy( 2 ); //db
	
	return err;
	}

