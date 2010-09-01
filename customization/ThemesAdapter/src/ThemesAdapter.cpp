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
* Description:  DM Themes Adapter
*
*/


/*

./Customization -+--> Themes -+--> AvailableThemes -+--> <X> * -+--> ID
                              |                                 |
                              |                                 |--> Name
                              |                                 |
                              |                                 |--> Version
                              |                                 |
                              |                                 |--> Data
                              |                                 |
                              |                                 |--> InstallOptions
                              |
                              |--> ActiveTheme

*/

  
#include <badesca.h>
#include <fbs.h>
#include <imcvcodc.h>
#include "debug.h"
#include "ThemesAdapter.h"
#include "ThemesList.h"
#include "nsmldmuri.h"
#include <implementationproxy.h> // For TImplementationProxy definition
#include <f32file.h>
#include <e32def.h>
#include "FileCoderB64.h"
#include "aminstalloptions.h"
#include "AknsSrvClient.h"
#include <AknSkinsInternalCRKeys.h>
#include <centralrepository.h>
#include <utf.h> 
//#ifdef __SAP_POLICY_MANAGEMENT
#include <PolicyEngineXACML.h>
#include <SettingEnforcementInfo.h>
//#endif
#include <featmgr.h>


_LIT( KThemesAdapterTmpInstallPath, "c:\\system\\temp\\DMTheme.sis" );
_LIT8(KOpenBrace, "(");
_LIT8(KCloseBrace, ")");

// ------------------------------------------------------------------------------------------------
// CThemesAdapter* CThemesAdapter::NewL( )
// ------------------------------------------------------------------------------------------------
CThemesAdapter* CThemesAdapter::NewL( MSmlDmCallback* aDmCallback )
	{
	RDEBUG("CThemesAdapter::NewL(): begin");

	CThemesAdapter* self = NewLC( aDmCallback );
	CleanupStack::Pop();
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CThemesAdapter* CThemesAdapter::NewLC( )
// ------------------------------------------------------------------------------------------------
CThemesAdapter* CThemesAdapter::NewLC( MSmlDmCallback* aDmCallback )
	{
	RDEBUG("CThemesAdapter::NewLC(): begin");
	CThemesAdapter* self = new( ELeave ) CThemesAdapter( aDmCallback );
	CleanupStack::PushL( self );
	self->ConstructL( );
	return self;
	}


// ------------------------------------------------------------------------------------------------
// CThemesAdapter::CThemesAdapter()
// ------------------------------------------------------------------------------------------------
CThemesAdapter::CThemesAdapter( MSmlDmCallback* aDmCallback )
	: CTARMDmAdapter( aDmCallback )
	, iCallBack( aDmCallback )
	, iThemes( 0 )
	{
	RDEBUG("CThemesAdapter::CThemesAdapter(): begin");
	}

// ------------------------------------------------------------------------------------------------
// CThemesAdapter::ConstructL()
// ------------------------------------------------------------------------------------------------
void CThemesAdapter::ConstructL( )
	{
		TRAPD( errf, FeatureManager::InitializeLibL() );
		if( errf != KErrNone )
		{
			User::Leave(errf);
		}
		
		if(FeatureManager::FeatureSupported(KFeatureIdSapThemesAdapter))
		{
				RDEBUG("CThemesAdapter::ConstructL(): begin");
				iThemes = CThemesList::NewL();
		}
		else
		{
			FeatureManager::UnInitializeLib();
	   		User::Leave( KErrNotSupported );
		}
	}

// ------------------------------------------------------------------------------------------------
// CThemesAdapter::~CThemesAdapter()
// ------------------------------------------------------------------------------------------------


CThemesAdapter::~CThemesAdapter()
	{
	RDEBUG("CThemesAdapter::~CThemesAdapter(): begin");
	
	FeatureManager::UnInitializeLib();
	delete iThemes;
	delete iActiveTheme;
	}

// ------------------------------------------------------------------------------------------------
//  CThemesAdapter::DDFVersionL()
// ------------------------------------------------------------------------------------------------
void CThemesAdapter::DDFVersionL( CBufBase& aDDFVersion )
	{
	RDEBUG("CThemesAdapter::DDFVersionL(): begin");
	aDDFVersion.InsertL( 0, KThemesAdapterDDFVersion );
	}


// ------------------------------------------------------------------------------------------------
//  CThemesAdapter::DDFStructureL()
//
// ------------------------------------------------------------------------------------------------
void CThemesAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
	{
	RDEBUG( "CThemesAdapter::DDFStructureL(): begin" );

	TSmlDmAccessTypes accessTypesGet;
	accessTypesGet.SetGet();

	TSmlDmAccessTypes accessTypesAddGet;
	accessTypesAddGet.SetAdd();
	accessTypesAddGet.SetGet();

	TSmlDmAccessTypes accessTypesGetReplace;
	accessTypesGetReplace.SetGet();
	accessTypesGetReplace.SetReplace();

	TSmlDmAccessTypes accessTypesDeleteGetReplace;
	accessTypesDeleteGetReplace.SetDelete();
	accessTypesDeleteGetReplace.SetGet();
	accessTypesDeleteGetReplace.SetReplace();
//sf- For testing:
	accessTypesDeleteGetReplace.SetAdd();
//sf- For testing ends.

	TSmlDmAccessTypes accessTypesReplace;
	accessTypesReplace.SetReplace();

	/*
	Node: ./Customization
	This interior node is the common parent to all customization..
	Status: Required
	Occurs: One
	Format: Node
	Access Types: Get
	Values: N/A
	*/
	// Customization
	MSmlDmDDFObject& customization = aDDF.AddChildObjectL( KThemesAdapterCustomization );
	FillNodeInfoL(customization, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KThemesAdapterCustomizationD, ETrue );

	/*
	Node: ./Customization/Themes

	Status: Required
	Occurs: One
	Format: Node
	Access Types: Get
	Values: N/A
	*/
	MSmlDmDDFObject& themes = customization.AddChildObjectL( KThemesAdapterThemes );
	FillNodeInfoL( themes, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KThemesAdapterThemesD, ETrue );

	/*
	Node: ./Customization/Themes/AvailableThemes

	Status: Required
	Occurs: One
	Format: Node
	Access Types: Add, Get
	Values: N/A
	*/
	MSmlDmDDFObject& availableThemes = themes.AddChildObjectL( KThemesAdapterAvailableThemes);
	FillNodeInfoL( availableThemes, accessTypesAddGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KThemesAdapterAvailableThemesD, ETrue );

	/*
	Node: ./Customization/Themes/AvailableThemes/<X>

	Status: Required
	Occurs: Zero or more
	Format: Node
	Access Types: Delete, Get, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& oneCurTheme = availableThemes.AddChildObjectGroupL();
	FillNodeInfoL( oneCurTheme, accessTypesDeleteGetReplace, MSmlDmDDFObject::EZeroOrMore, MSmlDmDDFObject::EDynamic,
		MSmlDmDDFObject::ENode, KThemesAdapterDynamicNodeD, ETrue );

	/*
	Node: ./Customization/Themes/AvailableThemes/<X>/ID

	Status: Required
	Occurs: One
	Format: Chr
	Access Types: Get
	Values: N/A
	*/
	MSmlDmDDFObject& id = oneCurTheme.AddChildObjectL( KThemesAdapterXId );
	FillNodeInfoL( id, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KThemesAdapterXIdD, ETrue );

	/*
	Node: ./Customization/Themes/AvailableThemes/<X>/Name

	Status: Optional
	Occurs: One
	Format: Chr
	Access Types: Get, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& name = oneCurTheme.AddChildObjectL( KThemesAdapterXName );
	FillNodeInfoL( name, accessTypesGetReplace, MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KThemesAdapterXNameD, ETrue );

	/*
	Node: ./Customization/Themes/AvailableThemes/<X>/Version

	Status: Optional
	Occurs: One
	Format: Chr
	Access Types: Get, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& version = oneCurTheme.AddChildObjectL( KThemesAdapterXVersion );
	FillNodeInfoL( version, accessTypesGetReplace, MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KThemesAdapterXVersionD, ETrue );

	/*
	Node: ./Customization/Themes/AvailableThemes/<X>/Data

	Status: Required
	Occurs: One
	Format: Chr
	Access Types: Get
	Values: N/A
	*/
	MSmlDmDDFObject& data = oneCurTheme.AddChildObjectL( KThemesAdapterXData );
	FillNodeInfoL( data, accessTypesReplace, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EBin, KThemesAdapterXDataD, ETrue );


	/*
	Node: ./Customization/Themes/AvailableThemes/<X>/InstallOptions

	Status: ?
	Occurs: One
	Format: Chr
	Access Types: Get
	Values: N/A
	*/
	MSmlDmDDFObject& installOptions = oneCurTheme.AddChildObjectL( KThemesAdapterXInstallOptions );
	FillNodeInfoL( installOptions, accessTypesReplace, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EXml, KThemesAdapterXInstallOptionsD, ETrue );


	/*
	Node: ./Customization/Themes/ActiveTheme

	Status: Required
	Occurs: One
	Format: Chr
	Access Types: Get, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& activeTheme = themes.AddChildObjectL( KThemesAdapterActiveTheme );
	FillNodeInfoL( activeTheme, accessTypesGetReplace, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KThemesAdapterActiveThemeD, ETrue );

	}

// ------------------------------------------------------------------------------------------------
//  CThemesAdapter::UpdateLeafObjectL()
//  
// ------------------------------------------------------------------------------------------------
void CThemesAdapter::_UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& /*aLUID*/, const TDesC8& aObject, const TDesC8& /*aType*/, const TInt aStatusRef )
	{
	RDEBUG( "CThemesAdapter::UpdateLeafObjectL(): begin" );
	
	TError ret = EOk ;

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TNodeId nodeId = NodeId( uriPtrc );
#else
	TNodeId nodeId = NodeId( aURI );
#endif
	
	switch( nodeId )
		{
	case ENodeXName:
		if (aObject.Length() > 0)
			{
#ifdef __TARM_SYMBIAN_CONVERGENCY	
			iCallBack->SetMappingL( uriPtrc, aObject );
#else		
			iCallBack->SetMappingL( aURI, aObject );
#endif			
			}
		else
			{
#ifdef __TARM_SYMBIAN_CONVERGENCY		
			iCallBack->SetMappingL( uriPtrc, _L8(" ") );
#else	
			iCallBack->SetMappingL( aURI, _L8(" ") );
#endif			
			}
		break;

	case ENodeXVersion:
		if (aObject.Length() > 0)
			{
#ifdef __TARM_SYMBIAN_CONVERGENCY	
			iCallBack->SetMappingL( uriPtrc, aObject );
#else		
			iCallBack->SetMappingL( aURI, aObject );
#endif	
			}
		else
			{
#ifdef __TARM_SYMBIAN_CONVERGENCY		
			iCallBack->SetMappingL( uriPtrc, _L8(" ") );
#else	
			iCallBack->SetMappingL( aURI, _L8(" ") );
#endif	
			}
		break;

	case ENodeXData:
		{
		RFs fs;
		User::LeaveIfError( fs.Connect() );
		CleanupClosePushL( fs );

		TFileCoderB64 decoder;
		TPtrC fileName( KThemesAdapterTmpInstallPath );

	 	TBool isEncoded = TFileCoderB64::CheckB64Encode( aObject );
	 	if( isEncoded )
	 		{
	 		decoder.DecodeToL( aObject, fileName );
	 		}
		else
			{
			// aData is original data, save it to fileName
			RFile file;
			User::LeaveIfError( file.Replace( fs, fileName, EFileWrite));
			CleanupClosePushL( file );
			file.Write( aObject );
			
			CleanupStack::PopAndDestroy(); // file
			}		

		TBuf8<KMaxUidName+20> sisPkgUid;
		TBuf8<KAknsPkgIDDesSize> themePkgId;

		// Install using options if given and otherwise with default options
		ret = iThemes->InstallThemeL( fileName, sisPkgUid, themePkgId );
		
		// Set LUIDs for theme and theme/id
		if (ret == KErrNone)
			{
#ifdef __TARM_SYMBIAN_CONVERGENCY		
			TPtrC8 parentURI = NSmlDmURI::ParentURI( uriPtrc );
#else	
			TPtrC8 parentURI = NSmlDmURI::ParentURI( aURI );
#endif					
			HBufC8* dataURI = HBufC8::NewLC( parentURI.Length() + KThemesAdapterXData().Length() + 1 );
			HBufC8* idURI = HBufC8::NewLC( parentURI.Length() + KThemesAdapterXId().Length() + 1 );
			dataURI->Des().Copy( parentURI );
			dataURI->Des().Append( KSlash );
			dataURI->Des().Append( KThemesAdapterXData );
			idURI->Des().Copy( parentURI );

			iCallBack->SetMappingL( *dataURI, sisPkgUid );
			iCallBack->SetMappingL( *idURI, themePkgId );

			CleanupStack::PopAndDestroy( idURI );
			CleanupStack::PopAndDestroy( dataURI );
			
			if( iActiveTheme != NULL )
				{
				TPtrC8 pid = NSmlDmURI::LastURISeg( *iActiveTheme );
				HBufC8* themeRef = HBufC8::NewLC( KThemesAdapterActiveThemeRef().Length() + pid.Length() + 1 );
				themeRef->Des().Copy( KThemesAdapterActiveThemeRef );
				themeRef->Des().Append( pid );
				HBufC8* luid = iCallBack->GetLuidAllocL( *themeRef );
				CleanupStack::PushL( luid );

				if (luid->Length() > 1)
					{
					pid.Set( *luid );
					ret = iThemes->SetActiveThemeL( pid );
					delete iActiveTheme;
					iActiveTheme = NULL;
					}

				CleanupStack::PopAndDestroy( luid );
				CleanupStack::PopAndDestroy( themeRef );
				
				}
		
			}

		// Remove temporary installations file
		fs.Delete( fileName );
		CleanupStack::PopAndDestroy( &fs );
		}
		break;

	case ENodeXInstallOptions:
		ret = iThemes->SetInstallOptionsL( aObject );
		break;

	case ENodeActiveTheme:
		{
		if( iActiveTheme != NULL )
			{
			delete iActiveTheme;
			iActiveTheme = NULL;
			}
		iActiveTheme = aObject.AllocL();
		TPtrC8 pid = NSmlDmURI::LastURISeg( aObject );
		HBufC8* themeRef = HBufC8::NewLC( KThemesAdapterActiveThemeRef().Length() + pid.Length() + 1 );
		themeRef->Des().Copy( KThemesAdapterActiveThemeRef );
		themeRef->Des().Append( pid );
		HBufC8* luid = iCallBack->GetLuidAllocL( *themeRef );
		CleanupStack::PushL( luid );

			if (luid->Length() > 1)
			{
			pid.Set( *luid );
			}
			ret = iThemes->SetActiveThemeL( pid );
			if( ret == MSmlDmAdapter::EOk )
				{
#ifdef __TARM_SYMBIAN_CONVERGENCY
				iCallBack->SetMappingL( uriPtrc, *themeRef );
#else
				iCallBack->SetMappingL( aURI, *themeRef );
#endif				
				}
			else
			{
				ret = EError;
			}

			delete iActiveTheme;
			iActiveTheme = NULL;

		CleanupStack::PopAndDestroy( luid );
		CleanupStack::PopAndDestroy( themeRef );
		}
		break;

	default:
		ret = EError;
		break;
		}
		
	iCallBack->SetStatusL( aStatusRef,  ret );
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CThemesAdapter::_UpdateLeafObjectL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/, RWriteStream*& /*aStream*/, const TDesC8& /*aType*/, TInt aStatusRef )
	{
	RDEBUG( "CThemesAdapter::UpdateLeafObjectL(...RWriteStream...): begin" );
	TError ret = EError ;
	iCallBack->SetStatusL( aStatusRef,  ret );
	}

// ------------------------------------------------------------------------------------------------
// CThemesAdapter::DeleteObjectL( const TDesC& aURI, const TDesC& aLUID )
// ------------------------------------------------------------------------------------------------
void CThemesAdapter::_DeleteObjectL( const TDesC8& aURI, const TDesC8& /*aLUID*/, const TInt aStatusRef)
	{
	RDEBUG( "CThemesAdapter::DeleteObjectL(): begin" );

	TError ret = EOk ;

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TNodeId nodeId = NodeId( uriPtrc );
#else
	TNodeId nodeId = NodeId( aURI );
#endif	

	switch( nodeId )
		{
	case ENodeX:
		{
		TUint pkgUid = 0;
#ifdef __TARM_SYMBIAN_CONVERGENCY
		HBufC8* dataURI = HBufC8::NewLC( uriPtrc.Length() + KThemesAdapterXData().Length() + 1 );
		dataURI->Des().Append( uriPtrc );
#else
		HBufC8* dataURI = HBufC8::NewLC( aURI.Length() + KThemesAdapterXData().Length() + 1 );
		dataURI->Des().Append( aURI );
#endif			
		
		HBufC8* themeId = iCallBack->GetLuidAllocL( *dataURI );
		CleanupStack::PushL( themeId );
		
		dataURI->Des().Append( KSlash );
		dataURI->Des().Append( KThemesAdapterXData );
		HBufC8* luid = iCallBack->GetLuidAllocL( *dataURI );
		CleanupStack::PushL( luid );
		
		if (luid->CompareF(KNullDesC8) == 0 )
		    {
		    ret = ENotFound;
		    CleanupStack::PopAndDestroy( 3 ); //luid, themeId, dataURI
		    break; 
		    }
				
		TLex8 parser( *luid );
		if ((parser.Get() == '[') &&
			(parser.Val( pkgUid, EHex ) == KErrNone) &&
			(parser.Get() == ']'))
			{
			//luid for getting index 
			ret = iThemes->DeleteThemeL(  *themeId, TUid::Uid( pkgUid ),luid);
			}
		else
			{
			ret = ENotAllowed;
            }
	
		CleanupStack::PopAndDestroy( 3 );
		
		}
		break;

	default:
		ret = EError;
		break;
		}

	iCallBack->SetStatusL( aStatusRef,  ret );
	}

// ------------------------------------------------------------------------------------------------
//  CThemesAdapter::GetLeafObjectL( const TDesC& aURI, const TDesC& aLUID, const TDesC& aType, CBufBase& aObject, TError& aRet )
// ------------------------------------------------------------------------------------------------
void CThemesAdapter::GetLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& /*aType*/, CBufBase& aResult, TError& aRet )
	{
	RDEBUG("CThemesAdapter::GetLeafObjectL(): begin");
	aRet = EOk;
	
#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TNodeId nodeId = NodeId( uriPtrc );
	TPtrC8 parentURI = NSmlDmURI::ParentURI( uriPtrc );
#else
	TNodeId nodeId = NodeId( aURI );
	TPtrC8 parentURI = NSmlDmURI::ParentURI( aURI );
#endif	
	
	TPtrC8 pid = NSmlDmURI::LastURISeg( parentURI );
	
	HBufC8* parentLUID = iCallBack->GetLuidAllocL(parentURI);
    CleanupStack::PushL(parentLUID);
	
	//Check for Active theme; if it is not, then only check for DM installed parent node
    if (nodeId != ENodeActiveTheme && parentLUID->CompareF(KNullDesC8) == 0)
        {
        aRet = ENotFound;
        }
		
	if (aRet == EOk)
        {
        HBufC8* trimmedLUID = aLUID.AllocL();
        CleanupStack::PushL(trimmedLUID);
        trimmedLUID->Des().Trim();

        switch (nodeId)
            {
            case ENodeXId:
                if (trimmedLUID->Length() > 0 && *trimmedLUID != *parentLUID)
                    {
                    aResult.InsertL(aResult.Size(), aLUID);
                    }
                else
                    {
                    if (parentLUID->Length() > 1)
                        {
                        pid.Set(*parentLUID);
                        }
                    aRet = iThemes->GetThemeIdL(pid, aResult);
                    }
                break;

            case ENodeXName:
                {

                if (trimmedLUID->Length() > 0 && *trimmedLUID != *parentLUID)
                    {
                    aResult.InsertL(aResult.Size(), aLUID);
                    }
                if (parentLUID->Length() > 1)
                    {
                    pid.Set(*parentLUID);
                    aResult.InsertL(aResult.Size(), KOpenBrace);
                    aRet = iThemes->GetThemeNameL(pid, aResult);
                    aResult.InsertL(aResult.Size(), KCloseBrace);
                    }
                }
                break;

            case ENodeXVersion:
                if (trimmedLUID->Length() > 0 && *trimmedLUID != *parentLUID)
                    {
                    aResult.InsertL(aResult.Size(), aLUID);
                    }
                else
                    {
                    if (parentLUID->Length() > 1)
                        {
                        pid.Set(*parentLUID);
                        }
                    aRet = iThemes->GetThemeVersionL(pid, aResult);
                    }
                break;

            case ENodeActiveTheme:
                {
#ifdef __TARM_SYMBIAN_CONVERGENCY
                TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
                HBufC8* luid = iCallBack->GetLuidAllocL( uriPtrc );
#else
                HBufC8* luid = iCallBack->GetLuidAllocL(aURI);
#endif
                CleanupStack::PushL(luid);

                HBufC8* themePID = iCallBack->GetLuidAllocL(aLUID);
                CleanupStack::PushL(themePID);
                // Get the current active theme UID
                TAknsPkgIDBuf buf;
                CRepository* skinsRepository = CRepository::NewLC(
                        KCRUidPersonalisation);
                TInt errCode = skinsRepository->Get(KPslnActiveSkinUid, buf);

                HBufC16* themePID16 = HBufC16::NewLC(themePID->Length());
                themePID16->Des().Copy(themePID->Des());

                if (themePID16->Compare(buf) == 0)
                    {
                    RDEBUG("CThemesAdapter::GetLeafObjectL(): UIDs MATCH ");
                    aResult.InsertL(aResult.Size(), *luid);
                    aResult.InsertL(aResult.Size(), KOpenBrace);
                    aRet = iThemes->GetThemeNameL(*themePID, aResult);
                    aResult.InsertL(aResult.Size(), KCloseBrace);
                    }
                else
                    {
                    //means Active theme set by DM server is changed to some other theme
                    //manually from settings
                    RDEBUG(
                            "CThemesAdapter::GetLeafObjectL(): UIDs Doesn't MATCH ");
                    HBufC8* buf8 = HBufC8::NewLC(KAknsPkgIDDesSize);
                    buf8->Des().Copy(buf);
                    aRet = iThemes->GetThemeNameL(*buf8, aResult);
                    CleanupStack::PopAndDestroy(buf8);
                    }
                CleanupStack::PopAndDestroy(themePID16);
                CleanupStack::PopAndDestroy(skinsRepository);
                CleanupStack::PopAndDestroy(themePID);
                CleanupStack::PopAndDestroy(luid);
                }
                break;

            default:
                aRet = EError;
                break;
            }

        CleanupStack::PopAndDestroy(trimmedLUID);
        }
	CleanupStack::PopAndDestroy( parentLUID );
	}

// ------------------------------------------------------------------------------------------------
//  CThemesAdapter::FetchLeafObjectL( const TDesC& aURI, const TDesC& aLUID, const TDesC& aType, CBufBase& aObject )
// ------------------------------------------------------------------------------------------------
void CThemesAdapter::_FetchLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aType, const TInt aResultsRef, const TInt aStatusRef )
	{
	RDEBUG("CThemesAdapter::FetchLeafObjectL(): begin");

	TError ret = EOk ;

	CBufBase *result = CBufFlat::NewL( 128 );
	CleanupStack::PushL( result );
	
#ifdef __TARM_SYMBIAN_CONVERGENCY
		TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
		GetLeafObjectL( uriPtrc, aLUID, aType, *result, ret );
#else
		GetLeafObjectL( aURI, aLUID, aType, *result, ret );
#endif

	if (ret == EOk)
		{
		iCallBack->SetResultsL( aResultsRef, *result, KThemesAdapterTextPlain() );
		}

	iCallBack->SetStatusL( aStatusRef, ret );

	CleanupStack::PopAndDestroy( result );
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CThemesAdapter::_FetchLeafObjectSizeL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aType, TInt aResultsRef, TInt aStatusRef )
	{
	RDEBUG("CThemesAdapter::FetchLeafObjectSizeL(): begin");

	TError ret = EOk ;

	CBufBase *result = CBufFlat::NewL( 128 );
	CleanupStack::PushL( result );

#ifdef __TARM_SYMBIAN_CONVERGENCY
		TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
		GetLeafObjectL( uriPtrc, aLUID, aType, *result, ret );
#else
		GetLeafObjectL( aURI, aLUID, aType, *result, ret );
#endif	

	if (ret == EOk)
		{
		TBuf8<20> len;
		len.AppendNum( result->Size() );
		result->Reset();
		result->InsertL( 0, len );

		iCallBack->SetResultsL( aResultsRef, *result, KThemesAdapterTextPlain() );
		}

	iCallBack->SetStatusL( aStatusRef, ret );

	CleanupStack::PopAndDestroy( result );
	}

// ------------------------------------------------------------------------------------------------
//  CThemesAdapter::ChildURIListL( const TDesC& aURI, const TDesC& aParentLUID, const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, CArrayFix<TPtrC>& aCurrentURISegmentList )
// ------------------------------------------------------------------------------------------------
void CThemesAdapter::_ChildURIListL( const TDesC8& aURI, const TDesC8& /*aLUID*/, const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, const TInt aResultsRef, const TInt aStatusRef  )
	{
	RDEBUG("CThemesAdapter::ChildURIListL(): begin");

	TError ret = EOk ;
	
#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TNodeId nodeId = NodeId( uriPtrc );
#else
	TNodeId nodeId = NodeId( aURI );
#endif	

	CBufBase *currentList = CBufFlat::NewL( 128 );
	CleanupStack::PushL( currentList );
	
	

	
        switch (nodeId)
        {
        case ENodeCustomization:
            currentList->InsertL(currentList->Size(), KThemesAdapterThemes);
            break;

        case ENodeThemes:
            currentList->InsertL(currentList->Size(),
                    KThemesAdapterAvailableThemes);
            currentList->InsertL(currentList->Size(), KSlash);
            currentList->InsertL(currentList->Size(),
                    KThemesAdapterActiveTheme);
            break;

        case ENodeAvailableThemes:
            {
            CDesC8ArrayFlat* newThemes = new (ELeave) CDesC8ArrayFlat(16);
            CleanupStack::PushL(newThemes);

            CDesC8ArrayFlat* themes = new (ELeave) CDesC8ArrayFlat(32);
            CleanupStack::PushL(themes);

            iThemes->GetInstalledPIDListL(aPreviousURISegmentList, *themes,
                    *newThemes);

            SyncWithAvkonSkinDB(*themes);
            //get actual theme names and send to DM server
            TInt i = 0;
            TPtrC8 pid;
            for (i = 0; i < themes->Count(); i++)
                {

                HBufC8* themeRef = HBufC8::NewLC(
                        KThemesAdapterActiveThemeRef().Length()
                                + (*themes)[i].Length() + 1);
                themeRef->Des().Copy(KThemesAdapterActiveThemeRef);
                themeRef->Des().Append((*themes)[i]);
                HBufC8* luid = iCallBack->GetLuidAllocL(*themeRef);
                CleanupStack::PushL(luid);

                if (luid->Length() > 1)
                    {
                    if (currentList->Size() > 0)
                        {
                        currentList->InsertL(currentList->Size(), KSlash);
                        }
                    pid.Set(*luid);
                    currentList->InsertL(currentList->Size(), (*themes)[i]);
                    currentList->InsertL(currentList->Size(), KOpenBrace);
                    TInt ret = iThemes->GetThemeNameL(pid, *currentList);
                    currentList->InsertL(currentList->Size(), KCloseBrace);
                    }
                CleanupStack::PopAndDestroy(luid);
                CleanupStack::PopAndDestroy(themeRef);
                }
            for (i = 0; i < newThemes->Count(); i++)
                {
                if (currentList->Size() > 0)
                    {
                    currentList->InsertL(currentList->Size(), KSlash);
                    }
                pid.Set((*newThemes)[i]);
                TInt ret = iThemes->GetThemeNameL(pid, *currentList);
                }

            CleanupStack::PopAndDestroy(themes);
            CleanupStack::PopAndDestroy(newThemes);
            }
            break;

        case ENodeX:
            {
            //Determine whether the node is DM installed node
            HBufC8* lUID = iCallBack->GetLuidAllocL(aURI);

            if (lUID->CompareF(KNullDesC8) == 0)
                {
                ret = ENotFound;
                }

            delete lUID;

            if (ret == EOk)
                {
                currentList->InsertL(currentList->Size(), KThemesAdapterXId);
                currentList->InsertL(currentList->Size(), KSlash);
                currentList->InsertL(currentList->Size(), KThemesAdapterXName);
                currentList->InsertL(currentList->Size(), KSlash);
                currentList->InsertL(currentList->Size(),
                        KThemesAdapterXVersion);
                currentList->InsertL(currentList->Size(), KSlash);
                currentList->InsertL(currentList->Size(), KThemesAdapterXData);
                currentList->InsertL(currentList->Size(), KSlash);
                currentList->InsertL(currentList->Size(),
                        KThemesAdapterXInstallOptions);
                }

            break;
            }

        case ENodeXId:
            // No children, leave list empty
            break;

        case ENodeXName:
            // No children, leave list empty
            break;

        case ENodeXVersion:
            // No children, leave list empty
            break;

        case ENodeXData:
            // No children, leave list empty
            break;

        case ENodeXInstallOptions:
            // No children, leave list empty
            break;

        case ENodeActiveTheme:
            // No children, leave list empty
            break;

        case ENodeUnknown:
            ret = ENotFound;
            break;

        default:
            break;
        }
	if( ret == EOk )
		{
		iCallBack->SetResultsL( aResultsRef, *currentList, KNullDesC8 );
		}

    iCallBack->SetStatusL(aStatusRef, ret);

	CleanupStack::PopAndDestroy(currentList); //currentList
	}


// ------------------------------------------------------------------------------------------------
// CThemesAdapter::AddNodeObjectL( const TDesC& aURI, const TDesC& aParentLUID )
// ------------------------------------------------------------------------------------------------
void CThemesAdapter::_AddNodeObjectL( const TDesC8& aURI, const TDesC8& /*aParentLUID*/, const TInt aStatusRef )
	{
	RDEBUG("CThemesAdapter::AddNodeObjectL(): begin");

	TError ret = EOk ;
	
#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TPtrC8 parentURI = NSmlDmURI::ParentURI( uriPtrc );
#else
	TPtrC8 parentURI = NSmlDmURI::ParentURI( aURI );
#endif	

	TNodeId parentNodeId = NodeId( parentURI );

	
	HBufC8* parentLUID = iCallBack->GetLuidAllocL( parentURI );
	CleanupStack::PushL( parentLUID );

#ifdef __TARM_SYMBIAN_CONVERGENCY
	HBufC8* trimmedLUID = iCallBack->GetLuidAllocL( uriPtrc );
#else	
	HBufC8* trimmedLUID = iCallBack->GetLuidAllocL( aURI );
#endif
	
	CleanupStack::PushL( trimmedLUID );
	trimmedLUID->Des().Trim();

	switch( parentNodeId )
		{
	case ENodeAvailableThemes:
		// Set LUID mapping to make this new node permanent
		if ( trimmedLUID->Length() > 0 && *trimmedLUID != *parentLUID)
			{
			ret = EAlreadyExists;
			}
		else
			{
#ifdef __TARM_SYMBIAN_CONVERGENCY
			iCallBack->SetMappingL( uriPtrc, _L8(" ") );	
#else	
			iCallBack->SetMappingL( aURI, _L8(" ") );
#endif				
			}
		break;

	default:
		ret = EError;
		break;
		}

	// Cleanup
	CleanupStack::Pop( trimmedLUID );
	CleanupStack::Pop( parentLUID );

	iCallBack->SetStatusL( aStatusRef, ret );
	}


// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CThemesAdapter::_ExecuteCommandL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/, const TDesC8& /*aArgument*/, const TDesC8& /*aType*/, TInt aStatusRef )
	{
	RDEBUG("CThemesAdapter::ExecuteCommandL(): begin");
	//Not supported
	TError ret = EError;
	iCallBack->SetStatusL( aStatusRef, ret );
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CThemesAdapter::_ExecuteCommandL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/, RWriteStream*& /*aStream*/, const TDesC8& /*aType*/, TInt aStatusRef )
	{
	RDEBUG("CThemesAdapter::ExecuteCommandL(...RWriteStream...): begin");
	//Not supported
	TError ret = EError;
	iCallBack->SetStatusL( aStatusRef, ret );
	}
	
// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CThemesAdapter::_CopyCommandL( const TDesC8& /*aTargetURI*/, const TDesC8& /*aTargetLUID*/, const TDesC8& /*aSourceURI*/, const TDesC8& /*aSourceLUID*/, const TDesC8& /*aType*/, TInt aStatusRef )
	{
	RDEBUG("CThemesAdapter::CopyCommandL(): begin");
	//Not supported
	TError ret = EError;
	iCallBack->SetStatusL( aStatusRef, ret );
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CThemesAdapter::StartAtomicL()
	{
	RDEBUG("CThemesAdapter::StartAtomicL(): begin");
	//Not supported
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CThemesAdapter::CommitAtomicL()
	{
	RDEBUG("CThemesAdapter::CommitAtomicL(): begin");
	//Not supported
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CThemesAdapter::RollbackAtomicL()
	{
	RDEBUG("CThemesAdapter::RollbackAtomicL(): begin");
	//Not supported
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
TBool CThemesAdapter::StreamingSupport( TInt& /*aItemSize*/ )
	{
	RDEBUG("CThemesAdapter::StreamingSupport(): begin");
	return EFalse;
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
#ifdef __TARM_SYMBIAN_CONVERGENCY	
void CThemesAdapter::StreamCommittedL( RWriteStream& /*aStream*/ )
#else
void CThemesAdapter::StreamCommittedL()
#endif	
	{	
	RDEBUG("CThemesAdapter::StreamCommittedL(): begin");
	RDEBUG("CThemesAdapter::StreamCommittedL(): end");
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CThemesAdapter::CompleteOutstandingCmdsL()
	{
	RDEBUG("CThemesAdapter::CompleteOutstandingCmdsL(): begin");
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
//#ifdef __SAP_POLICY_MANAGEMENT
TPtrC8 CThemesAdapter::PolicyRequestResourceL( const TDesC8& /*aURI*/ )
	{
	if(!FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement ))
	{
	User::Leave(KErrNotSupported);
	}
	RDEBUG("CThemesAdapter::PolicyRequestResourceL(): begin");
	return PolicyEngineXACML::KCustomizationManagement();
	}
//#endif
// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
CThemesAdapter::TNodeId CThemesAdapter::NodeId( const TDesC8& aURI )
	{
	TNodeId id = ENodeUnknown;

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TInt cnt( NSmlDmURI::NumOfURISegs( uriPtrc ) );
	TPtrC8 app( NSmlDmURI::LastURISeg( uriPtrc ) );
#else
	TInt cnt( NSmlDmURI::NumOfURISegs( aURI ) );
	TPtrC8 app( NSmlDmURI::LastURISeg( aURI ) );
#endif

	if ( cnt == 1 )
		{
		// /Customization (G)
		if (app == KThemesAdapterCustomization)
			{
			id = ENodeCustomization;
			}
		}
	else if ( cnt == 2 ) 
		{
		// /Customization/Themes (G)
		if (app == KThemesAdapterThemes)
			{
			id = ENodeThemes;
			}
		}
	else if ( cnt == 3 ) 
		{
		// /Customization/Themes/AvailableThemes (AG)
		// /Customization/Themes/ActiveTheme (GR)
		if (app == KThemesAdapterAvailableThemes)
			{
			id = ENodeAvailableThemes;
			}
		else if (app == KThemesAdapterActiveTheme)
			{
			id = ENodeActiveTheme;
			}
		}
	else if (cnt == 4)
		{
		// /Customization/Themes/CurrentThemes/<X> (DGR)
		id = ENodeX;
		}
	else if (cnt == 5)
		{
		// /Customization/Themes/CurrentThemes/<X>/ID (G)
		// /Customization/Themes/CurrentThemes/<X>/Name (GR)
		// /Customization/Themes/CurrentThemes/<X>/Version (GR)
		// /Customization/Themes/CurrentThemes/<X>/Data (R)
		// /Customization/Themes/CurrentThemes/<X>/InstallOptions (R)
		if (app == KThemesAdapterXId)
			{
			id = ENodeXId;
			}
		else if (app == KThemesAdapterXName)
			{
			id = ENodeXName;
			}
		else if (app == KThemesAdapterXVersion)
			{
			id = ENodeXVersion;
			}
		else if (app == KThemesAdapterXData)
			{
			id = ENodeXData;
			}
		else if (app == KThemesAdapterXInstallOptions)
			{
			id = ENodeXInstallOptions;
			}
		}

	return id;
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] = 
	{
	{ {KThemesAdapterImplUid}, (TProxyNewLPtr)CThemesAdapter::NewL }
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
// CThemesAdapter::FillNodeInfoL()
// Fills the node info in ddf structure
// -------------------------------------------------------------------------------------
void CThemesAdapter::FillNodeInfoL( MSmlDmDDFObject& aNode,TSmlDmAccessTypes aAccTypes,
										MSmlDmDDFObject::TOccurence aOccurrence, MSmlDmDDFObject::TScope aScope, 
										MSmlDmDDFObject::TDFFormat aFormat,const TDesC8& aDescription,TBool /*aObjectGroup*/)
	{
	aNode.SetAccessTypesL(aAccTypes);
	aNode.SetOccurenceL(aOccurrence);
	aNode.SetScopeL(aScope);
	aNode.SetDFFormatL(aFormat);
	if(aFormat!=MSmlDmDDFObject::ENode)
		{
		aNode.AddDFTypeMimeTypeL(KThemesAdapterTextPlain);
		}
	aNode.SetDescriptionL(aDescription);
	}

// -------------------------------------------------------------------------------------
// CThemesAdapter::IsImage()
// Checks whether the given mime type is image format (that we support)
// -------------------------------------------------------------------------------------
TBool CThemesAdapter::IsImage(const TDesC& aMime) 
	{
	
	if (aMime == _L("image/jpeg") || aMime == _L("image/gif")) 
		{
			return ETrue;
		}
	return ETrue ; 
	//return EFalse;
	}
void CThemesAdapter::SyncWithAvkonSkinDB(CDesC8ArrayFlat& themes)
    {
    TInt i = 0;
    TPtrC8 pid;
    for (i=0; i<themes.Count(); i++)
        {
        HBufC8* themeRef = HBufC8::NewLC(KThemesAdapterActiveThemeRef().Length() + themes[i].Length() + 1);
        themeRef->Des().Copy(KThemesAdapterActiveThemeRef);
        themeRef->Des().Append(themes[i]);
        HBufC8* luid = iCallBack->GetLuidAllocL( *themeRef);
        CleanupStack::PushL(luid);

        if (luid->Length() > 1)
            {
            pid.Set( *luid);
            }
        CBufBase *themeName = CBufFlat::NewL( 128);
        CleanupStack::PushL(themeName);
        TInt ret = iThemes->GetThemeNameL(pid, *themeName);
        if (themeName->Size()== 0)
            {
            iCallBack->SetMappingL( *themeRef, KNullDesC8);
            }
        CleanupStack::PopAndDestroy(themeName);
        CleanupStack::PopAndDestroy(luid);
        CleanupStack::PopAndDestroy(themeRef);
        }
    }
