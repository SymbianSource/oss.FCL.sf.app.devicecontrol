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



#ifndef __OPERATORLOGOADAPTER_H__
#define __OPERATORLOGOADAPTER_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include "TARMDmAdapter.h"

#include <fbs.h>

const TUint KOperatorLogoAdapterImplUid = 0x1020782A;

// the DDF version must be changed if any changes in DDF structure ( built in DDFStructureL() function )
_LIT8( KOperatorLogoDDFVersion, "1.0" ); 

_LIT8( KOperatorLogoCustomization, "Customization" );
_LIT8( KOperatorLogoCustomizationD, "Customization node" );
_LIT8( KOperatorLogoOperatorLogo, "OperatorLogo" );
_LIT8( KOperatorLogoOperatorLogoD, "OperatorLogo node" );
_LIT8( KOperatorLogoLogoData, "Data" );
_LIT8( KOperatorLogoLogoDataD, "LogoData node" );


_LIT8( KNSmlDMTextPlain, "text/plain" );
_LIT8( KNSmlDMImageAny, "image/*" );

// ------------------------------------------------------------------------------------------------
// COperatorLogoAdapter 
// ------------------------------------------------------------------------------------------------
class COperatorLogoAdapter : public CTARMDmAdapter
	{
public:
	static COperatorLogoAdapter* NewL(MSmlDmCallback* aDmCallback );
	static COperatorLogoAdapter* NewLC(MSmlDmCallback* aDmCallback );

	virtual ~COperatorLogoAdapter();


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
	void ConstructL( );

	COperatorLogoAdapter( MSmlDmCallback* aDmCallback );

	enum TNodeId
		{
		ENodeUnknown,

		ENodeCustomization,
		ENodeOperatorLogo,
		ENodeLogoData,
		
		ENodeLast
		};

	TNodeId NodeId( const TDesC8& aURI );

	void FillNodeInfoL( MSmlDmDDFObject& aNode,TSmlDmAccessTypes aAccTypes,MSmlDmDDFObject::TOccurence aOccurrence, MSmlDmDDFObject::TScope aScope, MSmlDmDDFObject::TDFFormat aFormat,const TDesC8& aDescription,TBool aObjectGroup);

	TBool IsText(const TDesC8& aMime);
	TBool IsImage(const TDesC8& aMime);
	TError GetOperatorLogoL( CBufBase& aData, TDes8& aMimeType );
	TError SetOperatorLogoL( const TDesC8& aData, const TDesC8& aMime );
	TError SetOperatorLogoText( const TDesC8& aData, const TDesC8& aMime );
	TError SetOperatorLogoImageL( const TDesC8& aData, const TDesC8& aMime );
	TError DeleteOperatorLogoImageL();

	void LoadBitmapL( const TDesC& aFileName, CFbsBitmap& aBitmap );
	void ScaleBitmapL( CFbsBitmap& aBitmap, TInt aX, TInt aY );
	void GetScreenDisplayModeL( TDisplayMode &aDisplayMode );
	void GetOpInfoL( TInt& aMCC, TInt& aMNC );
	void SaveOperatorLogoBitmapL( TInt aBitmapHandle, TInt aMCC, TInt aMNC );

private:

	MSmlDmCallback* iCallBack;


};



	

#endif // __OPERATORLOGOADAPTER_H__
