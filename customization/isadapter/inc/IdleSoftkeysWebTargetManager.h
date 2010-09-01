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
* Description:  DM Idle SoftKey Adapter
*
*/


#ifndef __IdleSoftkeysWebTargetManager_H__
#define __IdleSoftkeysWebTargetManager_H__


// symbian
#include <e32base.h>
#include <smldmadapter.h>
// s60
#include <favouritesdb.h>           // Favourites Database

class CUiSettingsUtil;

class CIdleSoftkeysWebTargetManager : public CBase
    {
    public:

    CIdleSoftkeysWebTargetManager::CIdleSoftkeysWebTargetManager(
        MSmlDmCallback *& aCallBack,
        RFavouritesDb   & aFavDbSrv );
        
  	~CIdleSoftkeysWebTargetManager();

    //
    // Webtargets
    //
    TInt        ListWebTargetsL                         ( CBufFlat &aList );

    void        CheckWebTargetsL                        ( );

#ifdef ISADAPTER_USE_CAPTION_IN_WEBTARGET_NAME
    HBufC8*     GenerateNodeNameL                       ( TInt aIndex );
#endif

    HBufC8*     GenerateNodeNameL                       ( TInt aIndex, const TDesC8 &aPrefix );

    HBufC8*     GenerateNodeNameL                       ( const TDesC8 &aText );

    TInt        RenameL                                 (
                                                        const TDesC8& aURI,
                                                        const TDesC8& aLUID,
                                                        const TDesC8& aNewURI
                                                        );

    TInt        GetTargetFromNodeNameL                  (
                                                        const TDesC8& aURI,
                                                        TInt &aIndex
                                                        );

    TInt        GetTargetFromNodeNameL                  ( 
                                                        const TDesC8& aURI,
                                                        CFavouritesItem &aItem,
                                                        TInt &aIndex
                                                        );

    void        PrepareItemL                            (
                                                        const TDesC8& aURI,
                                                        CFavouritesItem &aItem,
                                                        TInt &aIndex
                                                        );

    void        ReleaseItemL                            (
                                                        CFavouritesItem &aItem,
                                                        TBool aUpdate=ETrue
                                                        );

    void        SetCaptionL                             (
                                                        const TDesC8& aURI,
                                                        const TDesC8& aCaption
                                                        );

    void        FindWapApL                              (
                                                        const TDesC8& aURI,
                                                        TInt &aWapAp
                                                        );


    //
    // URI mapping database ( Webtarget node name / LUID )
    //
    TInt        UpdateLUIDDatabaseL     (
                                        const TDesC8& aURI,
                                        const TDesC8& aLUID,
                                        TInt &aIndex
                                        );

    TInt        FindLUIDByURIL          (
                                        const TDesC8& aURI,
                                        TDes8& aLUID,
                                        TInt &aIndex
                                        );

    TInt        FindURIByLUIDL          (
                                        TDes8& aURI,
                                        const TDesC8& aLUID,
                                        TInt &aIndex
                                        );

    TInt        CheckLUIDDatabaseL      (
                                        TBool aCleanDatabase=ETrue
                                        );

    private:
    
	TBool					iUiSettingsNotInitialized;
    MSmlDmCallback*         &iCallBack;
    RFavouritesDb           &iFavDb;
    CUiSettingsUtil 		*iUiSettings;
    };

#endif //#ifndef __IdleSoftkeysWebTargetManager_H__
