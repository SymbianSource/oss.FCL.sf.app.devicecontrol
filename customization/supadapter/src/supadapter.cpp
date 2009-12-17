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
* Description:  DM Startup Adapter
*
*/


/*
./Customization -+--> Startup -+--> WelcomeData
  */

// ------------------------------------------------------------------------------------------------
// Use this flag to activate mime type checking in adapter
// Data with unsupported mime types will be rejected
//  
// #define SUPADAPTER_CHECK_MIME_TYPES
#ifdef	SUPADAPTER_CHECK_MIME_TYPES

#define SUPADAPTER_CHECK_TEXT_MIME_TYPE
#define SUPADAPTER_CHECK_IMAGE_MIME_TYPE

#endif

// ------------------------------------------------------------------------------------------------
#include "supadapter.h"

// symbian
#ifndef __WINS_DEBUG_TESTING__
    #include <implementationproxy.h> // For TImplementationProxy definition
#endif
#include <fbs.h>
#include <f32file.h>
#include <imcvcodc.h>
// s60
#include <AknUtils.h>
#include <sysutil.h>
#include <startupdomaincrkeys.h>
// tarm
//#ifdef __SAP_POLICY_MANAGEMENT
#include <DMCert.h>
#include <SettingEnforcementInfo.h>
#include <PolicyEngineXACML.h>
//#endif
#include "nsmldmuri.h"
#include "TPtrC8I.h"
#include "TARMCharConv.h"
#include "FileCoderB64.h"
#include "debug.h"
#include <featmgr.h>
#include <utf.h> 
/**
*
* Supported MIME types of this adapter
*
**/
_LIT8(KText,              "text/");
_LIT8(KMimeTypeTextPlain, "text/plain");

#ifdef SUPADAPTER_CHECK_TEXT_MIME_TYPE
_LIT8(KMimeTypeTextAny,   "text/*");
#endif

_LIT8(KImage,			  "image/");
_LIT8(KMimeTypeImageMbm,  "image/x-epoc-mbm");

#ifdef SUPADAPTER_CHECK_IMAGE_MIME_TYPE
_LIT8(KMimeTypeImageGif,  "image/gif");
_LIT8(KMimeTypeImagePng,  "image/png");
_LIT8(KMimeTypeImageJpg,  "image/jpeg");
_LIT8(KMimeTypeImageAny,  "image/*");
#endif

// ------------------------------------------------------------------------------------------------

/**
* Folder for temporary files
**/
_LIT(KTempFilePath, "c:\\system\\temp\\");

// This length value is copied from S60/Startup/StartupUserWelcomeNote.h
const TInt KStartupTBufMaxLength( 100 );

// ------------------------------------------------------------------------------------------------
// CSupAdapter* CSupAdapter::NewL( )
// ------------------------------------------------------------------------------------------------
CSupAdapter* CSupAdapter::NewL(MSmlDmCallback* aDmCallback )
	{
	RDEBUG( "CSupAdapter::NewL()" );

	CSupAdapter* self = NewLC( aDmCallback );
	CleanupStack::Pop();
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CSupAdapter* CSupAdapter::NewLC( )
// ------------------------------------------------------------------------------------------------
CSupAdapter* CSupAdapter::NewLC(MSmlDmCallback* aDmCallback )
	{
	RDEBUG( "CSupAdapter::NewLC()" );

	CSupAdapter* self = new (ELeave) CSupAdapter(aDmCallback);
	CleanupStack::PushL(self);
	self->ConstructL(aDmCallback);
	return self;
	}


// ------------------------------------------------------------------------------------------------
// CSupAdapter::CSupAdapter()
// ------------------------------------------------------------------------------------------------
CSupAdapter::CSupAdapter(TAny* aEcomArguments)
: CTARMDmStreamAdapter((MSmlDmCallback*)aEcomArguments)
, iUndoImage(0)
, iUndoText(0)
, iUndoNoteType(-1)
	{
	}


// ------------------------------------------------------------------------------------------------
// CSupAdapter::~CSupAdapter()
// ------------------------------------------------------------------------------------------------
CSupAdapter::~CSupAdapter()
	{
	RDEBUG( "CSupAdapter::~CSupAdapter()" );

    delete iUndoImage; iUndoImage = 0;
    delete iUndoText;  iUndoText  = 0;

    if( iTemporaryImageFile.Length() > 0 )
        {
        RFs fs;
        if (fs.Connect())
        	{
        fs.Delete( iTemporaryImageFile );
        fs.Close();
      		}
        iTemporaryImageFile.Zero();
        }

//#ifdef __SAP_POLICY_MANAGEMENT
if(FeatureManager::FeatureSupported(KFeatureIdSapStartupAdapter))
{
	if(FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement ))
	{
	iPR.Close();
	iPE.Close();
	}

}
	
//#endif
FeatureManager::UnInitializeLib();
	}


// ------------------------------------------------------------------------------------------------
// CSupAdapter::ConstructL
// ------------------------------------------------------------------------------------------------
void CSupAdapter::ConstructL(MSmlDmCallback *aDmCallback)
	{
	RDEBUG( "CSupAdapter::ConstructL()" );
		TRAPD( errf, FeatureManager::InitializeLibL() );
		if( errf != KErrNone )
		{
			User::Leave(errf);
		}
			if(FeatureManager::FeatureSupported(KFeatureIdSapStartupAdapter))
			{
	iCallBack = aDmCallback;

			//#ifdef __SAP_POLICY_MANAGEMENT
			if(FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement ))
			{
    User::LeaveIfError( iPE.Connect() );
    User::LeaveIfError( iPR.Open( iPE ) );
			}
			    
			//#endif

    iTemporaryImageFile.Zero();

#ifdef __WINS_DEBUG_TEST__
    DebugTestsL();
#endif
			}
			else
			{
				
		   		User::Leave( KErrNotSupported );
			}
	}


// ------------------------------------------------------------------------------------------------
//  CSupAdapter::DDFVersionL()
// ------------------------------------------------------------------------------------------------
void CSupAdapter::DDFVersionL(CBufBase& aDDFVersion)
	{
	RDEBUG( "CSupAdapter::DDFVersionL()" );

	aDDFVersion.InsertL(0,KNSmlSUPDDFVersion);
	}


// ------------------------------------------------------------------------------------------------
//  CSupAdapter::DDFStructureL()
//
// ------------------------------------------------------------------------------------------------
void CSupAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
	{
	RDEBUG( "CSupAdapter::DDFStructureL()" );

	TSmlDmAccessTypes accessTypesGet;
	accessTypesGet.SetGet();

	/*
	Node: ./Customization
	This interior node is the common parent to all customization objects.
	Status: Required
	Occurs: One
	Format: Node
	Access Types: Get
	Values: N/A
	*/
	MSmlDmDDFObject& apps = aDDF.AddChildObjectL(KNSmlCustomizationNodeName);
	FillNodeInfoL(apps,accessTypesGet,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,
	    MSmlDmDDFObject::ENode, KNSmlSUPDescription,KNullDesC8());

	/*
	Node: ./Customization/Startup
	Startup is a common parent node to nodes customizing startup functionality
	Status: Required
	Occurs: One
	Format: Node
	Access Types: Get
	Values: N/A
	*/
	MSmlDmDDFObject& groupNode = apps.AddChildObjectL(KNSmlSUPStartupNodeName);
	FillNodeInfoL(groupNode,accessTypesGet,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode,KNSmlSUPStartupNodeDescription,KNullDesC8());

	/*
	Node: ./Customization/Startup/Data
	Image node stores the Startup image as binary data, and its runtime Type determines the interpretation.
·	Status: Optional
·	Occurs: One
·	Format: bin
·	Access Types: Replace
·	Values: N/A
	*/
	TSmlDmAccessTypes accessTypesReplace;
	accessTypesReplace.SetReplace();

	MSmlDmDDFObject& imageNode = groupNode.AddChildObjectL(KNSmlSUPWelcomeDataNodeName);
	FillNodeInfoL(imageNode,accessTypesReplace,MSmlDmDDFObject::EZeroOrOne,MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EBin,KNSmlSUPWelcomeDataNodeDescription,KNSmlSUPTextPlain());
	imageNode.AddDFTypeMimeTypeL(KMimeTypeImageMbm);
	
	/*
	Node: ./Customization/Startup/Text
	Text node stores the Startup text as text, and its runtime Type determines the interpretation.
·	Status: Optional
·	Occurs: One
·	Format: Chr
·	Access Types: Get, Replace
·	Values: N/A
	*/
	TSmlDmAccessTypes accessTypesGetReplace;
	accessTypesGetReplace.SetGet();
	accessTypesGetReplace.SetReplace();

	MSmlDmDDFObject& textNode = groupNode.AddChildObjectL(KNSmlSUPWelcomeTextNodeName);
	FillNodeInfoL(textNode,accessTypesGetReplace,MSmlDmDDFObject::EZeroOrOne,MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr,KNSmlSUPWelcomeTextNodeDescription,KNSmlSUPTextPlain());
	}


// ------------------------------------------------------------------------------------------------
//  CSupAdapter::UpdateLeafObjectL()
// ------------------------------------------------------------------------------------------------
void CSupAdapter::UpdateLeafObjectL( CSmlDmAdapter::TError& aStatus, const TDesC8& aURI,
                                     const TDesC8& /*aLUID*/, const TDesC8& aObject, const TDesC8& aType )
	{
	RDEBUG( "CSupAdapter::UpdateLeafObjectL()" );

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	CSupAdapter::TSupNodeIdentifier identifier = GetNodeIdentifier( uriPtrc );
#else
	CSupAdapter::TSupNodeIdentifier identifier = GetNodeIdentifier( aURI );
#endif

	TStartupAdapterLeafType type;

    switch( identifier )
        {
            case ESupNodeWelcomeText:
            	{
                HBufC16* source = CTARMCharConv::ConvertFromUtf8LC( aObject );
                TInt sourceLength = source->Length();
                TInt destLength = KStartupTBufMaxLength<sourceLength?KStartupTBufMaxLength:sourceLength;
                TPtrC16 sourcePtr( source->Ptr(), destLength );

				aStatus = CSmlDmAdapter::EOk;
				SetStartupTextL(aStatus, sourcePtr);
				CleanupStack::PopAndDestroy( source );
            	}
            	break;
            case ESupNodeWelcomeData:
				type = GetObjectType( aType );
				switch(type)
					{
					case CSupAdapter::EImage:
					    aStatus = CSmlDmAdapter::EOk;
					    SetStartupImageL(aStatus, aObject, aType);
						break;

					case CSupAdapter::EUnknown:
					    aStatus = CSmlDmAdapter::EInvalidObject;
						break;
					}
            	break;
            default:
            aStatus = CSmlDmAdapter::EError;
        }
	}


// ------------------------------------------------------------------------------------------------
//  CSupAdapter::_UpdateLeafObjectL()
// ------------------------------------------------------------------------------------------------
void CSupAdapter::_UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aObject, const TDesC8& aType, TInt aStatusRef )
	{
	RDEBUG8_3( "CSupAdapter::_UpdateLeafObjectL() (%S) (%S)", &aURI, &aType );

	TBuf8<KStartupTBufMaxLength> mimeType;
	CopyAndTrimMimeType(mimeType, aType);

//#ifdef __SAP_POLICY_MANAGEMENT
if(FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement ))
{
	User::LeaveIfError( CheckPolicyL() );	
}
//#endif

	CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;

    TRAPD(
        reason,
        // NOTE! UpdateLeafObjectL removes the "./" from the beginning
        UpdateLeafObjectL( status, aURI, aLUID, aObject, mimeType ) );

    if( KErrNone != reason )
        {
        status = CSmlDmAdapter::EError;
        }

    iCallBack->SetStatusL(aStatusRef, status);
	}


// ------------------------------------------------------------------------------------------------
//  CSupAdapter::_UpdateLeafObjectL()
// ------------------------------------------------------------------------------------------------
void CSupAdapter::_UpdateLeafObjectL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/, RWriteStream*& /*aStream*/, const TDesC8& /*aType*/, TInt aStatusRef )
    {       
	RDEBUG( "CSupAdapter::_UpdateLeafObjectL() (stream)" );

    CSmlDmAdapter::TError status = CSmlDmAdapter::EError;
    iCallBack->SetStatusL(aStatusRef, status);
    }


// ------------------------------------------------------------------------------------------------
// CSupAdapter::_DeleteObjectL
// ------------------------------------------------------------------------------------------------
void CSupAdapter::_DeleteObjectL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/, TInt aStatusRef )
	{
	RDEBUG( "CSupAdapter::_DeleteObjectL()" );

    CSmlDmAdapter::TError status = CSmlDmAdapter::EError;
    iCallBack->SetStatusL(aStatusRef, status);
	}


// ------------------------------------------------------------------------------------------------
// CSupAdapter::_ExecuteCommandL
// ------------------------------------------------------------------------------------------------
void CSupAdapter::_ExecuteCommandL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/, const TDesC8& /*aArgument*/, const TDesC8& /*aType*/, TInt aStatusRef )
    {
	RDEBUG( "CSupAdapter::_ExecuteCommandL()" );

    CSmlDmAdapter::TError status = CSmlDmAdapter::EError;
    iCallBack->SetStatusL(aStatusRef, status);        
    }
    
    
// ------------------------------------------------------------------------------------------------
// CSupAdapter::_ExecuteCommandL
// ------------------------------------------------------------------------------------------------
void CSupAdapter::_ExecuteCommandL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/, RWriteStream*& /*aStream*/, const TDesC8& /*aType*/, TInt aStatusRef)
    {
	RDEBUG( "CSupAdapter::_ExecuteCommandL() (stream)" );

    CSmlDmAdapter::TError status = CSmlDmAdapter::EError;
    iCallBack->SetStatusL(aStatusRef, status);        
    }
    
    
// ------------------------------------------------------------------------------------------------
// CSupAdapter::_CopyCommandL
// ------------------------------------------------------------------------------------------------
void CSupAdapter::_CopyCommandL( const TDesC8& /*aTargetURI*/, const TDesC8& /*aTargetLUID*/, const TDesC8& /*aSourceURI*/, const TDesC8& /*aSourceLUID*/, const TDesC8& /*aType*/, TInt aStatusRef)
    {
	RDEBUG( "CSupAdapter::_CopyCommandL()" );

    CSmlDmAdapter::TError status = CSmlDmAdapter::EError;
    iCallBack->SetStatusL(aStatusRef, status);        
    }


// ------------------------------------------------------------------------------------------------
// CSupAdapter::StartAtomicL
// ------------------------------------------------------------------------------------------------
void CSupAdapter::StartAtomicL()
    {
	RDEBUG( "CSupAdapter::StartAtomicL()" );

    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;

    delete iUndoImage; iUndoImage = 0;
    delete iUndoText;  iUndoText  = 0;
    iUndoNoteType = -1;
    
    // Copy startup image & text
    TInt imageSize;
    iUndoImage = GetStartupImageL(status, KMimeTypeImageMbm, imageSize);
    iUndoText  = GetStartupTextL(status, KMimeTypeTextPlain);
    }
    
// ------------------------------------------------------------------------------------------------
// CSupAdapter::CommitAtomicL
// ------------------------------------------------------------------------------------------------
void CSupAdapter::CommitAtomicL()
    {
	RDEBUG( "CSupAdapter::CommitAtomicL()" );

    delete iUndoImage; iUndoImage = 0;
    delete iUndoText;  iUndoText  = 0;        
    iUndoNoteType = -1;
    }
    
// ------------------------------------------------------------------------------------------------
// CSupAdapter::RollbackAtomicL
// ------------------------------------------------------------------------------------------------
void CSupAdapter::RollbackAtomicL()
    {
	RDEBUG( "CSupAdapter::RollbackAtomicL()" );

    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;

    if(iUndoImage != 0)        
        {
            SetStartupImageL(status, iUndoImage->Ptr(0), KMimeTypeImageMbm);
            
            if(status != CSmlDmAdapter::EOk)
                {
                    status = CSmlDmAdapter::ERollbackFailed;
                }
                
            delete iUndoImage; iUndoImage = 0;
        }

    if(iUndoText != 0)
        {
            HBufC16* source   = CTARMCharConv::ConvertFromUtf8LC( iUndoText->Ptr( 0 ) );
            TInt sourceLength = source->Length();
            TInt destLength   = KStartupTBufMaxLength<sourceLength?KStartupTBufMaxLength:sourceLength;
            TPtrC16 sourcePtr( source->Ptr(), destLength );

            SetStartupTextL(status, sourcePtr);
            CleanupStack::PopAndDestroy( source );
                
            if(status != CSmlDmAdapter::EOk)
                {
                status = CSmlDmAdapter::ERollbackFailed;
                }

            delete iUndoText; iUndoText = 0;
        }

    if(iUndoNoteType != -1)
        {
            RUISettingsSrv uiSrv;
            User::LeaveIfError( uiSrv.Connect() );
            CleanupClosePushL( uiSrv );

            User::LeaveIfError( uiSrv.SetStartupNoteType( iUndoNoteType ) );

            CleanupStack::PopAndDestroy( &uiSrv );
                
            iUndoNoteType = -1;
        }

    
    iCallBack->SetStatusL(0, status);
    }

// -----------------------------------------------------------------------------
// CSupAdapter::StreamType
// 
// -----------------------------------------------------------------------------
//
CTARMDmStreamAdapter::TAdapterStreamType CSupAdapter::StreamType( const TDesC8& aURI )
    {
#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	CSupAdapter::TSupNodeIdentifier identifier = GetNodeIdentifier( uriPtrc );
#else
	CSupAdapter::TSupNodeIdentifier identifier = GetNodeIdentifier( aURI );
#endif    

    switch( identifier )
        {
            case ESupNodeWelcomeData:
            return CTARMDmStreamAdapter::EStreamToBuffer;

            default:
            return CTARMDmStreamAdapter::EStreamToNone;
        };
    }

// ------------------------------------------------------------------------------------------------
// CSupAdapter::StreamingSupport
// ------------------------------------------------------------------------------------------------
TBool CSupAdapter::StreamingSupport( TInt& aItemSize )
    {
	RDEBUG( "CSupAdapter::StreamingSupport()" );
    aItemSize = 1024; // Large images are streamed
    return ETrue;
    }

// ------------------------------------------------------------------------------------------------
// CSupAdapter::CompleteOutstandingCmdsL
// ------------------------------------------------------------------------------------------------
void CSupAdapter::CompleteOutstandingCmdsL()
    {
	RDEBUG( "CSupAdapter::CompleteOutstandingCmdsL()" );
    }

// ------------------------------------------------------------------------------------------------
// CSupAdapter::FetchLeafObjectL
// ------------------------------------------------------------------------------------------------
CSmlDmAdapter::TError CSupAdapter::FetchLeafObjectL( const TDesC8& aURI, const TDesC8& /*aLUID*/, TDes8& aNewMime, CBufFlat* object, TInt &aUncodedSize )
    {
    RDEBUG( "CSupAdapter::FetchLeafObjectL() begin:" );

    TBuf8<KStartupTBufMaxLength> mimeType;
	CopyAndTrimMimeType(mimeType, aNewMime);

//#ifdef __SAP_POLICY_MANAGEMENT
  if(FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement ))
{
	User::LeaveIfError( CheckPolicyL() );	
}
 //#endif

    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;
    TStartupAdapterLeafType type;
    
#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	CSupAdapter::TSupNodeIdentifier identifier = GetNodeIdentifier( uriPtrc );
#else
	CSupAdapter::TSupNodeIdentifier identifier = GetNodeIdentifier( aURI );
#endif   

    switch( identifier )
        {
            case ESupNodeCustomization:
            case ESupNodeStartup:
                // No actions, leave object empty
            break;
        case ESupNodeWelcomeData:
            status = CSmlDmAdapter::EError;
            break;
        case ESupNodeWelcomeText:
                			    CBufFlat *object2 = GetStartupTextL(status, mimeType);
                			    CleanupStack::PushL(object2);

                			    object->InsertL(0, object2->Ptr(0));
                                aUncodedSize = object->Size();

                                aNewMime.Copy( KMimeTypeTextPlain );

                			    CleanupStack::PopAndDestroy(object2);

            break;
        case CSupAdapter::ESupNodeNotUsedAndAlwaysLast:
                			default:
                			    
                			    status = CSmlDmAdapter::EError;
            break;
        };
RDEBUG( "CSupAdapter::FetchLeafObjectL() End:" );
    return status;
    }

// ------------------------------------------------------------------------------------------------
//  CSupAdapter::_FetchLeafObjectL
// ------------------------------------------------------------------------------------------------
void CSupAdapter::_FetchLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aType, TInt aResultsRef, TInt aStatusRef )
	{
	RDEBUG( "CSupAdapter::_FetchLeafObjectL()" );

	TBuf8<KStartupTBufMaxLength> mimeType;
	CopyAndTrimMimeType(mimeType, aType);

	CBufFlat *object = CBufFlat::NewL(128);
	CleanupStack::PushL( object );

    TInt uncodedSize;
    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;

    TRAPD(
        reason,
        // note, FetchLeafObjectL will parse off the "./" from aURI, SymbianConvergency
	    status = FetchLeafObjectL( aURI, aLUID, mimeType, object, uncodedSize ) );

    if( KErrNone != reason )
        {
        status = CSmlDmAdapter::EError;
        }

	iCallBack->SetStatusL(aStatusRef, status);

	if( status == CSmlDmAdapter::EOk)
	    {
        object->Compress();
        iCallBack->SetResultsL( aResultsRef, *object, mimeType );
	    }

	CleanupStack::PopAndDestroy( object );
    }

// ------------------------------------------------------------------------------------------------
//  CSupAdapter::_FetchLeafObjectSizeL
// ------------------------------------------------------------------------------------------------
void CSupAdapter::_FetchLeafObjectSizeL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aType, TInt aResultsRef, TInt aStatusRef )
    {
	RDEBUG("CSupAdapter::_FetchLeafObjectSizeL()");

	TBuf8<KStartupTBufMaxLength> mimeType;
	CopyAndTrimMimeType(mimeType, aType);

	CBufFlat *object = CBufFlat::NewL(128);
	CleanupStack::PushL( object );

    TInt uncodedSize;
    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;

    TRAPD(
        reason,
        // NOTE! FetchLeafObjectL will parse off the "./" from the aURI <-> SymbianConveergency
        status = FetchLeafObjectL( aURI, aLUID, mimeType, object, uncodedSize ) );

    if( KErrNone != reason )
        {
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
        iCallBack->SetResultsL( aResultsRef, *object2, KMimeTypeTextPlain );

        CleanupStack::PopAndDestroy( object2 );
	    }

	CleanupStack::PopAndDestroy( object );
    }

// ------------------------------------------------------------------------------------------------
//  CSupAdapter::_ChildURIListL
// ------------------------------------------------------------------------------------------------
void CSupAdapter::_ChildURIListL( const TDesC8& aURI, const TDesC8& /*aLUID*/, const CArrayFix<TSmlDmMappingInfo>& /*aPreviousURISegmentList*/, TInt aResultsRef, TInt aStatusRef )
	{
	RDEBUG("CSupAdapter::_ChildURIListL()");

	CSmlDmAdapter::TError ret = CSmlDmAdapter::EOk;

//#ifdef __SAP_POLICY_MANAGEMENT
if(FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement ))
{
	User::LeaveIfError( CheckPolicyL() );	
}
//#endif

	CBufBase *currentList = CBufFlat::NewL(128);
	CleanupStack::PushL(currentList);

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	CSupAdapter::TSupNodeIdentifier identifier = GetNodeIdentifier( uriPtrc );
#else
	CSupAdapter::TSupNodeIdentifier identifier = GetNodeIdentifier( aURI );
#endif

    switch( identifier )
        {
            case ESupNodeCustomization:
            currentList->InsertL(0,KNSmlSUPStartup8());
            break;

            case ESupNodeStartup:
            currentList->InsertL(currentList->Size(),KNSmlSUPWelcomeDataNodeName());
            currentList->InsertL(currentList->Size(),KNSmlSUPWelcomeTextNodeName());
            break;

            case ESupNodeWelcomeData:
            case ESupNodeWelcomeText:
            // Empty
            break;

            default:
#ifdef __TARM_SYMBIAN_CONVERGENCY            
            if( uriPtrc.Length() == 0 )
#else
			if( aURI.Length() == 0 )
#endif            
                {
                currentList->InsertL(0,KNSmlCustomizationNodeName8());
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
		iCallBack->SetResultsL(aResultsRef, *currentList, KNSmlSUPTextPlain);
		}

	CleanupStack::PopAndDestroy( currentList );

	}

// ------------------------------------------------------------------------------------------------
// CSupAdapter::_AddNodeObjectL
// ------------------------------------------------------------------------------------------------
void CSupAdapter::_AddNodeObjectL( const TDesC8& /*aURI*/, const TDesC8& /*aParentLUID*/, TInt aStatusRef )
	{
	RDEBUG("CSupAdapter::_AddNodeObjectL()");

	CSmlDmAdapter::TError ret = CSmlDmAdapter::EError;
	iCallBack->SetStatusL(aStatusRef, ret);
	}

#ifndef __WINS_DEBUG_TESTING__
// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] = 
	{
	{ {KNSmlDMSUPAdapterImplUid}, (TProxyNewLPtr)CSupAdapter::NewL }
	};

// ------------------------------------------------------------------------------------------------
// ImplementationGroupProxy
// ------------------------------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}
#endif

// -------------------------------------------------------------------------------------
// CSupAdapter::FillNodeInfoL()
// Fills the node info in ddf structure
// -------------------------------------------------------------------------------------
void CSupAdapter::FillNodeInfoL( MSmlDmDDFObject& aNode,TSmlDmAccessTypes aAccTypes,
										MSmlDmDDFObject::TOccurence aOccurrence, MSmlDmDDFObject::TScope aScope, 
										MSmlDmDDFObject::TDFFormat aFormat,const TDesC8& aDescription,const TDesC8& aMimeType)
	{
	RDEBUG("CSupAdapter::FillNodeInfoL()");

	aNode.SetAccessTypesL(aAccTypes);
	aNode.SetOccurenceL(aOccurrence);
	aNode.SetScopeL(aScope);
	aNode.SetDFFormatL(aFormat);
	aNode.SetDescriptionL(aDescription);
	if(aMimeType.Length() > 0)
	    {    
        aNode.AddDFTypeMimeTypeL(aMimeType);
	    }
	}

// -------------------------------------------------------------------------------------
// CSupAdapter::IsSupportedImageType()
// Checks whether the given mime type is supported image format
// -------------------------------------------------------------------------------------
CSupAdapter::TStartupAdapterLeafType CSupAdapter::GetObjectType(const TDesC8& aMime)
    {
	RDEBUG("CSupAdapter::GetObjectType()");
/*
	if(IsSupportedTextType(aMime)) 
	{
		return CSupAdapter::EText;	
	}
*/
	if(IsSupportedImageType(aMime)) 
	{
		return CSupAdapter::EImage;
	}
	
	return CSupAdapter::EUnknown;
    }

// -------------------------------------------------------------------------------------
// CSupAdapter::IsSupportedImageType()
// Checks whether the given mime type is supported image format
// -------------------------------------------------------------------------------------
TBool CSupAdapter::IsSupportedImageType(const TDesC8& aMime)
    {
	RDEBUG("CSupAdapter::IsSupportedImageType()");

	TBuf8<KStartupTBufMaxLength> mimeType;
	CopyAndTrimMimeType(mimeType, aMime);

#ifdef SUPADAPTER_CHECK_IMAGE_MIME_TYPE

	if(
		mimeType == KMimeTypeImageJpg || 
	    mimeType == KMimeTypeImageGif ||
	    mimeType == KMimeTypeImagePng ||
	    mimeType == KMimeTypeImageAny
	  )
	{
		return ETrue;
	}

	return EFalse;	

#else

	TDesC8 KImageDesC8 = (TDesC8&)KImage;
	return mimeType.Left(KImageDesC8.Length()) == KImage;
	
#endif
    }

// -------------------------------------------------------------------------------------
// CSupAdapter::IsSupportedTextType()
// Checks whether the given mime type is supported text format
// -------------------------------------------------------------------------------------
TBool CSupAdapter::IsSupportedTextType(const TDesC8& aMime)
    {
	RDEBUG("CSupAdapter::IsSupportedTextType()");

	TBuf8<KStartupTBufMaxLength> mimeType;
	CopyAndTrimMimeType(mimeType, aMime);

#ifdef SUPADAPTER_CHECK_TEXT_MIME_TYPE

	if(
		mimeType == KMimeTypeTextPlain || 
	    mimeType == KMimeTypeTextAny
	  )
	{
		return ETrue;
	}
	
	return EFalse;
	
#else

    TDesC8 KTextDesC8  = (TDesC8&)KText;	
	return mimeType.Left(KTextDesC8.Length()) == KText;

#endif
    }

// -------------------------------------------------------------------------------------
// CSupAdapter::SetStartupTextL()
// Sets and activates startup text
// -------------------------------------------------------------------------------------
void CSupAdapter::SetStartupTextL(CSmlDmAdapter::TError& aStatus, const TDesC16& aText)
    {
	RDEBUG("CSupAdapter::SetStartupTextL()");

    RUISettingsSrv uiSrv;
    User::LeaveIfError( uiSrv.Connect() );
    CleanupClosePushL( uiSrv );

    if( (KErrNone == uiSrv.SetStartupText( aText )) &&
        (KErrNone == uiSrv.SetStartupNoteType( ETextWelcomeNote )))
        {
        // status remains the same
        }
    else
        {
        aStatus = CSmlDmAdapter::EError;
        }

    CleanupStack::PopAndDestroy( &uiSrv );
    }

// -------------------------------------------------------------------------------------
// CSupAdapter::SetStartupImageL()
// Sets and activates startup image
// -------------------------------------------------------------------------------------
void CSupAdapter::SetStartupImageL(CSmlDmAdapter::TError& aStatus, const TDesC8& aObject, const TDesC8& /*aType*/)
    {
	RDEBUG("CSupAdapter::SetStartupImageL()");

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

    User::LeaveIfError( file.Temp( fileSession, KTempFilePath, iTemporaryImageFile, EFileWrite ) );
    CleanupClosePushL( file );

	//
	// Decoce from base 64 and write to file
	//
	TImCodecB64 B64Coder;	
	HBufC8 *target = HBufC8::NewLC( ( aObject.Length() * 3 ) / 4 + 16 );
 	TPtr8 targetPtr( target->Des() );
 	TBool decodeResult = TFileCoderB64::CheckB64Encode( aObject );
	if( decodeResult )
		{
		// use decoded data 
		B64Coder.Decode( aObject, targetPtr );
		User::LeaveIfError( file.Write( targetPtr ) );
		}
	else
		{
		// use original data
		User::LeaveIfError( file.Write ( aObject ) );
		}

	file.Flush();
	CleanupStack::PopAndDestroy(target);

	// Close file
    CleanupStack::PopAndDestroy( &file );

    // Close fileSession
    CleanupStack::PopAndDestroy( &fileSession );

    //
    // Set startup image
    //
    RUISettingsSrv uiSrv;
    User::LeaveIfError( uiSrv.Connect() );
    CleanupClosePushL( uiSrv );
    
    if( (KErrNone == uiSrv.SetStartupImage( iTemporaryImageFile )) &&
        (KErrNone == uiSrv.SetStartupNoteType( EImageWelcomeNote )))
        {
        // status remains the same
        }
    else
        {
        aStatus = CSmlDmAdapter::EError;
        }

    CleanupStack::PopAndDestroy( &uiSrv );
    }

// -------------------------------------------------------------------------------------
// CSupAdapter::GetStartupTextL()
// Currently returns only plain text
// -------------------------------------------------------------------------------------
CBufFlat* CSupAdapter::GetStartupTextL(CSmlDmAdapter::TError& /*aStatus*/, const TDesC8& /*aType*/)
    {
	RDEBUG("CSupAdapter::GetStartupTextL()");

    CBufFlat* object = CBufFlat::NewL(128);

    RUISettingsSrv uiSrv;
    User::LeaveIfError( uiSrv.Connect() );
    CleanupClosePushL( uiSrv );

    HBufC* text16 = uiSrv.GetStartupTextL();
    CleanupStack::PushL( text16 );

    HBufC8* text8 = CTARMCharConv::ConvertToUtf8LC( *text16 );

    object->InsertL( 0, *text8 );

    CleanupStack::PopAndDestroy( text8 );
    CleanupStack::PopAndDestroy( text16 );
    CleanupStack::PopAndDestroy( &uiSrv );

    return object;
    }

// -------------------------------------------------------------------------------------
// CSupAdapter::GetStartupImageL()
// Currently returns only plain text
// -------------------------------------------------------------------------------------
CBufFlat* CSupAdapter::GetStartupImageL(CSmlDmAdapter::TError& aStatus, const TDesC8& /*aType*/, TInt &aImageSize)
    {
	RDEBUG("CSupAdapter::GetStartupImageL()");

    TBuf<KMaxFileName>  imageLocation;
    TBuf8<KMaxFileName> imageLocation8;
    CBufFlat* object = 0;

    // First, get image location in the file system
    //
    RUISettingsSrv uiSrv;
    User::LeaveIfError( uiSrv.Connect() );
    CleanupClosePushL( uiSrv );
    
    User::LeaveIfError( uiSrv.GetStartupImagePath( imageLocation ) );
    
    CleanupStack::PopAndDestroy( &uiSrv );    

    //
    // Then, create base64 encoded flat buffer of the image
    //
    imageLocation8.Copy(imageLocation);

    if(imageLocation8.Length() > 0)
        {
        //
        // Load image into memory
        // 
        RFs   fileSession;
        RFile file;
        
        User::LeaveIfError( fileSession.Connect() );
        CleanupClosePushL( fileSession );
        
		TInt err = file.Open ( fileSession, imageLocation, EFileRead );
		
		if ( err == KErrNone )
			{
			CleanupClosePushL( file );
		    TInt size( 0 );
		    
			err = file.Size( size );
			if ( err == KErrNone ) 
				{
				HBufC8 *data = HBufC8::NewLC( size );
				TPtr8 ptr( data->Des() );
				err = file.Read( ptr );
				
				TImCodecB64 B64Coder;
				HBufC8 *target = HBufC8::NewLC( ( size * 4 ) / 3 + 16 );
				TPtr8 targetPtr( target->Des() );
				
				B64Coder.Encode( *data, targetPtr );
				
                object = CBufFlat::NewL(targetPtr.Length());
				object->InsertL( 0, targetPtr );
				
				CleanupStack::PopAndDestroy(target);
				CleanupStack::PopAndDestroy(data);
				}

			file.Size(aImageSize);

            // Close file
            CleanupStack::PopAndDestroy( &file );        
			}
		else if ( err == KErrNotFound || err == KErrPathNotFound )
			{
			RDEBUG("CSupAdapter::GetStartupImageL(): No image file");

			// Remains ok : aStatus = CSmlDmAdapter::EOk;
			}
        else
            {
            aStatus = CSmlDmAdapter::EError;
            }

        // Close fileSession
        CleanupStack::PopAndDestroy( &fileSession );

        }
    else
        {
        
        // Remains ok: aStatus = CSmlDmAdapter::EOk;
        object = CBufFlat::NewL(0);
        }

    return object;
    }

// -------------------------------------------------------------------------------------
// CSupAdapter::CopyAndTrimMimeType()
// -------------------------------------------------------------------------------------
void CSupAdapter::CopyAndTrimMimeType(TDes8& aNewMime, const TDesC8& aType)
    {
	RDEBUG("CSupAdapter::CopyAndTrimMimeType()");

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
	RDEBUG8_2("CSupAdapter::CopyAndTrimMimeType() end (%S)", &aNewMime);
    }

// -------------------------------------------------------------------------------------
// CSupAdapter::GetFetchTypeL()
// -------------------------------------------------------------------------------------
CSupAdapter::TStartupAdapterLeafType CSupAdapter::GetFetchTypeL(CSmlDmAdapter::TError& aStatus, const TDesC8& aType)
    {
	RDEBUG("CSupAdapter::GetFetchTypeL()");

	TBuf8<KStartupTBufMaxLength> mimeType;
	CopyAndTrimMimeType(mimeType, aType);

	TStartupAdapterLeafType type = CSupAdapter::EUnknown;

	TBool validMimeType = ETrue;

	if(mimeType == KMimeTypeImageMbm)
	    {
	    type = CSupAdapter::EImage;
	    }
	    else
	if(mimeType == KMimeTypeTextPlain)
	    {
	    type = CSupAdapter::EText;
	    }   			
	    else
	if(mimeType.Length() > 0)
	    {
	    validMimeType = EFalse;
	    }
	else
	    {
	    validMimeType = ETrue;
	    }

    if(validMimeType)
        {
            if(type == CSupAdapter::EUnknown)
                {
                    type = GetSelectedTypeL(aStatus);

                    if(aStatus != CSupAdapter::EOk)
                        {
                            aStatus = CSmlDmAdapter::EError;
                        }
                }
        }
    else
        {
            aStatus = CSmlDmAdapter::EError;
        }

    return type;
    }

// -------------------------------------------------------------------------------------
// CTcAdapter::GetNodeIdentifier
// -------------------------------------------------------------------------------------
CSupAdapter::TSupNodeIdentifier CSupAdapter::GetNodeIdentifier(const TDesC8& aURI)
    {
	RDEBUG("CSupAdapter::GetNodeIdentifier()");

	// NOTE! This code expects that the aURI starts without "./" so with Symbian Convergency
	// make sure that the code that calls this function parses the "./" from the beginning of
	// the string!

    TInt numOfSegs = NSmlDmURI::NumOfURISegs( aURI );
    if( numOfSegs == 0 )
        {
        return CSupAdapter::ESupNodeNotUsedAndAlwaysLast;
        }

    TPtrC8I seg1 = NSmlDmURI::URISeg( aURI, 0 );

    if(seg1 == KNSmlCustomizationNodeName)
        {
        if(numOfSegs == 1)
            {
            return CSupAdapter::ESupNodeCustomization;
            }
        else
            {
            // At least 2 segments
            TPtrC8I seg2 = NSmlDmURI::URISeg( aURI, 1 );

            if(seg2 == KNSmlSUPStartupNodeName)
                {
                if(numOfSegs == 2)
                    {
                    return CSupAdapter::ESupNodeStartup;
                    }
                else
                    {
                    // At least 3 segments
                    TPtrC8I seg3 = NSmlDmURI::URISeg( aURI, 2 );

                    if(numOfSegs == 3)
                        {
                        if(seg3 == KNSmlSUPWelcomeDataNodeName)
                            {
                            return CSupAdapter::ESupNodeWelcomeData;
                            }
                        else if(seg3 == KNSmlSUPWelcomeTextNodeName)
                            {
                            return CSupAdapter::ESupNodeWelcomeText;
                            }
                        else
                            {
                            return CSupAdapter::ESupNodeNotUsedAndAlwaysLast;
                            }
                        }
                    else
                        {
                        return CSupAdapter::ESupNodeNotUsedAndAlwaysLast;
                        }
                    }
                }
            else
                {
                return CSupAdapter::ESupNodeNotUsedAndAlwaysLast;
                }
            }
        }
    else
        {
        return CSupAdapter::ESupNodeNotUsedAndAlwaysLast;
        }

    // Note: do not return anything here
    }

// -------------------------------------------------------------------------------------
// CSupAdapter::GetSelectedTypeL()
// -------------------------------------------------------------------------------------
CSupAdapter::TStartupAdapterLeafType CSupAdapter::GetSelectedTypeL(CSmlDmAdapter::TError& /*aStatus*/)
    {
	RDEBUG("CSupAdapter::GetSelectedTypeL()");

    TStartupAdapterLeafType retVal = CSupAdapter::EUnknown;
    TInt type = ETextWelcomeNote;

    RUISettingsSrv uiSrv;
    User::LeaveIfError( uiSrv.Connect() );
    CleanupClosePushL( uiSrv );

    User::LeaveIfError( uiSrv.GetStartupNoteType( type ) );

    CleanupStack::PopAndDestroy( &uiSrv );

    switch(type)
    {
        case ETextWelcomeNote:
            retVal = CSupAdapter::EText;
        break;

        case EImageWelcomeNote:
            retVal = CSupAdapter::EImage;
        break;
    }

    return retVal;
    }

// -------------------------------------------------------------------------------------
// CSupAdapter::CheckPolicyL
// -------------------------------------------------------------------------------------   
//#ifdef __SAP_POLICY_MANAGEMENT

TInt CSupAdapter::CheckPolicyL()
    {
    
    if(!FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement ))
    {
    	User::Leave(KErrNotSupported);
    }
    
    	
	TInt result( KErrNone );

#ifdef __WINS__

	return result;

#else
	RDEBUG("CSupAdapter::CheckPolicyL() ask if enforcement is set");
	CSettingEnforcementInfo* info = CSettingEnforcementInfo::NewL();
	CleanupStack::PushL(info);
    TBool enfed(EFalse);
	User::LeaveIfError(info->EnforcementActive(ECustomization, enfed));
	CleanupStack::PopAndDestroy(info);

	RDEBUG_2("CSupAdapter::CheckPolicyL() enforcement: (%d)", enfed);

	if( enfed )
    {
    RDMCert dmcert;
    TCertInfo ci;
    dmcert.Get( ci ); 

    // Policy Engine Request
    TRequestContext context;
    TResponse response;
    context.AddSubjectAttributeL(
        PolicyEngineXACML::KTrustedSubject,
        ci
        );
    context.AddResourceAttributeL(
        PolicyEngineXACML::KResourceId,
        PolicyEngineXACML::KCustomizationManagement,
        PolicyEngineXACML::KStringDataType
        );
    
    User::LeaveIfError( iPR.MakeRequest( context, response ) );
    TResponseValue resp = response.GetResponseValue();
    switch( resp )
        {
	    case EResponsePermit:
	        break;
      case EResponseDeny:
	    case EResponseIndeterminate:
	    case EResponseNotApplicable:
	    default:
    	    result = KErrAccessDenied;
	    }
    }

#endif
	RDEBUG_2("CSupAdapter::CheckPolicyL() end: response (%d)", result);
    return result;
    
    }
//#endif

// -----------------------------------------------------------------------------
// CSupAdapter::PolicyRequestResourceL
// -----------------------------------------------------------------------------
//
//#ifdef __SAP_POLICY_MANAGEMENT
TPtrC8 CSupAdapter::PolicyRequestResourceL( const TDesC8& /*aURI*/ )
	{
	
	if(!FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement ))
	{
		User::Leave(KErrNotSupported);
		
	}
	 RDEBUG("CSupAdapter::PolicyRequestResourceL()");

		return PolicyEngineXACML::KCustomizationManagement();	
	}
//#endif

#ifdef __WINS__

// -------------------------------------------------------------------------------------
// CSupAdapter::DebugTestsL()
// -------------------------------------------------------------------------------------
void CSupAdapter::DebugTestsL()
    {
	RDEBUG("CSupAdapter::DebugTestsL()");

    TBuf8<1024> buffer;

    ASSERT(NSmlDmURI::URISeg(_L8("1/2/3"), 0) == _L8("1"));
    ASSERT(NSmlDmURI::URISeg(_L8("1/2/3"), 1) == _L8("2"));
    ASSERT(NSmlDmURI::URISeg(_L8("1/2/3"), 2) == _L8("3"));
    ASSERT(NSmlDmURI::URISeg(_L8("1/2/3"), 3) == _L8(""));
    ASSERT(NSmlDmURI::URISeg(_L8("1/2/3"), 4) == _L8(""));
    ASSERT(NSmlDmURI::URISeg(_L8("1/2/3"), 5) == _L8(""));
    ASSERT(NSmlDmURI::URISeg(_L8("1/2/3"), -1) == _L8(""));

    ASSERT(NSmlDmURI::URISeg(_L8("yksi /kaksi / kolme"), 0) == _L8("yksi "));
    ASSERT(NSmlDmURI::URISeg(_L8("yksi /kaksi / kolme"), 1) == _L8("kaksi "));
    ASSERT(NSmlDmURI::URISeg(_L8("yksi /kaksi / kolme"), 2) == _L8(" kolme"));
    ASSERT(NSmlDmURI::URISeg(_L8("yksi /kaksi / kolme"), 3) == _L8(""));
    ASSERT(NSmlDmURI::URISeg(_L8("yksi /kaksi / kolme"), 4) == _L8(""));
    ASSERT(NSmlDmURI::URISeg(_L8("yksi /kaksi / kolme"), 5) == _L8(""));
    ASSERT(NSmlDmURI::URISeg(_L8("yksi /kaksi / kolme"), -1) == _L8(""));
    
    //
    // Text conversions
    //
    _LIT16(KText, "abcdefghijklmnopqrstuvxyz1234567890_:;-.,'*!#¤%&/()=?`+´^~¨<>|§½@£${[]}\\\"");

    HBufC8  *utf8Text = CTARMCharConv::ConvertToUtf8LC( KText() );
    HBufC16 *unicodeText = CTARMCharConv::ConvertFromUtf8LC( *utf8Text );
    ASSERT( unicodeText->Des().Compare( KText() ) == 0 );
    CleanupStack::PopAndDestroy( unicodeText );
    CleanupStack::PopAndDestroy( utf8Text );
  
    //
    // Valid mime types
    //
    ASSERT(IsSupportedTextType(_L8("text/")));
    ASSERT(IsSupportedTextType(_L8("text/plain")));
    ASSERT(IsSupportedTextType(_L8("TeXt/PlaiN")));
    ASSERT(IsSupportedTextType(_L8("TEXT/jepulis")));
    ASSERT(IsSupportedTextType(_L8("text/köök")));
    ASSERT(IsSupportedTextType(_L8("TEXT/,.,.,.")));
    ASSERT(IsSupportedTextType(_L8(" text/plain ")));    
    
    ASSERT(IsSupportedImageType(_L8("image/*")));
    ASSERT(IsSupportedImageType(_L8("ImaGe/PnG")));
    ASSERT(IsSupportedImageType(_L8("IMAGE/jepulis")));
    ASSERT(IsSupportedImageType(_L8("image/köök")));
    ASSERT(IsSupportedImageType(_L8("IMAGE/,.,.,.")));
    ASSERT(IsSupportedImageType(_L8(" IMage/jpeg ")));

    ASSERT(IsSupportedTextType(_L8("text /plain")));
    ASSERT(IsSupportedTextType(_L8("text / plain")));
    ASSERT(IsSupportedTextType(_L8("t e x t/jepulis")));

    //
    // Invalid mime types
    //    
    ASSERT(!IsSupportedTextType(_L8("text")));   
    ASSERT(!IsSupportedTextType(_L8("a text/plain")));
    ASSERT(!IsSupportedTextType(_L8("_text/plain")));
    ASSERT(!IsSupportedTextType(_L8("\"text/plain\"")));
    
    //
    // Get/Set startup text
    //    
    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;
    CBufBase *object, *object2;

    // Get startup text
    object = GetStartupTextL(status, _L8("text/"));   
    ASSERT(object != 0);
    ASSERT(status == CSmlDmAdapter::EOk);
    delete object;

    // Set startup text
    _LIT16(KStartupText1, "Startup text: Päivää!");
    _LIT16(KStartupText2, "Startup text: Päivää!");

    SetStartupTextL(status, KStartupText1());
    ASSERT(status == CSmlDmAdapter::EOk);

    // Get startup text
    object = GetStartupTextL(status, buffer);
    ASSERT(object != 0);
    ASSERT(status == CSmlDmAdapter::EOk);
    CleanupStack::PushL( object );

    HBufC16* source = CTARMCharConv::ConvertFromUtf8LC( object->Ptr( 0 ) );
    TInt sourceLength = source->Length();
    TInt destLength = KStartupTBufMaxLength<sourceLength?KStartupTBufMaxLength:sourceLength;
    TPtrC16 sourcePtr( source->Ptr(), destLength );
    ASSERT( sourcePtr == KStartupText2 );
	CleanupStack::PopAndDestroy( source );

    CleanupStack::PopAndDestroy( object );

    //
    // Set/Get startup image
    //    
    TUint8 jpeg[773] =
        {
        0xff,0xd8,0xff,0xe0,0x00,0x10,0x4a,0x46,0x49,0x46,0x00,0x01,0x01,0x01,0x01,0x2c,
        0x01,0x2c,0x00,0x00,0xff,0xdb,0x00,0x43,0x00,0x08,0x06,0x06,0x07,0x06,0x05,0x08,
        0x07,0x07,0x07,0x09,0x09,0x08,0x0a,0x0c,0x14,0x0d,0x0c,0x0b,0x0b,0x0c,0x19,0x12,
        0x13,0x0f,0x14,0x1d,0x1a,0x1f,0x1e,0x1d,0x1a,0x1c,0x1c,0x20,0x24,0x2e,0x27,0x20,
        0x22,0x2c,0x23,0x1c,0x1c,0x28,0x37,0x29,0x2c,0x30,0x31,0x34,0x34,0x34,0x1f,0x27,
        0x39,0x3d,0x38,0x32,0x3c,0x2e,0x33,0x34,0x32,0xff,0xdb,0x00,0x43,0x01,0x09,0x09,
        0x09,0x0c,0x0b,0x0c,0x18,0x0d,0x0d,0x18,0x32,0x21,0x1c,0x21,0x32,0x32,0x32,0x32,
        0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,
        0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,
        0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0xff,0xc0,
        0x00,0x11,0x08,0x00,0x14,0x00,0x14,0x03,0x01,0x22,0x00,0x02,0x11,0x01,0x03,0x11,
        0x01,0xff,0xc4,0x00,0x1f,0x00,0x00,0x01,0x05,0x01,0x01,0x01,0x01,0x01,0x01,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
        0x0a,0x0b,0xff,0xc4,0x00,0xb5,0x10,0x00,0x02,0x01,0x03,0x03,0x02,0x04,0x03,0x05,
        0x05,0x04,0x04,0x00,0x00,0x01,0x7d,0x01,0x02,0x03,0x00,0x04,0x11,0x05,0x12,0x21,
        0x31,0x41,0x06,0x13,0x51,0x61,0x07,0x22,0x71,0x14,0x32,0x81,0x91,0xa1,0x08,0x23,
        0x42,0xb1,0xc1,0x15,0x52,0xd1,0xf0,0x24,0x33,0x62,0x72,0x82,0x09,0x0a,0x16,0x17,
        0x18,0x19,0x1a,0x25,0x26,0x27,0x28,0x29,0x2a,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,
        0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,
        0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,
        0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,
        0x9a,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,
        0xb8,0xb9,0xba,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xd2,0xd3,0xd4,0xd5,
        0xd6,0xd7,0xd8,0xd9,0xda,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xf1,
        0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xff,0xc4,0x00,0x1f,0x01,0x00,0x03,
        0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
        0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0xff,0xc4,0x00,0xb5,0x11,0x00,
        0x02,0x01,0x02,0x04,0x04,0x03,0x04,0x07,0x05,0x04,0x04,0x00,0x01,0x02,0x77,0x00,
        0x01,0x02,0x03,0x11,0x04,0x05,0x21,0x31,0x06,0x12,0x41,0x51,0x07,0x61,0x71,0x13,
        0x22,0x32,0x81,0x08,0x14,0x42,0x91,0xa1,0xb1,0xc1,0x09,0x23,0x33,0x52,0xf0,0x15,
        0x62,0x72,0xd1,0x0a,0x16,0x24,0x34,0xe1,0x25,0xf1,0x17,0x18,0x19,0x1a,0x26,0x27,
        0x28,0x29,0x2a,0x35,0x36,0x37,0x38,0x39,0x3a,0x43,0x44,0x45,0x46,0x47,0x48,0x49,
        0x4a,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x63,0x64,0x65,0x66,0x67,0x68,0x69,
        0x6a,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x82,0x83,0x84,0x85,0x86,0x87,0x88,
        0x89,0x8a,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0xa2,0xa3,0xa4,0xa5,0xa6,
        0xa7,0xa8,0xa9,0xaa,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xc2,0xc3,0xc4,
        0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xe2,
        0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,
        0xfa,0xff,0xda,0x00,0x0c,0x03,0x01,0x00,0x02,0x11,0x03,0x11,0x00,0x3f,0x00,0xf7,
        0x7b,0xfb,0xeb,0x7d,0x33,0x4e,0xb9,0xbf,0xbc,0x93,0xcb,0xb5,0xb5,0x89,0xe6,0x99,
        0xf6,0x93,0xb5,0x14,0x12,0xc7,0x03,0x93,0x80,0x0f,0x4a,0x8f,0x4b,0xd4,0xa1,0xd5,
        0xac,0x16,0xee,0x15,0x91,0x01,0x77,0x8d,0xe3,0x90,0x00,0xd1,0xc8,0x8e,0x51,0xd0,
        0xe0,0x91,0x95,0x65,0x65,0xc8,0x24,0x1c,0x64,0x12,0x30,0x6a,0x9f,0x89,0x34,0x9b,
        0x8d,0x73,0x4e,0x8f,0x4d,0x8a,0xe7,0xec,0xf6,0xb3,0x4a,0x05,0xe3,0xae,0x3c,0xcf,
        0x28,0x02,0x76,0xa0,0x65,0x65,0x6d,0xce,0x11,0x59,0x5c,0x15,0x28,0x5c,0x11,0xcd,
        0x47,0xa0,0x68,0xb7,0x7a,0x1d,0xc5,0xfc,0x4f,0x7f,0x25,0xe5,0x9d,0xc3,0x8b,0x84,
        0x79,0xc2,0x09,0x56,0x66,0x2d,0xe6,0x8d,0xb1,0xa2,0x22,0xa1,0xc2,0x30,0xc0,0xc9,
        0x76,0x91,0x8e,0x49,0xa0,0x0d,0xca,0x28,0xa2,0x80,0x0a,0x28,0xa2,0x80,0x0a,0x28,
        0xa2,0x80,0x3f,0xff,0xd9            
        };
     TPtrC8 jpeg_Ptr8(jpeg, 773);

    // Encode JPEG        
	TImCodecB64 B64Coder;
	HBufC8 *target = HBufC8::NewLC( ( 773 * 4 ) / 3 + 16 );
	TPtr8 targetPtr( target->Des() );	
	B64Coder.Encode( jpeg_Ptr8, targetPtr );

    // Set image X
	SetStartupImageL(status,targetPtr,KMimeTypeImageMbm);
    ASSERT(status == CSmlDmAdapter::EOk);
        
    // Get image Y1 (returns encoded MBM)
    TInt imageSize;
    object = GetStartupImageL(status,KMimeTypeImageMbm,imageSize);
    ASSERT(status == CSmlDmAdapter::EOk);
    CleanupStack::PushL( object );
       
    // Set image Y1
	SetStartupImageL(status,object->Ptr(0),KMimeTypeImageMbm);
    ASSERT(status == CSmlDmAdapter::EOk);
    
    // Get image Y2
    object2 = GetStartupImageL(status,KMimeTypeImageMbm,imageSize);
    ASSERT(status == CSmlDmAdapter::EOk);

    // Assert Y1 == Y2
    ASSERT(object->Ptr(0) == object2->Ptr(0));
    
    CleanupStack::PopAndDestroy( object );
    delete object2;
    CleanupStack::PopAndDestroy(target);

    //
    // Other methods, GetFetchTypeL
    //     
    status = CSmlDmAdapter::EOk;

    // OK cases
    GetFetchTypeL(status, _L8(""));
    ASSERT(status == CSmlDmAdapter::EOk);
    GetFetchTypeL(status, _L8("text/plain"));
    ASSERT(status == CSmlDmAdapter::EOk);
    GetFetchTypeL(status, _L8("image/x-epoc-mbm"));
    ASSERT(status == CSmlDmAdapter::EOk);

    // Fail cases
    GetFetchTypeL(status, _L8(" t e x t / "));
    ASSERT(status != CSmlDmAdapter::EOk); status = CSmlDmAdapter::EOk;
    GetFetchTypeL(status, _L8("ima ge /"));
    ASSERT(status != CSmlDmAdapter::EOk); status = CSmlDmAdapter::EOk;
    GetFetchTypeL(status, _L8("text/"));
    ASSERT(status != CSmlDmAdapter::EOk); status = CSmlDmAdapter::EOk;
    GetFetchTypeL(status, _L8("text/"));
    ASSERT(status != CSmlDmAdapter::EOk); status = CSmlDmAdapter::EOk;
    GetFetchTypeL(status, _L8("video/"));
    ASSERT(status != CSmlDmAdapter::EOk); status = CSmlDmAdapter::EOk;
    GetFetchTypeL(status, _L8("*"));
    ASSERT(status != CSmlDmAdapter::EOk); status = CSmlDmAdapter::EOk;
    GetFetchTypeL(status, _L8("text/"));
    ASSERT(status != CSmlDmAdapter::EOk); status = CSmlDmAdapter::EOk;
    GetFetchTypeL(status, _L8("image/"));
    ASSERT(status != CSmlDmAdapter::EOk); status = CSmlDmAdapter::EOk;
    }
#endif


