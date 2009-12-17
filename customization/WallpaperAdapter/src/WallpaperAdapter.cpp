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
* Description:  DM Wallpaper Adapter
*
*/


/*
./Customization -+--> Wallpaper -+--> Image


./Customization -+--> Wallpaper -+--> Image

./Customization -+--> Themes -+--> CurrentThemes -+--> <X> * -+--> AppRef
                 |            |                               |
                 |            |                               |--> ID
                 |            |                               |
                 |            |                               |--> Name
                 |            |                               |
                 |            |                               |--> Version
                 |            |
                 |            |--> ActiveTheme
                 |            |
                 |            |--> InstallTheme
                 |            |
                 |            |--> InstallOptions
                 |
                 |
                 |
./Customization -+--> OperatorLogo -+--> LogoData
                 |
                 |
                 |
./Customization -+--> EmbeddedLinks -+--> <X> * -+--> Links -+--> <X> * -+--> URL
                 |                               |                       |
                 |                               |--> ID                 |--> Name
                 |                               |                       |
                 |                               |--> AppName            |--> UserName
                 |                                                       |
                 |                                                       |--> Password
                 |                                                       |
                 |                                                       |--> ConRef
                 |                                                       |
                 |                                                       |
                 |
                 |
./Customization -+--> Startup -+--> WelcomeData
                 |
                 |
                 |
./Customization -+--> ScreenSaver --+----> CurrentScreenSavers --> <X> * ---+-----> ID
				 |					|										|
				 |					|----> ActiveSc							|-----> Name
				 |					|										|
				 |					|----> InstallScreenSaver				|-----> Description
				 |					|										|
				 |					|----> InstallOptions					|-----> CustomProperty
				 |															|
				 |															|-----> Version
                 |
                 |
                 |
./Customization -+--> Wallpaper -+--> Image
                 |
                 |
                 |
./Customization -+--> IdleSoftkeys -+--> SoftKeys -+--> <X> * -+--> ID
                 |                  |                          |
                 |                  |                          |--> Caption
                 |                  |                          |
                 |                  |                          |--> Image
                 |                  |                          |
                 |                  |                          |--> TargetRef
                 |                  |
                 |                  |
                 |                  |--> Targets -+--> <X> * -+--> ID
                 |                  |                         |
                 |                  |                         |--> Description
                 |                  |
                 |                  |
                 |                  |--> WebTargets -+--> <X> * -+--> ID
                 |                                               |
                 |                                               |--> Description
                 |                                               |
                 |                                               |--> URL
                 |                                               |
                 |                                               |--> UserName
                 |                                               |
                 |                                               |--> Password
                 |                                               |
                 |                                               |--> ConRef
                 |
                 |
                 |-->
                 |
                 |-->
                 |
                 |-->
                 |
                 |-->
                 |
                 |-->
                 |
                 |-->
                 |
                 |-->
                 |
                 |
                 |

*/

#include <imcvcodc.h>
#include "debug.h"
#include "WallpaperAdapter.h"
#include "nsmldmuri.h"
#include <implementationproxy.h> // For TImplementationProxy definition
#include <f32file.h>
#include "FileCoderB64.h"
#include <AknsWallpaperUtils.h>

//#ifdef __SAP_POLICY_MANAGEMENT
#include <PolicyEngineXACML.h>
//#endif
#include <featmgr.h>
#ifdef __Sml_DEBUG__
#pragma message("Sml Debugging is ON!")
#endif


_LIT( KIVBackgroundImagePathBin, "c:\\system\\data\\backgroundimage.bin" );

_LIT8( KMimeTypeJpeg, "image/jpeg" );
_LIT8( KMimeTypeGif, "image/jpeg" );

// ------------------------------------------------------------------------------------------------
// CWallpaperAdapter* CWallpaperAdapter::NewL( )
// ------------------------------------------------------------------------------------------------
CWallpaperAdapter* CWallpaperAdapter::NewL( MSmlDmCallback* aDmCallback )
	{
	RDEBUG("CWallpaperAdapter::NewL(): begin");

	CWallpaperAdapter* self = NewLC( aDmCallback );
	CleanupStack::Pop();
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CWallpaperAdapter* CWallpaperAdapter::NewLC( )
// ------------------------------------------------------------------------------------------------
CWallpaperAdapter* CWallpaperAdapter::NewLC( MSmlDmCallback* aDmCallback )
	{
	CWallpaperAdapter* self = new( ELeave ) CWallpaperAdapter( aDmCallback );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CWallpaperAdapter::CWallpaperAdapter()
// ------------------------------------------------------------------------------------------------

CWallpaperAdapter::CWallpaperAdapter( MSmlDmCallback* aDmCallback )
	: CTARMDmAdapter( aDmCallback )
	, iCallBack( aDmCallback )
	{
	RDEBUG("CWallpaperAdapter::CWallpaperAdapter()");
	}

// ------------------------------------------------------------------------------------------------
// CWallpaperAdapter::~CWallpaperAdapter()
// ------------------------------------------------------------------------------------------------

CWallpaperAdapter::~CWallpaperAdapter()
	{
	RDEBUG("CWallpaperAdapter::~CWallpaperAdapter()");
	FeatureManager::UnInitializeLib();
	}

// ------------------------------------------------------------------------------------------------
//  CWallpaperAdapter::DDFVersionL()
// ------------------------------------------------------------------------------------------------
void CWallpaperAdapter::DDFVersionL( CBufBase& aDDFVersion )
	{
	aDDFVersion.InsertL( 0, KSmlWPDDFVersion );
	}

// ------------------------------------------------------------------------------------------------
//  CWallpaperAdapter::DDFStructureL()
//
// ------------------------------------------------------------------------------------------------
void CWallpaperAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
	{
	RDEBUG( "CWallpaperAdapter::DDFStructureL(): begin" );
	
	TSmlDmAccessTypes accessTypesGet;
	accessTypesGet.SetGet();
	
	/*
	Node: ./Customzation
	This interior node is the common parent to all customization..
	Status: Required
	Occurs: One
	Format: Node
	Access Types: Get
	Values: N/A
	*/
	// Apps
	MSmlDmDDFObject& apps = aDDF.AddChildObjectL( KSmlCustomizationN );
	FillNodeInfoL(apps, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KSmlCustomizationD, EFalse );

	/*
	Node: ./Customization/Wallpaper
	This interior node acts as a placeholder for one or more update packages within a same content provider group.
	Status: Required
	Occurs: One
	Format: Node
	Access Types: Get
	Values: N/A
	*/
	// Apps/<x>/
	// e.g. /Apps/Games
	MSmlDmDDFObject& groupNode = apps.AddChildObjectL( KSmlWPWallpaperN );
	FillNodeInfoL( groupNode, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KSmlWPWallpaperD, EFalse );

	/*
	Node: ./Customization/Wallpaper/Data
Image node stores the wallpaper image as binary data, and its runtime Type determines the interpretation.
·	Status: Optional
·	Occurs: One
·	Format: bin
·	Access Types: Get, Replace
·	Values: N/A
	*/
	TSmlDmAccessTypes accessTypesGetReplace;
	accessTypesGetReplace.SetGet();
	accessTypesGetReplace.SetReplace();

	MSmlDmDDFObject& imageNode = groupNode.AddChildObjectL( KSmlWPDataN );
	FillNodeInfoL( imageNode, accessTypesGetReplace, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EBin, KSmlWPImageD, EFalse);
	}

// ------------------------------------------------------------------------------------------------
//  CWallpaperAdapter::UpdateLeafObjectL()
//  
// ------------------------------------------------------------------------------------------------
void CWallpaperAdapter::_UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& /*aLUID*/, const TDesC8& aObject, const TDesC8& aType, const TInt aStatusRef )
	{

	RDEBUG( "CWallpaperAdapter::UpdateLeafObjectL(): begin" );
	TError ret = EOk ;
	TInt err = KErrNone;
	
#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);
	TInt cnt = NSmlDmURI::NumOfURISegs( uriPtrc ) ;
#else
	TInt cnt = NSmlDmURI::NumOfURISegs( aURI ) ;
#endif

	if ( cnt == 3 ) 
		{
		TPtrC8 app = NSmlDmURI::LastURISeg( aURI ) ;
		if ( app == KSmlWPDataN ) 
			{
			if( IsImageL( aType ) )
				{
				RFs fs;
				User::LeaveIfError( fs.Connect() );
				CleanupClosePushL( fs );
				
				TBool setWallpaper = aObject.Length();
				if (setWallpaper)
					{
					TFileCoderB64 decoder;
					TBool isEncoded = TFileCoderB64::CheckB64Encode( aObject );
					if (isEncoded )
						{
						decoder.DecodeToL( aObject, KIVBackgroundImagePathBin() );
						}
					else
						{
						// aObject is original data, save it to KIVBackgroundImagePathBin()
						RFile file;
						User::LeaveIfError( file.Replace( fs, KIVBackgroundImagePathBin(), EFileWrite));
						CleanupClosePushL( file );
						User::LeaveIfError( file.Write( aObject ) );
						
						CleanupStack::PopAndDestroy(); // file
						}					
					err = AknsWallpaperUtils::SetIdleWallpaper( KIVBackgroundImagePathBin(), 0 );
					}
				else
					{
					// Clear wallpaper
					err = AknsWallpaperUtils::SetIdleWallpaper( KNullDesC(), 0 );
					// Delete wallpaper file
					fs.Delete( KIVBackgroundImagePathBin() );
					}
				
				if (err != KErrNone)
					{
					ret = EError;
					}
				CleanupStack::PopAndDestroy(); // fs
				}
			else
				{
				RDEBUG("*** mime type is not supported!");
				ret = EInvalidObject;
				}
			}
		}
	else
		{
		ret = EError ;
		}

	iCallBack->SetStatusL( aStatusRef, ret );
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CWallpaperAdapter::_UpdateLeafObjectL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/, RWriteStream*& /*aStream*/, const TDesC8& /*aType*/, TInt aStatusRef )
	{
	RDEBUG( "CWallpaperAdapter::UpdateLeafObjectL(...RWriteStream...): begin" );
	//Not supported
	TError ret = EError;
	iCallBack->SetStatusL( aStatusRef, ret );
	}


// ------------------------------------------------------------------------------------------------
// CWallpaperAdapter::DeleteObjectL( const TDesC& aURI, const TDesC& aLUID )
// ------------------------------------------------------------------------------------------------
void CWallpaperAdapter::_DeleteObjectL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/, const TInt aStatusRef )
	{
	RDEBUG( "CWallpaperAdapter::DeleteObjectL(): begin" );
	//Not supported
	CSmlDmAdapter::TError ret = CSmlDmAdapter::EError;
	iCallBack->SetStatusL( aStatusRef, ret );
	}

// ------------------------------------------------------------------------------------------------
//  CWallpaperAdapter::FetchLeafObjectL( const TDesC& aURI, const TDesC& aLUID, const TDesC& aType, CBufBase& aObject )
// ------------------------------------------------------------------------------------------------
void CWallpaperAdapter::_FetchLeafObjectL( const TDesC8& aURI, const TDesC8& /*aLUID*/, const TDesC8& /*aType*/, const TInt /*aResultsRef*/, const TInt aStatusRef )
	{
	RDEBUG("CWallpaperAdapter::FetchLeafObjectL(): begin");
	CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);
	TInt numSeqs( NSmlDmURI::NumOfURISegs( uriPtrc ) );
#else
	TInt numSeqs( NSmlDmURI::NumOfURISegs( aURI ) );
#endif

	if( numSeqs == 3 ) 
		{
#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 mapping( NSmlDmURI::LastURISeg( uriPtrc ) );
#else
	TPtrC8 mapping( NSmlDmURI::LastURISeg( aURI ) );
#endif		
		if( mapping == KSmlWPDataN ) 
			{
			status = CSmlDmAdapter::EError;
			}
		} 
	else 
		{
		status = EInvalidObject ;
		}
	iCallBack->SetStatusL( aStatusRef, status );
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CWallpaperAdapter::_FetchLeafObjectSizeL( const TDesC8& aURI, const TDesC8& /*aLUID*/, const TDesC8& /*aType*/, TInt /*aResultsRef*/, TInt aStatusRef )
	{
	RDEBUG("CWallpaperAdapter::FetchLeafObjectSizeL(): begin");

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);
	TNodeId nodeId = NodeId( uriPtrc );
#else
	TNodeId nodeId = NodeId( aURI );
#endif

	TError ret = EOk ;

	switch( nodeId )
		{
		case ENodeCustomization:
			ret = EError;
			break;

		case ENodeWallpaper:
			ret = EError;
			break;

		case ENodeImage:
			ret = EError;
			break;

		default:
			ret = EError;
			break;
		}

	iCallBack->SetStatusL( aStatusRef, ret );
	}

// ------------------------------------------------------------------------------------------------
//  CWallpaperAdapter::ChildURIListL( const TDesC& aURI, const TDesC& aParentLUID, const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, CArrayFix<TPtrC>& aCurrentURISegmentList )
// ------------------------------------------------------------------------------------------------
void CWallpaperAdapter::_ChildURIListL( const TDesC8& aURI, const TDesC8& /*aParentLUID*/, const CArrayFix<TSmlDmMappingInfo>& /*aPreviousURISegmentList*/, const TInt aResultsRef, const TInt aStatusRef  )
	{
	RDEBUG("CWallpaperAdapter::ChildURIListL(): begin");
	
	CSmlDmAdapter::TError ret = CSmlDmAdapter::EOk;
		
	CBufBase *currentList = CBufFlat::NewL( 128 );
	CleanupStack::PushL( currentList );

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);
	TInt numSeqs( NSmlDmURI::NumOfURISegs( uriPtrc ) );
#else
	TInt numSeqs( NSmlDmURI::NumOfURISegs( aURI ) );
#endif

#ifdef __TARM_SYMBIAN_CONVERGENCY	
	if ( numSeqs == 0 || uriPtrc == _L8("") )
#else
	if ( numSeqs == 0 || aURI == _L8("") )
#endif	
		{
		currentList->InsertL( 0, KSmlCustomizationN() );
		}
	else if (numSeqs == 1)
		{
		currentList->InsertL( 0, KSmlWPWallpaperN() );
		}
	else if (numSeqs == 2)
		{
		currentList->InsertL( 0, KSmlWPDataN() );
		}
	else if (numSeqs == 3)
		{
		//Empty result, no error code
		//currentList->InsertL( 0, KNullDesC8() );
		}
	else
		{
		ret = CSmlDmAdapter::EError;
		}
	
	iCallBack->SetStatusL( aStatusRef, ret );
	
	if( ret==CSmlDmAdapter::EOk )
		{
		iCallBack->SetResultsL( aResultsRef, *currentList, KNullDesC8 );
		}
	
	CleanupStack::PopAndDestroy(); //currentList
	}

// ------------------------------------------------------------------------------------------------
// CWallpaperAdapter::AddNodeObjectL( const TDesC& aURI, const TDesC& aParentLUID )
// ------------------------------------------------------------------------------------------------
void CWallpaperAdapter::_AddNodeObjectL( const TDesC8& /*aURI*/, const TDesC8& /*aParentLUID*/, const TInt aStatusRef )
	{
	RDEBUG("CWallpaperAdapter::AddNodeObjectL(): begin");
	//Not supported
	CSmlDmAdapter::TError ret = CSmlDmAdapter::EError;
	iCallBack->SetStatusL( aStatusRef, ret );
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CWallpaperAdapter::_ExecuteCommandL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/, const TDesC8& /*aArgument*/, const TDesC8& /*aType*/, TInt aStatusRef )
	{
	RDEBUG("CWallpaperAdapter::ExecuteCommandL(): begin");
	//Not supported
	TError ret = EError;
	iCallBack->SetStatusL( aStatusRef, ret );
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CWallpaperAdapter::_ExecuteCommandL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/, RWriteStream*& /*aStream*/, const TDesC8& /*aType*/, TInt aStatusRef )
	{
	RDEBUG("CWallpaperAdapter::ExecuteCommandL(...RWriteStream...): begin");
	//Not supported
	TError ret = EError;
	iCallBack->SetStatusL( aStatusRef, ret );
	}
	
// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CWallpaperAdapter::_CopyCommandL( const TDesC8& /*aTargetURI*/, const TDesC8& /*aTargetLUID*/, const TDesC8& /*aSourceURI*/, const TDesC8& /*aSourceLUID*/, const TDesC8& /*aType*/, TInt aStatusRef )
	{
	RDEBUG("CWallpaperAdapter::CopyCommandL()");
	//Not supported
	TError ret = EError;
	iCallBack->SetStatusL( aStatusRef, ret );
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CWallpaperAdapter::StartAtomicL()
	{
	RDEBUG("CWallpaperAdapter::StartAtomicL()");
	//Not supported
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CWallpaperAdapter::CommitAtomicL()
	{
	RDEBUG("CWallpaperAdapter::CommitAtomicL()");
	//Not supported
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CWallpaperAdapter::RollbackAtomicL()
	{
	RDEBUG("CWallpaperAdapter::RollbackAtomicL()");
	//Not supported
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
TBool CWallpaperAdapter::StreamingSupport( TInt& /*aItemSize*/ )
	{
	RDEBUG("CWallpaperAdapter::StreamingSupport()");
	return EFalse;
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
#ifdef __TARM_SYMBIAN_CONVERGENCY	
void CWallpaperAdapter::StreamCommittedL( RWriteStream& /*aStream*/ )
#else
void CWallpaperAdapter::StreamCommittedL()
#endif	
	{	
	RDEBUG("CWallpaperAdapter::StreamCommittedL(): begin");
	RDEBUG("CWallpaperAdapter::StreamCommittedL(): end");
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CWallpaperAdapter::CompleteOutstandingCmdsL()
	{
	RDEBUG("CWallpaperAdapter::CompleteOutstandingCmdsL()");
	}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
//#ifdef __SAP_POLICY_MANAGEMENT
TPtrC8 CWallpaperAdapter::PolicyRequestResourceL( const TDesC8& /*aURI*/ )
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
CWallpaperAdapter::TNodeId CWallpaperAdapter::NodeId( const TDesC8& aURI )
	{
	TNodeId id = ENodeUnknown;

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TInt cnt( NSmlDmURI::NumOfURISegs( uriPtrc ) );
	TPtrC8 app = NSmlDmURI::LastURISeg( uriPtrc );
#else
	TInt cnt( NSmlDmURI::NumOfURISegs( aURI ) );
	TPtrC8 app = NSmlDmURI::LastURISeg( aURI );
#endif

	if ( cnt == 1 )
		{
		if (app == KSmlCustomizationN)
			{
			id = ENodeCustomization;
			}
		}
	else if ( cnt == 2 ) 
		{
		if (app == KSmlWPWallpaperN)
			{
			id = ENodeWallpaper;
			}
		}
	else if ( cnt == 3 ) 
		{
		if (app == KSmlWPDataN)
			{
			id = ENodeImage;
			}
		}

	return id;
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] = 
	{
	{ {KSmlDMWallpaperAdapterImplUid}, (TProxyNewLPtr)CWallpaperAdapter::NewL }
	};

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
	{
	aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
	return ImplementationTable;
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CWallpaperAdapter::ConstructL( )
	{
		TRAPD( errf, FeatureManager::InitializeLibL() );
		if( errf != KErrNone )
		{
			User::Leave(errf);
		}
		if( ! FeatureManager::FeatureSupported(KFeatureIdSapWallpaperAdapter))
		{
			
	   		User::Leave( KErrNotSupported );
		}
	}

// -------------------------------------------------------------------------------------
// CWallpaperAdapter::FillNodeInfoL()
// Fills the node info in ddf structure
// -------------------------------------------------------------------------------------
void CWallpaperAdapter::FillNodeInfoL( MSmlDmDDFObject& aNode,TSmlDmAccessTypes aAccTypes,
								MSmlDmDDFObject::TOccurence aOccurrence, MSmlDmDDFObject::TScope aScope, 
								MSmlDmDDFObject::TDFFormat aFormat,const TDesC8& aDescription,TBool aObjectGroup)
	{
	aNode.SetAccessTypesL(aAccTypes);
	aNode.SetOccurenceL(aOccurrence);
	aNode.SetScopeL(aScope);
	aNode.SetDFFormatL(aFormat);
	if(aFormat!=MSmlDmDDFObject::ENode)
		{
		aNode.AddDFTypeMimeTypeL(KSmlWPTextPlain);
		}
	aNode.SetDescriptionL(aDescription);
	if(aObjectGroup)
		{
		aNode.SetAsObjectGroup();
		}
	}

// -------------------------------------------------------------------------------------
// CWallpaperAdapter::IsImage()
// Checks whether the given mime type is image format (that we support)
// -------------------------------------------------------------------------------------
TBool CWallpaperAdapter::IsImageL( const TDesC8& aMime ) 
	{
	RDEBUG_2("CWallpaperAdapter::IsImageL( %S )", &aMime );
	TBool ret = EFalse;
	HBufC8* buf = aMime.AllocLC();
	TPtr8 ptr = buf->Des();
	ptr.LowerCase();
	ptr.Trim();
	
	
	
	if( ptr == KMimeTypeJpeg )
		{
		ret = ETrue;
		}
	else if( ptr == KMimeTypeGif )
		{
		ret = ETrue;
		}

	CleanupStack::PopAndDestroy( buf );
	return ret;
	}
	

