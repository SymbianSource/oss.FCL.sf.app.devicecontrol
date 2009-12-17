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
* Description:  DM Themes List
*
*/


#include "ThemesList.h"

#include <badesca.h>
#include <implementationproxy.h> // For TImplementationProxy definition
#include <e32def.h>
#include <centralrepository.h>
#include <AknSkinsInternalCRKeys.h>
#include <sisregistrysession.h>
#include <sisregistrypackage.h>

#include "FileCoderB64.h"
#include "aminstalloptions.h"
#include "debug.h"

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif //RD_MULTIPLE_DRIVEs

_LIT8( KSlash, "/" );
_LIT8(KCol,":");
const TInt KPostTimeOut = 10000000; // 10 seconds


// ------------------------------------------------------------------------------------------------
// CThemesList* CThemesList::NewL( )
// ------------------------------------------------------------------------------------------------
CThemesList* CThemesList::NewL( ) //sf-MSmlDmCallback* aDmCallback )
	{
	RDEBUG("CThemesList::NewL(): begin");

	CThemesList* self = NewLC();
	CleanupStack::Pop();
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CThemesList* CThemesList::NewLC( )
// ------------------------------------------------------------------------------------------------
CThemesList* CThemesList::NewLC( ) //sf-MSmlDmCallback* aDmCallback )
	{
	CThemesList* self = new( ELeave ) CThemesList();
	CleanupStack::PushL( self );
	self->ConstructL( );
	return self;
	}


// ------------------------------------------------------------------------------------------------
// CThemesList::CThemesList()
// ------------------------------------------------------------------------------------------------
CThemesList::CThemesList( ) //sf-MSmlDmCallback* aDmCallback )
//sf-	: iCallBack( aDmCallback )
	: iDirty( ETrue )
    , iAknsSrv( )
    , iSkins( 0 )
	, iInstallOptions()
	{
	GetDefaultInstallOptions( iInstallOptions );
	}

// ------------------------------------------------------------------------------------------------
// CThemesList::ConstructL()
// ------------------------------------------------------------------------------------------------
void CThemesList::ConstructL(  )
	{
	User::LeaveIfError( iAknsSrv.Connect() );
	}

// ------------------------------------------------------------------------------------------------
// CThemesList::~CThemesList()
// ------------------------------------------------------------------------------------------------
CThemesList::~CThemesList()
	{
	if (iSkins)
		{
		iSkins->ResetAndDestroy();
		delete iSkins;
		}
	iAknsSrv.Close();
	}

// ------------------------------------------------------------------------------------------------
//  CThemesList::GetInstalledPIDListL()
// ------------------------------------------------------------------------------------------------

MSmlDmAdapter::TError CThemesList::GetInstalledPIDListL( const CArrayFix<TSmlDmMappingInfo>& aPreviousList, CDesC8ArrayFlat& themes, CDesC8Array& aNewThemes )
	{
	TInt i = 0;
	TInt pos = 0;

	CheckUpdatesL();

	CDesC8ArrayFlat* luids = new (ELeave) CDesC8ArrayFlat( 32 );
	CleanupStack::PushL( luids );

	// Create list of existing themes
	for (i = 0 ; i < aPreviousList.Count() ; i++)
		{
		themes.AppendL( aPreviousList[i].iURISeg );
		luids->AppendL( aPreviousList[i].iURISegLUID );
		}

	// Find and add missing themes

	for (i = 0 ; i < iSkins->Count() ; i++)
		{
		TAknsPkgIDBuf pidBuf;
		TBuf8<KSmlMaxURISegLen> name;

        TAknsPkgID pid = (*iSkins)[i]->PID();
        pid.CopyToDes( pidBuf );
        name.Copy( pidBuf );

		if (luids->Find( name, pos ) != 0)
			{
			// not found
			// Check that current "name" is not in use
			if (themes.Find( name, pos ) != 0)
				{
				// not found
				// add it to both current and new themes
			//	themes->AppendL( name );
				aNewThemes.AppendL( name );
				}
			}
		}

	// Fill result
#if 0
	for (i = 0 ; i < themes->Count() ; i++)
		{
		if (i > 0)
			{
			aList.InsertL( aList.Size(), KSlash );
			}
		aList.InsertL( aList.Size(), (*themes)[i] );
		}

#endif
	// Cleanup
	CleanupStack::PopAndDestroy( luids );
	//CleanupStack::PopAndDestroy( themes );

	return MSmlDmAdapter::EOk;
	}

// ------------------------------------------------------------------------------------------------
//  CThemesList::GetThemeIdL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError CThemesList::GetThemeIdL( const TDesC8& aPID, CBufBase& aResult )
	{
	MSmlDmAdapter::TError ret = MSmlDmAdapter::ENotFound;

	CAknsSrvSkinInformationPkg* theme = FindThemeL( aPID );
	if (theme != 0)
		{
		TAknsPkgIDBuf pid;
		theme->PID().CopyToDes( pid );
		TBuf8<KAknsPkgIDDesSize> pid8;
		pid8.Copy( pid );
		aResult.InsertL( aResult.Size(), pid8 );
		ret = MSmlDmAdapter::EOk;
		}

	return ret;
	}

// ------------------------------------------------------------------------------------------------
//  CThemesList::GetThemeNameL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError CThemesList::GetThemeNameL( const TDesC8& aPID, CBufBase& aResult )
	{
	MSmlDmAdapter::TError ret = MSmlDmAdapter::ENotFound;

	CAknsSrvSkinInformationPkg* theme = FindThemeL( aPID );
	if (theme != 0)
		{
		TDesC& name = theme->Name();
		HBufC8* buf = HBufC8::NewLC( name.Length() );
		buf->Des().Copy( name );
		aResult.InsertL( aResult.Size(), *buf );
		CleanupStack::PopAndDestroy( buf );
		ret = MSmlDmAdapter::EOk;
		}

	return ret;
	}

// ------------------------------------------------------------------------------------------------
//  CThemesList::GetThemeVersionL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError CThemesList::GetThemeVersionL( const TDesC8& aPID, CBufBase& /*aResult*/ )
	{
	MSmlDmAdapter::TError ret = MSmlDmAdapter::ENotFound;

	CAknsSrvSkinInformationPkg* theme = FindThemeL( aPID );
	if (theme != 0)
		{
		// Empty version, not supported (yet?)
		ret = MSmlDmAdapter::EOk;
		}

	return ret;
	}

// ------------------------------------------------------------------------------------------------
//  CThemesList::GetActiveThemeL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError CThemesList::GetActiveThemeL( CBufBase& aResult )
	{
	MSmlDmAdapter::TError ret = MSmlDmAdapter::EOk;
	TInt errCode = KErrNone;
	TAknsPkgID pid = KAknsNullPkgID;
    TAknsPkgIDBuf buf;

    CRepository* skinsRepository = CRepository::NewLC( KCRUidPersonalisation );

    errCode = skinsRepository->Get( KPslnActiveSkinUid, buf );
    if( errCode == KErrNone )
        {
        TRAP( errCode, pid.SetFromDesL( buf ) );
        }
    if( errCode != KErrNone )
        {
//sf-        pid.Set( KAknsPIDDefaultSystemSkin );
//sf- changes to KAknsPIDS60DefaltSystemSkin and
//sf-         to KAknsPIDProductDefaultSystemSkin
//sf- and the product default should be used (because variation might be done).
//sf- Starting from S60 3.1 wk50 or wk52
//sf- But: CenRep key should always hold a value so this code is not necessary.
        }

    pid.CopyToDes( buf );

	CleanupStack::PopAndDestroy( skinsRepository );
	skinsRepository = 0;

	TBuf8<KAknsPkgIDDesSize> buf8;
	buf8.Copy( buf );

	aResult.InsertL( aResult.Size(), buf8 );

	return ret;
	}

// ------------------------------------------------------------------------------------------------
//  CThemesList::SetActiveThemeL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError CThemesList::SetActiveThemeL( const TDesC8& aPID )
	{
	MSmlDmAdapter::TError ret = MSmlDmAdapter::EOk;
	TInt errCode = KErrNone;

	CAknsSrvSkinInformationPkg* theme = FindThemeL( aPID );
	if (theme == 0)
		{
		errCode = KErrNotFound;
		}
	if (errCode == KErrNone)
		{
		errCode = iAknsSrv.SetAllDefinitionSets( theme->PID() );
		}
	if (errCode == KErrNone)
		{
		// Save settings to repository
        TAknsPkgIDBuf pidBuf;
        theme->PID().CopyToDes( pidBuf );

		TAknSkinSrvSkinPackageLocation location =
			LocationFromPath( theme->IniFileDirectory() );

		CRepository* skinsRepository = CRepository::NewLC( KCRUidPersonalisation );

        errCode = skinsRepository->Set( KPslnActiveSkinUid, pidBuf );
        errCode = skinsRepository->Set( KPslnActiveSkinLocation, location );

        CleanupStack::PopAndDestroy( skinsRepository );
		}

	if (errCode != KErrNone)
		{
		ret = MSmlDmAdapter::EError;
		}

	return ret;
	}

// ------------------------------------------------------------------------------------------------
//  CThemesList::SetInstallOptionsL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError CThemesList::SetInstallOptionsL( const TDesC8& aOptions )
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
//  CThemesList::InstallThemeL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError CThemesList::InstallThemeL( const TDesC& aFileName, TDes8& aSisPkgUid, TDes8& aThemePkgId )
	{
	MSmlDmAdapter::TError ret = MSmlDmAdapter::EOk;
	TInt errCode = KErrNone;

	CDesC8Array* pkgUidsOrg = InstalledPackagesL( );
	CleanupStack::PushL( pkgUidsOrg );
	CDesC8Array* thmPidsOrg = InstalledThemesL( );
	CleanupStack::PushL( thmPidsOrg );

	// Install using options if given and otherwise with default options
	SwiUI::RSWInstLauncher installer;
	User::LeaveIfError( installer.Connect() );
	CleanupClosePushL( installer );

	SwiUI::TInstallOptionsPckg optPckg( iInstallOptions );
	errCode = installer.SilentInstall( aFileName, optPckg );
	CleanupStack::PopAndDestroy( &installer );

	// Retrieve new list of installed packages and themes
	CDesC8Array* pkgUidsNew = InstalledPackagesL( );
	CleanupStack::PushL( pkgUidsNew );
	CDesC8Array* thmPidsNew = InstalledThemesL( );
	CleanupStack::PushL( thmPidsNew );

	// Try to find sis package uid and theme package id
	CDesC8Array* pkgUidsDif = FindNewItemsL( *pkgUidsOrg, *pkgUidsNew );
	CleanupStack::PushL( pkgUidsDif );

	CDesC8Array* thmPidsDif = FindNewItemsL( *thmPidsOrg, *thmPidsNew );
	CleanupStack::PushL( thmPidsDif );

	if (pkgUidsDif->Count() == 1)
		{
		aSisPkgUid = (*pkgUidsDif)[0];
		}
	if (thmPidsDif->Count() > 0)
		{
		aThemePkgId = (*thmPidsDif)[0];
		}
	RDEBUG_2( " CThemesList::InstallThemeL : SISPKGUID %X", &(aSisPkgUid) );
	RDEBUG_2( " CThemesList::InstallThemeL : THEMEPKGUID %X", &(aThemePkgId) );

	// Cleanup
	CleanupStack::PopAndDestroy( thmPidsDif );
	CleanupStack::PopAndDestroy( pkgUidsDif );
	CleanupStack::PopAndDestroy( thmPidsNew );
	CleanupStack::PopAndDestroy( pkgUidsNew );
	CleanupStack::PopAndDestroy( thmPidsOrg );
	CleanupStack::PopAndDestroy( pkgUidsOrg );

	// Convert error code
	if (errCode != KErrNone)
		{
		ret = MSmlDmAdapter::EError;
		}

	return ret;
	}

// ------------------------------------------------------------------------------------------------
//  CThemesList::DeleteThemeL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError CThemesList::DeleteThemeL( const TDesC8& aThemeId, const TUid& aPkgUid,HBufC8* aLuid)
	{
	MSmlDmAdapter::TError ret = MSmlDmAdapter::EOk;
	TInt errCode = KErrNone;
	
	CAknsSrvSkinInformationPkg* theme = FindThemeL( aThemeId );
	if (theme != NULL)
		{
		// Save settings to repository
        TAknsPkgIDBuf pidBuf;
        TAknsPkgID themepid = theme->PID();
        themepid.CopyToDes(pidBuf);
                
        CRepository* skinsRepository = CRepository::NewLC(
                KCRUidPersonalisation);

        TAknsPkgIDBuf repBuf;
        errCode = skinsRepository->Get(KPslnActiveSkinUid, repBuf);
        if (errCode == KErrNone)
            {
            TAknsPkgID activepid = KAknsNullPkgID;
            activepid.SetFromDesL(repBuf);

            //Comparing the UIds of the active theme and the theme to be deleted
            if (themepid.Uid() == activepid.Uid())
                {
                // deleted theme is active theme, deactive it first
                TInt count = iSkins->Count();
                if (count > 1)
                    {
                    pidBuf.Zero();
                    CAknsSrvSkinInformationPkg* defaultTheme = (*iSkins)[0];
                    defaultTheme->PID().CopyToDes(pidBuf);
                    TBuf8<KAknsPkgIDDesSize> pidBuf8;
                    pidBuf8.Copy(pidBuf);
                    errCode = SetActiveThemeL(pidBuf8);

                    if (errCode == KErrNone)
                        {
                        TRequestStatus timeoutStatus /*= KRequestPending*/;

                        RTimer timeout;
                        User::LeaveIfError(timeout.CreateLocal());
                        CleanupClosePushL(timeout);
                        timeout.After(timeoutStatus, KPostTimeOut);

                        User::WaitForRequest(timeoutStatus);

                        if (timeoutStatus.Int() == KRequestPending)
                            {
                            timeout.Cancel();
                            User::WaitForRequest(timeoutStatus);
                            }
                        CleanupStack::PopAndDestroy(&timeout);
                        }
                    }
                else
                    {
                    errCode = KErrGeneral;
                    }
                }

            CleanupStack::PopAndDestroy(skinsRepository);
            }

        }
    else
        {
        errCode = KErrNotFound;
        }
	

	if( errCode == KErrNone )
		{
		iDirty = ETrue;

		// Uninstall silent
		SwiUI::RSWInstLauncher installer;
		User::LeaveIfError( installer.Connect() );
		CleanupClosePushL( installer );
		
		//Silent uninstall not working because of getting wrong package name
		//So we used SilentCustomUninstall which uses  index and pkgUid to get correct package name
		
		TInt idx( KErrNotFound );
		SwiUI::TOpUninstallIndexParam params;
        params.iUid = aPkgUid;
        TPtrC8 ptr1 = aLuid->Des();
       	idx= ptr1.Find(KCol);
       	if(idx != KErrNotFound)
       		{
        	TPtrC8 ptr = ptr1.Right((ptr1.Length()-idx)-1);
        	TLex8 lex(ptr); 
			lex.Val(params.iIndex); 
       		}
       	else
       		{
       	    ret = MSmlDmAdapter::ENotFound;
			}
         	 
        SwiUI::TOpUninstallIndexParamPckg pckg( params );
		SwiUI::TOperation operation( SwiUI::EOperationUninstallIndex );
		
		SwiUI::TUninstallOptions uninstallOptions;
		SwiUI::TUninstallOptionsPckg optPckg( uninstallOptions );

  		errCode = installer.SilentCustomUninstall( operation, optPckg, pckg,SwiUI::KSisxMimeType() );
		RDEBUG_2("CThemesList::DeleteThemeL Uninstall silent :  errCode %X",errCode );

		CleanupStack::PopAndDestroy( &installer );
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
//  CThemesList::CheckUpdatesL()
// ------------------------------------------------------------------------------------------------
void CThemesList::CheckUpdatesL()
	{
	if (iDirty)
		{
		LoadSkinsL();
		iDirty = EFalse;
		}
	}

// ------------------------------------------------------------------------------------------------
//  CThemesList::LoadSkinsL()
// ------------------------------------------------------------------------------------------------
void CThemesList::LoadSkinsL()
	{
	// Cleanup old list
	if (iSkins)
		{
		iSkins->ResetAndDestroy();
		delete iSkins;
		iSkins = 0;
		}

    // Drives z and c case EAknsSrvPhone: 
    // drive e case EAknsSrvMMC: 
    // all drives case EAknsSrvAll: 
    iSkins = iAknsSrv.EnumerateSkinPackagesL( EAknsSrvAll );
	}

// ------------------------------------------------------------------------------------------------
//  CThemesList::
// ------------------------------------------------------------------------------------------------
CAknsSrvSkinInformationPkg* CThemesList::FindThemeL( const TDesC8& aPID )
	{
	CheckUpdatesL();

	CAknsSrvSkinInformationPkg* theme = 0;
	TInt i = 0;
	TAknsPkgID pid = KAknsNullPkgID;

	TAknsPkgIDBuf pidBuf;
	pidBuf.Copy( aPID );

	TRAPD( err, pid.SetFromDesL( pidBuf ) );

	if (err == KErrNone)
		{
		for (i = 0 ; i < iSkins->Count() ; i++)
			{
			if ((*iSkins)[i]->PID() == pid)
				{
				theme = (*iSkins)[i];
				break;
				}
			}
		}

	return theme;
	}

// ------------------------------------------------------------------------------------------------
//  CThemesList::GetDefaultInstallOptions()
// ------------------------------------------------------------------------------------------------
void CThemesList::GetDefaultInstallOptions( SwiUI::TInstallOptions &aOptions )
	{
	SwiUI::TInstallOptions defaultOptions;

	// Return options
	aOptions = defaultOptions;
	}

// ------------------------------------------------------------------------------------------------
//  CThemesList::
// ------------------------------------------------------------------------------------------------
TAknSkinSrvSkinPackageLocation CThemesList::LocationFromPath( const TDesC& aPath )
    {
#ifndef RD_MULTIPLE_DRIVE
    TBuf<1> driveLetterBuf;
    driveLetterBuf.CopyUC( aPath.Left(1) );
    if( driveLetterBuf.Compare( _L("E") ) == 0 )
        {
        return EAknsSrvMMC;
        }
    return EAknsSrvPhone;
#else
	RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    TInt drive = EDriveC;
    TUint driveStatus = 0;
    TInt err = RFs::CharToDrive( aPath[0], drive );
    err = DriveInfo::GetDriveStatus( fs, drive, driveStatus );
    CleanupStack::PopAndDestroy(); // fs
    if ( driveStatus & DriveInfo::EDriveExternallyMountable )
        {
        RDEBUG("CThemesList::LocationFromPath(): from MMC");
        return EAknsSrvMMC;
        }

    
    RDEBUG("CThemesList::LocationFromPath(): from Phone");
    return EAknsSrvPhone;

#endif  //RD_MULTIPLE_DRIVE
    }

// ------------------------------------------------------------------------------------------------
//  CThemesList::
// ------------------------------------------------------------------------------------------------
CDesC8Array* CThemesList::InstalledPackagesL( )
	{
	CDesC8ArrayFlat* packages = new (ELeave) CDesC8ArrayFlat( 32 );
	CleanupStack::PushL( packages );

	RArray< TUid > uids;
	CleanupClosePushL( uids );

	RPointerArray< Swi::CSisRegistryPackage > pkgs;
	CleanupClosePushL( pkgs );

	Swi::RSisRegistrySession reg;
	User::LeaveIfError( reg.Connect() );
	CleanupClosePushL( reg );

	reg.InstalledUidsL( uids );
	reg.InstalledPackagesL( pkgs );

	TInt i = 0;
	TInt count = uids.Count();
	for (i = 0 ; i < count ; i++)
		{
		TBuf8<KMaxUidName> uidBuf8;
		uidBuf8.Copy( uids[i].Name() );

		RDEBUG8_3( "uid[%d]=%S", i, &uidBuf8 );
		}

	count = pkgs.Count();
	for (i = 0 ; i < count ; i++)
		{
		TPtrC uidName = pkgs[i]->Uid().Name();
		TPtrC name = pkgs[i]->Name();
		TPtrC vendor = pkgs[i]->Vendor();

		RDEBUG_5( "pkg[%d]: uid=%S, idx=%d, name=%S", i,
			&uidName,
			pkgs[i]->Index(),
			&name );


		TBuf<KMaxUidName+20> uidBuf;
		uidBuf = pkgs[i]->Uid().Name();
		uidBuf.Append( _L(":") );
		uidBuf.AppendNum( pkgs[i]->Index() );

		TBuf8<KMaxUidName+20> uidBuf8;
		uidBuf8.Copy( uidBuf );

		packages->AppendL( uidBuf8 );
		}

	CleanupStack::PopAndDestroy( &reg );

	pkgs.ResetAndDestroy();
	CleanupStack::PopAndDestroy( &pkgs );
	CleanupStack::PopAndDestroy( &uids );

	CleanupStack::Pop( packages );
	return packages;
	}


// ------------------------------------------------------------------------------------------------
//  CThemesList::
// ------------------------------------------------------------------------------------------------
CDesC8Array* CThemesList::InstalledThemesL( )
	{
	LoadSkinsL();

	CDesC8ArrayFlat* themes = new (ELeave) CDesC8ArrayFlat( 32 );
	CleanupStack::PushL( themes );

	TInt i = 0;
	TInt count = iSkins->Count();

	for (i = 0 ; i < count ; i++)
		{
		TAknsPkgIDBuf pidBuf;
		TBuf8<KAknsPkgIDDesSize> pidBuf8;

        TAknsPkgID pid = (*iSkins)[i]->PID();
        pid.CopyToDes( pidBuf );
        pidBuf8.Copy( pidBuf );

		themes->AppendL( pidBuf8 );

        TPtrC directory = (*iSkins)[i]->Directory();
        TPtrC iniFileDirectory = (*iSkins)[i]->IniFileDirectory();
        TPtrC name = (*iSkins)[i]->Name();
        TPtrC idleStateWallPaperImageFileName = (*iSkins)[i]->IdleStateWallPaperImageFileName();
        TPtrC pinboarWallPaperImageFileName = (*iSkins)[i]->PinboarWallPaperImageFileName();
        TInt idleStateWallPaperImageIndex = (*iSkins)[i]->IdleStateWallPaperImageIndex();
        TInt pinboardWallPaperImageIndex = (*iSkins)[i]->PinboardWallPaperImageIndex();
        TBool isDeletable = (*iSkins)[i]->IsDeletable();
        TBool isCopyable = (*iSkins)[i]->IsCopyable();
        TAknsPkgID colorSchemePID = (*iSkins)[i]->ColorSchemePID();
        TBool hasIconset = (*iSkins)[i]->HasIconset();
        TAknsSkinSrvSkinProtectionType protectionType = (*iSkins)[i]->ProtectionType();
        TBool isCorrupted = (*iSkins)[i]->IsCorrupted();
        TDesC& fullName = (*iSkins)[i]->FullName();


		TAknsPkgIDBuf colorSchemeBuf;
        colorSchemePID.CopyToDes( colorSchemeBuf );

		RDEBUG_4( "skin[%d]: pid=%S, name=%S", i, &pidBuf, &name );
		RDEBUG_2( "          directory=%S", &directory );
		RDEBUG_2( "          iniFileDirectory=%S", &iniFileDirectory );
		RDEBUG_2( "          name=%S", &name );
		RDEBUG_2( "          idleStateWallPaperImageFileName=%S", &idleStateWallPaperImageFileName );
		RDEBUG_2( "          pinboarWallPaperImageFileName=%S", &pinboarWallPaperImageFileName );
		RDEBUG_2( "          idleStateWallPaperImageIndex=%d", idleStateWallPaperImageIndex );
		RDEBUG_2( "          pinboardWallPaperImageIndex=%d", pinboardWallPaperImageIndex );
		RDEBUG_2( "          isDeletable=%d", isDeletable );
		RDEBUG_2( "          isCopyable=%d", isCopyable );
		RDEBUG_2( "          colorSchemePID=%S", &colorSchemeBuf );
		RDEBUG_2( "          hasIconset=%d", hasIconset );
		RDEBUG_2( "          protectionType=%d", protectionType );
		RDEBUG_2( "          isCorrupted=%d", isCorrupted );
		RDEBUG_2( "          fullName=%S", &fullName );
		}

	themes->Sort();

	CleanupStack::Pop( themes );
	return themes;
	}

// ------------------------------------------------------------------------------------------------
//  CThemesList::
// ------------------------------------------------------------------------------------------------
CDesC8Array* CThemesList::FindNewItemsL( const CDesC8Array& aItemsOrg, const CDesC8Array& aItemsNew )
	{
	CDesC8Array* result = new (ELeave) CDesC8ArrayFlat( 16 );
	CleanupStack::PushL( result );

	TInt i = 0;
	TInt count = aItemsNew.Count();
	for (i = 0 ; i < count ; i++)
		{
		result->AppendL( aItemsNew[ i ] );
		}

	result->Sort();

	count = aItemsOrg.Count();
	for (i = 0 ; i < count ; i++)
		{
		TInt idx = -1;
		if (result->FindIsq( aItemsOrg[ i ], idx, ECmpNormal ) == 0)
			{
			result->Delete( idx );
			}
		}

	result->Compress( );

	CleanupStack::Pop( result );
	return result;
	}

// ------------------------------------------------------------------------------------------------
// EOF
// ------------------------------------------------------------------------------------------------
