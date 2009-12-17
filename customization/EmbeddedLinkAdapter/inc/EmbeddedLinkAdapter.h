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
* Description:  DM Email Settings Adapter
*
*/



#ifndef __NSMLEMBEDDEDLINKADAPTER_H__
#define __NSMLEMBEDDEDLINKADAPTER_H__


#include "TARMDmAdapter.h"
#include "EmbeddedLinkApp.h"

const TUint KNSmlEmbeddedLinkAdapterImplUid = 0x1020782C;

// the DDF version must be changed if any changes in DDF structure ( built in DDFStructureL() function )
_LIT8( KNSmlEmbeddedLinkAdapterDDFVersion, "1.0" ); 

_LIT8( KNSmlEmbeddedLinkAdapterTextPlain, "text/plain" );
_LIT8( KNSmlEmbeddedLinkAdapterNodeName, "Apps" );
_LIT8( KNSmlEmbeddedLinkEmbeddedLinks,"EmbeddedLinks") ;
_LIT8( KNSmlEmbeddedLinkCustomization, "Customization");
_LIT8( KNSmlEmbeddedLinkAdapterShutdown,"reboot") ;
_LIT8( KNSmlEmbeddedLinkAdapterDescription, "The interior object holds all apps objects" );
_LIT8( KNSmlEmbeddedLinkAdapterDynamicNodeDescription, "Placeholder for one or more Group objects" );
_LIT8( KNSmlEmbeddedLinkAdapterDynamicNode2Description, "Placeholder for one or more Vendor objects" );
_LIT8( KNSmlEmbeddedLinkAdapterDynamicNode3Description, "Placeholder for one or more Package objects" );
_LIT8( KNSmlEmbeddedLinkAdapterPkgDataDescription, "The Data associated with the DM tree application package" );
_LIT8( KNSmlEmbeddedLinkAdapterPkgNameDescription, "The Name associated with the DM tree application package" );
_LIT8( KNSmlEmbeddedLinkAdapterPkgVersionDescription, "Version associated with the DM tree application package" );
_LIT8( KNSmlEmbeddedLinkAdapterPkgMimeTypeDescription, "Mime type associated with the DM tree application package" );
_LIT8( KNSmlEmbeddedLinkAdapterIsInstalledDescription, "IsInstalled status associated with the DM tree application package" );
_LIT8( KNSmlEmbeddedLinkAdapterIsRunningDescription, "IsRunning status associated with the DM tree application package" );
_LIT8( KNSmlEmbeddedLinkAdapterPkgVendorDescription, "name of the vendor associated with the DM tree application package" );
_LIT8( KNSmlEmbeddedLinkAdapterInstallationDateDescription, "InstallationDate associated with the DM tree application package" );
_LIT8( KNSmlEmbeddedLinkAdapterInstallationPathDescription, "InstallationPath associated with the DM tree application package" );

_LIT8( KNSmlEmbeddedLinkDynamicNode, "" );
_LIT8( KNSmlEmbeddedLinkLink, "Links");
_LIT8( KNSmlEmbeddedLinkURL, "URL");
_LIT8( KNSmlEmbeddedLinkName, "Name");
_LIT8( KNSmlEmbeddedLinkUserName, "UserName" );
_LIT8( KNSmlEmbeddedLinkPassword, "Password" );
_LIT8( KNSmlEmbeddedLinkConfRef, "ConRef");
_LIT8( KNSmlEmbeddedLinkID, "ID" );
_LIT8( KNSmlEmbeddedLinkAppName, "AppName" );

_LIT8( KLinkLeafs8, "URL/Name/UserName/Password/ConRef" );
_LIT8( KRootLeafs8, "EmbeddedLinks" );
_LIT8( KAppLeafs8, "Links/ID/AppName" );

// WapAp strings
_LIT8( KWapApDefault, "" );
_LIT8( KWapApNull, "null" );


class CEmbeddedLinkHandler;

// ------------------------------------------------------------------------------------------------
// CNSmlDmEmailAdapter 
// ------------------------------------------------------------------------------------------------
class CNSmlEmbeddedLinkAdapter : public CTARMDmAdapter
	{
public:
	static CNSmlEmbeddedLinkAdapter* NewL(MSmlDmCallback* aDmCallback );
	static CNSmlEmbeddedLinkAdapter* NewLC(MSmlDmCallback* aDmCallback );

	virtual ~CNSmlEmbeddedLinkAdapter();

	// Implementation of MSmlDmAdapter interface
	// =========================================
	void DDFVersionL( CBufBase& aVersion );
	void DDFStructureL( MSmlDmDDFObject& aDDF );
	
	void _UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aObject, const TDesC8& aType, TInt aStatusRef );
	void _UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, RWriteStream*& aStream, const TDesC8& aType, TInt aStatusRef );
	void _DeleteObjectL( const TDesC8& aURI, const TDesC8& aLUID, TInt aStatusRef );
	void _FetchLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aType, TInt aResultsRef, TInt aStatusRef );
	void _FetchLeafObjectSizeL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aType, TInt aResultsRef, TInt aStatusRef );
	void _ChildURIListL( const TDesC8& aURI, const TDesC8& aLUID, const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, TInt aResultsRef, TInt aStatusRef );
	
	void _AddNodeObjectL( const TDesC8& aURI, const TDesC8& aParentLUID, TInt aStatusRef );
	void _ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aArgument, const TDesC8& aType, TInt aStatusRef );
	void _ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, RWriteStream*& aStream, const TDesC8& aType, TInt aStatusRef );
	void _CopyCommandL( const TDesC8& aTargetURI, const TDesC8& aTargetLUID, const TDesC8& aSourceURI, const TDesC8& aSourceLUID, const TDesC8& aType, TInt aStatusRef );

	void StartAtomicL();
	void CommitAtomicL();
	void RollbackAtomicL();
	TBool StreamingSupport( TInt& aItemSize );
#ifdef __TARM_SYMBIAN_CONVERGENCY	
		virtual void StreamCommittedL( RWriteStream& aStream );
#else
		virtual void StreamCommittedL();
#endif	
	void CompleteOutstandingCmdsL();

	// PolicyCheck, resource name
	TPtrC8 PolicyRequestResourceL( const TDesC8& aURI );

	TBool LuidMappingInAdapter() { return ETrue ;}
	TBool AdapterCanChangeACL() { return ETrue ; } 

private:
	void ConstructL( );	

	CNSmlEmbeddedLinkAdapter( MSmlDmCallback* aDmCallback );

	enum TNodeId
		{
		ENodeUnknown,

		ENodeCustomization,
		ENodeEmbeddedLinks,
		ENodeEmbeddedLinksX1,
		ENodeLinks,
		ENodeID,
		ENodeAppName,
		ENodeLinksX2,
		ENodeURL,
		ENodeName,
		ENodeUserName,
		ENodePassword,
		ENodeConRef,

		ENodeLast
		};

	TNodeId NodeId( const TDesC8& aURI );

	void FillNodeInfoL( MSmlDmDDFObject& aNode,TSmlDmAccessTypes aAccTypes,MSmlDmDDFObject::TOccurence aOccurrence, MSmlDmDDFObject::TScope aScope, MSmlDmDDFObject::TDFFormat aFormat,const TDesC8& aDescription );
	TInt RecognizeAppTypeL(const TDesC8& aURI, TDes& aAppName);

private:

	MSmlDmCallback* iCallBack;

	/** Temporary buffer for got messages.. */
	RArray<CEmbeddedLinkApp> iBuffer;

	TInt iLuid;
	CEmbeddedLinkHandler* iLinkHandler;

};



	

#endif // __CNSmlEmbeddedLinkAdapter_H__
