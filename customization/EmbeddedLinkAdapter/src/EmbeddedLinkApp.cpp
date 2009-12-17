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


#include "EmbeddedLinkApp.h"


// ---------------------------------------------------------
// CEmbeddedLinkApp::NewL
//
//
// ---------------------------------------------------------
CEmbeddedLinkApp* CEmbeddedLinkApp::NewL( TInt aId )
	{
	CEmbeddedLinkApp* self = CEmbeddedLinkApp::NewLC( aId );
    CleanupStack::Pop( self );
    return self;
	}

// ---------------------------------------------------------
// CEmbeddedLinkApp::NewLC
//
//
// ---------------------------------------------------------
CEmbeddedLinkApp* CEmbeddedLinkApp::NewLC( TInt aId )
	{
	CEmbeddedLinkApp* self = new ( ELeave ) CEmbeddedLinkApp;
    CleanupStack::PushL( self );
    self->ConstructL( aId );
    return self;
	}

// ---------------------------------------------------------
// CEmbeddedLinkApp::ConstructL
//
//
// ---------------------------------------------------------
void CEmbeddedLinkApp::ConstructL( TInt aId )
	{
	User::LeaveIfError( iFavouritesSess.Connect() );


	iId=aId;
	User::LeaveIfError( GetAppName( iAppName ) );
//sf-	iFolderUid=0; //sf-FolderUidL();
	}

// ---------------------------------------------------------
// CEmbeddedLinkApp::~CEmbeddedLinkApp
//
//
// ---------------------------------------------------------
CEmbeddedLinkApp::~CEmbeddedLinkApp()
	{
	iFavouritesSess.Close();
	}

// ---------------------------------------------------------
// CEmbeddedLinkApp::AddLinkL
//
//
// ---------------------------------------------------------
TInt CEmbeddedLinkApp::AddLinkL( TDesC& aName, TDesC& aUrl, const TDesC& aUsername, 
					const TDesC& aPassword, TInt aApId )
	{
	TInt err( KErrNone );

  RFavouritesDb db;
  User::LeaveIfError( db.Open( iFavouritesSess, KBrowserBookmarks ) );
  CleanupClosePushL<RFavouritesDb>( db );


	CFavouritesItem* favItem = CFavouritesItem::NewLC();

	favItem->SetNameL( aName );
	favItem->SetUrlL( aUrl );
	favItem->SetType( CFavouritesItem::EItem );
	
	if ( aUsername != KNullDesC ) 
		{
		favItem->SetUserNameL( aUsername );
		}
	if ( aPassword != KNullDesC )
		{
		favItem->SetPasswordL( aPassword );
		}
	
	if ( aApId != KErrNotFound )
		{
		TFavouritesWapAp wapAp;
		wapAp=aApId;
		favItem->SetWapAp( wapAp );	
		}

	favItem->SetParentFolder( FolderUidL() ); //sf-iFolderUid );

	err = db.Add( *favItem, ETrue );
//	db->CommitL();			KAATAA !!!!!
	db.Close();

	if ( err == KErrNone )
		{
		err = favItem->Uid();
		}
	else 
		{
		}

	CleanupStack::PopAndDestroy( 2 ); //db, favItem

	return err;
	}

// ---------------------------------------------------------
// CEmbeddedLinkApp::GetLinkUidL
//
//
// ---------------------------------------------------------
TInt CEmbeddedLinkApp::GetLinkUidL( TDesC& aName, TDesC& /*aAddress*/, TInt folderUid, TInt& aUid )
	{
  RFavouritesDb db;
  User::LeaveIfError( db.Open( iFavouritesSess, KBrowserBookmarks ) );
  CleanupClosePushL<RFavouritesDb>( db );

	CArrayFixFlat<TInt>* array = new ( ELeave ) CArrayFixFlat<TInt>( 16 );
	CleanupStack::PushL( array );
	TInt err = KErrNone;

	err=db.GetUids( *array, folderUid, CFavouritesItem::EItem, &aName, KFavouritesNullContextId );	

	if ( err == KErrNone && array->Count() < 2 )
		{
		for ( TInt i = 0; i < array->Count(); i++ )
			{
			aUid=array->At( i );
			}
		}
	else
		{
		err = KErrBadName;		
		}	 

	db.Close();

	CleanupStack::PopAndDestroy( 2 );	// db, array
		
	return err;
	}

// ---------------------------------------------------------
// CEmbeddedLinkApp::GetLinkL
//
//
// ---------------------------------------------------------
TInt CEmbeddedLinkApp::GetLinkL( TInt aUid, CFavouritesItem& aFavItem )
	{
	TInt err( KErrNone );

  RFavouritesDb db;
  User::LeaveIfError( db.Open( iFavouritesSess, KBrowserBookmarks ) );
  CleanupClosePushL<RFavouritesDb>( db );

	err=db.Get( aUid, aFavItem );
	db.Close();

	
	CleanupStack::PopAndDestroy();
	return err;
	}

// ---------------------------------------------------------
// CEmbeddedLinkApp::GetLinksL
//
//
// ---------------------------------------------------------
TInt CEmbeddedLinkApp::GetLinksL( CFavouritesItemList& aFavItemList )
	{
	TInt err( KErrNone );
	
	CArrayFixFlat<TInt>* array = new (ELeave) CArrayFixFlat<TInt>( 16 );
	CleanupStack::PushL( array );

  RFavouritesDb db;
  User::LeaveIfError( db.Open( iFavouritesSess, KBrowserBookmarks ) );
  CleanupClosePushL<RFavouritesDb>( db );

	err = db.GetUids( *array, KFavouritesNullUid, CFavouritesItem::EFolder, NULL, iId );	// Get all folder uids related to the app


	if ( err == KErrNone )																	// Then get the links in the folder
		{
		for ( TInt i = 0; i < array->Count(); i++ )
			{
			err = db.GetAll( aFavItemList, array->At( i ), CFavouritesItem::EItem, NULL, KFavouritesNullContextId );	
			}	
		}	

	db.Close();
	
	CleanupStack::PopAndDestroy( 2 ); // array, db
	return err;
	}
	
// ---------------------------------------------------------
// CEmbeddedLinkApp::RemoveLinkL
//
//
// ---------------------------------------------------------
TInt CEmbeddedLinkApp::RemoveLinkL( TInt aUid )
	{
	TInt err = KErrNone;

  RFavouritesDb db;
  User::LeaveIfError( db.Open( iFavouritesSess, KBrowserBookmarks ) );
  CleanupClosePushL<RFavouritesDb>( db );

	err = db.Delete( aUid );
	db.Close();


	CleanupStack::PopAndDestroy(); // db

	return err;
 	}

// ---------------------------------------------------------
// CEmbeddedLinkApp::GetAppNameL
//
//
// ---------------------------------------------------------
TInt CEmbeddedLinkApp::GetAppName( TDes& aName )
	{
	TInt err( KErrNone );

	switch( iId )
		{
		case KFavouritesNullContextId:
		aName.Copy(_L("Unused"));
		break;
		
		case KFavouritesApplicationContextId:
		aName.Copy(_L("Application"));
		break;
		
		case KFavouritesImageContextId:
		aName.Copy(_L("Image"));
		break;
		
		case KFavouritesAudioContextId:
		aName.Copy(_L("Audio"));
		break;
		
		case KFavouritesVideoContextId:
		aName.Copy(_L("Video"));
		break;
		
		case KFavouritesGameDataContextId:
		aName.Copy(_L("GameData"));
		break;
		
		case KFavouritesRingingToneContextId:
		aName.Copy(_L("RingingTone"));
		break;
		
		case KFavouritesSkinContextId:
		aName.Copy(_L("Skin"));
		break;
		
		case KFavouritesMusicContextId:
		aName.Copy(_L("Music"));
		break;

		default:
		err=KErrNotFound;
		break;
		}

	return err;
	}

// ---------------------------------------------------------
// CEmbeddedLinkApp::UpdateUrlL
//
//
// ---------------------------------------------------------
TInt CEmbeddedLinkApp::UpdateUrlL( const TDesC& aUrl, TInt aUid )
	{
	TInt err( KErrNone );

  RFavouritesDb db;
  User::LeaveIfError( db.Open( iFavouritesSess, KBrowserBookmarks ) );
  CleanupClosePushL<RFavouritesDb>( db );

	CFavouritesItem* favItem = CFavouritesItem::NewLC();
	
	err = db.Get( aUid, *favItem );

	if ( err==KErrNone )
		{
		favItem->SetUrlL( aUrl );
		err = db.Update( *favItem, aUid, ETrue );
		}
	
	db.Close();

	CleanupStack::PopAndDestroy( 2 ); //db, favItem
	
	return err;
}

// ---------------------------------------------------------
// CEmbeddedLinkApp::UpdateNameL
//
//
// ---------------------------------------------------------
TInt CEmbeddedLinkApp::UpdateNameL( const TDesC& aName, TInt aUid )
{
	TInt err( KErrNone );

  RFavouritesDb db;
  User::LeaveIfError( db.Open( iFavouritesSess, KBrowserBookmarks ) );
  CleanupClosePushL<RFavouritesDb>( db );

	CFavouritesItem* favItem = CFavouritesItem::NewLC();
	
	err = db.Get( aUid, *favItem );

	if ( err == KErrNone )
		{
		favItem->SetNameL( aName );
		err = db.Update( *favItem, aUid, ETrue );
		}
		
	db.Close();

	CleanupStack::PopAndDestroy( 2 ); //db, favItem
	
	return err;
}

// ---------------------------------------------------------
// CEmbeddedLinkApp::UpdateUserNameL
//
//
// ---------------------------------------------------------
TInt CEmbeddedLinkApp::UpdateUserNameL( const TDesC& aUserName, const TInt aUid )
	{
	TInt err( KErrNone );

  RFavouritesDb db;
  User::LeaveIfError( db.Open( iFavouritesSess, KBrowserBookmarks ) );
  CleanupClosePushL<RFavouritesDb>( db );

	CFavouritesItem* favItem = CFavouritesItem::NewLC();
	
	err = db.Get( aUid, *favItem );

	if ( err == KErrNone )
		{
		favItem->SetUserNameL( aUserName );
		err = db.Update( *favItem, aUid, ETrue );
		}

	db.Close();

	CleanupStack::PopAndDestroy( 2 ); //db, favItem

	return err;
	}

// ---------------------------------------------------------
// CEmbeddedLinkApp::UpdatePasswordL
//
//
// ---------------------------------------------------------
TInt CEmbeddedLinkApp::UpdatePasswordL( const TDesC& aPassword, const TInt aUid )
	{
	TInt err( KErrNone );

  RFavouritesDb db;
  User::LeaveIfError( db.Open( iFavouritesSess, KBrowserBookmarks ) );
  CleanupClosePushL<RFavouritesDb>( db );

	CFavouritesItem* favItem = CFavouritesItem::NewLC();
	
	err = db.Get( aUid, *favItem );

	if ( err == KErrNone )
		{
		favItem->SetPasswordL( aPassword );
		err = db.Update( *favItem, aUid, ETrue );
		}

	db.Close();

	CleanupStack::PopAndDestroy( 2 ); //db, favItem
	
	return err;
	}

// ---------------------------------------------------------
// CEmbeddedLinkApp::UpdateWapApL
//
//
// ---------------------------------------------------------
TInt CEmbeddedLinkApp::UpdateWapApL( const TDesC& aWapAp, const TInt aUid )
	{
	TInt err( KErrNone );

  RFavouritesDb db;
  User::LeaveIfError( db.Open( iFavouritesSess, KBrowserBookmarks ) );
  CleanupClosePushL<RFavouritesDb>( db );

	CFavouritesItem* favItem = CFavouritesItem::NewLC();
	
	err = db.Get( aUid, *favItem );

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

		err = db.Update( *favItem, aUid, ETrue );
		}

	db.Close();

	CleanupStack::PopAndDestroy( 2 ); //db, favItem
	
	return err;
	}

// ---------------------------------------------------------
// CEmbeddedLinkApp::GetURI
//
//
// ---------------------------------------------------------
TPtrC CEmbeddedLinkApp::GetURI()
	{
	return iURI.Ptr();
	}

// ---------------------------------------------------------
// CEmbeddedLinkApp::SetURI
//
//
// ---------------------------------------------------------
TInt CEmbeddedLinkApp::SetURI( const TDesC& aURI )
	{
	iURI.Copy(aURI);
	return KErrNone;
	}

// ---------------------------------------------------------
// CEmbeddedLinkApp::AppId
//
//
// ---------------------------------------------------------
TInt CEmbeddedLinkApp::AppId() const
	{
	return iId;
	}

// ---------------------------------------------------------
// CEmbeddedLinkApp::AppName
//
//
// ---------------------------------------------------------
TPtrC CEmbeddedLinkApp::AppName() const
	{
	return iAppName;
	}

// ---------------------------------------------------------
// CEmbeddedLinkApp::FolderUidL
// ---------------------------------------------------------
TInt CEmbeddedLinkApp::FolderUidL( )
	{
  RFavouritesDb db;
  User::LeaveIfError( db.Open( iFavouritesSess, KBrowserBookmarks ) );
  CleanupClosePushL<RFavouritesDb>( db );

	CArrayFixFlat<TInt>* array = new ( ELeave ) CArrayFixFlat<TInt>( 16 );
	CleanupStack::PushL( array );

	TInt err = KErrNone;

	err = db.GetUids( *array, KFavouritesNullUid, CFavouritesItem::EFolder, NULL, iId );

	User::LeaveIfError( err );

	TInt folderUid = 0;

	if (array->Count() == 0)
		{
//sf-		User::Leave( KErrNotFound );
		}
	else
		{
		folderUid = (*array)[0];
		}

	db.Close();

	CleanupStack::PopAndDestroy( 2 ); //db, array

	return folderUid;
	}
