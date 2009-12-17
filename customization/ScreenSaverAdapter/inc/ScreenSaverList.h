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



#ifndef __ScreenSaverList_H__
#define __ScreenSaverList_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <e32base.h>
#include <smldmadapter.h>
#include <SWInstApi.h>

// Constants
const TInt KPkgId_ScrId_Separator = ':';
const TInt KDisableScreensaver     = 1;
const TInt KEnableScreensaver      = 0;


// ------------------------------------------------------------------------------------------------
// CScreenSaverInfo
// ------------------------------------------------------------------------------------------------
class CScreenSaverInfo : public CBase
	{
public:
	static CScreenSaverInfo* NewL( const TDesC& aId );
	static CScreenSaverInfo* NewLC( const TDesC& aId );

	virtual ~CScreenSaverInfo();

	// Other
	const TDesC8& Id();

	const TDesC8& Name();
	void SetNameL( const TDesC8& aName );
	void SetNameL( const TDesC& aName );

	const TDesC8& Description();
	void SetDescriptionL( const TDesC8& aDescription );

	const TDesC8& CustomProperty();
	void SetCustomPropertyL( const TDesC8& aCustomProperty );

	const TDesC8& Version();
	void SetVersionL( const TDesC8& aVersion );

	void ActivateL();

	enum TScreenSaverType 
		{
		EText = 0,
		EDateTime = 1,
		EObject = 3
		};
		
	TScreenSaverType TypeId();
	TBool IsActivatedL();
	const TDesC8& FileName();


protected:

private:
	CScreenSaverInfo( );
	void ConstructL( const TDesC& aId );

private:
	HBufC8* iId;
	HBufC8* iName;
	HBufC8* iDescription;
	HBufC8* iCustomProperty;
	HBufC8* iVersion;
	};

// ------------------------------------------------------------------------------------------------
// CScreenSaverList
// ------------------------------------------------------------------------------------------------
class CScreenSaverList : public CBase
	{
public:
	static CScreenSaverList* NewL( );
	static CScreenSaverList* NewLC( );

	virtual ~CScreenSaverList();

	MSmlDmAdapter::TError GetScreenSaverListL( const CArrayFix<TSmlDmMappingInfo>& aPreviousList, CBufBase& aList );

	MSmlDmAdapter::TError GetIdL( const TDesC8& aId, CBufBase& aResult );

	MSmlDmAdapter::TError GetNameL( const TDesC8& aId, CBufBase& aResult );
	MSmlDmAdapter::TError SetNameL( const TDesC8& aId, const TDesC8& aData );

	MSmlDmAdapter::TError GetDescriptionL( const TDesC8& aId, CBufBase& aResult );
	MSmlDmAdapter::TError SetDescriptionL( const TDesC8& aId, const TDesC8& aData );

	MSmlDmAdapter::TError GetCustomPropertyL( const TDesC8& aId, CBufBase& aResult );
	MSmlDmAdapter::TError SetCustomPropertyL( const TDesC8& aId, const TDesC8& aData );

	MSmlDmAdapter::TError GetVersionL( const TDesC8& aId, CBufBase& aResult );
	MSmlDmAdapter::TError SetVersionL( const TDesC8& aId, const TDesC8& aVersion );

	MSmlDmAdapter::TError GetActiveL( CBufBase& aResult );
	MSmlDmAdapter::TError SetActiveL( const TDesC8& aId );

	MSmlDmAdapter::TError SetInstallOptionsL( const TDesC8& aOptions );
	MSmlDmAdapter::TError InstallL( const TDesC& aFileName );

	MSmlDmAdapter::TError DeleteL( const TDesC8& aLUID );

private:
	CScreenSaverList( );
	void ConstructL( );

	void CheckUpdatesL();
	void LoadL();
	TBool LoadPluginsL( const TDesC& aPath );

	CScreenSaverInfo* FindByIdL( const TDesC8& aId );
	TPtrC8 ScreenSaverIdFromLUID( const TDesC8& aLUID );
	void GetDefaultInstallOptions( SwiUI::TInstallOptions& aOptions );

private:
	TBool iDirty;
    CArrayPtr< CScreenSaverInfo >* iScreenSavers;
	SwiUI::TInstallOptions iInstallOptions;
	};


#endif // __ScreenSaverList_H__
