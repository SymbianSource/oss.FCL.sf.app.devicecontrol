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
* Description:  DM Screensaver Adapter
*
*/


/*

./Customization -+--> ScreenSaver --+--> AvailableScreenSavers --> <X> * ---+---> Data
									|										|
									|--> ActiveSc							|---> ID
									 										|
																			|---> Name
																			|
																			|---> Description
																			|
																			|---> CustomProperty
																			|
																			|---> Version
																			|
																			|---> InstallOptions

*/



#include <badesca.h>
#include <implementationproxy.h> // For TImplementationProxy definition
#include <sisregistrysession.h>
#include <ScreensaverpluginIntDef.h>
#include <mmfcontrollerpluginresolver.h> // For CleanupResetAndDestroyPushL

//#ifdef __SAP_POLICY_MANAGEMENT
#include <PolicyEngineXACML.h>
//#endif

#include "ScreenSaverAdapter.h"
#include "ScreenSaverList.h"
#include "nsmldmuri.h"
#include "FileCoderB64.h"
#include "debug.h"

#include "ScreenSaverClient.h"
#include "e32property.h"
#include <ScreensaverInternalPSKeys.h>
#include <featmgr.h>


_LIT( KScreenSaverAdapterTmpInstallPath, "c:\\system\\temp\\DM_ScrSav.sisx" );
const TInt KPostInstallTimeOut = 10000000; // 10 seconds
_LIT8( KScreenSaverActiveRef, "Customization/ScreenSavers/AvailableScreenSavers/" );

// ------------------------------------------------------------------------------------------------
// CScreenSaverAdapter* CScreenSaverAdapter::NewL( )
// ------------------------------------------------------------------------------------------------
CScreenSaverAdapter* CScreenSaverAdapter::NewL(MSmlDmCallback* aDmCallback )
	{
	CScreenSaverAdapter* self = NewLC( aDmCallback );
	CleanupStack::Pop();
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CScreenSaverAdapter* CScreenSaverAdapter::NewLC( )
// ------------------------------------------------------------------------------------------------
CScreenSaverAdapter* CScreenSaverAdapter::NewLC(MSmlDmCallback* aDmCallback )
	{
	CScreenSaverAdapter* self = new (ELeave) CScreenSaverAdapter( aDmCallback );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}


// ------------------------------------------------------------------------------------------------
// CScreenSaverAdapter::CScreenSaverAdapter()
// ------------------------------------------------------------------------------------------------

CScreenSaverAdapter::CScreenSaverAdapter( MSmlDmCallback* aDmCallback ) //: CSmlDmAdapter()
	: CTARMDmAdapter( aDmCallback )
	, iCallBack( aDmCallback )
	, iScreenSavers( 0 )
	{
	}

// ------------------------------------------------------------------------------------------------
// CScreenSaverAdapter::ConstructL()
// ------------------------------------------------------------------------------------------------
void CScreenSaverAdapter::ConstructL( )
	{
	
		TRAPD( errf, FeatureManager::InitializeLibL() );
		if( errf != KErrNone )
		{
			User::Leave(errf);
		}
		if(FeatureManager::FeatureSupported(KFeatureIdSapScreensaverAdapter))
		{
			iScreenSavers = CScreenSaverList::NewL();
			iScreensaverOff = EFalse;	
		}
		else
		{
			FeatureManager::UnInitializeLib();
	   		User::Leave( KErrNotSupported );
		}
	}

// ------------------------------------------------------------------------------------------------
// CScreenSaverAdapter::~CScreenSaverAdapter()
// ------------------------------------------------------------------------------------------------


CScreenSaverAdapter::~CScreenSaverAdapter()
	{
	TInt error;
	FeatureManager::UnInitializeLib();
	if (iScreensaverOff)
	{
		
		error = RProperty::Define(
        KPSUidScreenSaver,
        KScreenSaverAllowScreenSaver,
        RProperty::EInt,
        ECapabilityReadUserData,
        ECapabilityWriteUserData);
      if(error)  
  		  RDEBUG_2("CScreenSaverInfo::KPSUidScreenSaver defined error %d",error);
      		
   		error=RProperty::Set(KPSUidScreenSaver,KScreenSaverAllowScreenSaver,KEnableScreensaver);
   		if(error)
   		  RDEBUG_2("CScreenSaverInfo::KPSUidScreenSaver while setting error %d",error);
  	}

  	
  	RDEBUG_2("CScreenSaverInfo::KScreenSaverOn set to one error %d",error);	
	delete iScreenSavers;
	iScreenSavers = 0;
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverAdapter::DDFVersionL()
// ------------------------------------------------------------------------------------------------
void CScreenSaverAdapter::DDFVersionL(CBufBase& aDDFVersion)
	{
	aDDFVersion.InsertL(0,KNSmlSCDDFVersion);
	}


// ------------------------------------------------------------------------------------------------
//  CScreenSaverAdapter::DDFStructureL()
//
// ------------------------------------------------------------------------------------------------
void CScreenSaverAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
	{


	TSmlDmAccessTypes accessTypesGet;
	accessTypesGet.SetGet();

	TSmlDmAccessTypes accessTypesAddGet;
	accessTypesAddGet.SetAdd();
	accessTypesAddGet.SetGet();

	TSmlDmAccessTypes accessTypesGetReplace;
	accessTypesGetReplace.SetGet();
	accessTypesGetReplace.SetReplace();

	TSmlDmAccessTypes accessTypesReplace;
	accessTypesReplace.SetReplace();

	TSmlDmAccessTypes accessTypesGetReplaceDelete;
	accessTypesGetReplaceDelete.SetGet();
	accessTypesGetReplaceDelete.SetReplace();
	accessTypesGetReplaceDelete.SetDelete();
//sf- For testing:
	accessTypesGetReplaceDelete.SetAdd();
//sf- For testing ends.

	/*
	Node: ./Customization
	This interior node is the common parent to all customization..
	Status: Required
	Occurs: One
	Format: Node
	Access Types: Get
	Values: N/A
	*/
	MSmlDmDDFObject& customization = aDDF.AddChildObjectL(KNSmlCustomizationNodeName);
	FillNodeInfoL(customization, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KNSmlSCDescription);

	/*
	Node: ./Customization/ScreenSavers
	ScreenSavers is a common parent node for nodes managing screen savers. .
	Status: Required
	Occurs: One
	Format: Node
	Access Types: Add,Get
	Values: N/A
	*/
	MSmlDmDDFObject& screenSavers = customization.AddChildObjectL(KNSmlSCScreenSavers);
	FillNodeInfoL(screenSavers, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KNSmlSCScreenSaversNodeDescription);

	/*
	Node: ./Customization/ScreenSavers/AvailableScreenSavers
	This is parent node for currently existing screen savers..
	Status: Required
	Occurs: One
	Format: Node
	Access Types: Add, Get
	Values: N/A
	*/
	MSmlDmDDFObject& availableScreenSavers = screenSavers.AddChildObjectL(KNSmlSCAvailableScreenSaversNodeName);
	FillNodeInfoL(availableScreenSavers, accessTypesAddGet, MSmlDmDDFObject::EZeroOrMore, MSmlDmDDFObject::EDynamic,
		MSmlDmDDFObject::ENode, KNSmlSCAvailableScreenSaversNodeDescription);

	/*
	Node: ./Customization/ScreenSavers/AvailableScreenSavers/<X>
	This is a placeholder to screen saver ID identifying a screen saver.
	Status: Required
	Occurs: ZeroOrMore
	Format: Node
	Access Types: Get, Replace, Delete
	Values: N/A
	*/
	MSmlDmDDFObject& dynNode = availableScreenSavers.AddChildObjectGroupL();
	FillNodeInfoL(dynNode, accessTypesGetReplaceDelete, MSmlDmDDFObject::EZeroOrMore, MSmlDmDDFObject::EDynamic,
		MSmlDmDDFObject::ENode, KNSmlSCDynamicDescription);

	/*
	Node: ./Customization/ScreenSavers/ActiveSc
	This node is a link to the currently active screen saver.
	Status: Required
	Occurs: One
	Format: Chr (reference to management tree)
	Access Types: Get, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& activescNode = screenSavers.AddChildObjectL(KNSmlSCActiveScNodeName);
	FillNodeInfoL(activescNode,accessTypesGetReplace,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr,KNSmlSCActiveScNodeDescription);

	/*
	Node: ./Customization/ScreenSavers/AvailableScreenSavers/<X>/Data
	Replace command will install a screen saver that's in the data field of the command.
	Status: Optional
	Occurs: One
	Format: bin
	Access Types: Replace
	Values: N/A
	*/
	MSmlDmDDFObject& data = dynNode.AddChildObjectL(KNSmlSCDataNodeName);
	FillNodeInfoL(data, accessTypesReplace, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EBin, KNSmlSCDataNodeDescription);
	data.AddDFTypeMimeTypeL( SwiUI::KSisxMimeType() );

	/*
	Node: ./Customization/ScreenSavers/AvailableScreenSavers/<X>/ID
	This leaf node holds the ID of the screen saver
	Status: Required
	Occurs: One
	Format: chr
	Access Types: Get, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& idNode = dynNode.AddChildObjectL(KNSmlSCIDNodeName);
	FillNodeInfoL(idNode,accessTypesGetReplace,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EDynamic,
		MSmlDmDDFObject::EChr,KNSmlSCIDNodeDescription);

	/*
	Node: ./Customization/ScreenSavers/AvailableScreenSavers/<X>/Name
	This leaf node holds the name of the screen saver. 
	Status: Optional
	Occurs: One
	Format: chr
	Access Types: Get, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& nameNode = dynNode.AddChildObjectL(KNSmlSCNameNodeName);
	FillNodeInfoL(nameNode,accessTypesGetReplace,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EDynamic,
		MSmlDmDDFObject::EChr,KNSmlSCNameNodeDescription);

	/*
	Node: ./Customization/ScreenSavers/AvailableScreenSavers/<X>/Description
	This leaf node holds the description of the screen saver. 
	Status: Optional
	Occurs: One
	Format: chr
	Access Types: Get, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& descNode = dynNode.AddChildObjectL(KNSmlSCDescriptionNodeName);
	FillNodeInfoL(descNode,accessTypesGetReplace,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EDynamic,
		MSmlDmDDFObject::EChr,KNSmlSCDescriptionNodeDescription);

	/*
	Node: ./Customization/ScreenSavers/AvailableScreenSavers/<X>/CustomProperty
	This leaf node holds the custom property value of the screen saver. 
	Status: Optional
	Occurs: One
	Format: chr
	Access Types: Get, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& customNode = dynNode.AddChildObjectL(KNSmlSCCustomPropertyNodeName);
	FillNodeInfoL(customNode,accessTypesGetReplace,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EDynamic,
		MSmlDmDDFObject::EChr,KNSmlSCCustomPropertyNodeDescription);

	/*
	Node: ./Customization/ScreenSavers/AvailableScreenSavers/<X>/Version
	This leaf node holds the version of the screen saver. 
	Status: Optional
	Occurs: One
	Format: chr
	Access Types: Get, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& verNode = dynNode.AddChildObjectL(KNSmlSCVersionNodeName);
	FillNodeInfoL(verNode,accessTypesGetReplace,MSmlDmDDFObject::EOne,MSmlDmDDFObject::EDynamic,
		MSmlDmDDFObject::EChr,KNSmlSCVersionNodeDescription);

	/*
	Node: ./Customization/ScreenSavers/AvailableScreenSavers/<X>/InstallOptions
	Holds the install options for next install. 
	Status: Optional 
	Occurs: One
	Format: Chr
	Access Types: Replace
	Values: N/A
	*/
	MSmlDmDDFObject& installOptionsNode = dynNode.AddChildObjectL(KNSmlSCInstallOptionsNodeName);
	FillNodeInfoL(installOptionsNode, accessTypesReplace, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KNSmlSCInstallOptionsNodeDescription);




	}


// ------------------------------------------------------------------------------------------------
//  CScreenSaverAdapter::UpdateLeafObjectL()
//  
// ------------------------------------------------------------------------------------------------
void CScreenSaverAdapter::_UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& /*aLUID*/, const TDesC8& aObject, const TDesC8& /*aType*/, const TInt aStatusRef )
	{
	RDEBUG("CScreenSaverAdapter::UpdateLeafObjectL(): begin");

	TError ret = EOk ;
	
#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TNodeId nodeId = NodeId( uriPtrc );
	TPtrC8 parentURI = NSmlDmURI::ParentURI( uriPtrc );
#else
	TNodeId nodeId = NodeId( aURI );
	TPtrC8 parentURI = NSmlDmURI::ParentURI( aURI );
#endif	
	
	TPtrC8 screenSaverId = NSmlDmURI::LastURISeg( parentURI );
	HBufC8* parentLUID = iCallBack->GetLuidAllocL( parentURI );
	CleanupStack::PushL( parentLUID );
	
	switch( nodeId )
		{
	case ENodeActiveSc:
		{
		TPtrC8 activeName = NSmlDmURI::LastURISeg( aObject );

		HBufC8* fullName = HBufC8::NewLC( KScreenSaverActiveRef().Length() + activeName.Length() + 1 );
		fullName->Des().Copy( KScreenSaverActiveRef );
		fullName->Des().Append( activeName );
		HBufC8* luid = iCallBack->GetLuidAllocL( *fullName );
		CleanupStack::PushL( luid );

		ret = iScreenSavers->SetActiveL( *luid  );
		if( ret == MSmlDmAdapter::EOk )
			{
			iScreensaverOff = ETrue;
#ifdef __TARM_SYMBIAN_CONVERGENCY
			iCallBack->SetMappingL( uriPtrc, *fullName );
#else
			iCallBack->SetMappingL( aURI, *fullName );
#endif			
			}
		
		CleanupStack::PopAndDestroy( luid );
		CleanupStack::PopAndDestroy( fullName );
		break;
		}
	case ENodeData:
		{
		RFs fs;
		User::LeaveIfError( fs.Connect() );
		CleanupClosePushL( fs );

		TFileCoderB64 decoder;
		TPtrC fileName( KScreenSaverAdapterTmpInstallPath );
		TBool isEncoded = TFileCoderB64::CheckB64Encode( aObject );
		if( isEncoded )
			{
			decoder.DecodeToL( aObject, fileName );
			}
		else
			{
			// aObject is original data, save it to fileName
			RFile file;
			User::LeaveIfError( file.Replace( fs, fileName, EFileWrite));
			CleanupClosePushL( file );
			User::LeaveIfError( file.Write( aObject ) );
			
			CleanupStack::PopAndDestroy(); // file
			}		

		// Get current status of installed packages and screen savers
		RArray< TUid > oldPackages;
		CleanupClosePushL( oldPackages );

		RArray< TUid > newPackages;
		CleanupClosePushL( newPackages );

		CDesCArrayFlat* oldScreenSavers = new (ELeave) CDesCArrayFlat( 8 );
		CleanupStack::PushL( oldScreenSavers );

		CDesCArrayFlat* newScreenSavers = new (ELeave) CDesCArrayFlat( 8 );
		CleanupStack::PushL( newScreenSavers );

		InstalledPackagesL( oldPackages );
		InstalledScreenSaversL( *oldScreenSavers );
		
		
		  
		User::ResetInactivityTime(); 	
		TInt  error = RProperty::Define(
        KPSUidScreenSaver,
        KScreenSaverAllowScreenSaver,
        RProperty::EInt,
        ECapabilityReadUserData,
        ECapabilityWriteUserData);
        
	 	error=RProperty::Set(KPSUidScreenSaver,KScreenSaverAllowScreenSaver,KDisableScreensaver);
		iScreensaverOff = ETrue;
		// Open ECom session and setup notifier
		REComSession& ecom = REComSession::OpenL();
		CleanupClosePushL( ecom );
		TRequestStatus ecomStatus;

		// Install screen saver
		ret = iScreenSavers->InstallL( fileName );

		ecom.NotifyOnChange( ecomStatus );

		// Wait until ECom server finds new plugin
		if (ret == EOk)
			{
			TRequestStatus timeoutStatus;
			RTimer timeout;
			User::LeaveIfError( timeout.CreateLocal() );
			CleanupClosePushL( timeout );
			timeout.After( timeoutStatus, KPostInstallTimeOut );

			User::WaitForRequest( ecomStatus, timeoutStatus );

			if( timeoutStatus.Int() == KRequestPending)
				{
				timeout.Cancel();
				User::WaitForRequest( timeoutStatus );
				}
			CleanupStack::PopAndDestroy( &timeout );
			}

		// Close ECom session
		if (ecomStatus.Int() == KRequestPending)
			{
			ecom.CancelNotifyOnChange( ecomStatus );
			User::WaitForRequest( ecomStatus );
			}
		CleanupStack::PopAndDestroy( &ecom );
			
		// Find out which package and screen savers where installed
		InstalledPackagesL( newPackages );
		InstalledScreenSaversL( *newScreenSavers );

		TInt count = 0;
		TInt i = 0;

		count = oldPackages.Count();
		for (i = 0 ; i < count ; i++)
			{
			TInt idx = newPackages.FindInSignedKeyOrder( oldPackages[ i ] );
			if (idx >= 0)
				{
				newPackages.Remove( idx );
				}
			}

		count = oldScreenSavers->Count();
		for (i = 0 ; i < count ; i++)
			{
			TInt idx = 0;
			TInt err = newScreenSavers->Find( (*oldScreenSavers)[ i ], idx );
			if (err == 0)
				{
				newScreenSavers->Delete( idx );
				}
			}

		RDEBUG_3("newPackages.Count() (%d) newScreenSavers->Count() (%d)", newPackages.Count(), newScreenSavers->Count());

		// Save package uid to DMTree with LUID mapping
		// We can handle only situation where only one new package is found.
		if (newPackages.Count() == 1 && newScreenSavers->Count() > 0)
			{
			TUid pkgUid = newPackages[ 0 ];

			HBufC8* luid = HBufC8::NewL( KMaxUidName + 1 + (*newScreenSavers)[0].Length() );
			CleanupStack::PushL( luid );
			TPtr8 luidPtr = luid->Des();
			luidPtr.Copy( pkgUid.Name() );
			luidPtr.Append( KPkgId_ScrId_Separator );
			luidPtr.Append( (*newScreenSavers)[0] );
			
			RDEBUG8_2("CScreenSaverAdapter::UpdateLeafObjectL() Luid for new screensaver (%S)", &luidPtr);
			iCallBack->SetMappingL( parentURI, *luid );

			CleanupStack::PopAndDestroy( luid );
			}
		else
			{
			ret = EError;
			}
			
		// Remove temporary installations file
		fs.Delete( fileName );

		// Cleanup
		CleanupStack::PopAndDestroy( newScreenSavers );
		CleanupStack::PopAndDestroy( oldScreenSavers );
		CleanupStack::PopAndDestroy( &newPackages );
		CleanupStack::PopAndDestroy( &oldPackages );
		CleanupStack::PopAndDestroy( &fs );
		}
		break;

	case ENodeInstallOptions:
		ret = iScreenSavers->SetInstallOptionsL( aObject );
		break;

	case ENodeName:
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

		if (parentLUID->Length() > 1)
			{
			screenSaverId.Set( *parentLUID );
			ret = iScreenSavers->SetNameL( screenSaverId, aObject );
			}
		else
			{
			ret = EError;
			}
		break;

	case ENodeDescription:
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

		if (parentLUID->Length() > 1)
			{
			screenSaverId.Set( *parentLUID );
			ret = iScreenSavers->SetDescriptionL( screenSaverId, aObject );
			}
		else
			{
			ret = EError;
			}
		
		break;

	case ENodeCustomProperty:
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

		if (parentLUID->Length() > 1)
			{
			screenSaverId.Set( *parentLUID );
			ret = iScreenSavers->SetCustomPropertyL( screenSaverId, aObject );
			}
		else
			{
			ret = EError;
			}
		
		break;

	case ENodeVersion:
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

		if (parentLUID->Length() > 1)
			{
			screenSaverId.Set( *parentLUID );
			ret = iScreenSavers->SetVersionL( screenSaverId, aObject );
			}
		else
			{
			ret = EError;
			}
		
		break;

	case ENodeUnknown:
		ret = ENotFound;
		break;

	default:
		ret = EError;
		break;
		}

	CleanupStack::PopAndDestroy( parentLUID );

	iCallBack->SetStatusL( aStatusRef, ret );
	RDEBUG("CScreenSaverAdapter::UpdateLeafObjectL(): end");
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CScreenSaverAdapter::_UpdateLeafObjectL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/, RWriteStream*& /*aStream*/, const TDesC8& /*aType*/, TInt aStatusRef )
	{
	RDEBUG( "CScreenSaverAdapter::UpdateLeafObjectL(...RWriteStream...): begin" );
	//Not supported
	TError ret = EError;
	iCallBack->SetStatusL( aStatusRef, ret );
	}


// ------------------------------------------------------------------------------------------------
// CScreenSaverAdapter::DeleteObjectL( const TDesC& aURI, const TDesC& aLUID )
// ------------------------------------------------------------------------------------------------
void CScreenSaverAdapter::_DeleteObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TInt aStatusRef )
	{
	RDEBUG( "CScreenSaverAdapter::DeleteObjectL(): begin" );
	TError ret = EOk ;
	
#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TNodeId nodeId = NodeId( uriPtrc );
	TPtrC8 screenSaverId = NSmlDmURI::LastURISeg( uriPtrc );
#else
	TNodeId nodeId = NodeId( aURI );
	TPtrC8 screenSaverId = NSmlDmURI::LastURISeg( aURI );
#endif	

	switch( nodeId )
		{
	case ENodeAvailableScreenSaversX:
		{
#ifdef __TARM_SYMBIAN_CONVERGENCY	
		TPtrC8 parentURI = NSmlDmURI::ParentURI( uriPtrc );
#else	
		TPtrC8 parentURI = NSmlDmURI::ParentURI( aURI );
#endif	
		HBufC8* parentLUID = iCallBack->GetLuidAllocL( parentURI );
		CleanupStack::PushL( parentLUID );

		// Set default return code to ENotFound in case we can't event try to delete package
		ret = ENotFound;

		if (aLUID != *parentLUID)
			{
			ret = iScreenSavers->DeleteL( aLUID );
			}

		CleanupStack::PopAndDestroy( parentLUID );
		}
		break;

	case ENodeUnknown:
		ret = ENotFound;
		break;

	default:
		ret = EError;
		break;
		}

	iCallBack->SetStatusL( aStatusRef, ret );
	RDEBUG( "CScreenSaverAdapter::DeleteObjectL(): end" );
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverAdapter::GetLeafObjectL()
// ------------------------------------------------------------------------------------------------
void CScreenSaverAdapter::GetLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& /*aType*/, CBufBase& aResult, TError& aRet )
	{	
#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TNodeId nodeId = NodeId( uriPtrc );
	TPtrC8 parentURI = NSmlDmURI::ParentURI( uriPtrc );
#else
	TNodeId nodeId = NodeId( aURI );
	TPtrC8 parentURI = NSmlDmURI::ParentURI( aURI );
#endif		
	
	TPtrC8 screenSaverId = NSmlDmURI::LastURISeg( parentURI );

	HBufC8* parentLUID = iCallBack->GetLuidAllocL( parentURI );
	CleanupStack::PushL( parentLUID );

	HBufC8* trimmedLUID = aLUID.AllocL();
	CleanupStack::PushL( trimmedLUID );
	trimmedLUID->Des().Trim();

	switch( nodeId )
		{
	case ENodeActiveSc:
		{
#ifdef __TARM_SYMBIAN_CONVERGENCY
		HBufC8* luid = iCallBack->GetLuidAllocL( uriPtrc );
#else
		HBufC8* luid = iCallBack->GetLuidAllocL( aURI );
#endif		
		CleanupStack::PushL( luid );
		aResult.InsertL( aResult.Size(), *luid );
		CleanupStack::PopAndDestroy( luid );
		}
		break;

	case ENodeID:
		{
		if (parentLUID->Length() > 1)
			{
			screenSaverId.Set( *parentLUID );
			}
		aRet = iScreenSavers->GetIdL( screenSaverId, aResult );
		}
		break;

	case ENodeName:
		if ( trimmedLUID->Length() > 0 && *trimmedLUID != *parentLUID)
			{
			aResult.InsertL( aResult.Size(), aLUID );
			}
		else
			{
			if (parentLUID->Length() > 1)
				{
				screenSaverId.Set( *parentLUID );
				}
			aRet = iScreenSavers->GetNameL( screenSaverId, aResult );
			}
		break;

	case ENodeDescription:
		if ( trimmedLUID->Length() > 0 && *trimmedLUID != *parentLUID)
			{
			aResult.InsertL( aResult.Size(), aLUID );
			}
		else
			{
			if (parentLUID->Length() > 1)
				{
				screenSaverId.Set( *parentLUID );
				}
			aRet = iScreenSavers->GetDescriptionL( screenSaverId, aResult );
			}
		break;

	case ENodeCustomProperty:
		if ( trimmedLUID->Length() > 0 && *trimmedLUID != *parentLUID)
			{
			aResult.InsertL( aResult.Size(), aLUID );
			}
		else
			{
			if (parentLUID->Length() > 1)
				{
				screenSaverId.Set( *parentLUID );
				}
			aRet = iScreenSavers->GetCustomPropertyL( screenSaverId, aResult );
			}
		break;

	case ENodeVersion:
		if ( trimmedLUID->Length() > 0 && *trimmedLUID != *parentLUID)
			{
			aResult.InsertL( aResult.Size(), aLUID );
			}
		else
			{
			if (parentLUID->Length() > 1)
				{
				screenSaverId.Set( *parentLUID );
				}
			aRet = iScreenSavers->GetVersionL( screenSaverId, aResult );
			}
		break;

	case ENodeUnknown:
		aRet = ENotFound;
		break;

	default:
		aRet = EError;
		break;
		}

	CleanupStack::PopAndDestroy( trimmedLUID );
	CleanupStack::PopAndDestroy( parentLUID );
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverAdapter::FetchLeafObjectL( const TDesC& aURI, const TDesC& aLUID, const TDesC& aType, CBufBase& aObject )
// ------------------------------------------------------------------------------------------------
void CScreenSaverAdapter::_FetchLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aType, const TInt aResultsRef, const TInt aStatusRef )
	{
	RDEBUG("CScreenSaverAdapter::FetchLeafObjectL(): begin");
	TError ret = EOk ;

	CBufBase *result = CBufFlat::NewL( 128 );
	CleanupStack::PushL( result );

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	GetLeafObjectL( uriPtrc, aLUID, aType, *result, ret );
#else
	GetLeafObjectL( aURI, aLUID, aType, *result, ret );
#endif

	if( ret == EOk )
		{
		iCallBack->SetResultsL( aResultsRef, *result, KNullDesC8 );
		}

	iCallBack->SetStatusL( aStatusRef, ret );

	CleanupStack::PopAndDestroy( result );
	RDEBUG("CScreenSaverAdapter::FetchLeafObjectL(): end");
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CScreenSaverAdapter::_FetchLeafObjectSizeL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aType, TInt aResultsRef, TInt aStatusRef )
	{
	RDEBUG("CScreenSaverAdapter::FetchLeafObjectSizeL(): begin");
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

		iCallBack->SetResultsL( aResultsRef, *result, KNullDesC8() );
		}

	iCallBack->SetStatusL( aStatusRef, ret );

	CleanupStack::PopAndDestroy( result );
	RDEBUG("CScreenSaverAdapter::FetchLeafObjectSizeL(): end");
	}

// ------------------------------------------------------------------------------------------------
//  CScreenSaverAdapter::ChildURIListL( const TDesC& aURI, const TDesC& aParentLUID, const CArrayFix<TNSmlDmMappingInfo>& aPreviousURISegmentList, CArrayFix<TPtrC>& aCurrentURISegmentList )
// ------------------------------------------------------------------------------------------------
void CScreenSaverAdapter::_ChildURIListL( const TDesC8& aURI, const TDesC8& /*aParentLUID*/, const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, const TInt aResultsRef, const TInt aStatusRef )
	{
	RDEBUG("CScreenSaverAdapter::ChildURIListL(): begin");
	TError ret = EOk;

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TNodeId nodeId = NodeId( uriPtrc );
#else
	TNodeId nodeId = NodeId( aURI );
#endif	

	CBufBase *result = CBufFlat::NewL( 128 );
	CleanupStack::PushL( result );

	switch( nodeId )
		{
	case ENodeCustomization:
		result->InsertL( result->Size(), KNSmlSCScreenSavers() );
		break;
	case ENodeScreenSavers:
		result->InsertL( result->Size(), KNSmlSCAvailableScreenSaversNodeName() );
		result->InsertL( result->Size(), KSlash() );
		result->InsertL( result->Size(), KNSmlSCActiveScNodeName() );
		break;
	case ENodeAvailableScreenSavers:
		ret = iScreenSavers->GetScreenSaverListL( aPreviousURISegmentList, *result );
		break;
	case ENodeActiveSc:
		break;
	case ENodeInstallOptions:
		break;
	case ENodeAvailableScreenSaversX:
		result->InsertL( result->Size(), KNSmlSCDataNodeName() );
		result->InsertL( result->Size(), KSlash() );
		result->InsertL( result->Size(), KNSmlSCIDNodeName() );
		result->InsertL( result->Size(), KSlash() );
		result->InsertL( result->Size(), KNSmlSCNameNodeName() );
		result->InsertL( result->Size(), KSlash() );
		result->InsertL( result->Size(), KNSmlSCDescriptionNodeName() );
		result->InsertL( result->Size(), KSlash() );
		result->InsertL( result->Size(), KNSmlSCCustomPropertyNodeName() );
		result->InsertL( result->Size(), KSlash() );
		result->InsertL( result->Size(), KNSmlSCVersionNodeName() );
		result->InsertL( result->Size(), KSlash() );
		result->InsertL( result->Size(), KNSmlSCInstallOptionsNodeName() );
		break;
	case ENodeData:
		break;
	case ENodeID:
		break;
	case ENodeName:
		break;
	case ENodeDescription:
		break;
	case ENodeCustomProperty:
		break;
	case ENodeVersion:
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
		iCallBack->SetResultsL( aResultsRef, *result, KNullDesC8 );
		}

	iCallBack->SetStatusL( aStatusRef, ret );

	CleanupStack::PopAndDestroy( result );
	RDEBUG("CScreenSaverAdapter::ChildURIListL(): end");
	}


// ------------------------------------------------------------------------------------------------
// CScreenSaverAdapter::AddNodeObjectL( const TDesC& aURI, const TDesC& aParentLUID )
// ------------------------------------------------------------------------------------------------
void CScreenSaverAdapter::_AddNodeObjectL( const TDesC8& aURI, const TDesC8& /*aParentLUID*/, const TInt aStatusRef )
	{
	RDEBUG("CScreenSaverAdapter::AddNodeObjectL(): begin");
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
	case ENodeAvailableScreenSavers:
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
	RDEBUG("CScreenSaverAdapter::AddNodeObjectL(): end");
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CScreenSaverAdapter::_ExecuteCommandL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/, const TDesC8& /*aArgument*/, const TDesC8& /*aType*/, TInt aStatusRef )
	{
	RDEBUG("CScreenSaverAdapter::ExecuteCommandL(): begin");
	// Not supported for any node
	CSmlDmAdapter::TError ret = CSmlDmAdapter::EError;
	iCallBack->SetStatusL(aStatusRef, ret);
	RDEBUG("CScreenSaverAdapter::ExecuteCommandL(): end");
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CScreenSaverAdapter::_ExecuteCommandL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/, RWriteStream*& /*aStream*/, const TDesC8& /*aType*/, TInt aStatusRef )
	{
	RDEBUG("CScreenSaverAdapter::ExecuteCommandL(...RWriteStream...): begin");
	//Not supported
	TError ret = EError;
	iCallBack->SetStatusL( aStatusRef, ret );
	}
	
// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CScreenSaverAdapter::_CopyCommandL( const TDesC8& /*aTargetURI*/, const TDesC8& /*aTargetLUID*/, const TDesC8& /*aSourceURI*/, const TDesC8& /*aSourceLUID*/, const TDesC8& /*aType*/, TInt aStatusRef )
	{
	RDEBUG("CScreenSaverAdapter::CopyCommandL(): begin");
	//Not supported
	TError ret = EError;
	iCallBack->SetStatusL( aStatusRef, ret );
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CScreenSaverAdapter::StartAtomicL()
	{
	RDEBUG("CScreenSaverAdapter::StartAtomicL(): begin");
	//Not supported
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CScreenSaverAdapter::CommitAtomicL()
	{
	RDEBUG("CScreenSaverAdapter::CommitAtomicL(): begin");
	//Not supported
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CScreenSaverAdapter::RollbackAtomicL()
	{
	RDEBUG("CScreenSaverAdapter::RollbackAtomicL(): begin");
	//Not supported
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
TBool CScreenSaverAdapter::StreamingSupport( TInt& /*aItemSize*/ )
	{
	RDEBUG("CScreenSaverAdapter::StreamingSupport(): begin");
	return EFalse;
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
#ifdef __TARM_SYMBIAN_CONVERGENCY	
void CScreenSaverAdapter::StreamCommittedL( RWriteStream& /*aStream*/ )
#else
void CScreenSaverAdapter::StreamCommittedL()
#endif	
	{	
	RDEBUG("CScreenSaverAdapter::StreamCommittedL(): begin");
	RDEBUG("CScreenSaverAdapter::StreamCommittedL(): end");
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CScreenSaverAdapter::CompleteOutstandingCmdsL()
	{
	RDEBUG("CScreenSaverAdapter::CompleteOutstandingCmdsL(): begin");
	}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
//#ifdef __SAP_POLICY_MANAGEMENT
TPtrC8 CScreenSaverAdapter::PolicyRequestResourceL( const TDesC8& /*aURI*/ )
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
CScreenSaverAdapter::TNodeId CScreenSaverAdapter::NodeId( const TDesC8& aURI )
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

/*
./Customization -+--> ScreenSaver --+----> CurrentScreenSavers --> <X> * ---+-----> ID
									|										|
									|----> ActiveSc							|-----> Name
									|										|
									|----> InstallScreenSaver				|-----> Description
									|										|
									|----> InstallOptions					|-----> CustomProperty
																			|
																			|-----> Version
																																
*/

	if ( cnt == 1 )
		{
		if (app == KNSmlCustomizationNodeName)
			{
			id = ENodeCustomization;
			}
		}
	else if ( cnt == 2 ) 
		{
		if (app == KNSmlSCScreenSavers)
			{
			id = ENodeScreenSavers;
			}
		}
	else if ( cnt == 3 ) 
		{
		if (app == KNSmlSCAvailableScreenSaversNodeName)
			{
			id = ENodeAvailableScreenSavers;
			}
		else if (app == KNSmlSCActiveScNodeName)
			{
			id = ENodeActiveSc;
			}
		}
	else if ( cnt == 4 ) 
		{
		id = ENodeAvailableScreenSaversX;
		}
	else if ( cnt == 5 ) 
		{
		if (app == KNSmlSCDataNodeName)
			{
			id = ENodeData;
			}
		else if (app == KNSmlSCIDNodeName)
			{
			id = ENodeID;
			}
		else if (app == KNSmlSCNameNodeName)
			{
			id = ENodeName;
			}
		else if (app == KNSmlSCDescriptionNodeName)
			{
			id = ENodeDescription;
			}
		else if (app == KNSmlSCCustomPropertyNodeName)
			{
			id = ENodeCustomProperty;
			}
		else if (app == KNSmlSCVersionNodeName)
			{
			id = ENodeVersion;
			}
		else if (app == KNSmlSCInstallOptionsNodeName)
			{
			id = ENodeInstallOptions;
			}
		}

	return id;
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] = 
	{
	{ {KNSmlDMScreenSaverAdapterImplUid}, (TProxyNewLPtr)CScreenSaverAdapter::NewL }
	};

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

// -------------------------------------------------------------------------------------
// CScreenSaverAdapter::FillNodeInfoL()
// Fills the node info in ddf structure
// -------------------------------------------------------------------------------------
void CScreenSaverAdapter::FillNodeInfoL( MSmlDmDDFObject& aNode,TSmlDmAccessTypes aAccTypes,
										MSmlDmDDFObject::TOccurence aOccurrence, MSmlDmDDFObject::TScope aScope, 
										MSmlDmDDFObject::TDFFormat aFormat,const TDesC8& aDescription )
	{
	aNode.SetAccessTypesL(aAccTypes);
	aNode.SetOccurenceL(aOccurrence);
	aNode.SetScopeL(aScope);
	aNode.SetDFFormatL(aFormat);
	if(aFormat!=MSmlDmDDFObject::ENode)
		{
		aNode.AddDFTypeMimeTypeL(KNSmlSCTextPlain);
		}
	aNode.SetDescriptionL(aDescription);
	}

// -------------------------------------------------------------------------------------
// CScreenSaverAdapter::IsImage()
// Checks whether the given mime type is image format (that we support)
// -------------------------------------------------------------------------------------
TBool CScreenSaverAdapter::IsImage(const TDesC8& aMime) 
	{
	
	if (aMime == _L8("image/jpeg") || aMime == _L8("image/gif")) 
		{
			return ETrue;
		}
	return EFalse;
	}

// -------------------------------------------------------------------------------------
// CScreenSaverAdapter::
// -------------------------------------------------------------------------------------
void CScreenSaverAdapter::InstalledPackagesL( RArray< TUid >& aUids )
	{
	Swi::RSisRegistrySession reg;
	User::LeaveIfError( reg.Connect() );
	CleanupClosePushL( reg );

	reg.InstalledUidsL( aUids );
	aUids.SortSigned( );

	CleanupStack::PopAndDestroy( &reg );
	}



// -------------------------------------------------------------------------------------
// CScreenSaverAdapter::
// -------------------------------------------------------------------------------------
void CScreenSaverAdapter::InstalledScreenSaversL( CDesCArray& aIds )
	{
	RDEBUG("	-> CScreenSaverAdapter: GetScreenSaverList ... START!");
	
	RScreenSaverClient ssClient;
	User::LeaveIfError( ssClient.Connect() );
	CleanupClosePushL( ssClient );
	ssClient.GetScreenSaverListL(aIds );
	CleanupStack::PopAndDestroy( &ssClient );
	
	RDEBUG("	-> CScreenSaverAdapter: GetScreenSaverList ... END!");
	}


