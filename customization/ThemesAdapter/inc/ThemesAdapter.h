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



#ifndef __THEMESADAPTER_H__
#define __THEMESADAPTER_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <e32base.h>
#include <smldmadapter.h>
#include "TARMDmAdapter.h"
#include "SWInstApi.h"


const TUint KThemesAdapterImplUid = 0x10207828;

// the DDF version must be changed if any changes in DDF structure ( built in DDFStructureL() function )
_LIT8( KThemesAdapterDDFVersion, "1.0" ); 

_LIT8( KThemesAdapterTextPlain, "text/plain" );
_LIT8( KSlash, "/" );
_LIT8( KThemesAdapterCustomization, "Customization" );
_LIT8( KThemesAdapterCustomizationD, "Customization node" );
_LIT8( KThemesAdapterThemes, "Themes" );
_LIT8( KThemesAdapterThemesD, "Themes node" );
_LIT8( KThemesAdapterAvailableThemes, "AvailableThemes" );
_LIT8( KThemesAdapterAvailableThemesD, "AvailableThemes node" );
_LIT8( KThemesAdapterDynamicNodeD, "" );
_LIT8( KThemesAdapterXId, "ID" );
_LIT8( KThemesAdapterXIdD, "ID node" );
_LIT8( KThemesAdapterXName, "Name" );
_LIT8( KThemesAdapterXNameD, "Name node" );
_LIT8( KThemesAdapterXVersion, "Version" );
_LIT8( KThemesAdapterXVersionD, "Version node" );
_LIT8( KThemesAdapterXData, "Data" );
_LIT8( KThemesAdapterXDataD, "Data node" );
_LIT8( KThemesAdapterXInstallOptions, "InstallOptions" );
_LIT8( KThemesAdapterXInstallOptionsD, "InstallOptions node" );
_LIT8( KThemesAdapterActiveTheme, "ActiveTheme" );
_LIT8( KThemesAdapterActiveThemeD, "ActiveTheme node" );
_LIT8( KThemesAdapterActiveThemeRef, "Customization/Themes/AvailableThemes/" );
_LIT8( KThemesAdapterAvailableThemeBase, "Customization/Themes/AvailableThemes/" );
_LIT8( KThemesAdapterTrue, "True" );
_LIT8( KThemesAdapterFalse, "False" );
_LIT8( KThemesAdapterListOfLeafs, "Image" );
_LIT( KThemesAdapterSeparator16, "/" );
_LIT8( KWPSeparator8, "/" );


class CThemesList;

// ------------------------------------------------------------------------------------------------
// CThemesAdapter 
// ------------------------------------------------------------------------------------------------
class CThemesAdapter : public CTARMDmAdapter
	{
public:
	static CThemesAdapter* NewL(MSmlDmCallback* aDmCallback );
	static CThemesAdapter* NewLC(MSmlDmCallback* aDmCallback );

	virtual ~CThemesAdapter();

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

	// Other
	void GetLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aType, CBufBase& aResult, TError& aRet );


private:
	CThemesAdapter( MSmlDmCallback* aDmCallback );
	void ConstructL(/*MSmlDmCallback* aDmCallback*/);

	enum TNodeId
		{
		ENodeUnknown,

		ENodeCustomization,
		ENodeThemes,
		ENodeAvailableThemes,
		ENodeX,
		ENodeXId,
		ENodeXName,
		ENodeXVersion,
		ENodeXData,
		ENodeXInstallOptions,
		ENodeActiveTheme,
		
		ENodeLast
		};

	TNodeId NodeId( const TDesC8& aURI );

	void FillNodeInfoL( MSmlDmDDFObject& aNode,TSmlDmAccessTypes aAccTypes,MSmlDmDDFObject::TOccurence aOccurrence, MSmlDmDDFObject::TScope aScope, MSmlDmDDFObject::TDFFormat aFormat,const TDesC8& aDescription,TBool aObjectGroup);
	TBool IsImage(const TDesC& aMime) ;
	void SyncWithAvkonSkinDB(CDesC8ArrayFlat& themes);

private:

	MSmlDmCallback* iCallBack;
	CThemesList* iThemes;
	HBufC8*		iActiveTheme;

	};

#endif // __THEMESADAPTER_H__
