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



#ifndef __NSMLScreenSaverAdapter_H__
#define __NSMLScreenSaverAdapter_H__


#include "TARMDmAdapter.h"


const TUint KNSmlDMScreenSaverAdapterImplUid = 0x1020782E;

// the DDF version must be changed if any changes in DDF structure ( built in DDFStructureL() function )
_LIT8( KNSmlSCDDFVersion, "1.0" ); 

_LIT8( KNSmlSCTextPlain, "text/plain" );

_LIT8( KNSmlCustomizationNodeName, "Customization" );
_LIT8( KNSmlSCDescription, "The interior object holds all customization objects" );

_LIT8( KNSmlSCScreenSavers, "ScreenSavers" );
_LIT8( KNSmlSCScreenSaversNodeDescription, "ScreenSavers is a common parent node for nodes managing screen savers");

_LIT8( KNSmlSCAvailableScreenSaversNodeName, "AvailableScreenSavers" );
_LIT8( KNSmlSCAvailableScreenSaversNodeDescription, "This node holds the available screen saver placeholders");

_LIT8( KNSmlSCActiveScNodeName, "ActiveSc" );
_LIT8( KNSmlSCActiveScNodeDescription, "This node is a link to the currently active screen saver");

_LIT8( KNSmlSCDynamicDescription, "This is a placeholder to screen saver ID identifying a screen saver");

_LIT8( KNSmlSCDataNodeName, "Data" );
_LIT8( KNSmlSCDataNodeDescription, "Replace command will install a screen saver that's in the data field of the command");

_LIT8( KNSmlSCIDNodeName, "ID" );
_LIT8( KNSmlSCIDNodeDescription, "This leaf node holds the ID of the screen saver");

_LIT8( KNSmlSCNameNodeName, "Name" );
_LIT8( KNSmlSCNameNodeDescription, "This leaf node holds the name of the screen saver");

_LIT8( KNSmlSCDescriptionNodeName, "Description" );
_LIT8( KNSmlSCDescriptionNodeDescription, "This leaf node holds the description of the screen saver");

_LIT8( KNSmlSCCustomPropertyNodeName, "CustomProperty" );
_LIT8( KNSmlSCCustomPropertyNodeDescription, "This leaf node holds the custom property value of the screen saver");

_LIT8( KNSmlSCVersionNodeName, "Version" );
_LIT8( KNSmlSCVersionNodeDescription, "This leaf node holds the version of the screen saver");

_LIT8( KNSmlSCInstallOptionsNodeName, "InstallOptions" );
_LIT8( KNSmlSCInstallOptionsNodeDescription, "Holds the install options for next install");

_LIT8( KNSmlSCActiveScRef, "Customization/ScreenSavers/AvailableScreenSavers/" );

_LIT8( KSlash, "/" );


class CScreenSaverList;

// ------------------------------------------------------------------------------------------------
// CNSmlDmEmailAdapter 
// ------------------------------------------------------------------------------------------------
class CScreenSaverAdapter : public CTARMDmAdapter
	{
public:
	static CScreenSaverAdapter* NewL(MSmlDmCallback* aDmCallback );
	static CScreenSaverAdapter* NewLC(MSmlDmCallback* aDmCallback );

	virtual ~CScreenSaverAdapter();

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
//#ifdef __SAP_POLICY_MANAGEMENT	
	TPtrC8 PolicyRequestResourceL( const TDesC8& aURI );
//#endif

protected:
	// Other
	void GetLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aType, CBufBase& aResult, TError& aRet );

private:
	void ConstructL( );
	CScreenSaverAdapter( MSmlDmCallback* aDmCallback );

	enum TNodeId
		{
		ENodeUnknown,

		ENodeCustomization,
		ENodeScreenSavers,

		ENodeAvailableScreenSavers,
		ENodeActiveSc,

		ENodeAvailableScreenSaversX,

		ENodeData,
		ENodeID,
		ENodeName,
		ENodeDescription,
		ENodeCustomProperty,
		ENodeVersion,
		ENodeInstallOptions,

		ENodeLast
		};

	TNodeId NodeId( const TDesC8& aURI );

	void FillNodeInfoL( MSmlDmDDFObject& aNode,TSmlDmAccessTypes aAccTypes,MSmlDmDDFObject::TOccurence aOccurrence, MSmlDmDDFObject::TScope aScope, MSmlDmDDFObject::TDFFormat aFormat,const TDesC8& aDescription );
	TBool IsImage(const TDesC8& aMime) ;


	void InstalledPackagesL( RArray< TUid >& aUids );
//	void InstalledScreenSaversL( RArray< TUid >& aUids );
	void InstalledScreenSaversL( CDesCArray& aIds );

private:

	MSmlDmCallback* iCallBack;
	CScreenSaverList* iScreenSavers;
	TBool iScreensaverOff;

};

#endif // __NSMLScreenSaverAdapter_H__
