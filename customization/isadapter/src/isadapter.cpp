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
* Description:  DM Idle SoftKey  Adapter
*
*/


#include "isadapter.h"

#include "IsAdapterDefines.h"
#include "IsAdapterLiterals.h"
#include "IdleSoftkeysAppTargetManager.h"
#include "UiSettingsUtil.h"

// symbian
#include <implementationproxy.h>
#include <centralrepository.h>
#include <imcvcodc.h>
// s60
#include <FavouritesItemList.h>

// tarm
//#ifdef __SAP_POLICY_MANAGEMENT
#include <SettingEnforcementInfo.h>
#include <PolicyEngineXACML.h>
#include <DMCert.h>
//#endif

//#include "UISettingsSrvConstants.h"
#include "IsAdapterConstants.h"
#include "nsmldmiapmatcher.h"
#include "nsmldmuri.h"
#include "TPtrC8I.h"
#include "TARMCharConv.h"
#include "FileCoderB64.h"
#include "debug.h"
#include <featmgr.h>
// ------------------------------------------------------------------------------------------------
// CIsAdapter* CIsAdapter::NewL( )
// ------------------------------------------------------------------------------------------------
CIsAdapter* CIsAdapter::NewL(MSmlDmCallback* aDmCallback )
    {
    RDEBUG( "CIsAdapter::NewL() >" );
    CIsAdapter* self = NewLC( aDmCallback );
    CleanupStack::Pop();
    RDEBUG( "CIsAdapter::NewL() <" );
    return self;
    }

// ------------------------------------------------------------------------------------------------
// CIsAdapter* CIsAdapter::NewLC( )
// ------------------------------------------------------------------------------------------------
CIsAdapter* CIsAdapter::NewLC(MSmlDmCallback* aDmCallback )
    {
    RDEBUG( "CIsAdapter::NewLC() >" );
    CIsAdapter* self = new (ELeave) CIsAdapter(aDmCallback);
    CleanupStack::PushL(self);
    self->ConstructL(aDmCallback);
    RDEBUG( "CIsAdapter::NewLC() <" );
    return self;
    }

// ------------------------------------------------------------------------------------------------
// CIsAdapter::CIsAdapter()
// ------------------------------------------------------------------------------------------------
CIsAdapter::CIsAdapter(TAny* aEcomArguments)
: CTARMDmStreamAdapter((MSmlDmCallback*)aEcomArguments)
, iFavList(0)
, iWebTargetManager(0)
, iAppTargetManager(0)
, iUiSettings(0)
    {
    RDEBUG( "CIsAdapter::CIsAdapter()" );
    iUiSettingsNotInitialized = ETrue;
    }

// ------------------------------------------------------------------------------------------------
// CIsAdapter::~CIsAdapter()
// ------------------------------------------------------------------------------------------------
CIsAdapter::~CIsAdapter()
    {
    RDEBUG( "CIsAdapter::~CIsAdapter() >" );
	FeatureManager::UnInitializeLib();
    delete iWebTargetManager;
    delete iAppTargetManager;
    delete iUiSettings;

    iFavDb.Close();
    iFavSession.Close();

    if( iFavList != 0 )
        {
        delete iFavList;
        iFavList = 0;
        }

    if( iTemporaryImageFile.Length() > 0 )
        {
        RDEBUG( "CIsAdapter::~CIsAdapter() Deleting temporary file" );
        RFs fs;
        TInt err = fs.Connect();
        if( err == KErrNone )
        	{
        	fs.Delete( iTemporaryImageFile );
        	fs.Close();
        	iTemporaryImageFile.Zero();
        	}
        }    

    RDEBUG( "CIsAdapter::~CIsAdapter() <" );
    }



// ------------------------------------------------------------------------------------------------
//  CIsAdapter::DDFVersionL()
// ------------------------------------------------------------------------------------------------
void CIsAdapter::DDFVersionL(CBufBase& aDDFVersion)
    {
    RDEBUG( "CIsAdapter::DDFVersionL() >" );

    aDDFVersion.InsertL(0,KNSmlISDDFVersion);

    RDEBUG( "CIsAdapter::DDFVersionL() <" );
    }

// ------------------------------------------------------------------------------------------------
//  CIsAdapter::DDFStructureL()
// ------------------------------------------------------------------------------------------------
void CIsAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
    {
    RDEBUG( "CIsAdapter::DDFStructureL() >" );

    TSmlDmAccessTypes accessTypesGet;
    accessTypesGet.SetGet();

    TSmlDmAccessTypes accessTypesAddGet;
    accessTypesAddGet.SetAdd();
    accessTypesAddGet.SetGet();

    TSmlDmAccessTypes accessTypesGetReplace;
    accessTypesGetReplace.SetGet();
    accessTypesGetReplace.SetReplace();

    TSmlDmAccessTypes accessTypesADGR;
    accessTypesADGR.SetAdd();
    accessTypesADGR.SetDelete();
    accessTypesADGR.SetGet();
    accessTypesADGR.SetReplace();

    /*
    Node: ./Customzation
    This interior node is the common parent to all customization..
    Status: Required
    Occurs: One
    Format: Node
    Access Types: Get
    Values: N/A
    */
    MSmlDmDDFObject& apps = aDDF.AddChildObjectL(KNSmlCustomizationNodeName);
    FillNodeInfoL(apps,accessTypesGet,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,MSmlDmDDFObject::ENode,
        KNSmlISDescription,KNullDesC8());

    /*
    Node: ./Customization/IdleSoftKey
    This interior node acts as a placeholder for one or more update packages within a same content provider group.
    Status: Required
    Occurs: One
    Format: Node
    Access Types: Get
    Values: N/A
    */
    MSmlDmDDFObject& groupNode = apps.AddChildObjectL(KNSmlIS_IdleSoftKeyNodeName);
    FillNodeInfoL(groupNode,accessTypesGet,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::ENode,KNSmlISIdleSoftKeyNodeDescription,KNullDesC8());

    // IdleSoftkeys/*
    MSmlDmDDFObject& groupNodeSoftkeys = groupNode.AddChildObjectL(KNSmlIS_SoftKeysNodeName);
    FillNodeInfoL(groupNodeSoftkeys,accessTypesGet,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::ENode,KNSmlISSoftkeysNodeDescription,KNullDesC8());

    MSmlDmDDFObject& groupNodeTargets = groupNode.AddChildObjectL(KNSmlIS_TargetsNodeName);
    FillNodeInfoL(groupNodeTargets,accessTypesGet,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::ENode,KNSmlISTargetsNodeDescription,KNullDesC8());

    MSmlDmDDFObject& groupNodeWebTargets = groupNode.AddChildObjectL(KNSmlIS_WebTargetsNodeName);
    FillNodeInfoL(groupNodeWebTargets,accessTypesAddGet,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::ENode,KNSmlISWebTargetsNodeDescription,KNullDesC8());

    // Dynamic node lists, IdleSoftkeys/*/X
    MSmlDmDDFObject& groupNodeSoftkeysX = groupNodeSoftkeys.AddChildObjectGroupL();
    FillNodeInfoL(groupNodeSoftkeysX,accessTypesGet,MSmlDmDDFObject::EOneOrMore,MSmlDmDDFObject::EDynamic,
        MSmlDmDDFObject::ENode,KNSmlISSoftkeysXNodeDescription,KNullDesC8());

    MSmlDmDDFObject& groupNodeTargetsX = groupNodeTargets.AddChildObjectGroupL();
    FillNodeInfoL(groupNodeTargetsX,accessTypesGet,MSmlDmDDFObject::EZeroOrMore,MSmlDmDDFObject::EDynamic,
        MSmlDmDDFObject::ENode,KNSmlISTargetsXNodeDescription,KNullDesC8());

    MSmlDmDDFObject& groupNodeWebTargetsX = groupNodeWebTargets.AddChildObjectGroupL();
    FillNodeInfoL(groupNodeWebTargetsX,accessTypesADGR,MSmlDmDDFObject::EZeroOrMore,MSmlDmDDFObject::EDynamic,
        MSmlDmDDFObject::ENode,KNSmlISWebTargetsXNodeDescription,KNullDesC8());

    // Softkeys/X/*
    MSmlDmDDFObject& sId = groupNodeSoftkeysX.AddChildObjectL(KNSmlIS_SoftkeysIDNodeName);
    FillNodeInfoL(sId,accessTypesGet,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EChr,KNSmlIS_SoftkeysIDDescription,KNSmlISTextPlain());
    MSmlDmDDFObject& sRef = groupNodeSoftkeysX.AddChildObjectL(KNSmlIS_SoftkeysTargetRefNodeName);
    FillNodeInfoL(sRef,accessTypesGetReplace,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EChr,KNSmlIS_SoftkeysTargetRefDescription,KNSmlISTextPlain());
#ifdef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
    MSmlDmDDFObject& sImage = groupNodeSoftkeysX.AddChildObjectL(KNSmlIS_SoftkeysImageNoneName);
    FillNodeInfoL(sImage,accessTypesGetReplace,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EBin,KNSmlIS_SoftkeysImageDescription,KMimeTypeImageMbm());
#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
    MSmlDmDDFObject& sImageMask = groupNodeSoftkeysX.AddChildObjectL(KNSmlIS_SoftkeysMaskNodeName);
    FillNodeInfoL(sImageMask,accessTypesGetReplace,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EBin,KNSmlIS_SoftkeysMaskDescription,KMimeTypeImageMbm());
#endif
#endif
    // Targets/X/*
    MSmlDmDDFObject& tId = groupNodeTargetsX.AddChildObjectL(KNSmlIS_TargetsIDNodeName);
    FillNodeInfoL(tId,accessTypesGet,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EChr,KNSmlIS_TargetsIDDescription,KNSmlISTextPlain());

    MSmlDmDDFObject& tDesc = groupNodeTargetsX.AddChildObjectL(KNSmlIS_TargetsDescriptionNodeName);
    FillNodeInfoL(tDesc,accessTypesGet,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EChr,KNSmlIS_TargetsDescriptionDescription,KNSmlISTextPlain());

    MSmlDmDDFObject& tCaption = groupNodeTargetsX.AddChildObjectL(KNSmlIS_TargetsCaptionName);
    FillNodeInfoL(tCaption,accessTypesGetReplace,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EChr,KNSmlIS_TargetsCaptionDescription,KNSmlISTextPlain());

#ifndef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
    MSmlDmDDFObject& tImage = groupNodeTargetsX.AddChildObjectL(KNSmlIS_TargetsImageNodeName);
    FillNodeInfoL(tImage,accessTypesGetReplace,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EBin,KNSmlIS_TargetsImageDescription,KMimeTypeImageMbm());

#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
    MSmlDmDDFObject& tImageMask = groupNodeTargetsX.AddChildObjectL(KNSmlIS_TargetsMaskNodeName);
    FillNodeInfoL(tImageMask,accessTypesGetReplace,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EBin,KNSmlIS_TargetsMaskDescription,KMimeTypeImageMbm());
#endif
#endif

    // WebTargets/X/*
    MSmlDmDDFObject& wId = groupNodeWebTargetsX.AddChildObjectL(KNSmlIS_WebTargetsIDNodeName);
    FillNodeInfoL(wId,accessTypesGet,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EChr,KNSmlIS_WebTargetsIDDescription,KNSmlISTextPlain());

    MSmlDmDDFObject& wDesc = groupNodeWebTargetsX.AddChildObjectL(KNSmlIS_WebTargetsDescriptionNodeName);
    FillNodeInfoL(wDesc,accessTypesGetReplace,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EChr,KNSmlIS_WebTargetsDescriptionDescription,KNSmlISTextPlain());

    MSmlDmDDFObject& wURL = groupNodeWebTargetsX.AddChildObjectL(KNSmlIS_WebTargetsURLNodeName);
    FillNodeInfoL(wURL,accessTypesGetReplace,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EChr,KNSmlIS_WebTargetsURLDescription,KNSmlISTextPlain());

    MSmlDmDDFObject& wUserName = groupNodeWebTargetsX.AddChildObjectL(KNSmlIS_WebTargetsUserNameNodeName);
    FillNodeInfoL(wUserName,accessTypesGetReplace,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EChr,KNSmlIS_WebTargetsUserNameDescription,KNSmlISTextPlain());

    MSmlDmDDFObject& wPassword = groupNodeWebTargetsX.AddChildObjectL(KNSmlIS_WebTargetsPasswordNodeName);
    FillNodeInfoL(wPassword,accessTypesGetReplace,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EChr,KNSmlIS_WebTargetsPasswordDescription,KNSmlISTextPlain());

    MSmlDmDDFObject& wConRef = groupNodeWebTargetsX.AddChildObjectL(KNSmlIS_WebTargetsConRefNodeName);
    FillNodeInfoL(wConRef,accessTypesGetReplace,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EChr,KNSmlIS_WebTargetsConRefDescription,KNSmlISTextPlain());
        
    MSmlDmDDFObject& wCaption = groupNodeWebTargetsX.AddChildObjectL(KNSmlIS_WebTargetsCaptionNodeName);
    FillNodeInfoL(wCaption,accessTypesGetReplace,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EChr,KNSmlIS_WebTargetsCaptionDescription,KNSmlISTextPlain());

#ifndef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
    MSmlDmDDFObject& wImage = groupNodeWebTargetsX.AddChildObjectL(KNSmlIS_WebTargetsImageNodeName);
    FillNodeInfoL(wImage,accessTypesGetReplace,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EBin,KNSmlIS_WebTargetsImageDescription,KMimeTypeImageMbm());

#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
    MSmlDmDDFObject& wImageMask = groupNodeWebTargetsX.AddChildObjectL(KNSmlIS_WebTargetsMaskNodeName);
    FillNodeInfoL(wImageMask,accessTypesGetReplace,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EBin,KNSmlIS_WebTargetsMaskDescription,KMimeTypeImageMbm());
#endif
#endif

    RDEBUG( "CIsAdapter::DDFStructureL() <" );
    }

// ------------------------------------------------------------------------------------------------
//  CIsAdapter::UpdateLeafObjectL()
// ------------------------------------------------------------------------------------------------
void CIsAdapter::UpdateLeafObjectL( CSmlDmAdapter::TError &aStatus,
                                    const TDesC8& aURI, const TDesC8& aLUID,
                                    const TDesC8& aObject, const TDesC8& aType)
    {
    RDEBUG( "CIsAdapter::UpdateLeafObjectL() >" );
#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	CIsAdapter::TIsNodeIdentifier identifier = GetNodeIdentifier( uriPtrc );
#else
	CIsAdapter::TIsNodeIdentifier identifier = GetNodeIdentifier( aURI );
#endif

    HBufC16* strBuf;
    TInt value;
    CFavouritesItem *favItem = CFavouritesItem::NewLC();
    RDEBUG_2( "CIsAdapter::UpdateLeafObjectL() CFavouritesItem ALLOC %x", favItem );
    TPtrC8 ptr8(0, 0);

    switch( identifier )
        {

#ifdef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
            case EIsNodeSoftKeysXImage:
	#ifdef __TARM_SYMBIAN_CONVERGENCY            
            SetSoftkeyImageL( uriPtrc, aObject, aType );
	#else
			SetSoftkeyImageL( aURI, aObject, aType );
	#endif            
            break;

#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
            case EIsNodeSoftKeysXMask:
	#ifdef __TARM_SYMBIAN_CONVERGENCY            
            SetSoftkeyImageL( uriPtrc, aObject, aType );
	#else
			SetSoftkeyImageL( aURI, aObject, aType );
	#endif     
            break;
#endif
#endif

            case EIsNodeSoftKeysXTargetRef:
#ifdef __TARM_SYMBIAN_CONVERGENCY            
            ptr8.Set( NSmlDmURI::URISeg( uriPtrc, 3 ) );
#else
			ptr8.Set( NSmlDmURI::URISeg( aURI, 3 ) );
#endif                 
            value = GetSoftkeyUidFromNodeNameL( ptr8 );
            SetShortcutTargetL( aObject, value );
            break;

            case EIsNodeTargetsXCaption:
#ifdef __TARM_SYMBIAN_CONVERGENCY            
            iAppTargetManager->SetCaptionL( uriPtrc, aObject );
#else
			iAppTargetManager->SetCaptionL( aURI, aObject );
#endif            
            break;

#ifndef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
            case EIsNodeTargetsXImage:
	#ifdef __TARM_SYMBIAN_CONVERGENCY            
            SetShortcutTargetImageL( uriPtrc, aObject, aType );
	#else
			SetShortcutTargetImageL( aURI, aObject, aType );
	#endif               
            
            break;

#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
            case EIsNodeTargetsXImageMask:
	#ifdef __TARM_SYMBIAN_CONVERGENCY            
            SetShortcutTargetImageL( uriPtrc, aObject, aType );
	#else
			SetShortcutTargetImageL( aURI, aObject, aType );
	#endif                          
            break;
#endif
#endif

            case EIsNodeWebTargetsX:
#ifdef __TARM_SYMBIAN_CONVERGENCY             
            if( KErrNone != iWebTargetManager->RenameL( uriPtrc, aLUID, aObject ) )
#else
			if( KErrNone != iWebTargetManager->RenameL( aURI, aLUID, aObject ) )
#endif            
                {
                aStatus = CSmlDmAdapter::EError;
                }
            break;

            case EIsNodeWebTargetsXDescription:
            case EIsNodeWebTargetsXURL:
            case EIsNodeWebTargetsXUserName:
            case EIsNodeWebTargetsXPassword:
            case EIsNodeWebTargetsXConRef:
                {
#ifdef __TARM_SYMBIAN_CONVERGENCY                   
                iWebTargetManager->PrepareItemL( uriPtrc, *favItem, value );
#else
				iWebTargetManager->PrepareItemL( aURI, *favItem, value );
#endif                
                strBuf = CTARMCharConv::ConvertFromUtf8LC( aObject );

                switch( identifier )
                {
                    case EIsNodeWebTargetsXDescription:
                        favItem->SetNameL( *strBuf );
                    break;

                    case EIsNodeWebTargetsXURL:
                        favItem->SetUrlL( *strBuf );
                        if( favItem->Url().Compare( *strBuf ) != 0 )
                            {
                            RDEBUG( "CIsAdapter::UpdateLeafObjectL() Bookmark URL setting failed with FavouritesEngine!" );
                            aStatus = CSmlDmAdapter::EError;
                            }
                    break;

                    case EIsNodeWebTargetsXUserName:
                        favItem->SetUserNameL( *strBuf );
                    break;

                    case EIsNodeWebTargetsXPassword:
                        favItem->SetPasswordL( *strBuf );
                    break;

                    case EIsNodeWebTargetsXConRef:
                        TFavouritesWapAp wapAp;
                        if( aObject.Length() == 0 )
                            {
                            wapAp.SetDefault();
                            }
                        else
                            {
                            iWebTargetManager->FindWapApL( aObject, value );
                            wapAp.SetApId( value );
                            }
                        favItem->SetWapAp( wapAp );
                    break;
                }

                CleanupStack::PopAndDestroy( strBuf );
                iWebTargetManager->ReleaseItemL( *favItem );
                }
                break;

            case EIsNodeWebTargetsXCaption:
#ifdef __TARM_SYMBIAN_CONVERGENCY              
            iWebTargetManager->SetCaptionL( uriPtrc, aObject );
#else
			iWebTargetManager->SetCaptionL( aURI, aObject );
#endif            
            break;

#ifndef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
            case EIsNodeWebTargetsXImage:
	#ifdef __TARM_SYMBIAN_CONVERGENCY             
            SetShortcutTargetImageL( uriPtrc, aObject, aType );
	#else
			SetShortcutTargetImageL( aURI, aObject, aType );
	#endif            
            break;

#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
            case EIsNodeWebTargetsXImageMask:
	#ifdef __TARM_SYMBIAN_CONVERGENCY              
            SetShortcutTargetImageL( uriPtrc, aObject, aType );
	#else
			SetShortcutTargetImageL( aURI, aObject, aType );
	#endif            
            break;
#endif
#endif

            default:
                aStatus = CSmlDmAdapter::EError;
            break;
        };

    CleanupStack::PopAndDestroy( favItem );

    RDEBUG( "CIsAdapter::UpdateLeafObjectL() <" );
    }

// ------------------------------------------------------------------------------------------------
//  CIsAdapter::_UpdateLeafObjectL()
// ------------------------------------------------------------------------------------------------
void CIsAdapter::_UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID,
                                    const TDesC8& aObject, const TDesC8& aType,
                                    TInt aStatusRef )
    {
    RDEBUG( "CIsAdapter::_UpdateLeafObjectL() >" );

    TBuf8<KISTBufMaxLength> mimeType;
    CopyAndTrimMimeType(mimeType, aType);

    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TRAPD( reason, 
           UpdateLeafObjectL( status, uriPtrc, aLUID, aObject, aType ) );
#else
	TRAPD( reason, 
           UpdateLeafObjectL( status, aURI, aLUID, aObject, aType ) );
#endif

    if( KErrNone != reason )
        {
        RDEBUG_2( "CIsAdapter::_UpdateLeafObjectL: Error code %d", reason );
        status = CSmlDmAdapter::EError;
        }

    iCallBack->SetStatusL(aStatusRef, status);

    RDEBUG( "CIsAdapter::_UpdateLeafObjectL() <" );
    }

// ------------------------------------------------------------------------------------------------
//  CIsAdapter::_UpdateLeafObjectL()
// ------------------------------------------------------------------------------------------------
void CIsAdapter::_UpdateLeafObjectL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/,
                                    RWriteStream*& /*aStream*/, const TDesC8& /*aType*/,
                                    TInt aStatusRef )
    {
    RDEBUG( "CIsAdapter::_UpdateLeafObjectL() (stream) >" );

	// SymbianConvergencyNote! Check _UpdateLeafObjectL() <-> removes the "./" !

    CSmlDmAdapter::TError ret = CSmlDmAdapter::EError;
    iCallBack->SetStatusL(aStatusRef, ret);

    RDEBUG( "CIsAdapter::_UpdateLeafObjectL() (stream) <" );
    }

// ------------------------------------------------------------------------------------------------
// CIsAdapter::DeleteObjectL
// ------------------------------------------------------------------------------------------------
void CIsAdapter::DeleteObjectL( CSmlDmAdapter::TError &aStatus, const TDesC8& aURI,
                                const TDesC8& /*aLUID*/)
    {
    RDEBUG( "CIsAdapter::DeleteObjectL() >" );

	// NOTE! Look _DeleteObjectL() <-> removes the "./" from the aURI !!

    CIsAdapter::TIsNodeIdentifier identifier = GetNodeIdentifier( aURI );
    CFavouritesItem *favItem = CFavouritesItem::NewLC();
    RDEBUG_2( "CIsAdapter::DeleteObjectL() CFavouritesItem ALLOC %x", favItem );
    TInt index;
    TInt status = KErrNone;

    switch( identifier )
        {
            case EIsNodeWebTargetsX:
            status = iWebTargetManager->GetTargetFromNodeNameL( aURI, *favItem, index );
            if( (0 != favItem) && (KErrNone == status) )
                {
                User::LeaveIfError( iFavDb.Delete( favItem->Uid() ) );
                iWebTargetManager->CheckLUIDDatabaseL( ETrue );
                //iWebTargetManager->iUiSettings->ReadBookmarksL();
                }
            else
                {
                aStatus = CSmlDmAdapter::EError;
                }
            break;

            default:
                aStatus = CSmlDmAdapter::EError;
            break;
        };

    CleanupStack::PopAndDestroy( favItem );    

    RDEBUG( "CIsAdapter::DeleteObjectL() <" );
    }

// ------------------------------------------------------------------------------------------------
// CIsAdapter::_DeleteObjectL
// ------------------------------------------------------------------------------------------------
void CIsAdapter::_DeleteObjectL( const TDesC8& aURI, const TDesC8& aLUID, TInt aStatusRef )
    {
    RDEBUG( "CIsAdapter::_DeleteObjectL() >" );

    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;
  
#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TRAPD( reason,
           DeleteObjectL( status, uriPtrc, aLUID ) );
#else
	TRAPD( reason,
           DeleteObjectL( status, aURI, aLUID ) );
#endif  

    if( KErrNone != reason )
        {
        RDEBUG_2( "CIsAdapter::_DeleteObjectL: Error code %d", reason );
        status = CSmlDmAdapter::EError;
        }

    iCallBack->SetStatusL(aStatusRef, status);

    RDEBUG( "CIsAdapter::_DeleteObjectL() <" );
    }

// ------------------------------------------------------------------------------------------------
//  CIsAdapter::FetchLeafObjectL
// ------------------------------------------------------------------------------------------------
CSmlDmAdapter::TError CIsAdapter::FetchLeafObjectL( const TDesC8& aURI, const TDesC8& /*aLUID*/,
                                                    TDes8& aNewMime, CBufFlat& aObject )
    {
    RDEBUG( "CIsAdapter::FetchLeafObjectL() >" );

	// SymbianConvergency NOTE! Check _FetchLeafObjectL() <-> removes the "./" from the aURI

    TBuf8<KISTBufMaxLength> mimeType;
    CopyAndTrimMimeType(mimeType, aNewMime);

    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;

    CIsAdapter::TIsNodeIdentifier identifier = GetNodeIdentifier( aURI );

    TInt softkeyUid;
    HBufC8* strBuf = 0;
    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> numBuf;
    TBuf <MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> numBuf16;
    CFavouritesItem *favItem = CFavouritesItem::NewLC();
    RDEBUG_2( "CIsAdapter::FetchLeafObjectL() CFavouritesItem ALLOC %x", favItem );
    TBuf8<ISADAPTER_SHORTCUT_CAPTION_MAXLENGTH> caption;
    TInt index;

    switch( identifier )
        {
            case EIsNodeCustomization:
            case EIsNodeIdleSoftkeys:
            case EIsNodeSoftKeys:
            case EIsNodeTargets:
            case EIsNodeWebTargets:
            case EIsNodeSoftKeysX:
            case EIsNodeTargetsX:
            case EIsNodeWebTargetsX:
            // Empty
            break;

#ifdef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
            case EIsNodeSoftKeysXImage:
            status = CSmlDmAdapter::EError;
            //softkeyUid = GetSoftkeyUidFromNodeNameL( NSmlDmURI::URISeg( aURI, 3 ) );
            //GetSoftkeyImageL( softkeyUid, aObject );
            break;

#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
            case EIsNodeSoftKeysXMask:
            status = CSmlDmAdapter::EError;
            //softkeyUid = GetSoftkeyUidFromNodeNameL( NSmlDmURI::URISeg( aURI, 3 ) );
            //GetSoftkeyImageL( softkeyUid, aObject, ETrue );
            break;
#endif
#endif

            case EIsNodeSoftKeysXID:
            softkeyUid = GetSoftkeyUidFromNodeNameL( NSmlDmURI::URISeg( aURI, 3 ) );
            numBuf.Num( softkeyUid );
            aObject.InsertL( aObject.Size(), numBuf );
            aObject.InsertL( aObject.Size(), KNSmlISSeparator() );
            AddShortcutNameL( aObject, softkeyUid );
            break;

            case EIsNodeSoftKeysXTargetRef:
            status = GetShortcutTargetURIL( aObject, GetSoftkeyUidFromNodeNameL( NSmlDmURI::URISeg( aURI, 3 ) ) );
            break;

            case EIsNodeTargetsXID:
            iAppTargetManager->GetTargetFromNodeNameL( NSmlDmURI::URISeg( aURI, 3 ), index, caption );
            numBuf.Num( index );
            aObject.InsertL( aObject.Size(), numBuf );
            break;

            case EIsNodeTargetsXCaption:
            iAppTargetManager->GetTargetFromNodeNameL( NSmlDmURI::URISeg( aURI, 3 ), index, caption );
            aObject.InsertL( aObject.Size(), caption );
            break;

#ifndef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
            case EIsNodeTargetsXImage:
            status = CSmlDmAdapter::EError;
            //iAppTargetManager->GetTargetFromNodeNameL( NSmlDmURI::URISeg( aURI, 3 ), index, caption );
            //GetApplicationImageL( index, aObject );
            break;

#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
            case EIsNodeTargetsXImageMask:
            status = CSmlDmAdapter::EError;
            //iAppTargetManager->GetTargetFromNodeNameL( NSmlDmURI::URISeg( aURI, 3 ), index, caption );
            //GetApplicationImageL( index, aObject, ETrue );
            break;
#endif
#endif

            case EIsNodeTargetsXDescription:
            iAppTargetManager->GetTargetFromNodeNameL( NSmlDmURI::URISeg( aURI, 3 ), index, caption );
            aObject.InsertL( aObject.Size(), caption );
            break;

            case EIsNodeWebTargetsXID:
            case EIsNodeWebTargetsXDescription:
            case EIsNodeWebTargetsXURL:
            case EIsNodeWebTargetsXUserName:
            case EIsNodeWebTargetsXPassword:
            case EIsNodeWebTargetsXCaption:
                {
                    iWebTargetManager->PrepareItemL( aURI, *favItem, index );
                    switch( identifier )
                        {
                        case EIsNodeWebTargetsXID:
                        numBuf16.Num( favItem->Uid() );
                        strBuf = CTARMCharConv::ConvertToUtf8LC( numBuf16 );
                        aObject.InsertL( aObject.Size(), *strBuf );
                        break;

                        case EIsNodeWebTargetsXDescription:
                        strBuf = CTARMCharConv::ConvertToUtf8LC( favItem->Name() );
                        aObject.InsertL( aObject.Size(), *strBuf );
                        break;

                        case EIsNodeWebTargetsXURL:
                        strBuf = CTARMCharConv::ConvertToUtf8LC( favItem->Url() );
                        aObject.InsertL( aObject.Size(), *strBuf );
                        break;

                        case EIsNodeWebTargetsXUserName:
                        status = CSmlDmAdapter::EError;
                        
                        //strBuf = CTARMCharConv::ConvertToUtf8LC( favItem->UserName() );
                        //aObject.InsertL( aObject.Size(), *strBuf );
                        break;

                        case EIsNodeWebTargetsXPassword:
                        status = CSmlDmAdapter::EError;
                        
                        //strBuf = CTARMCharConv::ConvertToUtf8LC( favItem->Password() );
                        //aObject.InsertL( aObject.Size(), *strBuf );
                        break;

                        case EIsNodeWebTargetsXCaption:
                        strBuf = CTARMCharConv::ConvertToUtf8LC( favItem->Name() );
                        aObject.InsertL( aObject.Size(), *strBuf );
                        break;
                        }
                    if(strBuf!=NULL) 
                        {
                        CleanupStack::PopAndDestroy( strBuf );
                        }
                    iWebTargetManager->ReleaseItemL( *favItem, EFalse );
                }
                break;

            case EIsNodeWebTargetsXConRef:
                {
                iWebTargetManager->PrepareItemL( aURI, *favItem, index );

                TFavouritesWapAp wapAp = favItem->WapAp();
                if( wapAp.IsDefault() )
                    {
                    aObject.InsertL( aObject.Size(), KDefaultApName8() );
                    }
                else if( wapAp.IsNull() )
                    {
                    // Leave empty
                    }
                else
                    {
                    CNSmlDMIAPMatcher *matcher = CNSmlDMIAPMatcher::NewLC( iCallBack );
                    TInt id = wapAp.ApId();
                    HBufC8* buf = matcher->URIFromIAPIdL( id );
                    if( 0 == buf )
                        {
                        User::Leave( KErrNotFound );
                        }
                    CleanupStack::PushL( buf );
                    aObject.InsertL( aObject.Size(), *buf );
                    CleanupStack::PopAndDestroy( buf );
                    CleanupStack::PopAndDestroy( matcher );
                    }

                iWebTargetManager->ReleaseItemL( *favItem, EFalse );
                }
            break;

#ifndef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
            case EIsNodeWebTargetsXImage:
            status = CSmlDmAdapter::EError;
            //iWebTargetManager->PrepareItemL( aURI, *favItem, index );
            //GetBookmarkImageL( index, aObject );
            //iWebTargetManager->ReleaseItemL( *favItem, EFalse );
            break;

#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
            case EIsNodeWebTargetsXImageMask:
            status = CSmlDmAdapter::EError;
            //iWebTargetManager->PrepareItemL( aURI, *favItem, index );
            //GetBookmarkImageL( index, aObject, ETrue );
            //iWebTargetManager->ReleaseItemL( *favItem, EFalse );
            break;
#endif
#endif

            default:
                status = CSmlDmAdapter::EError;
            break;
        };

    CleanupStack::PopAndDestroy( favItem );

    RDEBUG_2( "CIsAdapter::FetchLeafObjectL() < %d", (TInt)status );
    return status;
    }

// ------------------------------------------------------------------------------------------------
//  CIsAdapter::_FetchLeafObjectL
// ------------------------------------------------------------------------------------------------
void CIsAdapter::_FetchLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID,
                                   const TDesC8& aType, TInt aResultsRef,
                                   TInt aStatusRef )
    {
    RDEBUG("CIsAdapter::_FetchLeafObjectL() >");

    TBuf8<KISTBufMaxLength> mimeType;
    CopyAndTrimMimeType(mimeType, aType);

    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;

    CBufFlat *object = CBufFlat::NewL(128);
    CleanupStack::PushL( object );

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TRAPD(
        reason,
        status = FetchLeafObjectL( uriPtrc, aLUID, mimeType, *object ) );
#else
	TRAPD(
        reason,
        status = FetchLeafObjectL( aURI, aLUID, mimeType, *object ) );
#endif

    if( KErrNone != reason )
        {
        RDEBUG_2( "CIsAdapter::_FetchLeafObjectL: Error code %d", reason);
        status = CSmlDmAdapter::EError;
        }

    iCallBack->SetStatusL(aStatusRef, status);

    if( status == CSmlDmAdapter::EOk)
        {
        object->Compress();
        iCallBack->SetResultsL( aResultsRef, *object, mimeType );
        }

    CleanupStack::PopAndDestroy( object );    

    RDEBUG("CIsAdapter::_FetchLeafObjectL() <");
    }

// ------------------------------------------------------------------------------------------------
//  CIsAdapter::_FetchLeafObjectSizeL
// ------------------------------------------------------------------------------------------------
void CIsAdapter::_FetchLeafObjectSizeL( const TDesC8& aURI, const TDesC8& aLUID,
                                       const TDesC8& aType, TInt aResultsRef,
                                       TInt aStatusRef )
    {
    RDEBUG("CIsAdapter::_FetchLeafObjectSizeL() >");

    TBuf8<KISTBufMaxLength> mimeType;
    CopyAndTrimMimeType(mimeType, aType);

    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;

    CBufFlat *object = CBufFlat::NewL(128);
    CleanupStack::PushL( object );

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TRAPD(
        reason,
        status = FetchLeafObjectL( uriPtrc, aLUID, mimeType, *object ) );
#else
	TRAPD(
        reason,
        status = FetchLeafObjectL( aURI, aLUID, mimeType, *object ) );
#endif

    if( KErrNone != reason )
        {
        RDEBUG_2( "CIsAdapter::_FetchLeafObjectSizeL: Error code %d", reason);
        status = CSmlDmAdapter::EError;
        }

    iCallBack->SetStatusL(aStatusRef, status);

    if( status == CSmlDmAdapter::EOk)
        {
        TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> numBuf;
        object->Compress();

        CBufFlat *object2 = CBufFlat::NewL(128);
        CleanupStack::PushL( object2 );

        numBuf.Num(object->Size());
        object2->InsertL(0, numBuf);
        object2->Compress();
        iCallBack->SetResultsL( aResultsRef, *object2, KNSmlISTextPlain );

        CleanupStack::PopAndDestroy( object2 );
        }

    CleanupStack::PopAndDestroy( object );    

    RDEBUG("CIsAdapter::_FetchLeafObjectSizeL() <");
    }

// ------------------------------------------------------------------------------------------------
//  CIsAdapter::_ChildURIListL
// ------------------------------------------------------------------------------------------------
void CIsAdapter::_ChildURIListL( const TDesC8& aURI, const TDesC8& /*aLUID*/,
                                const CArrayFix<TSmlDmMappingInfo>& /*aPreviousURISegmentList*/,
                                TInt aResultsRef, TInt aStatusRef )
    {
    RDEBUG( "CIsAdapter::_ChildURIListL() >" );

    CSmlDmAdapter::TError ret = CSmlDmAdapter::EOk;

    CBufFlat *currentList = CBufFlat::NewL(128);
    CleanupStack::PushL(currentList);

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	CIsAdapter::TIsNodeIdentifier identifier = GetNodeIdentifier( uriPtrc );
#else
	CIsAdapter::TIsNodeIdentifier identifier = GetNodeIdentifier( aURI );
#endif

    switch( identifier )
        {
        case EIsNodeCustomization:
        currentList->InsertL(currentList->Size(),KNSmlIS_IdleSoftKeyNodeName());
        break;
        
        case EIsNodeIdleSoftkeys:
        currentList->InsertL(currentList->Size(),KNSmlIS_SoftKeysNodeName());
        currentList->InsertL(currentList->Size(),KNSmlISSeparator());        
        currentList->InsertL(currentList->Size(),KNSmlIS_TargetsNodeName());
        currentList->InsertL(currentList->Size(),KNSmlISSeparator());        
        currentList->InsertL(currentList->Size(),KNSmlIS_WebTargetsNodeName());
        break;

        case EIsNodeSoftKeys:
        ListSoftkeysL( *currentList );
        break;

        case EIsNodeTargets:
        iAppTargetManager->ListTargetsL( *currentList );
        break;

        case EIsNodeWebTargets:
        
        //User::LeaveIfError( iUiSrv.UpdateShortcutTargetList() );
        iWebTargetManager->ListWebTargetsL( *currentList );
        iWebTargetManager->CheckLUIDDatabaseL( ETrue );
        break;

        case EIsNodeSoftKeysX:
        currentList->InsertL(currentList->Size(),KNSmlIS_SoftkeysIDNodeName());
        currentList->InsertL(currentList->Size(),KNSmlISSeparator());
        currentList->InsertL(currentList->Size(),KNSmlIS_SoftkeysTargetRefNodeName());
#ifdef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
        currentList->InsertL(currentList->Size(),KNSmlISSeparator());
        currentList->InsertL(currentList->Size(),KNSmlIS_SoftkeysImageNoneName());
#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
        currentList->InsertL(currentList->Size(),KNSmlISSeparator());
        currentList->InsertL(currentList->Size(),KNSmlIS_SoftkeysMaskNodeName());
#endif
#endif
        break;
        
        case EIsNodeTargetsX:
        currentList->InsertL(currentList->Size(),KNSmlIS_TargetsIDNodeName());
        currentList->InsertL(currentList->Size(),KNSmlISSeparator());
        currentList->InsertL(currentList->Size(),KNSmlIS_TargetsDescriptionNodeName());
        currentList->InsertL(currentList->Size(),KNSmlISSeparator());
        currentList->InsertL(currentList->Size(),KNSmlIS_TargetsCaptionName());
#ifndef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
        currentList->InsertL(currentList->Size(),KNSmlISSeparator());
        currentList->InsertL(currentList->Size(),KNSmlIS_TargetsImageNodeName());
#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
        currentList->InsertL(currentList->Size(),KNSmlISSeparator());
        currentList->InsertL(currentList->Size(),KNSmlIS_TargetsMaskNodeName());
#endif
#endif
        break;
        
        case EIsNodeWebTargetsX:
        currentList->InsertL(currentList->Size(),KNSmlIS_WebTargetsIDNodeName());
        currentList->InsertL(currentList->Size(),KNSmlISSeparator());
        currentList->InsertL(currentList->Size(),KNSmlIS_WebTargetsDescriptionNodeName());
        currentList->InsertL(currentList->Size(),KNSmlISSeparator());
        currentList->InsertL(currentList->Size(),KNSmlIS_WebTargetsURLNodeName());
        currentList->InsertL(currentList->Size(),KNSmlISSeparator());
        currentList->InsertL(currentList->Size(),KNSmlIS_WebTargetsUserNameNodeName());
        currentList->InsertL(currentList->Size(),KNSmlISSeparator());
        currentList->InsertL(currentList->Size(),KNSmlIS_WebTargetsPasswordNodeName());
        currentList->InsertL(currentList->Size(),KNSmlISSeparator());
        currentList->InsertL(currentList->Size(),KNSmlIS_WebTargetsConRefNodeName());
        currentList->InsertL(currentList->Size(),KNSmlISSeparator());
        currentList->InsertL(currentList->Size(),KNSmlIS_WebTargetsCaptionNodeName());
#ifndef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
        currentList->InsertL(currentList->Size(),KNSmlISSeparator());
        currentList->InsertL(currentList->Size(),KNSmlIS_WebTargetsImageNodeName());
#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
        currentList->InsertL(currentList->Size(),KNSmlISSeparator());
        currentList->InsertL(currentList->Size(),KNSmlIS_WebTargetsMaskNodeName());
#endif
#endif
        break;

        case EIsNodeSoftKeysXID:
        case EIsNodeSoftKeysXTargetRef:
#ifdef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
        case EIsNodeSoftKeysXImage:
#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
        case EIsNodeSoftKeysXMask:
#endif
#endif
        // Empty
        break;

        case EIsNodeTargetsXID:
        case EIsNodeTargetsXDescription:
        case EIsNodeTargetsXCaption:
#ifndef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
        case EIsNodeTargetsXImage:        
#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
        case EIsNodeTargetsXImageMask:
#endif
#endif
        // Empty
        break;

        case EIsNodeWebTargetsXID:
        case EIsNodeWebTargetsXDescription:
        case EIsNodeWebTargetsXURL:
        case EIsNodeWebTargetsXUserName:
        case EIsNodeWebTargetsXPassword:
        case EIsNodeWebTargetsXConRef:
        case EIsNodeWebTargetsXCaption:
#ifndef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
        case EIsNodeWebTargetsXImage:        
#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
        case EIsNodeWebTargetsXImageMask:
#endif
#endif
        // Empty
        break;

        default:
#ifdef __TARM_SYMBIAN_CONVERGENCY
	if( uriPtrc.Length() == 0 )
#else
	if( aURI.Length() == 0 )
#endif        
            {
            currentList->InsertL(0,KNSmlCustomizationNodeName());
            }
        else
            {                
            ret = CSmlDmAdapter::ENotFound;
            }
        break;
        }

    iCallBack->SetStatusL(aStatusRef, ret);

    if( ret == CSmlDmAdapter::EOk )
        {
        iCallBack->SetResultsL(aResultsRef, *currentList, KNSmlISTextPlain);
        }

    CleanupStack::PopAndDestroy( currentList );

    RDEBUG( "CIsAdapter::_ChildURIListL() <" );
    }

// ------------------------------------------------------------------------------------------------
// CIsAdapter::AddNodeObjectL
// ------------------------------------------------------------------------------------------------
void CIsAdapter::AddNodeObjectL( CSmlDmAdapter::TError &aStatus, const TDesC8& aURI,
                                 const TDesC8& /*aParentLUID*/ )
    {
    RDEBUG( "CIsAdapter::AddNodeObjectL() >" );

	// SymbianConvergency NOTE! See _AddNodeObjectL() <-> it removes the "./" from aURI

    CIsAdapter::TIsNodeIdentifier identifier = GetNodeIdentifier( aURI );

    HBufC8* dataH = HBufC8::NewLC( KRepositoryEntryMaxLength );
    RDEBUG_2( "CIsAdapter::AddNodeObjectL() HBufC8 dataH ALLOC %x", dataH);
    TPtr8 data( dataH->Des() );
    TInt index = -1;
    TInt ret = KErrNone;

    switch( identifier )
        {
            case EIsNodeWebTargetsX:
                {
                TRAPD( reason,
                    ret = 
                    iWebTargetManager->GetTargetFromNodeNameL( aURI, index ) ); // Ignore index                
                if( (KErrNone != reason) || (KErrNone != ret) )
                    {
                    RDEBUG_2( "CIsAdapter::AddNodeObjectL: New WebTarget (%d)", reason);
                    CFavouritesItem *favItem = CFavouritesItem::NewLC();
                    RDEBUG_2( "CIsAdapter::AddNodeObjectL() CFavouritesItem ALLOC %x", favItem);

                    TPtrC8 namePtr( NSmlDmURI::LastURISeg(aURI) );
                    HBufC* nameBuf = HBufC::NewLC( namePtr.Length() );
                    RDEBUG_2( "CIsAdapter::AddNodeObjectL() HBufC nameBuf ALLOC %x", nameBuf);

                    nameBuf->Des().Copy( namePtr );
                    favItem->SetNameL( *nameBuf );
                    CleanupStack::PopAndDestroy( nameBuf );

                    favItem->SetUrlL(_L("http://"));
                    favItem->SetParentFolder(KFavouritesRootUid);

                    if( KErrNone == iFavDb.Add( *favItem, ETrue ) )
                        {
                        TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> newUID;
                        newUID.Num( favItem->Uid() );

                        if( KErrNone == iWebTargetManager->FindLUIDByURIL( aURI, data, index ) )
                            {
                            aStatus = CSmlDmAdapter::EAlreadyExists;
                            }
                         
                            /*
                        else
                        if( KErrNone == iWebTargetManager->FindURIByLUIDL( data, newUID, index ) )
                            {
                            aStatus = CSmlDmAdapter::EAlreadyExists;
                            }*/
                        else
                            {
                            RDEBUG8_2( "CIsAdapter::AddNodeObjectL: New WebTarget UID=%S", &newUID );
                            iWebTargetManager->FindURIByLUIDL( data, newUID, index ); // Replace if LUid exists...
                            iWebTargetManager->UpdateLUIDDatabaseL( aURI, newUID, index );
                            iCallBack->SetMappingL( aURI, newUID );
                            }
                        }
                    else
                        {
                        aStatus = CSmlDmAdapter::EError;
                        }

                    CleanupStack::PopAndDestroy( favItem );
                    }
                else
                    {
                    aStatus = CSmlDmAdapter::EAlreadyExists;
                    }
                }
            break;

            default:
                aStatus = CSmlDmAdapter::EError;
            break;
        };

    CleanupStack::PopAndDestroy(dataH);

    RDEBUG( "CIsAdapter::AddNodeObjectL() <" );
    }

// ------------------------------------------------------------------------------------------------
// CIsAdapter::_AddNodeObjectL
// ------------------------------------------------------------------------------------------------
void CIsAdapter::_AddNodeObjectL( const TDesC8& aURI, const TDesC8& aParentLUID, TInt aStatusRef )
    {
    RDEBUG( "CIsAdapter::_AddNodeObjectL() >" );

    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TRAPD(
        reason,
        AddNodeObjectL( status, uriPtrc, aParentLUID ) );
#else
	TRAPD(
        reason,
        AddNodeObjectL( status, aURI, aParentLUID ) );
#endif

    if( KErrNone != reason )
        {
        RDEBUG_2( "CIsAdapter::_AddNodeObjectL: Error code %d", reason);
        status = CSmlDmAdapter::EError;
        }

    iCallBack->SetStatusL(aStatusRef, status);

    RDEBUG( "CIsAdapter::_AddNodeObjectL() <" );
    }

// ------------------------------------------------------------------------------------------------
// CIsAdapter::_ExecuteCommandL
// ------------------------------------------------------------------------------------------------
void CIsAdapter::_ExecuteCommandL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/,
                                  const TDesC8& /*aArgument*/, const TDesC8& /*aType*/,
                                  TInt aStatusRef )
    {
    RDEBUG( "CIsAdapter::_ExecuteCommandL() >" );

    CSmlDmAdapter::TError ret = CSmlDmAdapter::EError;
    iCallBack->SetStatusL(aStatusRef, ret);    

    RDEBUG( "CIsAdapter::_ExecuteCommandL() <" );
    }

// ------------------------------------------------------------------------------------------------
// CIsAdapter::_ExecuteCommandL
// ------------------------------------------------------------------------------------------------
void CIsAdapter::_ExecuteCommandL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/,
                                  RWriteStream*& /*aStream*/, const TDesC8& /*aType*/,
                                  TInt aStatusRef )
    {
    RDEBUG( "CIsAdapter::_ExecuteCommandL() (stream) >" );

    CSmlDmAdapter::TError ret = CSmlDmAdapter::EError;
    iCallBack->SetStatusL(aStatusRef, ret);    

    RDEBUG( "CIsAdapter::_ExecuteCommandL() (stream) <" );
    }

// ------------------------------------------------------------------------------------------------
// CIsAdapter::_CopyCommandL
// ------------------------------------------------------------------------------------------------
void CIsAdapter::_CopyCommandL( const TDesC8& /*aTargetURI*/, const TDesC8& /*aTargetLUID*/,
                               const TDesC8& /*aSourceURI*/, const TDesC8& /*aSourceLUID*/,
                               const TDesC8& /*aType*/, TInt aStatusRef )
    {
    RDEBUG( "CIsAdapter::_CopyCommandL() >" );

    CSmlDmAdapter::TError ret = CSmlDmAdapter::EError;
    iCallBack->SetStatusL(aStatusRef, ret);    

    RDEBUG( "CIsAdapter::_CopyCommandL() <" );
    }

// ------------------------------------------------------------------------------------------------
// CIsAdapter::StartAtomicL
// ------------------------------------------------------------------------------------------------
void CIsAdapter::StartAtomicL()
    {
    RDEBUG( "CIsAdapter::StartAtomicL() ><" );
    }

// ------------------------------------------------------------------------------------------------
// CIsAdapter::CommitAtomicL
// ------------------------------------------------------------------------------------------------
void CIsAdapter::CommitAtomicL()
    {
    RDEBUG( "CIsAdapter::CommitAtomicL() ><" );
    }

// ------------------------------------------------------------------------------------------------
// CIsAdapter::RollbackAtomicL
// ------------------------------------------------------------------------------------------------
void CIsAdapter::RollbackAtomicL()
    {
    RDEBUG( "CIsAdapter::RollbackAtomicL() ><" );

    
    CSmlDmAdapter::TError status = CSmlDmAdapter::ERollbackFailed;
    iCallBack->SetStatusL(0, status);    
    }

// -----------------------------------------------------------------------------
// CIsAdapter::StreamType
// 
// -----------------------------------------------------------------------------
CTARMDmStreamAdapter::TAdapterStreamType CIsAdapter::StreamType( const TDesC8& /*aURI*/ )
    {
	RDEBUG( "CIsAdapter::StreamType()" );
    return CTARMDmStreamAdapter::EStreamToBuffer;
    }

// ------------------------------------------------------------------------------------------------
// CIsAdapter::StreamingSupport
// ------------------------------------------------------------------------------------------------
TBool CIsAdapter::StreamingSupport( TInt& aItemSize )
    {
	RDEBUG( "CIsAdapter::StreamingSupport()" );
    aItemSize = 1024; // Large images are streamed 
    return EFalse;	// don't support stream
    }

// ------------------------------------------------------------------------------------------------
// CIsAdapter::CompleteOutstandingCmdsL
// ------------------------------------------------------------------------------------------------
void CIsAdapter::CompleteOutstandingCmdsL()
    {
    RDEBUG( "CIsAdapter::CompleteOutstandingCmdsL() ><" );
    }

// ------------------------------------------------------------------------------------------------
// ImplementationTable
// ------------------------------------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] = 
    {
    { { KNSmlDMISAdapterImplUidValue }, (TProxyNewLPtr)CIsAdapter::NewL }
    };

// ------------------------------------------------------------------------------------------------
// ImplementationGroupProxy
// ------------------------------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

// ------------------------------------------------------------------------------------------------
// CIsAdapter::ConstructL
// ------------------------------------------------------------------------------------------------
void CIsAdapter::ConstructL(MSmlDmCallback *aDmCallback)
    {
    RDEBUG( "CIsAdapter::ConstructL() >" );
	
	TRAPD( errf, FeatureManager::InitializeLibL() );
	if( errf != KErrNone )
	{
		User::Leave(errf);
	}
	
	if(FeatureManager::FeatureSupported(KFeatureIdSapIdleSoftkeyAdapter))
	{
    iCallBack = aDmCallback;
	
    //User::LeaveIfError( iUiSrv.Connect() );
    //iUiSrv.UpdateShortcutTargetList();

    User::LeaveIfError( iFavSession.Connect() );
    User::LeaveIfError( iFavDb.Open( iFavSession, KBrowserBookmarks ) );

    TInt count = 0;
    // Get / create the count
    //
    CRepository* rep = CRepository::NewLC( KNSmlDMISAdapterCentRepUid );
    RDEBUG_2( "CIsAdapter::ConstructL() CRepository ALLOC %x", rep);

    TInt ret = rep->Get((TUint32)KRepositoryIdBookmarkCount, count);
    if( ret == KErrNotFound )
        {
        ret = rep->Create((TUint32)KRepositoryIdBookmarkCount, count);
        }

    CleanupStack::PopAndDestroy( rep );

    iWebTargetManager = new (ELeave) CIdleSoftkeysWebTargetManager(
                                                      iCallBack,
                                                      iFavDb 
                                                      );
    iWebTargetManager->CheckLUIDDatabaseL( EFalse );

    iAppTargetManager = new (ELeave) CIdleSoftkeysAppTargetManager(
                                                      iCallBack 
    
                                                      );
    // just creating a pointer to the class
    // this needs to be initialized before it is used
    // to initialize we use ReadBookmarksL and ReadTargetsL function
    iUiSettings = CUiSettingsUtil::NewL( );

    RDEBUG( "CIsAdapter::ConstructL() <" );
    }
    else
	{
		FeatureManager::UnInitializeLib();
   		User::Leave( KErrNotSupported );
	}
    
    }

// -------------------------------------------------------------------------------------
// CIsAdapter::FillNodeInfoL()
// Fills the node info in ddf structure
// -------------------------------------------------------------------------------------
void CIsAdapter::FillNodeInfoL( MSmlDmDDFObject& aNode,TSmlDmAccessTypes aAccTypes,
                                        MSmlDmDDFObject::TOccurence aOccurrence, MSmlDmDDFObject::TScope aScope, 
                                        MSmlDmDDFObject::TDFFormat aFormat,const TDesC8& aDescription,const TDesC8& aMimeType)
    {
    RDEBUG( "CIsAdapter::FillNodeInfoL() >" );

    aNode.SetAccessTypesL(aAccTypes);
    aNode.SetOccurenceL(aOccurrence);
    aNode.SetScopeL(aScope);
    aNode.SetDFFormatL(aFormat);
    aNode.SetDescriptionL(aDescription);
    if(aMimeType.Length() > 0)
        {
        aNode.AddDFTypeMimeTypeL(aMimeType);
        }

    RDEBUG( "CIsAdapter::FillNodeInfoL() <" );
    }

// -------------------------------------------------------------------------------------
// CIsAdapter::GetNodeIdentifier
// -------------------------------------------------------------------------------------
CIsAdapter::TIsNodeIdentifier CIsAdapter::GetNodeIdentifier(const TDesC8& aURI)
    {
    RDEBUG( "CIsAdapter::GetNodeIdentifier() >" );

	// SymbianConvergency NOTE! Look _DeleteObjectL() <-> removes the "./" from the aURI !!

    TInt numOfSegs = NSmlDmURI::NumOfURISegs( aURI );
    if(numOfSegs == 0)
        {
        return CIsAdapter::EIsNodeNotUsedAndAlwaysLast;
        }
    
    TPtrC8I seg1 = NSmlDmURI::URISeg( aURI, 0 );

    if(seg1 == KNSmlCustomizationNodeName)
        {
        if(numOfSegs == 1)
            {
            return EIsNodeCustomization;
            }

        // At least 2 segments
        TPtrC8I seg2 = NSmlDmURI::URISeg( aURI, 1 );
        if(seg2 == KNSmlIS_IdleSoftKeyNodeName)
            {
            if(numOfSegs == 2)
                {
                return CIsAdapter::EIsNodeIdleSoftkeys;
                }

            // At least 3 segments
            TPtrC8I seg3 = NSmlDmURI::URISeg( aURI, 2 );
            if(seg3 == KNSmlIS_SoftKeysNodeName)
                {
                if(numOfSegs == 3)
                    {
                    return CIsAdapter::EIsNodeSoftKeys;
                    }
                    
                // At least 4 segments  
                if(numOfSegs == 4)
                    {
                    return CIsAdapter::EIsNodeSoftKeysX;
                    }

                // At least 5 segments
                TPtrC8I seg5 = NSmlDmURI::URISeg( aURI, 4 );
                if(seg5 == KNSmlIS_SoftkeysIDNodeName)
                    {
                    return CIsAdapter::EIsNodeSoftKeysXID;
                    }
                    else
                if(seg5 == KNSmlIS_SoftkeysTargetRefNodeName)
                    {
                    return CIsAdapter::EIsNodeSoftKeysXTargetRef;
                    }
#ifdef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
                    else
                if(seg5 == KNSmlIS_SoftkeysImageNoneName)
                    {
                    return CIsAdapter::EIsNodeSoftKeysXImage;
                    }
#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
                    else
                if(seg5 == KNSmlIS_SoftkeysMaskNodeName)
                    {
                    return CIsAdapter::EIsNodeSoftKeysXMask;
                    }
#endif
#endif
                else
                    {
                    return CIsAdapter::EIsNodeNotUsedAndAlwaysLast;
                    }
                }
                else
            if(seg3 == KNSmlIS_TargetsNodeName)
                {
                if(numOfSegs == 3)
                    {
                    return CIsAdapter::EIsNodeTargets;
                    }

                // At least 4 segments  
                if(numOfSegs == 4)
                    {
                    return CIsAdapter::EIsNodeTargetsX;
                    }

                // At least 5 segments
                TPtrC8I seg5 = NSmlDmURI::URISeg( aURI, 4 );
                if(seg5 == KNSmlIS_TargetsIDNodeName)
                    {
                    return CIsAdapter::EIsNodeTargetsXID;
                    }
                    else
                if(seg5 == KNSmlIS_TargetsDescriptionNodeName)
                    {
                    return CIsAdapter::EIsNodeTargetsXDescription;
                    }
                // ...
                    else
                if(seg5 == KNSmlIS_TargetsCaptionName)
                    {
                    return CIsAdapter::EIsNodeTargetsXCaption;
                    }
#ifndef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
                    else
                if(seg5 == KNSmlIS_TargetsImageNodeName)
                    {
                    return CIsAdapter::EIsNodeTargetsXImage;
                    }
#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
                    else
                if(seg5 == KNSmlIS_TargetsMaskNodeName)
                    {
                    return CIsAdapter::EIsNodeTargetsXImageMask;
                    }
#endif
#endif
                // ...
                else
                
                    {
                    return CIsAdapter::EIsNodeNotUsedAndAlwaysLast;
                    }
                }
                else
            if(seg3 == KNSmlIS_WebTargetsNodeName)
                {
                if(numOfSegs == 3)
                    {
                    return CIsAdapter::EIsNodeWebTargets;
                    }

                // At least 4 segments  
                if(numOfSegs == 4)
                    {
                    return CIsAdapter::EIsNodeWebTargetsX;
                    }

                // At least 5 segments
                TPtrC8I seg5 = NSmlDmURI::URISeg( aURI, 4 );
                if(seg5 == KNSmlIS_WebTargetsIDNodeName)
                    {
                    return CIsAdapter::EIsNodeWebTargetsXID;
                    }
                    else
                if(seg5 == KNSmlIS_WebTargetsDescriptionNodeName)
                    {
                    return CIsAdapter::EIsNodeWebTargetsXDescription;
                    }
                    else
                if(seg5 == KNSmlIS_WebTargetsURLNodeName)
                    {
                    return CIsAdapter::EIsNodeWebTargetsXURL;
                    }
                    else
                if(seg5 == KNSmlIS_WebTargetsUserNameNodeName)
                    {
                    return CIsAdapter::EIsNodeWebTargetsXUserName;
                    }
                    else
                if(seg5 == KNSmlIS_WebTargetsPasswordNodeName)
                    {
                    return CIsAdapter::EIsNodeWebTargetsXPassword;
                    }
                    else
                if(seg5 == KNSmlIS_WebTargetsConRefNodeName)
                    {
                    return CIsAdapter::EIsNodeWebTargetsXConRef;
                    }
                // ...
                    else
                if(seg5 == KNSmlIS_WebTargetsCaptionNodeName)
                    {
                    return CIsAdapter::EIsNodeWebTargetsXCaption;
                    }
#ifndef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
                    else
                if(seg5 == KNSmlIS_WebTargetsImageNodeName)
                    {
                    return CIsAdapter::EIsNodeWebTargetsXImage;
                    }
#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
                    else
                if(seg5 == KNSmlIS_WebTargetsMaskNodeName)
                    {
                    return CIsAdapter::EIsNodeWebTargetsXImageMask;
                    }
#endif
#endif
                // ...
                else
                    {
                    return CIsAdapter::EIsNodeNotUsedAndAlwaysLast;
                    }                
                }
            else
                {
                return CIsAdapter::EIsNodeNotUsedAndAlwaysLast;
                }
            }
        else
            {
            return CIsAdapter::EIsNodeNotUsedAndAlwaysLast;
            }
        }
    else
        {
        return CIsAdapter::EIsNodeNotUsedAndAlwaysLast;
        }

    // Note: do not return anything here
    }

   
// -------------------------------------------------------------------------------------
// CIsAdapter::CopyAndTrimMimeType
// -------------------------------------------------------------------------------------
void CIsAdapter::CopyAndTrimMimeType(TDes8& aNewMime, const TDesC8& aType)
    {
    RDEBUG( "CIsAdapter::CopyAndTrimMimeType() >" );

    if(aNewMime.MaxLength() < aType.Length())
        {
        aNewMime.Copy( aType.Left(aNewMime.MaxLength()) );
        }
    else
        {
        aNewMime.Copy( aType );           
        }        
    aNewMime.TrimAll();
    aNewMime.LowerCase();
    
    TInt pos;
    while(( pos = aNewMime.Locate(' ')  ) != KErrNotFound)
        {
            aNewMime.Delete(pos, 1);
        }

    RDEBUG( "CIsAdapter::CopyAndTrimMimeType() <" );
    }

// -------------------------------------------------------------------------------------
// CIsAdapter::ListSoftkeysL
// -------------------------------------------------------------------------------------
TInt CIsAdapter::ListSoftkeysL( CBufFlat &aList )
    {
    RDEBUG( "CIsAdapter::ListSoftkeysL() >" );

    TInt i;
    for(i=0; i<NumberOfSoftkeys/*iUiSettings->GetSoftkeyCountL()*/; i++)
        {
        if(i>0)
            {
            aList.InsertL( aList.Size(), KNSmlISSeparator() );
            }

        HBufC8* softkeyName = GenerateSoftkeyNodeNameL( i );
        CleanupStack::PushL( softkeyName );
        
        aList.InsertL( aList.Size(), *softkeyName );

        CleanupStack::PopAndDestroy( softkeyName );
        }

    RDEBUG( "CIsAdapter::ListSoftkeysL() <" );
    return KErrNone;
    }

// -------------------------------------------------------------------------------------
// CIsAdapter::GenerateSoftkeyNodeNameL
// -------------------------------------------------------------------------------------
HBufC8* CIsAdapter::GenerateSoftkeyNodeNameL(TInt aIndex )
    {
    RDEBUG( "CIsAdapter::GenerateSoftkeyNodeNameL() >" );

    TInt index = aIndex;

    HBufC8* softkeyName = HBufC8::NewL( KPrefixSoftkeyNodeName().Length()
                                + MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64 );

    TPtr8 ptr( softkeyName->Des() );

    ptr.Format(KFormatSoftkeyNodeName, index+1);
    
    RDEBUG8_2( "CIsAdapter::GenerateSoftkeyNodeNameL() < %S", &softkeyName );
    return softkeyName;
    }

// -------------------------------------------------------------------------------------
// CIsAdapter::GetSoftkeyIndexFromUid
// -------------------------------------------------------------------------------------
TInt CIsAdapter::GetSoftkeyIndexFromUid( TInt aUid )
    {
    RDEBUG( "CIsAdapter::GetSoftkeyIndexFromUid() >" );

    switch( aUid )
        {
        case KScActionTypeUidValueSoftkeyLeft:
        return 0;
        case KScActionTypeUidValueSoftkeyRight:
        return 1;
        case KScActionTypeUidValueScrollkeyLeft:
        return 2;
        case KScActionTypeUidValueScrollkeyRight:
        return 3;
        case KScActionTypeUidValueScrollkeyUp:
        return 4;
        case KScActionTypeUidValueScrollkeyDown:
        return 5;
        case KScActionTypeUidValueScrollkeySelect:
        return 6;
        case KActiveIdleScUidValueItem1:
        return 7;
        case KActiveIdleScUidValueItem2:
        return 8;
        case KActiveIdleScUidValueItem3:
        return 9;
        case KActiveIdleScUidValueItem4:
        return 10;
        case KActiveIdleScUidValueItem5:
        return 11;
        default:
        return -1;
        }
        
    //    return aUid;
    }

// -------------------------------------------------------------------------------------
// CIsAdapter::GetSoftkeyUidFromNodeNameL
// -------------------------------------------------------------------------------------
TInt CIsAdapter::GetSoftkeyUidFromNodeNameL(const TDesC8& aNodeName)
    {
    RDEBUG( "CIsAdapter::GetSoftkeyUidFromNodeNameL() >" );

    TInt index = -1;
    TInt compareLen = KPrefixSoftkeyNodeName().Length();
    if(aNodeName.Left(compareLen) == KFormatSoftkeyNodeName().Left(compareLen))
        {
        TLex8 lex;
        lex.Assign( aNodeName );
        
        lex.Inc( compareLen );
        User::LeaveIfError( lex.Val(index) );
        index--;
        User::LeaveIfError( index>=0 && index<NumberOfSoftkeys/*iUiSettings->GetSoftkeyCountL()*/ ? KErrNone : KErrGeneral );
        
        HBufC8* softkeyName = GenerateSoftkeyNodeNameL( index );
        CleanupStack::PushL( softkeyName );
        
        TPtr8 ptr( softkeyName->Des() );
        User::LeaveIfError( ptr == aNodeName ? KErrNone : KErrGeneral );
        
        CleanupStack::PopAndDestroy( softkeyName );
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    RDEBUG_2( "CIsAdapter::GetSoftkeyUidFromNodeNameL() < %d", iSoftkeyList[ index ] );
    return iSoftkeyList[ index ];
    }

// -------------------------------------------------------------------------------------
// CIsAdapter::AddShortcutNameL
// -------------------------------------------------------------------------------------
void CIsAdapter::AddShortcutNameL(CBufFlat& aBuffer, TInt aUid)
    {
    RDEBUG( "CIsAdapter::AddShortcutNameL() >" );

    switch( aUid )
        {
        
            case KScActionTypeUidValueSoftkeyLeft:
            aBuffer.InsertL( aBuffer.Size(), KISLeftSoftkeyName() );
            break;
            case KScActionTypeUidValueSoftkeyRight:
            aBuffer.InsertL( aBuffer.Size(), KISRightSoftkeyName() );
            break;
            case KScActionTypeUidValueScrollkeyLeft:
            aBuffer.InsertL( aBuffer.Size(), KISScrollLeftSoftkeyName() );
            break;
            case KScActionTypeUidValueScrollkeyRight:
            aBuffer.InsertL( aBuffer.Size(), KISScrollRightSoftkeyName() );
            break;
            case KScActionTypeUidValueScrollkeyUp:
            aBuffer.InsertL( aBuffer.Size(), KISScrollUpSoftkeyName() );
            break;
            case KScActionTypeUidValueScrollkeyDown:
            aBuffer.InsertL( aBuffer.Size(), KISScrollDownSoftkeyName() );
            break;
            case KScActionTypeUidValueScrollkeySelect:
            aBuffer.InsertL( aBuffer.Size(), KISSelectionSoftkeyName() );
            break;

            case KActiveIdleScUidValueItem1:
            aBuffer.InsertL( aBuffer.Size(), KISAISoftkeyName1() );
            break;
            case KActiveIdleScUidValueItem2:
            aBuffer.InsertL( aBuffer.Size(), KISAISoftkeyName2() );
            break;
            case KActiveIdleScUidValueItem3:
            aBuffer.InsertL( aBuffer.Size(), KISAISoftkeyName3() );
            break;
            case KActiveIdleScUidValueItem4:
            aBuffer.InsertL( aBuffer.Size(), KISAISoftkeyName4() );
            break;
            case KActiveIdleScUidValueItem5:
            aBuffer.InsertL( aBuffer.Size(), KISAISoftkeyName5() );
            break;
            case KActiveIdleScUidValueItem6:
            aBuffer.InsertL( aBuffer.Size(), KISAISoftkeyName6() );
            break;
            
            
            
        }
    RDEBUG( "CIsAdapter::AddShortcutNameL() <" );
    }

#ifdef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES

void CIsAdapter::GetSoftkeyImageL(
                    TInt /*aSoftkeyUid*/,
                    CBufFlat &aImageBuffer,
                    TBool /*aGetMask*//*=EFalse*/ )
    {
    RDEBUG( "CIsAdapter::GetSoftkeyImageL() >" );

    TFileName  filename;
    TBuf8<128> mimetype;
    
    
    
    /*

    if( aGetMask )
        {
        User::LeaveIfError( iUiSrv.GetSoftkeyMask(
                                aSoftkeyUid,
                                filename,
                                mimetype
                            ) );
        }
    else
        {
        User::LeaveIfError( iUiSrv.GetSoftkeyBitmap(
                                aSoftkeyUid,
                                filename,
                                mimetype
                            ) );
        }
        
        */

    RFs   fileSession;
    RFile file;

    User::LeaveIfError( fileSession.Connect() );
    CleanupClosePushL( fileSession );

    //
    // Create new temp file
    //
    User::LeaveIfError( file.Open( fileSession, filename, EFileRead ) );
    CleanupClosePushL( file );

    //
    // Decode data to buffer
    //
    TInt size = 0;
    file.Size( size );
    HBufC8* buffer = HBufC8::NewLC( size );

    TPtr8 ptr( 0, 0 );
    ptr.Set( buffer->Des() );
    User::LeaveIfError( file.Read( ptr ) );

    EncodeImageL( aImageBuffer, ptr );

    CleanupStack::PopAndDestroy( buffer );

    // Close file
    CleanupStack::PopAndDestroy( &file );

    // Close fileSession
    CleanupStack::PopAndDestroy( &fileSession );

    RDEBUG( "CIsAdapter::GetSoftkeyImageL() <" );
    }

#endif

#ifndef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES

// -------------------------------------------------------------------------------------
// CIsAdapter::GetApplicationImageL
// -------------------------------------------------------------------------------------
void CIsAdapter::GetApplicationImageL(
                        TInt aIndex,
                        CBufFlat &aImageBuffer,
                        TBool aGetMask/*=EFalse*/ )
    {
    RDEBUG( "CIsAdapter::GetApplicationImageL() >" );

    GetShortcutImageL(
        EUiSrvRtTypeApp,
        aIndex,
        aImageBuffer,
        aGetMask
        );

    RDEBUG( "CIsAdapter::GetApplicationImageL() <" );
    }

// -------------------------------------------------------------------------------------
// CIsAdapter::GetBookmarkImageL
// -------------------------------------------------------------------------------------
void CIsAdapter::GetBookmarkImageL(
                        TInt aIndex,
                        CBufFlat &aImageBuffer,
                        TBool aGetMask/*=EFalse*/ )
    {
    RDEBUG( "CIsAdapter::GetBookmarkImageL() >" );

    GetShortcutImageL(
        EUiSrvRtTypeBm,
        aIndex,
        aImageBuffer,
        aGetMask
        );

    RDEBUG( "CIsAdapter::GetBookmarkImageL() <" );
    }

#endif

#ifndef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES

// -------------------------------------------------------------------------------------
// CIsAdapter::GetShortcutImageL
// -------------------------------------------------------------------------------------
void CIsAdapter::GetShortcutImageL(
                        TInt aRtMask,
                        TInt aIndex,
                        CBufFlat &aImageBuffer,
                        TBool aGetMask/*=EFalse*/ )
    {
    RDEBUG( "CIsAdapter::GetShortcutImageL() >" );
    TFileName  filename;
    TBuf8<128> mimetype;

    if( aGetMask )
        {
        User::LeaveIfError( iUiSrv.GetShortcutTargetMask(
                                aRtMask,
                                aIndex,
                                filename,
                                mimetype
                            ) );
        }
    else
        {
        User::LeaveIfError( iUiSrv.GetShortcutTargetBitmap(
                                aRtMask,
                                aIndex,
                                filename,
                                mimetype
                            ) );
        }

    RFs   fileSession;
    RFile file;

    User::LeaveIfError( fileSession.Connect() );
    CleanupClosePushL( fileSession );

    //
    // Create new temp file
    //
    User::LeaveIfError( file.Open( fileSession, filename, EFileRead ) );
    CleanupClosePushL( file );

    //
    // Decode data to buffer
    //
    TInt size = 0;
    file.Size( size );
    HBufC8* buffer = HBufC8::NewLC( size );

    TPtr8 ptr( 0, 0 );
    ptr.Set( buffer->Des() );
    User::LeaveIfError( file.Read( ptr ) );

    EncodeImageL( aImageBuffer, ptr );

    CleanupStack::PopAndDestroy( buffer );

    // Close file
    CleanupStack::PopAndDestroy( &file );

    // Close fileSession
    CleanupStack::PopAndDestroy( &fileSession );

    RDEBUG( "CIsAdapter::GetShortcutImageL() <" );
    }
#endif

#ifdef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
void CIsAdapter::SetSoftkeyImageL (
                            const TDesC8& aURI,
                            const TDesC8& aObject,
                            const TDesC8& aType)
    {
    RDEBUG( "CIsAdapter::SetSoftkeyImageL() >" );

	// SymbianConvergency NOTE! Functions that call this function will parse off the "./"
	// from the aURI -> no need to parse "./" here

    if( aType.Length() == 0 )
        {
        RDEBUG( "CIsAdapter::SetSoftkeyImageL() WARNING Mime type is empty!" );
        }

    CIsAdapter::TIsNodeIdentifier identifier = GetNodeIdentifier( aURI );
    TInt softkeyUid = 0;

    if(

           (EIsNodeSoftKeysXImage    != identifier)

#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK

        && (EIsNodeSoftKeysXMask     != identifier)

#endif

        )
        {
        User::Leave( KErrArgument );
        }

    //
    // Get shortcut target index
    //
    switch( identifier )
        {

        // Softkeys/*
        case EIsNodeSoftKeysXImage:
#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
        case EIsNodeSoftKeysXMask:
#endif
            {
            softkeyUid = GetSoftkeyUidFromNodeNameL( NSmlDmURI::URISeg( aURI, 3 ) );
            if(softkeyUid)
               RDEBUG_2( "CIsAdapter::GetShortcutImageL() < %d", softkeyUid  );
            }
        break;

        default:
            User::Leave( KErrArgument );
        break;

        }
	
    //
    // Decode image data
    //
    DecodeImageToFileL( aObject );
/*
    switch( identifier )
        {
        case EIsNodeSoftKeysXImage:
        User::LeaveIfError( iUiSrv.SetSoftkeyBitmap(
                                        softkeyUid,
                                        iTemporaryImageFile,
                                        aType ) );
        break;

#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
        case EIsNodeSoftKeysXMask:
        User::LeaveIfError( iUiSrv.SetSoftkeyMask(
                                        softkeyUid,
                                        iTemporaryImageFile,
                                        aType ) );
        break;

#endif
        }
        */

    RDEBUG( "CIsAdapter::SetSoftkeyImageL() <" );   
    }
#endif

#ifndef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
// -------------------------------------------------------------------------------------
// CIsAdapter::SetShortcutTargetImageL
// -------------------------------------------------------------------------------------
void CIsAdapter::SetShortcutTargetImageL(
                               const TDesC8& aURI,
                               const TDesC8& aObject,
                               const TDesC8& aType)
    {
    RDEBUG( "CIsAdapter::SetShortcutTargetImageL() >" );
    
    // SymbianConvergency NOTE! Functions that call this function will parse off the "./"
	// from the aURI -> no need to parse "./" here
    
    if( aType.Length() == 0 )
        {
        RDEBUG( "CIsAdapter::SetShortcutTargetImageL() WARNING Mime type is empty!" );
        }

    CIsAdapter::TIsNodeIdentifier identifier = GetNodeIdentifier( aURI );
    TInt targetIndex = 0;

    if(
           (EIsNodeTargetsXImage    != identifier)
        && (EIsNodeWebTargetsXImage != identifier)

#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK

        && (EIsNodeTargetsXImageMask    != identifier)
        && (EIsNodeWebTargetsXImageMask != identifier)

#endif

        )
        {
        User::Leave( KErrArgument );
        }

    //
    // Get shortcut target index
    //
    switch( identifier )
        {

        // Targets/*
        case EIsNodeTargetsXImage:
#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
        case EIsNodeTargetsXImageMask:
#endif
            {
            TBuf8<ISADAPTER_SHORTCUT_CAPTION_MAXLENGTH> caption;
            iAppTargetManager->GetTargetFromNodeNameL( NSmlDmURI::URISeg( aURI, 3 ), targetIndex, caption );
            }

        break;

        // WebTargets/*
        case EIsNodeWebTargetsXImage:
#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
        case EIsNodeWebTargetsXImageMask:
#endif
            {
            iWebTargetManager->CheckWebTargetsL();
            CFavouritesItem *favItem = CFavouritesItem::NewLC();
            RDEBUG_2( _L("CIsAdapter::SetShortcutTargetImageL() CFavouritesItem ALLOC %x"), favItem);
            if( KErrNone !=
                iWebTargetManager->GetTargetFromNodeNameL( NSmlDmURI::URISeg( aURI, 0, 4 ), *favItem, targetIndex ) )
                {
                User::Leave( KErrNotFound );
                }
            CleanupStack::PopAndDestroy( favItem );        
            }

        break;

        }

    //
    // Decode image data
    //
    DecodeImageToFileL( aObject );

    switch( identifier )
        {
        case EIsNodeTargetsXImage:
        User::LeaveIfError( iUiSrv.SetShortcutTargetBitmap( EUiSrvRtTypeApp,
                                        targetIndex,
                                        iTemporaryImageFile,
                                        aType ) );
        break;

        case EIsNodeWebTargetsXImage:
        User::LeaveIfError( iUiSrv.SetShortcutTargetBitmap( EUiSrvRtTypeBm,
                                        targetIndex,
                                        iTemporaryImageFile,
                                        aType ) );
        break;

#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
        case EIsNodeTargetsXImageMask:
        User::LeaveIfError( iUiSrv.SetShortcutTargetMask( EUiSrvRtTypeApp,
                                        targetIndex,
                                        iTemporaryImageFile,
                                        aType ) );
        break;

        case EIsNodeWebTargetsXImageMask:
        User::LeaveIfError( iUiSrv.SetShortcutTargetMask( EUiSrvRtTypeBm,
                                        targetIndex,
                                        iTemporaryImageFile,
                                        aType ) );
        break;
#endif
        }

    RDEBUG( "CIsAdapter::SetShortcutTargetImageL() <" );
    }
#endif

// -------------------------------------------------------------------------------------
// CIsAdapter::DecodeImageToFileL
// -------------------------------------------------------------------------------------
TInt CIsAdapter::DecodeImageToFileL( const TDesC8& aObject )
    {
    RDEBUG( "CIsAdapter::DecodeImageToFileL() >" );

    //
    // Write file to a temp file
    //
    RFs   fileSession;
    RFile file;

    User::LeaveIfError( fileSession.Connect() );
    CleanupClosePushL( fileSession );

    //
    // Delete previous temp file
    //
    if( iTemporaryImageFile.Length() > 0 )
        {
        fileSession.Delete( iTemporaryImageFile );
        iTemporaryImageFile.Zero();
        }

    //
    // Create new temp file
    //
    User::LeaveIfError( file.Temp( fileSession, KTempFilePath, iTemporaryImageFile, EFileWrite ) );
    CleanupClosePushL( file );

    //
    // Decode data to buffer
    //
    CBufFlat* buffer = CBufFlat::NewL( 128 );
    CleanupStack::PushL( buffer );
    DecodeImageL( *buffer, aObject );

    // Write buffer to file and release buffer
    User::LeaveIfError( file.Write( buffer->Ptr(0) ) );
    CleanupStack::PopAndDestroy( buffer );

    // Close file
    CleanupStack::PopAndDestroy( &file );

    // Close fileSession
    CleanupStack::PopAndDestroy( &fileSession );

    RDEBUG( "CIsAdapter::DecodeImageToFileL() <" );
    return KErrNone;
    }

// -------------------------------------------------------------------------------------
// CIsAdapter::DecodeImageL
// -------------------------------------------------------------------------------------
void CIsAdapter::DecodeImageL( CBufFlat& aBuffer, const TDesC8& aObject )
    {
    RDEBUG( "CIsAdapter::DecodeImageL() >" );

    //
    // Decoce from base 64
    //
    TImCodecB64 B64Coder;
    HBufC8 *uncoded = HBufC8::NewLC( ( aObject.Length() * 3 ) / 4 + 16 );
    RDEBUG_2( "CIsAdapter::DecodeImageL() HBufC8 uncoded ALLOC %x", uncoded);
    TPtr8 uncodedPtr( uncoded->Des() );
    
    TBool uncodeResult = TFileCoderB64::CheckB64Encode( aObject );//

	if( uncodeResult )
		{
		// use base64 decode
		B64Coder.Decode( aObject, uncodedPtr );
		aBuffer.InsertL( 0, uncodedPtr );
		}
	else
		{
		// original data
		aBuffer.InsertL( 0, aObject );
  		}

    CleanupStack::PopAndDestroy( uncoded );

    RDEBUG( "CIsAdapter::DecodeImageL() <" );
    }

// -------------------------------------------------------------------------------------
// CIsAdapter::EncodeImageL
// -------------------------------------------------------------------------------------
void CIsAdapter::EncodeImageL( CBufFlat& aBuffer, const TDesC8& aObject )
    {
    RDEBUG( "CIsAdapter::EncodeImageL() >" );

    //
    // Decoce from base 64
    //
    TImCodecB64 B64Coder;
    HBufC8 *encoded = HBufC8::NewLC( ( aObject.Length() * 4 ) / 3 + 16 );
    RDEBUG_2( "CIsAdapter::EncodeImageL() HBufC8 encoded ALLOC %x", encoded);
    TPtr8 encodedPtr( encoded->Des() );
    TBool encodeResult = B64Coder.Encode( aObject, encodedPtr );

    aBuffer.InsertL( 0, encodedPtr );

    CleanupStack::PopAndDestroy( encoded );

    RDEBUG( "CIsAdapter::EncodeImageL() <" );
    }

// -------------------------------------------------------------------------------------
// CIsAdapter::GetShortcutTargetURIL
// -------------------------------------------------------------------------------------
CSmlDmAdapter::TError CIsAdapter::GetShortcutTargetURIL(CBufFlat& aBuffer, TInt aUid )
    {
    RDEBUG( "CIsAdapter::GetShortcutTargetURIL() >" );

    TInt rtType = 0;
    if(iUiSettingsNotInitialized)
   	{	
   		iUiSettings->ReadTargetsL() ;
   		iUiSettings->ReadBookmarksL() ;
   		iUiSettingsNotInitialized = EFalse;
   	}
    iUiSettings->GetShortcutRtTypeL( aUid, rtType );
    // TScActionType acType = TUid::Uid( aUid );
    CSmlDmAdapter::TError retVal = CSmlDmAdapter::EOk;


    switch( rtType )
        {
            default:
                {
                TInt index = -1;
               	User::LeaveIfError(iUiSettings->GetShortcutTargetIndexL( aUid, index ));
				
                if( index == -1 )
                    {
                    return CSmlDmAdapter::ENotFound;
                    }
                else
                    {
                    HBufC8* nodeName = iAppTargetManager->GenerateNodeNameL( index );
                    CleanupStack::PushL( nodeName );
                    aBuffer.InsertL( aBuffer.Size(), KNSmlCustomizationNodeName() );
                    aBuffer.InsertL( aBuffer.Size(), KNSmlISSeparator() );
                    aBuffer.InsertL( aBuffer.Size(), KNSmlIS_IdleSoftKeyNodeName() );
                    aBuffer.InsertL( aBuffer.Size(), KNSmlISSeparator() );
                    aBuffer.InsertL( aBuffer.Size(), KNSmlIS_TargetsNodeName() );
                    aBuffer.InsertL( aBuffer.Size(), KNSmlISSeparator() );
                    aBuffer.InsertL( aBuffer.Size(), *nodeName );
                    CleanupStack::PopAndDestroy( nodeName );

                    return CSmlDmAdapter::EOk;
                    }
                }
            

            case KScRtTypeUidValueBookmark:
                {
                TInt bmId = -1, index = -1;
                User::LeaveIfError(iUiSettings->GetShortcutTargetIndexL( aUid, index ));
                User::LeaveIfError(iUiSettings->GetWebTargetAppUid( index, bmId ));
          
                TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> numBuf;
                numBuf.Num( bmId );
                TInt ind = -1;

                HBufC8* dataH = HBufC8::NewLC( KRepositoryEntryMaxLength );
                TPtr8 data( dataH->Des() );
                TBool ret = iWebTargetManager->FindURIByLUIDL( data, numBuf, ind );
                User::LeaveIfError( ret );

                aBuffer.InsertL( aBuffer.Size(), data );

                CleanupStack::PopAndDestroy( dataH );
                }
            break;
            
        }
        

    RDEBUG_2( "CIsAdapter::GetShortcutTargetURIL() < %d", retVal );
    return retVal;
    }

// -------------------------------------------------------------------------------------
// CIsAdapter::SetShortcutTargetL
// -------------------------------------------------------------------------------------
void CIsAdapter::SetShortcutTargetL(const TDesC8& aURI, TInt aUid)
    {
    RDEBUG8_2( "CIsAdapter::SetShortcutTargetL() > %S ", &aURI );

	// SymbianConvergency NOTE! this should be ok, for the aURI part
    if(iUiSettingsNotInitialized)
   	{	
   		iUiSettings->ReadTargetsL() ;
   		iUiSettings->ReadBookmarksL() ;
   		iUiSettingsNotInitialized = EFalse;
   	}
    TPtrC8 uri( aURI );
    if( uri.Left(2).Compare( _L8("./") ) == 0 )
        {
        uri.Set( aURI.Right( aURI.Length() - 2 ) );
        }

    CIsAdapter::TIsNodeIdentifier identifier = GetNodeIdentifier( uri );
    if( (EIsNodeTargetsX != identifier) &&
        (EIsNodeWebTargetsX != identifier) )
            {
            User::Leave( KErrArgument );
            }

    HBufC16* buf        = CreateCopy16LC( uri );
    TPtrC8   targetNode = NSmlDmURI::URISeg( uri, 2 );
    TPtrC8   segment    = NSmlDmURI::URISeg( uri, 3 );
    TInt index = -1;

    if( targetNode == KNSmlIS_WebTargetsNodeName() )
        {
        iWebTargetManager->CheckWebTargetsL();
        CFavouritesItem *favItem = CFavouritesItem::NewLC();
        RDEBUG_2( "CIsAdapter::SetShortcutTargetL() CFavouritesItem ALLOC %x", favItem);

        TInt status = KErrNone;
        TRAPD( reason,
               status = iWebTargetManager->GetTargetFromNodeNameL( uri, *favItem, index ) );
        if( (KErrNone == reason) && (0 != favItem) && (KErrNone == status) && (-1 != index) )
            {
                  User::LeaveIfError(iUiSettings->SetShortcutTargetWebIndexL (aUid, index));
            }
        else
            {
            RDEBUG8_2( "CIsAdapter::SetShortcutTargetL: Error in argument, WebTarget %S", &targetNode);
            User::Leave( KErrArgument );
            }

        CleanupStack::PopAndDestroy( favItem );
        }
    else
        {
        if( targetNode == KNSmlIS_TargetsNodeName() )
            {
            // Set new target
            TBuf8<ISADAPTER_SHORTCUT_CAPTION_MAXLENGTH> caption;
            iAppTargetManager->GetTargetFromNodeNameL( segment, index, caption );
     		User::LeaveIfError(iUiSettings->SetShortcutTargetAppIndexL (aUid, index));
            }
        else
            {
            RDEBUG8_2( "CIsAdapter::SetShortcutTargetL: Error in argument, AppTarget %S", &targetNode);
            User::Leave( KErrArgument );
            }
        }

    CleanupStack::PopAndDestroy( buf );

    RDEBUG( "CIsAdapter::SetShortcutTargetL() <" );
    }

// -------------------------------------------------------------------------------------
// CIsAdapter::CreateCopy16LC
// -------------------------------------------------------------------------------------
HBufC16* CIsAdapter::CreateCopy16LC( const TDesC8& aText )
    {
    RDEBUG( "CIsAdapter::CreateCopy16LC() >" );

    HBufC16* buf = HBufC16::NewLC( aText.Length() );
    RDEBUG_2( "CIsAdapter::CreateCopy16LC() HBufC16 buf ALLOC %x", buf);
    buf->Des().Copy( aText );

    RDEBUG( "CIsAdapter::CreateCopy16LC() <" );
    return buf;
    }

// -----------------------------------------------------------------------------
// CIsAdapter::PolicyRequestResourceL
// -----------------------------------------------------------------------------
//
//#ifdef __SAP_POLICY_MANAGEMENT
TPtrC8 CIsAdapter::PolicyRequestResourceL( const TDesC8& /*aURI*/ )
{
	if(! FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement ))
	{
    User::Leave(KErrNotSupported);
  }  
  
    RDEBUG8_2( "CIsAdapter::PolicyRequestResourceL() >< %S",
                &PolicyEngineXACML::KCustomizationManagement() );

	return PolicyEngineXACML::KCustomizationManagement();
		
}
//#endif	

