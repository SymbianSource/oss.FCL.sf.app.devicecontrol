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
* Description:  DM ScreenSaver List
*
*/


#include "ScreenSaverList.h"

#include <implementationproxy.h> // For TImplementationProxy definition
#include <e32def.h>
#include <centralrepository.h>
#include <ScreensaverpluginIntDef.h>
#include <mmfcontrollerpluginresolver.h> // For CleanupResetAndDestroyPushL
#include "aminstalloptions.h"
#include <ScreensaverInternalCRKeys.h>
#include <ScreensaverInternalPSKeys.h>
#include "debug.h"
#include "ScreenSaverClient.h"
#include <e32property.h>
_LIT8( KSlash, "/" );
#include <e32cmn.h>
_LIT( KPslnScreenSaverTypeDateTime, "DateTime" );
_LIT( KPslnScreenSaverTypeText, "Text" );

const TInt KPslnMaxNumberOfScreenSaverText  = 15;
const TInt KPostTimeOut = 10000000; // 10 seconds

// ------------------------------------------------------------------------------------------------
// CScreenSaverList* CScreenSaverList::NewL( )
// ------------------------------------------------------------------------------------------------
CScreenSaverList* CScreenSaverList::NewL( )
	{
	RDEBUG("CScreenSaverList::NewL(): begin");

	CScreenSaverList* self = NewLC( );
	CleanupStack::Pop();
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CScreenSaverList* CScreenSaverList::NewLC( )
// ------------------------------------------------------------------------------------------------
CScreenSaverList* CScreenSaverList::NewLC( )
	{
	CScreenSaverList* self = new( ELeave ) CScreenSaverList( );
	CleanupStack::PushL( self );
	self->ConstructL( );
	return self;
	}


// ------------------------------------------------------------------------------------------------
// CScreenSaverList::CScreenSaverList()
// ------------------------------------------------------------------------------------------------
CScreenSaverList::CScreenSaverList(  )
	: iDirty( ETrue )
    , iScreenSavers( 0 )
	, iInstallOptions()
	{
	GetDefaultInstallOptions( iInstallOptions );
	}

// ------------------------------------------------------------------------------------------------
// CScreenSaverList::ConstructL()
// ------------------------------------------------------------------------------------------------
void CScreenSaverList::ConstructL(  )
	{
	}

// ------------------------------------------------------------------------------------------------
// CScreenSaverList::~CScreenSaverList()
// ------------------------------------------------------------------------------------------------
CScreenSaverList::~CScreenSaverList()
	{
	if (iScreenSavers)
		{
		iScreenSavers->ResetAndDestroy();
		delete iScreenSavers;
		}
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverList::GetScreenSaverListL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError CScreenSaverList::GetScreenSaverListL( const CArrayFix<TSmlDmMappingInfo>& aPreviousList, CBufBase& aList )
	{
	CheckUpdatesL();

	CDesC8ArrayFlat* names = new (ELeave) CDesC8ArrayFlat( 32 );
	CleanupStack::PushL( names );

	CDesC8ArrayFlat* luids = new (ELeave) CDesC8ArrayFlat( 32 );
	CleanupStack::PushL( luids );

	TInt i = 0;

	// Create list of previous screen savers
	for (i = 0 ; i < aPreviousList.Count() ; i++)
		{
		TPtrC8 screenSaverId = ScreenSaverIdFromLUID( aPreviousList[i].iURISegLUID );

		names->AppendL( aPreviousList[i].iURISeg );
		luids->AppendL( screenSaverId );
		}

	// Find and add missing screen savers
	for (i = 0 ; i < iScreenSavers->Count() ; i++)
		{
		TInt idx = 0;
		TPtrC8 name = (*iScreenSavers)[i]->Id();
		if (luids->Find( name, idx ) != 0)
			{
			// not found
			// Check that current "name" is not in use
			if (names->Find( name, idx ) != 0)
				{
				// not found, add it
				names->AppendL( name );
				}
			}
		}


	// Fill result
	for (i = 0 ; i < names->Count() ; i++)
		{
		if (i > 0)
			{
			aList.InsertL( aList.Size(), KSlash );
			}
		aList.InsertL( aList.Size(), (*names)[i] );
		}

	// Cleanup
	CleanupStack::PopAndDestroy( luids );
	CleanupStack::PopAndDestroy( names );

	return MSmlDmAdapter::EOk;

//sf-	TInt i = 0;
//sf-	for (i = 0 ; i < iScreenSavers->Count() ; i++)
//sf-		{
//sf-		if (i > 0)
//sf-			{
//sf-			aList.InsertL( aList.Size(), KSlash );
//sf-			}
//sf-
//sf-	    CScreenSaverInfo *info = (*iScreenSavers)[i];
//sf-		aList.InsertL( aList.Size(), info->Id() );
//sf-		}
//sf-
//sf-
//sf-	return MSmlDmAdapter::EOk;
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverList::GetIdL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError CScreenSaverList::GetIdL( const TDesC8& aId, CBufBase& aResult )
	{
	MSmlDmAdapter::TError ret = MSmlDmAdapter::ENotFound;

	CScreenSaverInfo* screenSaverInfo = FindByIdL( aId );
	if (screenSaverInfo != 0)
		{
		aResult.InsertL( aResult.Size(), screenSaverInfo->Id() );
		ret = MSmlDmAdapter::EOk;
		}

	return ret;
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverList::GetNameL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError CScreenSaverList::GetNameL( const TDesC8& aId, CBufBase& aResult )
	{
	MSmlDmAdapter::TError ret = MSmlDmAdapter::ENotFound;

	CScreenSaverInfo* screenSaverInfo = FindByIdL( aId );
	if (screenSaverInfo != 0)
		{
		aResult.InsertL( aResult.Size(), screenSaverInfo->Name() );
		ret = MSmlDmAdapter::EOk;
		}

	return ret;
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverList::SetNameL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError CScreenSaverList::SetNameL( const TDesC8& aId, const TDesC8& aData )
	{
	MSmlDmAdapter::TError ret = MSmlDmAdapter::ENotFound;

	CScreenSaverInfo* screenSaverInfo = FindByIdL( aId );
	if (screenSaverInfo != 0)
		{
		screenSaverInfo->SetNameL( aData );
		ret = MSmlDmAdapter::EOk;
		}

	return ret;
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverList::GetDescriptionL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError CScreenSaverList::GetDescriptionL( const TDesC8& aId, CBufBase& aResult )
	{
	MSmlDmAdapter::TError ret = MSmlDmAdapter::ENotFound;

	CScreenSaverInfo* screenSaverInfo = FindByIdL( aId );
	if (screenSaverInfo != 0)
		{
		aResult.InsertL( aResult.Size(), screenSaverInfo->Description() );
		ret = MSmlDmAdapter::EOk;
		}

	return ret;
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverList::SetDescriptionL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError CScreenSaverList::SetDescriptionL( const TDesC8& aId, const TDesC8& aData )
	{
	MSmlDmAdapter::TError ret = MSmlDmAdapter::ENotFound;

	CScreenSaverInfo* screenSaverInfo = FindByIdL( aId );
	if (screenSaverInfo != 0)
		{
		screenSaverInfo->SetDescriptionL( aData );
		ret = MSmlDmAdapter::EOk;
		}

	return ret;
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverList::GetCustomPropertyL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError CScreenSaverList::GetCustomPropertyL( const TDesC8& aId, CBufBase& aResult )
	{
	MSmlDmAdapter::TError ret = MSmlDmAdapter::ENotFound;

	CScreenSaverInfo* screenSaverInfo = FindByIdL( aId );
	if (screenSaverInfo != 0)
		{
		aResult.InsertL( aResult.Size(), screenSaverInfo->CustomProperty() );
		ret = MSmlDmAdapter::EOk;
		}

	return ret;
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverList::SetCustomPropertyL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError CScreenSaverList::SetCustomPropertyL( const TDesC8& aId, const TDesC8& aData )
	{
	MSmlDmAdapter::TError ret = MSmlDmAdapter::ENotFound;

	CScreenSaverInfo* screenSaverInfo = FindByIdL( aId );
	if (screenSaverInfo != 0)
		{
		screenSaverInfo->SetCustomPropertyL( aData );
		ret = MSmlDmAdapter::EOk;
		}

	return ret;
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverList::GetVersionL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError CScreenSaverList::GetVersionL( const TDesC8& aId, CBufBase& aResult )
	{
	MSmlDmAdapter::TError ret = MSmlDmAdapter::ENotFound;

	CScreenSaverInfo* screenSaverInfo = FindByIdL( aId );
	if (screenSaverInfo != 0)
		{
		aResult.InsertL( aResult.Size(), screenSaverInfo->Version() );
		ret = MSmlDmAdapter::EOk;
		}

	return ret;
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverList::SetVersionL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError CScreenSaverList::SetVersionL( const TDesC8& aId, const TDesC8& aVersion )
	{
	MSmlDmAdapter::TError ret = MSmlDmAdapter::ENotFound;

	CScreenSaverInfo* screenSaverInfo = FindByIdL( aId );
	if (screenSaverInfo != 0)
		{
		screenSaverInfo->SetVersionL( aVersion );
		ret = MSmlDmAdapter::EOk;
		}

	return ret;
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverList::GetActiveL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError CScreenSaverList::GetActiveL( CBufBase& aResult )
	{
	MSmlDmAdapter::TError ret = MSmlDmAdapter::EOk;

    TInt screenSaverTypeId = 0; // Text
    TFileName screenSaverFileName;

    CRepository* repository = CRepository::NewLC( KCRUidScreenSaver );
    TInt error = repository->Get( KScreenSaverObject, screenSaverTypeId );

    if( error == KErrNone )
    	{
		if( screenSaverTypeId == 0 ) // Text
	        {
			screenSaverFileName = KPslnScreenSaverTypeText;
	        }
	    else if( screenSaverTypeId == 1 ) // Date&time
	        {
			screenSaverFileName = KPslnScreenSaverTypeDateTime;
	        }
		else if( screenSaverTypeId == 3 )// Object
			{
			error = repository->Get( KScreenSaverPluginName, screenSaverFileName );
			}
		else
			{
			error = KErrUnknown;
			}
    	}

    CleanupStack::PopAndDestroy( repository );

    if( error == KErrNone )
    	{
		TBuf8<KMaxFileName> screenSaverFileName8;
		screenSaverFileName8.Copy( screenSaverFileName );

		aResult.InsertL( aResult.Size(), screenSaverFileName8 );
    	}
    else
    	{
    	ret = MSmlDmAdapter::EError;
    	}

	return ret;
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverList::SetActiveL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError CScreenSaverList::SetActiveL( const TDesC8& aId )
	{
	MSmlDmAdapter::TError ret = MSmlDmAdapter::EOk;

	CScreenSaverInfo* screenSaverInfo = FindByIdL( aId );
	if (screenSaverInfo != 0)
		{
		screenSaverInfo->ActivateL();
		}
	else
		{
		ret = MSmlDmAdapter::EError;
		}

	return ret;
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverList::SetInstallOptionsL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError CScreenSaverList::SetInstallOptionsL( const TDesC8& aOptions )
	{
	MSmlDmAdapter::TError ret = MSmlDmAdapter::EOk;
	TInt errCode = KErrNone;

	GetDefaultInstallOptions( iInstallOptions );

	if (aOptions.Length() > 0)
		{
		TRAP( errCode, InstallOptionsParser::ParseOptionsL( aOptions, iInstallOptions ) );
		}
	if (errCode != KErrNone)
		{
		ret = MSmlDmAdapter::EError;
		}

	return ret;
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverList::InstallL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError CScreenSaverList::InstallL( const TDesC& aFileName )
	{
	MSmlDmAdapter::TError ret = MSmlDmAdapter::EOk;
	TInt errCode = KErrNone;

	iDirty = ETrue;

	// Install using options if given and otherwise with default options
	SwiUI::RSWInstLauncher installer;
	User::LeaveIfError( installer.Connect() );
	CleanupClosePushL( installer );

	SwiUI::TInstallOptionsPckg optPckg( iInstallOptions );
	errCode = installer.SilentInstall( aFileName, optPckg );
	CleanupStack::PopAndDestroy( &installer );

	if (errCode != KErrNone)
		{
		ret = MSmlDmAdapter::EError;
		}

	return ret;
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverList::DeleteL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError CScreenSaverList::DeleteL( const TDesC8& aLUID )
	{
	MSmlDmAdapter::TError ret = MSmlDmAdapter::EOk;
	TInt errCode = KErrNone;

	CScreenSaverInfo* screenSaverInfo = FindByIdL( aLUID );
	if( screenSaverInfo != NULL )
		{
	    CRepository* repository = CRepository::NewLC( KCRUidScreenSaver );

	    if( screenSaverInfo->TypeId() == CScreenSaverInfo::EObject )
	        {
		    TFileName screenSaverFileName;
		    screenSaverFileName.Copy( screenSaverInfo->FileName() );
		    
		    TFileName repFileName;
			repository->Get( KScreenSaverPluginName, repFileName );
			if( repFileName == screenSaverFileName )
				{
				TBuf<KPslnMaxNumberOfScreenSaverText> screenText;
				screenText.Copy( screenSaverInfo->CustomProperty() );
		        errCode = repository->Set( KScreenSaverText, screenText );
				errCode = repository->Set( KScreenSaverObject, CScreenSaverInfo::EText );  
				
				TRequestStatus timeoutStatus /*= KRequestPending*/;
				RTimer timeout;
				User::LeaveIfError( timeout.CreateLocal() );
				CleanupClosePushL( timeout );
				timeout.After( timeoutStatus, KPostTimeOut );

				User::WaitForRequest( timeoutStatus );

				if( timeoutStatus.Int() == KRequestPending)
					{
					timeout.Cancel();
					User::WaitForRequest( timeoutStatus );
					}
				CleanupStack::PopAndDestroy( &timeout );
				     
				}
	        }
	    CleanupStack::PopAndDestroy( repository );
		
		}
	else
		{
		errCode = KErrNotFound;
		}

	if( errCode == KErrNone )
		{
		iDirty = ETrue;

		TUint pkgUid = 0;
		TLex8 parser( aLUID );
		if ((parser.Get() == '[') &&
			(parser.Val( pkgUid, EHex ) == KErrNone) &&
			(parser.Get() == ']'))
			{
			// Uninstall silent
			SwiUI::RSWInstLauncher installer;
			User::LeaveIfError( installer.Connect() );
			CleanupClosePushL( installer );

			SwiUI::TUninstallOptions uninstallOptions;
			SwiUI::TUninstallOptionsPckg optPckg( uninstallOptions );

			errCode = installer.SilentUninstall( TUid::Uid( pkgUid ), optPckg, SwiUI::KSisxMimeType() );

			CleanupStack::PopAndDestroy( &installer );
			}
		}
		
	if (errCode == KErrNotFound)
		{
		ret = MSmlDmAdapter::ENotFound;
		}
	else if (errCode != KErrNone)
		{
		ret = MSmlDmAdapter::EError;
		}

	return ret;
	}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Internal methods
//
///////////////////////////////////////////////////////////////////////////////////////////////////


// ------------------------------------------------------------------------------------------------
//  CScreenSaverList::CheckUpdatesL()
// ------------------------------------------------------------------------------------------------
void CScreenSaverList::CheckUpdatesL()
	{
	if (iDirty)
		{
		LoadL();
		iDirty = EFalse;
		}
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverList::LoadL()
// ------------------------------------------------------------------------------------------------
void CScreenSaverList::LoadL()
	{
	CScreenSaverInfo* entry = 0;
    // Add descriptors for system screensavers
    delete iScreenSavers;
    iScreenSavers = 0;
    iScreenSavers = new (ELeave) CArrayPtrFlat< CScreenSaverInfo >( 8 );

	entry = CScreenSaverInfo::NewLC( KPslnScreenSaverTypeDateTime );
    iScreenSavers->AppendL( entry );
    CleanupStack::Pop( entry );

	entry = CScreenSaverInfo::NewLC( KPslnScreenSaverTypeText );
    iScreenSavers->AppendL( entry );
    CleanupStack::Pop( entry );

    // The path variable is not used
    // The signature of this function could be changed in later releases
    LoadPluginsL( KNullDesC );
//sf-    LoadPluginsL( KPslnScreensaverPathROM );
//sf-    LoadPluginsL( KPslnScreensaverPathLFFS );
//sf-    LoadPluginsL( KPslnScreensaverPathMMC );
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverList::LoadPluginsL()
// ------------------------------------------------------------------------------------------------
TBool CScreenSaverList::LoadPluginsL( const TDesC& /*aPath*/ )
	{
	RDEBUG(" CScreenSaverList: LoadPluginsL ... <");
    TBool found = EFalse;
    
    RScreenSaverClient ssClient;
    
    User::LeaveIfError( ssClient.Connect() );
	CleanupClosePushL( ssClient );
	
	RSSListInfoPtrArray sslistinfo;
	CleanupResetAndDestroyPushL( sslistinfo );
	
	ssClient.GetScreenSaverListInfoL(sslistinfo );
	const TInt count = sslistinfo.Count();
	RDEBUG_2(" CScreenSaverList: LoadPluginsL : SScount is :%d",count);
	
	for( TInt i = 0 ; i < count ; i++ )
        {
        CScreenSaverListInfo* implInfo = sslistinfo[i];
        
		CScreenSaverInfo* entry = CScreenSaverInfo::NewLC(
		implInfo->ImplementationUid().Name() );
		entry->SetNameL( implInfo->DisplayName() );

		TBuf8<20> version;
		version.Num( implInfo->Version() );
		entry->SetVersionL( version );
		entry->SetCustomPropertyL( implInfo->OpaqueData() );

		iScreenSavers->AppendL( entry );
		CleanupStack::Pop( entry );

        found = ETrue;
        }
	
	CleanupStack::PopAndDestroy( &sslistinfo );
	CleanupStack::PopAndDestroy( &ssClient );
	RDEBUG(" CScreenSaverList: LoadPluginsL ... >");
	return found;
    }

// ------------------------------------------------------------------------------------------------
//  CScreenSaverList::
// ------------------------------------------------------------------------------------------------
CScreenSaverInfo* CScreenSaverList::FindByIdL( const TDesC8& aId )
	{
	CheckUpdatesL();

	CScreenSaverInfo* entry = 0;
	TInt i = 0;
	TPtrC8 id = ScreenSaverIdFromLUID( aId );
	if (id.Length() == 0)
		{
		id.Set( aId );
		}

	for (i = 0 ; i < iScreenSavers->Count() ; i++)
		{
		if (id == (*iScreenSavers)[i]->Id())
			{
			entry = (*iScreenSavers)[i];
			break;
			}
		}

	return entry;
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverList::ScreenSaverIdFromLUID()
// ------------------------------------------------------------------------------------------------
TPtrC8 CScreenSaverList::ScreenSaverIdFromLUID( const TDesC8& aLUID )
	{
	TInt pos = aLUID.Locate( KPkgId_ScrId_Separator );
	if (pos == KErrNotFound)
		{
		pos = aLUID.Length() - 1;
		}
	return aLUID.Mid( pos + 1 );
	}


// ------------------------------------------------------------------------------------------------
//  CScreenSaverList::GetDefaultInstallOptions()
// ------------------------------------------------------------------------------------------------
void CScreenSaverList::GetDefaultInstallOptions( SwiUI::TInstallOptions &aOptions )
	{
	SwiUI::TInstallOptions defaultOptions;

	// Return options
	aOptions = defaultOptions;
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverList::
// ------------------------------------------------------------------------------------------------

//=================================================================================================
//=================================================================================================
//====
//==== CScreenSaverInfo
//====
//=================================================================================================
//=================================================================================================

// ------------------------------------------------------------------------------------------------
//  CScreenSaverInfo::NewL()
// ------------------------------------------------------------------------------------------------
CScreenSaverInfo* CScreenSaverInfo::NewL( const TDesC& aId )
	{
	RDEBUG("CScreenSaverInfo::NewL(): begin");

	CScreenSaverInfo* self = NewLC( aId );
	CleanupStack::Pop();
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CScreenSaverInfo* CScreenSaverInfo::NewLC()
// ------------------------------------------------------------------------------------------------
CScreenSaverInfo* CScreenSaverInfo::NewLC( const TDesC& aId )
	{
	CScreenSaverInfo* self = new( ELeave ) CScreenSaverInfo( );
	CleanupStack::PushL( self );
	self->ConstructL( aId );
	return self;
	}


// ------------------------------------------------------------------------------------------------
//  CScreenSaverInfo::
// ------------------------------------------------------------------------------------------------
CScreenSaverInfo::CScreenSaverInfo()
	: iId( 0 )
	, iName( 0 )
	, iDescription( 0 )
	, iCustomProperty( 0 )
	, iVersion( 0 )
	{
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverInfo::
// ------------------------------------------------------------------------------------------------
void CScreenSaverInfo::ConstructL( const TDesC& aId )
	{
	iId = HBufC8::NewL( aId.Length() );
	iId->Des().Copy( aId );
	iName = KNullDesC8().AllocL();
	iDescription = KNullDesC8().AllocL();
	iCustomProperty = KNullDesC8().AllocL();
	iVersion = KNullDesC8().AllocL();
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverInfo::
// ------------------------------------------------------------------------------------------------
CScreenSaverInfo::~CScreenSaverInfo()
	{
	delete iId;
	delete iName;
	delete iDescription;
	delete iCustomProperty;
	delete iVersion;
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverInfo::
// ------------------------------------------------------------------------------------------------
const TDesC8& CScreenSaverInfo::Id()
	{
	return *iId;
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverInfo::
// ------------------------------------------------------------------------------------------------
const TDesC8& CScreenSaverInfo::Name()
	{
	return *iName;
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverInfo::
// ------------------------------------------------------------------------------------------------
void CScreenSaverInfo::SetNameL( const TDesC8& aName )
	{
	delete iName;
	iName = 0;
	iName = aName.AllocL();
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverInfo::
// ------------------------------------------------------------------------------------------------
void CScreenSaverInfo::SetNameL( const TDesC& aName )
	{
	delete iName;
	iName = 0;
	iName = HBufC8::NewL( aName.Length() );
	iName->Des().Copy( aName );
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverInfo::
// ------------------------------------------------------------------------------------------------
const TDesC8& CScreenSaverInfo::Description()
	{
	return *iDescription;
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverInfo::
// ------------------------------------------------------------------------------------------------
void CScreenSaverInfo::SetDescriptionL( const TDesC8& aDescription )
	{
	delete iDescription;
	iDescription = 0;
	iDescription = aDescription.AllocL();
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverInfo::
// ------------------------------------------------------------------------------------------------
const TDesC8& CScreenSaverInfo::CustomProperty()
	{
	return *iCustomProperty;
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverInfo::
// ------------------------------------------------------------------------------------------------
void CScreenSaverInfo::SetCustomPropertyL( const TDesC8& aCustomProperty )
	{
	delete iCustomProperty;
	iCustomProperty = 0;
	iCustomProperty = aCustomProperty.AllocL();
	// Update settings if needed
	if (IsActivatedL() && TypeId() == EText)
		{
		ActivateL();
		}
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverInfo::
// ------------------------------------------------------------------------------------------------
const TDesC8& CScreenSaverInfo::Version()
	{
	return *iVersion;
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverInfo::
// ------------------------------------------------------------------------------------------------
void CScreenSaverInfo::SetVersionL( const TDesC8& aVersion )
	{
	delete iVersion;
	iVersion = 0;
	iVersion = aVersion.AllocL();
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverInfo::
// ------------------------------------------------------------------------------------------------
CScreenSaverInfo::TScreenSaverType CScreenSaverInfo::TypeId()
	{
	TScreenSaverType screenSaverTypeId = EText;
	TFileName fileName;
	fileName.Copy( FileName() );

    if( fileName.CompareF( KPslnScreenSaverTypeText ) == 0 )
        {
        screenSaverTypeId = EText; // 0=Text
        }
    else if( fileName.CompareF( KPslnScreenSaverTypeDateTime ) == 0 )
        {
        screenSaverTypeId = EDateTime; // 1=Date&time
        }
	else
		{
		screenSaverTypeId = EObject; // 3=Object
		}

	return screenSaverTypeId;
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverInfo::
// ------------------------------------------------------------------------------------------------
TBool CScreenSaverInfo::IsActivatedL()
	{
	TBool activated = EFalse;

    TInt screenSaverTypeId = 0; // Text
    TFileName screenSaverFileName;

    CRepository* repository = CRepository::NewLC( KCRUidScreenSaver );

    TInt error = repository->Get( KScreenSaverObject, screenSaverTypeId );
    if( screenSaverTypeId == 3 )
        {
		repository->Get( KScreenSaverPluginName, screenSaverFileName );
        }
    CleanupStack::PopAndDestroy( repository );

    if( screenSaverTypeId == 0 && TypeId() == EText ) // Text
        {
		activated = ETrue;
        }
    else if( screenSaverTypeId == 1 && TypeId() == EDateTime ) // Date&time
        {
		activated = ETrue;
        }
	else if( screenSaverTypeId == 3 && TypeId() == EObject )// Object
		{
		TFileName fileName;
		fileName.Copy( FileName() );
		activated = ( fileName.CompareF( screenSaverFileName ) == 0 );
		}

	return activated;
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverInfo::
// ------------------------------------------------------------------------------------------------
void CScreenSaverInfo::ActivateL()
	{
    CRepository* repository = CRepository::NewLC( KCRUidScreenSaver );


	if( TypeId() == EText )
        {
		TBuf<KPslnMaxNumberOfScreenSaverText> screenText;
		screenText.Copy( CustomProperty() );
        repository->Set( KScreenSaverText, screenText );
        }
	else if( TypeId() == EDateTime )
        {
//sf-		TBuf<KPslnMaxNumberOfScreenSaverText> screenText;
//sf-		screenText.Copy( CustomProperty() );
//sf-        error = repository->Set( KScreenSaverText, screenText );
        }
    else if( TypeId() == EObject )
        {
	    TFileName screenSaverFileName;
	    screenSaverFileName.Copy( FileName() );
		repository->Set( KScreenSaverPluginName, screenSaverFileName );
        }
    repository->Set( KScreenSaverObject, TypeId() );
    CleanupStack::PopAndDestroy( repository );
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverInfo::
// ------------------------------------------------------------------------------------------------
const TDesC8& CScreenSaverInfo::FileName()
	{
	return *iId;
	}

// ------------------------------------------------------------------------------------------------
// EOF
// ------------------------------------------------------------------------------------------------
