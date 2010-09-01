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



#ifndef __ThemesList_H__
#define __ThemesList_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <e32base.h>
#include <smldmadapter.h>
#include <SWInstApi.h>
#include <AknsSrvClient.h>


class CAknsSrvSkinInformationPkg;
class CThemeItem;
class CDesC8Array;

// ------------------------------------------------------------------------------------------------
// CThemesList
// ------------------------------------------------------------------------------------------------
class CThemesList : public CBase
	{
public:
	static CThemesList* NewL( );
	static CThemesList* NewLC( );

	virtual ~CThemesList();

	// Other
	
	MSmlDmAdapter::TError GetInstalledPIDListL( const CArrayFix<TSmlDmMappingInfo>& aPreviousList, CDesC8ArrayFlat& themes, CDesC8Array& aNewThemes );
	MSmlDmAdapter::TError GetThemeIdL( const TDesC8& aPID, CBufBase& aResult );
	MSmlDmAdapter::TError GetThemeNameL( const TDesC8& aPID, CBufBase& aResult );
	MSmlDmAdapter::TError GetThemeVersionL( const TDesC8& aPID, CBufBase& aResult );

	MSmlDmAdapter::TError GetActiveThemeL( CBufBase& aResult );
	MSmlDmAdapter::TError SetActiveThemeL( const TDesC8& aPID );

	MSmlDmAdapter::TError SetInstallOptionsL( const TDesC8& aOptions );
	MSmlDmAdapter::TError InstallThemeL( const TDesC& aFileName, TDes8& aSisPkgUid, TDes8& aThemePkgId );

	MSmlDmAdapter::TError DeleteThemeL( const TDesC8& aThemeId, const TUid& aPkgUid, HBufC8* luid);

protected:

private:
	CThemesList( );
	void ConstructL( );

	void CheckUpdatesL();
	void LoadSkinsL();
	CAknsSrvSkinInformationPkg* FindThemeL( const TDesC8& aPID );
	void GetDefaultInstallOptions( SwiUI::TInstallOptions& aOptions );

	TAknSkinSrvSkinPackageLocation LocationFromPath( const TDesC& aPath );

	CDesC8Array* InstalledPackagesL( );
	CDesC8Array* InstalledThemesL( );
	CDesC8Array* FindNewItemsL( const CDesC8Array& aItemsOrg, const CDesC8Array& aItemsNew );

private:

	TBool iDirty;
    RAknsSrvSession iAknsSrv;
    CArrayPtr<CAknsSrvSkinInformationPkg>* iSkins;

	RArray<CThemeItem> iThemeItems;

	SwiUI::TInstallOptions iInstallOptions;
	};

// ------------------------------------------------------------------------------------------------
// CThemeItem
// ------------------------------------------------------------------------------------------------
class CThemeItem : public CBase
	{
public:
	static CThemeItem* NewL( MSmlDmCallback* aDmCallback );
	static CThemeItem* NewLC( MSmlDmCallback* aDmCallback );

	virtual ~CThemeItem();

	// Other
	MSmlDmAdapter::TError GetDMNameL( CBufBase& aResult );
	MSmlDmAdapter::TError GetIdL( CBufBase& aResult );
	MSmlDmAdapter::TError GetNameL( CBufBase& aResult );
	MSmlDmAdapter::TError GetVersionL( CBufBase& aResult );

protected:

private:
	CThemeItem( );
	void ConstructL( );

private:
	HBufC8* iDMName;
    CAknsSrvSkinInformationPkg* iSkinInfo;
	};

#endif // __ThemesList_H__
