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



#ifndef __NSMLISADAPTER_H__
#define __NSMLISADAPTER_H__

#define IDLESOFTKEY_ADAPTER_IMAGEMASK

#define IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES

// tarm
#include "TARMDmStreamAdapter.h"

#include "IdleSoftkeysWebTargetManager.h"
#include "centralrepository.h"


//_LIT( KBrowserBookmarks, "BrowserBookmarks" );

class CIdleSoftkeysWebTargetManager;
class CIdleSoftkeysAppTargetManager;
class CUiSettingsUtil;

// ------------------------------------------------------------------------------------------------
// CIsAdapter
// ------------------------------------------------------------------------------------------------
class CIsAdapter : public CTARMDmStreamAdapter
    {
private:
    enum TIsNodeIdentifier
        {
        EIsNodeCustomization = 0,
        EIsNodeIdleSoftkeys,
        EIsNodeSoftKeys,
        EIsNodeTargets,
        EIsNodeWebTargets,
        EIsNodeSoftKeysX,
        EIsNodeTargetsX,
        EIsNodeWebTargetsX,
        EIsNodeSoftKeysXID,
        EIsNodeSoftKeysXTargetRef,
#ifdef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
        EIsNodeSoftKeysXImage,
#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
        EIsNodeSoftKeysXMask,
#endif
#endif
        EIsNodeTargetsXID,
        EIsNodeTargetsXDescription,
        EIsNodeTargetsXCaption,
#ifndef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
        EIsNodeTargetsXImage,
#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
        EIsNodeTargetsXImageMask,
#endif
#endif
        EIsNodeWebTargetsXID,
        EIsNodeWebTargetsXDescription,
        EIsNodeWebTargetsXURL,
        EIsNodeWebTargetsXUserName,
        EIsNodeWebTargetsXPassword,
        EIsNodeWebTargetsXConRef,
        EIsNodeWebTargetsXCaption,
#ifndef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
        EIsNodeWebTargetsXImage,
#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
        EIsNodeWebTargetsXImageMask,
#endif
#endif
        EIsNodeNotUsedAndAlwaysLast
        };

public:
    CIsAdapter(TAny* aEcomArguments);

    static CIsAdapter* NewL(MSmlDmCallback* aDmCallback );
    static CIsAdapter* NewLC(MSmlDmCallback* aDmCallback );

    virtual ~CIsAdapter();

    // Pure virtual methods
    // From CSmlDmAdapter
    void DDFVersionL                ( CBufBase& aVersion );
    void DDFStructureL              ( MSmlDmDDFObject& aDDF );

    void UpdateLeafObjectL          ( CSmlDmAdapter::TError &aStatus,
                                      const TDesC8& aURI, const TDesC8& aLUID,
                                      const TDesC8& aObject, const TDesC8& aType );
    void DeleteObjectL              ( CSmlDmAdapter::TError &aStatus, const TDesC8& aURI,
                                      const TDesC8& aLUID );
    CSmlDmAdapter::TError
         FetchLeafObjectL           ( const TDesC8& aURI, const TDesC8& aLUID,
                                      TDes8& aNewMime, CBufFlat& object );
    void AddNodeObjectL             ( CSmlDmAdapter::TError &aStatus, const TDesC8& aURI,
                                      const TDesC8& aParentLUID );

    void _UpdateLeafObjectL         ( const TDesC8& aURI, const TDesC8& aLUID,
                                      const TDesC8& aObject, const TDesC8& aType, TInt aStatusRef );
    void _UpdateLeafObjectL         ( const TDesC8& aURI, const TDesC8& aLUID, RWriteStream*& aStream,
                                      const TDesC8& aType, TInt aStatusRef );
    void _DeleteObjectL             ( const TDesC8& aURI, const TDesC8& aLUID, TInt aStatusRef );
    void _FetchLeafObjectL          ( const TDesC8& aURI, const TDesC8& aLUID,
                                      const TDesC8& aType, TInt aResultsRef, TInt aStatusRef );
    void _FetchLeafObjectSizeL      ( const TDesC8& aURI, const TDesC8& aLUID,
                                      const TDesC8& aType, TInt aResultsRef, TInt aStatusRef );
    void _ChildURIListL             ( const TDesC8& aURI, const TDesC8& aLUID,
                                      const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
                                      TInt aResultsRef, TInt aStatusRef );
    void _AddNodeObjectL            ( const TDesC8& aURI, const TDesC8& aParentLUID, TInt aStatusRef );
    void _ExecuteCommandL           ( const TDesC8& aURI, const TDesC8& aLUID,
                                      const TDesC8& aArgument, const TDesC8& aType, TInt aStatusRef );
    void _ExecuteCommandL           ( const TDesC8& aURI, const TDesC8& aLUID,
                                      RWriteStream*& aStream, const TDesC8& aType, TInt aStatusRef );
    void _CopyCommandL              ( const TDesC8& aTargetURI, const TDesC8& aTargetLUID,
                                      const TDesC8& aSourceURI, const TDesC8& aSourceLUID,
                                      const TDesC8& aType, TInt aStatusRef );
    void StartAtomicL               ( );
    void CommitAtomicL              ( );
    void RollbackAtomicL            ( );
    TAdapterStreamType StreamType   ( const TDesC8& aURI );
    TBool StreamingSupport          ( TInt& aItemSize );
    void CompleteOutstandingCmdsL   ( );

private:
    void ConstructL(MSmlDmCallback* aDmCallback);

    //
    // DDF methods
    //
    void FillNodeInfoL( MSmlDmDDFObject& aNode,TSmlDmAccessTypes aAccTypes,
            MSmlDmDDFObject::TOccurence aOccurrence,
            MSmlDmDDFObject::TScope aScope,
            MSmlDmDDFObject::TDFFormat aFormat,
            const TDesC8& aDescription,
            const TDesC8& aMimeType);

    //
    // URI & argument handling
    //
    void        CopyAndTrimMimeType         (TDes8& aNewMime, const TDesC8& aType);

    TIsNodeIdentifier
                GetNodeIdentifier           (const TDesC8& aURI);

    //
    // Softkeys
    //
    TInt        ListSoftkeysL               ( CBufFlat &aList );

    TInt        GetSoftkeyUidFromNodeNameL  ( const TDesC8& aNodeName );

    HBufC8*     GenerateSoftkeyNodeNameL    ( TInt aIndex );

    TInt        GetSoftkeyIndexFromUid      ( TInt aUid );

    void        AddShortcutNameL            ( CBufFlat& aBuffer,
                                              TInt aUid );

    void        SetShortcutTargetL          ( const TDesC8& aURI, TInt aUid );

    //
    // Common for targets
    //
#ifdef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
    void        GetSoftkeyImageL            ( TInt aSoftkeyUid, CBufFlat &aImageBuffer, TBool aGetMask=EFalse );
#endif

#ifndef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
    void        GetApplicationImageL                   (
                                                        TInt aIndex,
                                                        CBufFlat &aImageBuffer,
                                                        TBool aGetMask=EFalse
                                                        );

    void        GetBookmarkImageL                      (
                                                        TInt aIndex,
                                                        CBufFlat &aImageBuffer,
                                                        TBool aGetMask=EFalse
                                                        );

    void        GetShortcutImageL                      (
                                                        TInt aRtMask,
                                                        TInt aIndex,
                                                        CBufFlat &aImageBuffer,
                                                        TBool aGetMask=EFalse
                                                        );

#endif

    //
    // Other
    //
    CSmlDmAdapter::TError GetShortcutTargetURIL         (
                                                        CBufFlat& aBuffer,
                                                        TInt aUid
                                                        );

    HBufC16*    CreateCopy16LC                          ( const TDesC8& aText );

//#ifdef __SAP_POLICY_MANAGEMENT
    TPtrC8      PolicyRequestResourceL                  ( const TDesC8& aURI );
//#endif

    //
    // Image handling
    //
#ifdef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
    void        SetSoftkeyImageL               ( const TDesC8& aURI,
                                                 const TDesC8& aObject,
                                                 const TDesC8& aType);
#endif

#ifndef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
    void        SetShortcutTargetImageL        ( const TDesC8& aURI,
                                                 const TDesC8& aObject,
                                                 const TDesC8& aType);
#endif

    TInt        DecodeImageToFileL             ( const TDesC8& aObject );

    void        DecodeImageL                   ( CBufFlat& aBuffer, const TDesC8& aObject );

    void        EncodeImageL                   ( CBufFlat& aBuffer, const TDesC8& aObject );

private:
    TFileName               iTemporaryImageFile;
    static const TInt     iSoftkeyList[];
	TBool					iUiSettingsNotInitialized;
	
    RFs                     iFs;
    RFavouritesSession      iFavSession;
    RFavouritesDb           iFavDb;

    MSmlDmCallback*         iCallBack;
    CFavouritesItemList*    iFavList;

    CIdleSoftkeysWebTargetManager *iWebTargetManager;
    CIdleSoftkeysAppTargetManager *iAppTargetManager;
    CUiSettingsUtil *iUiSettings;
};

#endif // __NSMLISADAPTER_H__
