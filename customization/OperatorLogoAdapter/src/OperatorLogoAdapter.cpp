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
* Description:  DM Operator Logo Adapter
*
*/


/*

./Customization -+--> OperatorLogo -+--> LogoData

*/

#include "OperatorLogoAdapter.h"

#include <implementationproxy.h> // For TImplementationProxy definition
#include <coemain.h>
#include <imageconversion.h>
#include <bitmaptransforms.h>
#include <RPhCltServer.h>

#include <CPhCltImageHandler.h>
#include <CPhCltBaseImageParams.h>

//#ifdef __SAP_POLICY_MANAGEMENT
#include <PolicyEngineXACML.h>
//#endif

#include "nsmldmuri.h"
#include "FileCoderB64.h"
#include "debug.h"
#include <featmgr.h>

const TInt KLogoMaxWidthPixels      = 97;
const TInt KLogoMaxHeightPixels     = 25;
_LIT( KOperatorLogoAdapterTmpFilePath, "c:\\system\\temp\\DM_OPLogo.bin" );

class CWaitScheduler : public CActive
	{
	// Functions
	public:
		static CWaitScheduler* NewL( TInt aPriority = EPriorityStandard );

		TInt WaitForRequest();

	private:
		CWaitScheduler( TInt aPriority );
		void ConstructL();
		virtual ~CWaitScheduler();

		void RunL();
		void DoCancel();

	// Data
	private:
		CActiveSchedulerWait iScheduler;

	};
// ------------------------------------------------------------------------------------------------

CWaitScheduler* CWaitScheduler::NewL( TInt aPriority )
	{
	CWaitScheduler* self = new( ELeave ) CWaitScheduler( aPriority );
	CleanupStack::PushL( self );
	self->ConstructL( );
	CleanupStack::Pop( self );
	return self;
	}

CWaitScheduler::CWaitScheduler( TInt aPriority )
	: CActive( aPriority )
	{
	CActiveScheduler::Add( this );
	}

void CWaitScheduler::ConstructL()
	{
	}

CWaitScheduler::~CWaitScheduler()
	{
	if ( IsActive() )
		{
		User::Panic( _L("CSyncWait"), 1 );
		}
	}

TInt CWaitScheduler::WaitForRequest()
	{
	SetActive();
	iScheduler.Start();
	return iStatus.Int();
	}

void CWaitScheduler::RunL()
	{
	iScheduler.AsyncStop();
	}

void CWaitScheduler::DoCancel()
	{
	User::Panic( _L("CSyncWait"), 2 );
	}


// ------------------------------------------------------------------------------------------------
// COperatorLogoAdapter* COperatorLogoAdapter::NewL( )
// ------------------------------------------------------------------------------------------------
COperatorLogoAdapter* COperatorLogoAdapter::NewL( MSmlDmCallback* aDmCallback )
	{
	RDEBUG("COperatorLogoAdapter::NewL(): begin");

	COperatorLogoAdapter* self = NewLC( aDmCallback );
	CleanupStack::Pop();
	return self;
	}

// ------------------------------------------------------------------------------------------------
// COperatorLogoAdapter* COperatorLogoAdapter::NewLC( )
// ------------------------------------------------------------------------------------------------
COperatorLogoAdapter* COperatorLogoAdapter::NewLC( MSmlDmCallback* aDmCallback )
	{
	COperatorLogoAdapter* self = new( ELeave ) COperatorLogoAdapter( aDmCallback );
	CleanupStack::PushL( self );
	self->ConstructL( );
	return self;
	}


// ------------------------------------------------------------------------------------------------
// COperatorLogoAdapter::COperatorLogoAdapter()
// ------------------------------------------------------------------------------------------------

COperatorLogoAdapter::COperatorLogoAdapter( MSmlDmCallback* aDmCallback )
	: CTARMDmAdapter( aDmCallback )
	, iCallBack( aDmCallback )
	{
	}

// -------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void COperatorLogoAdapter::ConstructL( )
	{
	
		TRAPD( errf, FeatureManager::InitializeLibL() );
		if( errf != KErrNone )
		{
			User::Leave(errf);
		}
		
		
	    if(FeatureManager::FeatureSupported(KFeatureIdSapOperatorLogoAdapter))
		{
	    User::LeaveIfError( RFbsSession::Connect() );
		}
		else
		{
			FeatureManager::UnInitializeLib();
	   		User::Leave( KErrNotSupported );
		}
	}

// ------------------------------------------------------------------------------------------------
// COperatorLogoAdapter::~COperatorLogoAdapter()
// ------------------------------------------------------------------------------------------------


COperatorLogoAdapter::~COperatorLogoAdapter()
	{
	
	FeatureManager::UnInitializeLib();
	RFbsSession::Disconnect();
	}

// ------------------------------------------------------------------------------------------------
//  COperatorLogoAdapter::DDFVersionL()
// ------------------------------------------------------------------------------------------------
void COperatorLogoAdapter::DDFVersionL( CBufBase& aDDFVersion )
	{
	aDDFVersion.InsertL( 0, KOperatorLogoDDFVersion );
	}


// ------------------------------------------------------------------------------------------------
//  COperatorLogoAdapter::DDFStructureL()
//
// ------------------------------------------------------------------------------------------------
void COperatorLogoAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
	{
	RDEBUG( "COperatorLogoAdapter::DDFStructureL(): begin" );
	
	TSmlDmAccessTypes accessTypesGet;
	accessTypesGet.SetGet();
	
	TSmlDmAccessTypes accessTypesGetReplace;
	accessTypesGetReplace.SetGet();
	accessTypesGetReplace.SetReplace();


	/*
	Node: ./Customzation
	This interior node is the common parent to all customization..
	Status: Required
	Occurs: One
	Format: Node
	Access Types: Get
	Values: N/A
	*/
	MSmlDmDDFObject& customization = aDDF.AddChildObjectL( KOperatorLogoCustomization );
	FillNodeInfoL(customization, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KOperatorLogoCustomizationD, ETrue );
	
	/*
	Node: ./Customization/OperatorLogo
	This interior node acts as a placeholder for one or more update packages within a same content provider group.
	Status: Required
	Occurs: One
	Format: Node
	Access Types: Get
	Values: N/A
	*/
	MSmlDmDDFObject& operatorLogo = customization.AddChildObjectL( KOperatorLogoOperatorLogo );
	FillNodeInfoL( operatorLogo, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KOperatorLogoOperatorLogoD, ETrue );
	
	/*
	Node: ./Customization/OperatorLogo/Data
	Status: Optional
	Occurs: One
	Format: bin
	Access Types: Get, Replace
	Values: N/A
	*/

	MSmlDmDDFObject& logoData = operatorLogo.AddChildObjectL(KOperatorLogoLogoData);
	FillNodeInfoL( logoData, accessTypesGetReplace, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EBin, KOperatorLogoLogoDataD, EFalse);
	logoData.AddDFTypeMimeTypeL( KNSmlDMTextPlain );
	logoData.AddDFTypeMimeTypeL( KNSmlDMImageAny );
	}

// ------------------------------------------------------------------------------------------------
//  COperatorLogoAdapter::UpdateLeafObjectL()
//  
// ------------------------------------------------------------------------------------------------
void COperatorLogoAdapter::_UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& /*aLUID*/, const TDesC8& aObject, const TDesC8& aType, const TInt aStatusRef )
	{
	RDEBUG( "COperatorLogoAdapter::UpdateLeafObjectL(): begin" );

	TError ret = EOk ;
	
#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TNodeId nodeId = NodeId( uriPtrc );
#else
	TNodeId nodeId = NodeId( aURI );
#endif	
	
	switch( nodeId )
		{
	case ENodeLogoData:
		ret = SetOperatorLogoL( aObject, aType );
		break;

	case ENodeUnknown:
		ret = ENotFound;
		break;

	default:
		ret = EError;
		break;
		}

	iCallBack->SetStatusL( aStatusRef, ret );
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void COperatorLogoAdapter::_UpdateLeafObjectL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/, RWriteStream*& /*aStream*/, const TDesC8& /*aType*/, TInt aStatusRef )
	{
	RDEBUG( "COperatorLogoAdapter::UpdateLeafObjectL(...RWriteStream...): begin" );
	// Not supported
	TError ret = EError;
	iCallBack->SetStatusL( aStatusRef, ret );
	}

// ------------------------------------------------------------------------------------------------
// COperatorLogoAdapter::DeleteObjectL( const TDesC& aURI, const TDesC& aLUID )
// ------------------------------------------------------------------------------------------------
void COperatorLogoAdapter::_DeleteObjectL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/, const TInt aStatusRef )
	{
	RDEBUG( "COperatorLogoAdapter::DeleteObjectL(): begin" );
	// Not supported
	TError ret = EError;
	iCallBack->SetStatusL( aStatusRef, ret );
	}


// ------------------------------------------------------------------------------------------------
//  COperatorLogoAdapter::FetchLeafObjectL( const TDesC& aURI, const TDesC& aLUID, const TDesC& aType, CBufBase& aObject )
// ------------------------------------------------------------------------------------------------
void COperatorLogoAdapter::_FetchLeafObjectL( const TDesC8& aURI, const TDesC8& /*aLUID*/, const TDesC8& /*aType*/, const TInt /*aResultsRef*/, const TInt aStatusRef )
	{
	RDEBUG("COperatorLogoAdapter::FetchLeafObjectL(): begin");

	TError ret = EOk ;
	
#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TNodeId nodeId = NodeId( uriPtrc );
#else
	TNodeId nodeId = NodeId( aURI );
#endif	

	switch( nodeId )
		{
	case ENodeLogoData:
		// Get not supported for operator logo
		ret = EError;
		break;

	case ENodeUnknown:
		ret = ENotFound;
		break;

	default:
		ret = EError;
		break;
		}

	iCallBack->SetStatusL( aStatusRef, ret );

}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void COperatorLogoAdapter::_FetchLeafObjectSizeL( const TDesC8& aURI, const TDesC8& /*aLUID*/, const TDesC8& /*aType*/, TInt /*aResultsRef*/, TInt aStatusRef )
	{
	RDEBUG("COperatorLogoAdapter::FetchLeafObjectSizeL(): begin");

	TError ret = EOk ;
	
#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TNodeId nodeId = NodeId( uriPtrc );
#else
	TNodeId nodeId = NodeId( aURI );
#endif	

	switch( nodeId )
		{
	case ENodeLogoData:
		// Get not supported for operator logo
		ret = EError;
		break;

	case ENodeUnknown:
		ret = ENotFound;
		break;
	default:
		ret = EError;
		break;
		}


	iCallBack->SetStatusL( aStatusRef, ret );

	}

// ------------------------------------------------------------------------------------------------
//  COperatorLogoAdapter::ChildURIListL( const TDesC& aURI, const TDesC& aParentLUID, const CArrayFix<TNSmlDmMappingInfo>& aPreviousURISegmentList, CArrayFix<TPtrC>& aCurrentURISegmentList )
// ------------------------------------------------------------------------------------------------
void COperatorLogoAdapter::_ChildURIListL( const TDesC8& aURI, const TDesC8& /*aParentLUID*/, const CArrayFix<TSmlDmMappingInfo>& /*aPreviousURISegmentList*/, TInt aResultsRef, TInt aStatusRef  )
	{
	RDEBUG("COperatorLogoAdapter::ChildURIListL(): begin");

	TError ret = EOk ;
	
#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TNodeId nodeId = NodeId( uriPtrc );
#else
	TNodeId nodeId = NodeId( aURI );
#endif	

	CBufBase *currentList = CBufFlat::NewL( 128 );
	CleanupStack::PushL( currentList );

	switch( nodeId )
		{
	case ENodeCustomization:
		currentList->InsertL( currentList->Size(), KOperatorLogoOperatorLogo() );
		break;

	case ENodeOperatorLogo:
		currentList->InsertL( currentList->Size(), KOperatorLogoLogoData );
		break;

	case ENodeLogoData:
		// No children, leave list empty
		break;

	case ENodeUnknown:
		ret = ENotFound;
		break;

	default:
		ret = EError;
		break;
		}

	if( ret == EOk )
		{
		iCallBack->SetResultsL( aResultsRef, *currentList, KNullDesC8 );
		}

	iCallBack->SetStatusL( aStatusRef, ret );

	CleanupStack::PopAndDestroy(currentList); //currentList
	}


// ------------------------------------------------------------------------------------------------
// COperatorLogoAdapter::AddNodeObjectL( const TDesC& aURI, const TDesC& aParentLUID )
// ------------------------------------------------------------------------------------------------
void COperatorLogoAdapter::_AddNodeObjectL( const TDesC8& /*aURI*/, const TDesC8& /*aParentLUID*/, const TInt aStatusRef )
	{
	RDEBUG("COperatorLogoAdapter::AddNodeObjectL(): begin");
	// Not supported
	TError ret = EError;
	iCallBack->SetStatusL( aStatusRef, ret );
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void COperatorLogoAdapter::_ExecuteCommandL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/, const TDesC8& /*aArgument*/, const TDesC8& /*aType*/, TInt aStatusRef )
	{
	RDEBUG("COperatorLogoAdapter::ExecuteCommandL(): begin");
	// Not supported
	TError ret = EError;
	iCallBack->SetStatusL( aStatusRef, ret );
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void COperatorLogoAdapter::_ExecuteCommandL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/, RWriteStream*& /*aStream*/, const TDesC8& /*aType*/, TInt aStatusRef )
	{
	RDEBUG("COperatorLogoAdapter::ExecuteCommandL(...RWriteStream...): begin");
	//Not supported
	TError ret = EError;
	iCallBack->SetStatusL( aStatusRef, ret );
	}
	
// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void COperatorLogoAdapter::_CopyCommandL( const TDesC8& /*aTargetURI*/, const TDesC8& /*aTargetLUID*/, const TDesC8& /*aSourceURI*/, const TDesC8& /*aSourceLUID*/, const TDesC8& /*aType*/, TInt aStatusRef )
	{
	RDEBUG("COperatorLogoAdapter::CopyCommandL(): begin");
	//Not supported
	TError ret = EError;
	iCallBack->SetStatusL( aStatusRef, ret );
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void COperatorLogoAdapter::StartAtomicL()
	{
	RDEBUG("COperatorLogoAdapter::StartAtomicL(): begin");
	//Not supported
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void COperatorLogoAdapter::CommitAtomicL()
	{
	RDEBUG("COperatorLogoAdapter::CommitAtomicL(): begin");
	//Not supported
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void COperatorLogoAdapter::RollbackAtomicL()
	{
	RDEBUG("COperatorLogoAdapter::RollbackAtomicL(): begin");
	//Not supported
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
TBool COperatorLogoAdapter::StreamingSupport( TInt& /*aItemSize*/ )
	{
	RDEBUG("COperatorLogoAdapter::StreamingSupport(): begin");
	return EFalse;
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
#ifdef __TARM_SYMBIAN_CONVERGENCY	
void COperatorLogoAdapter::StreamCommittedL( RWriteStream& /*aStream*/ )
#else
void COperatorLogoAdapter::StreamCommittedL()
#endif	
	{	
	RDEBUG("COperatorLogoAdapter::StreamCommittedL(): begin");
	RDEBUG("COperatorLogoAdapter::StreamCommittedL(): end");
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void COperatorLogoAdapter::CompleteOutstandingCmdsL()
	{
	RDEBUG("COperatorLogoAdapter::CompleteOutstandingCmdsL(): begin");
	}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
//#ifdef __SAP_POLICY_MANAGEMENT
TPtrC8 COperatorLogoAdapter::PolicyRequestResourceL( const TDesC8& /*aURI*/ )
	{
	if(!FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement ))
	{
		User::Leave(KErrNotSupported);
		
	}
	
	return PolicyEngineXACML::KCustomizationManagement();	

	}
//#endif
// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
COperatorLogoAdapter::TNodeId COperatorLogoAdapter::NodeId( const TDesC8& aURI )
	{
	TNodeId id = ENodeUnknown;

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TInt cnt = NSmlDmURI::NumOfURISegs( uriPtrc );
	TPtrC8 app = NSmlDmURI::LastURISeg( uriPtrc );
#else
	TInt cnt = NSmlDmURI::NumOfURISegs( aURI );
	TPtrC8 app = NSmlDmURI::LastURISeg( aURI );
#endif	

	if ( cnt == 1 )
		{
		if (app == KOperatorLogoCustomization)
			{
			id = ENodeCustomization;
			}
		}
	else if ( cnt == 2 ) 
		{
		if (app == KOperatorLogoOperatorLogo)
			{
			id = ENodeOperatorLogo;
			}
		}
	else if ( cnt == 3 ) 
		{
		if (app == KOperatorLogoLogoData)
			{
			id = ENodeLogoData;
			}
		}

	return id;
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] = 
	{
	{ {KOperatorLogoAdapterImplUid}, (TProxyNewLPtr)COperatorLogoAdapter::NewL }
	};

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
	{
	aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
	return ImplementationTable;
	}



// -------------------------------------------------------------------------------------
// COperatorLogoAdapter::FillNodeInfoL()
// Fills the node info in ddf structure
// -------------------------------------------------------------------------------------
void COperatorLogoAdapter::FillNodeInfoL( MSmlDmDDFObject& aNode,TSmlDmAccessTypes aAccTypes,
										MSmlDmDDFObject::TOccurence aOccurrence, MSmlDmDDFObject::TScope aScope, 
										MSmlDmDDFObject::TDFFormat aFormat,const TDesC8& aDescription,TBool /*aObjectGroup*/)
	{
	aNode.SetAccessTypesL(aAccTypes);
	aNode.SetOccurenceL(aOccurrence);
	aNode.SetScopeL(aScope);
	aNode.SetDFFormatL(aFormat);
	if(aFormat!=MSmlDmDDFObject::ENode)
		{
		aNode.AddDFTypeMimeTypeL(KNSmlDMTextPlain);
		}
	aNode.SetDescriptionL(aDescription);
	}

// -------------------------------------------------------------------------------------
// COperatorLogoAdapter::IsText()
// Checks whether the given mime type is text format (that we support)
// -------------------------------------------------------------------------------------
TBool COperatorLogoAdapter::IsText(const TDesC8& aMime)
	{
	TBool ret = EFalse;
	if (aMime == _L8("text/plain") || aMime == _L8("text/*"))
		{
			ret = ETrue;
		}
	return ret;
	}

// -------------------------------------------------------------------------------------
// COperatorLogoAdapter::IsImage()
// Checks whether the given mime type is image format (that we support)
// -------------------------------------------------------------------------------------
TBool COperatorLogoAdapter::IsImage(const TDesC8& aMime)
	{
	TBool ret = EFalse;
	if (aMime == _L8("image/jpeg") || aMime == _L8("image/gif"))
		{
			ret = ETrue;
		}
	return ret;
	}





// ------------------------------------------------------------------------------------------------
//  COperatorLogoAdapter::SetOperatorLogoL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError COperatorLogoAdapter::SetOperatorLogoL(const TDesC8& aData, const TDesC8& aMime) 
	{
	TError ret = EOk;

	if (aData.Length() > 0)
		{
		ret = SetOperatorLogoImageL( aData, aMime );
		}
	else
		{
		ret = DeleteOperatorLogoImageL();
		}

	return ret;
	}

// ------------------------------------------------------------------------------------------------
//  COperatorLogoAdapter::SetOperatorLogoImageL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError COperatorLogoAdapter::SetOperatorLogoImageL(const TDesC8& aData, const TDesC8& /*aMime*/) 
	{
	RDEBUG("COperatorLogoAdapter::SetOperatorLogoImageL(): begin");
	TError ret = EOk;

	TInt mcc = 0;
	TInt mnc = 0;


	TFileCoderB64 decoder;
	TPtrC fileName( KOperatorLogoAdapterTmpFilePath );

	RFs fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL( fs );

	TBool isEncoded = TFileCoderB64::CheckB64Encode( aData );
	if( isEncoded )
		{
		decoder.DecodeToL( aData, fileName );
		}
	else
		{
		// aData is original data, save it to fileName
		RFile file;
		User::LeaveIfError( file.Replace( fs, fileName, EFileWrite));
		CleanupClosePushL( file );
		User::LeaveIfError( file.Write( aData ) );
		
		CleanupStack::PopAndDestroy();//file
		}
		
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL( bitmap );

	LoadBitmapL( fileName, *bitmap );

#ifndef TEST_CODE_LOAD_AND_SCALE
	TSize bmpSize = bitmap->SizeInPixels();
	if (bmpSize.iWidth > KLogoMaxWidthPixels ||
		bmpSize.iHeight > KLogoMaxHeightPixels)
		{
		ScaleBitmapL( *bitmap, KLogoMaxWidthPixels, KLogoMaxHeightPixels );
		}
#endif // !TEST_CODE_LOAD_AND_SCALE

	GetOpInfoL( mcc, mnc );
	SaveOperatorLogoBitmapL( bitmap->Handle(), mcc, mnc );

	CleanupStack::PopAndDestroy( bitmap );

	fs.Delete( fileName );
	CleanupStack::PopAndDestroy();//fs


	RDEBUG("COperatorLogoAdapter::SetOperatorLogoImageL(): end");
	return ret;
	}

// ------------------------------------------------------------------------------------------------
//  COperatorLogoAdapter::DeleteOperatorLogoImageL()
// ------------------------------------------------------------------------------------------------
MSmlDmAdapter::TError COperatorLogoAdapter::DeleteOperatorLogoImageL()
	{
	TError ret = EOk;
	TInt bmpHandle = 0;
	TInt mcc = 0;
	TInt mnc = 0;

	GetOpInfoL( mcc, mnc );
	// Setting operatorlogo bitmap handle to zero will delete it
	SaveOperatorLogoBitmapL( bmpHandle, mcc, mnc );

	return ret;
	}



// ------------------------------------------------------------------------------------------------
//  COperatorLogoAdapter::
// ------------------------------------------------------------------------------------------------
void COperatorLogoAdapter::LoadBitmapL( const TDesC& aFileName, CFbsBitmap& aBitmap )
	{
	RFs fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL( fs );

	const CImageDecoder::TOptions opt =
		(CImageDecoder::TOptions)
			(
			CImageDecoder::EOptionAlwaysThread
#ifndef TEST_CODE_LOAD_AND_DITHER
			| CImageDecoder::EOptionNoDither
#endif // !TEST_CODE_LOAD_AND_DITHER
			);
	CImageDecoder* decoder = CImageDecoder::FileNewL( fs, aFileName, opt );
	CleanupStack::PushL( decoder );

	TInt frameCount = decoder->FrameCount();
	TFrameInfo frameInfo = decoder->FrameInfo();

	TSize bmpSize = frameInfo.iOverallSizeInPixels;
	TDisplayMode bmpDispMode = frameInfo.iFrameDisplayMode;

	// Adjust Display Mode
	if (bmpDispMode > EColor64K)
		{
		bmpDispMode = EColor64K;
		}

	// Check target bitmap size
	const TSize KLogoMaxSize( KLogoMaxWidthPixels, KLogoMaxHeightPixels );

#ifdef TEST_CODE_LOAD_AND_SCALE
	TUint32 ratioX = (static_cast<TUint32>( (KLogoMaxSize.iWidth) << 16) ) / bmpSize.iWidth;
	TUint32 ratioY = (static_cast<TUint32>( (KLogoMaxSize.iHeight) << 16) ) / bmpSize.iHeight;
	TUint32 ratio = Min( ratioX, ratioY );

	if ( (frameInfo.iFlags & TFrameInfo::EFullyScaleable) == 0 )
		{
		if (ratio < (1<<13) )
			{
			ratio = (1<<13); // 1/8;
			}
		else if (ratio < (1<<14) )
			{
			ratio = (1<<14); // 1/4;
			}
		else if (ratio < (1<<15) )
			{
			ratio = (1<<15); // 1/2;
			}
		else
			{
			ratio = (1<<16); // 1/1;
			}
		}

	bmpSize.iWidth = (((bmpSize.iWidth << 1) + 1) * ratio) >> 17;
	bmpSize.iHeight = (((bmpSize.iHeight << 1) + 1) * ratio) >> 17;
#endif // TEST_CODE_LOAD_AND_SCALE

#ifdef TEST_CODE_LOAD_AND_DITHER
	GetScreenDisplayModeL( bmpDispMode );
	if (bmpDispMode == EColor16MA)
		{
		bmpDispMode = EColor16M;
		}
#endif // TEST_CODE_LOAD_AND_DITHER

	aBitmap.Reset();
	TInt err = aBitmap.Create( bmpSize, bmpDispMode );
	User::LeaveIfError( err );

	TRequestStatus reqStatus = KRequestPending;

	decoder->Convert( &reqStatus, aBitmap );
	User::WaitForRequest( reqStatus );

	err = reqStatus.Int();
	User::LeaveIfError( err );

	CleanupStack::PopAndDestroy( decoder );
	CleanupStack::PopAndDestroy( &fs );
	}


// ------------------------------------------------------------------------------------------------
//  COperatorLogoAdapter::
// ------------------------------------------------------------------------------------------------
void COperatorLogoAdapter::ScaleBitmapL( CFbsBitmap& aBitmap, TInt aX, TInt aY )
	{
	CBitmapScaler* scaler = CBitmapScaler::NewL();
	CleanupStack::PushL( scaler );

	CWaitScheduler* waitScheduler = CWaitScheduler::NewL();
	CleanupStack::PushL( waitScheduler );

	TSize dstSize = aBitmap.SizeInPixels();
	if (dstSize.iWidth > aX || dstSize.iHeight > aY)
		{
		dstSize.iWidth = aX;
		dstSize.iHeight = aY;

		scaler->Scale( &waitScheduler->iStatus, aBitmap, dstSize );//, TBool aMaintainAspectRatio=ETrue);
		waitScheduler->WaitForRequest();
		}

	// Cleanup
	CleanupStack::PopAndDestroy( waitScheduler );
	CleanupStack::PopAndDestroy( scaler );
	}


// ------------------------------------------------------------------------------------------------
//  COperatorLogoAdapter::
// ------------------------------------------------------------------------------------------------
void COperatorLogoAdapter::GetScreenDisplayModeL( TDisplayMode &aDisplayMode )
	{
	// Device display mode
	RWsSession ws;
	User::LeaveIfError( ws.Connect() );
	CleanupClosePushL( ws );

	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice( ws );
	CleanupStack::PushL( screen );
	User::LeaveIfError( screen->Construct() );

	aDisplayMode = screen->DisplayMode();

	CleanupStack::PopAndDestroy( screen );
	screen = 0;

	CleanupStack::PopAndDestroy( &ws );
	}



// ------------------------------------------------------------------------------------------------
//  COperatorLogoAdapter::
// ------------------------------------------------------------------------------------------------
void COperatorLogoAdapter::GetOpInfoL( TInt& aMCC, TInt& aMNC )
	{
	RDEBUG("COperatorLogoAdapter::GetOpInfoL(): begin");
	TInt err = KErrNone ;

	aMCC = 0; // -1;
	aMNC = 0; // -1;

	RTelServer telServer ;
	RMobilePhone mobilePhone;

	CleanupClosePushL( telServer );
	CleanupClosePushL( mobilePhone );

	err = telServer.Connect();
	User::LeaveIfError( err );

	TInt phoneCount = 0;
	err =  telServer.EnumeratePhones( phoneCount );
	User::LeaveIfError( err );

	TInt i = 0;
	TBool found = EFalse;

	for (i = 0 ; i < phoneCount && !found ; i++)
		{
		RTelServer::TPhoneInfo phoneInfo;
		err = telServer.GetPhoneInfo( i, phoneInfo );
		User::LeaveIfError( err );

		err = mobilePhone.Open( telServer, phoneInfo.iName );

		if (err == KErrNone)
			{
			TRequestStatus status;
			RMobilePhone::TMobilePhoneNetworkInfoV1 v1Info;
			RMobilePhone::TMobilePhoneNetworkInfoV1Pckg pckgInfo( v1Info );

			// There are some problems with GetHomeNetwork() returned MNC when using old SIM-cards that have additional or invalid MNC in them.
			// Therefor we prefer using network returned MNC if we are registered to home network.
			// Otherwise we try to use MNC originated from SIM, should work but doesn't always because of SIM-errors.
			RMobilePhone::TMobilePhoneRegistrationStatus regStatus;
			mobilePhone.GetNetworkRegistrationStatus( status, regStatus );
			User::WaitForRequest( status );

			if (regStatus == RMobilePhone::ERegisteredOnHomeNetwork)
				{
				mobilePhone.GetCurrentNetwork( status, pckgInfo );
				User::WaitForRequest( status );
				}
			else
				{

				mobilePhone.GetHomeNetwork( status, pckgInfo );
				User::WaitForRequest( status );
				}

			TLex lex;
			lex.Assign( v1Info.iCountryCode );
			lex.Val( aMCC );
			lex.Assign( v1Info.iNetworkId );
			lex.Val( aMNC );

			found = ETrue;
			}

		mobilePhone.Close();
		}

	telServer.Close();

	CleanupStack::Pop( &mobilePhone );
	CleanupStack::Pop( &telServer );
	RDEBUG("COperatorLogoAdapter::GetOpInfoL(): end");
	}




// ------------------------------------------------------------------------------------------------
//  COperatorLogoAdapter::SaveOperatorLogoBitmapL()
// ------------------------------------------------------------------------------------------------
void COperatorLogoAdapter::SaveOperatorLogoBitmapL( TInt aBitmapHandle, TInt aMCC, TInt aMNC )
	{
	TInt err = 0;
	// Check parameters
	if(aMCC == 0 || aMNC == 0)
		{
		return;
		}
		
	
    
	CPhCltImageHandler* handler = CPhCltImageHandler::NewL(); 	
	User::LeaveIfNull( handler );

	// Create image parameter class
	CPhCltImageParams* params = handler->CPhCltBaseImageParamsL( EPhCltTypeOperatorLogo );
	CPhCltExtOperatorLogoParams* imageParams = static_cast<CPhCltExtOperatorLogoParams*>(params);

	// Specify country and network code
	imageParams->SetCodesL( aMCC, aMNC, EPhCltLogoTypeOTA );

	// set CFbsBitmap handle of existing logo instance
	if (aBitmapHandle == 0)
		{
		aBitmapHandle = KPhCltDeleteOperatorLogo;
		}
	imageParams->AddImageL( aBitmapHandle );

	// store image
	err = handler->SaveImages( *imageParams );
	User::LeaveIfError( err );
		
    
	}

