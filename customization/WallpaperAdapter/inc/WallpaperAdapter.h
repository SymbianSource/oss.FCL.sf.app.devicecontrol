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



#ifndef __WallpaperAdapter_H__
#define __WallpaperAdapter_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <e32base.h>
#include <smldmadapter.h>
#include "TARMDmAdapter.h"


const TUint KSmlDMWallpaperAdapterImplUid = 0x10207830;

// the DDF version must be changed if any changes in DDF structure ( built in DDFStructureL() function )
_LIT8( KSmlWPDDFVersion, "1.0" ); 
_LIT8( KSmlWPTextPlain, "text/plain" );
_LIT( KSmlWPDescription, "The interior object holds all wallpaper objects" );

_LIT8( KSmlCustomizationN, "Customization" );
_LIT8( KSmlCustomizationD, "Customization node" );
_LIT8( KSmlWPWallpaperN, "Wallpaper" );
_LIT8( KSmlWPWallpaperD, "Wallpaper node" );
_LIT8( KSmlWPDataN, "Data" );
_LIT8( KSmlWPImageD, "Image node" );

_LIT8( KWPSeparator, "/" );

// ------------------------------------------------------------------------------------------------
// CWallpaperAdapter 
// ------------------------------------------------------------------------------------------------
class CWallpaperAdapter : public CTARMDmAdapter
	{
public:
	static CWallpaperAdapter* NewL(MSmlDmCallback* aDmCallback );
	static CWallpaperAdapter* NewLC(MSmlDmCallback* aDmCallback );

	virtual ~CWallpaperAdapter();

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
private:
	void ConstructL();
	CWallpaperAdapter( MSmlDmCallback* aDmCallback );

	enum TNodeId
		{
		ENodeUnknown,
		ENodeCustomization,
		ENodeWallpaper,
		ENodeImage,
		ENodeLast
		};

	TNodeId NodeId( const TDesC8& aURI );

	void FillNodeInfoL( MSmlDmDDFObject& aNode,
						TSmlDmAccessTypes aAccTypes,
						MSmlDmDDFObject::TOccurence aOccurrence,
						MSmlDmDDFObject::TScope aScope,
						MSmlDmDDFObject::TDFFormat aFormat,
						const TDesC8& aDescription,
						TBool aObjectGroup);

	TBool IsImageL(const TDesC8& aMime) ;
private:

	MSmlDmCallback* iCallBack;

};



	

#endif // __WallpaperAdapter_H__
