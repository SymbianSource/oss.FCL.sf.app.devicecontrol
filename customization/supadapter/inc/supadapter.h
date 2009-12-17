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



#ifndef __NSMLSUPADAPTER_H__
#define __NSMLSUPADAPTER_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
// symbian
#include <e32base.h>
#include <smldmadapter.h>
// s60
#include "TARMDmStreamAdapter.h"
#include "UISettingsSrvClient.h"
//#ifdef __SAP_POLICY_MANAGEMENT
#include <PolicyEngineClient.h>
//#endif

const TUint KNSmlDMSUPAdapterImplUid = 0x10207822;

// the DDF version must be changed if any changes in DDF structure ( built in DDFStructureL() function )
_LIT8( KNSmlSUPDDFVersion, "1.0" ); 

_LIT8( KNSmlSUPTextPlain, "text/plain" );
_LIT8( KNSmlSUPNodeName, "Apps" );
_LIT8( KNSmlSUPDescription, "This node is the common parent to all customization objects." );
_LIT8( KNSmlSUPStartupNodeDescription, "Startup is a common parent node to nodes customizing startup functionality");
_LIT8( KNSmlSUPWelcomeDataNodeDescription, "Image node stores the WelcomeData (image or text) as binary data, and its runtime Type determines the interpretation");
_LIT8( KNSmlSUPWelcomeTextNodeDescription, "Text node stores the Welcome Text");

_LIT8( KNSmlSUPDynamicNode, "" );
_LIT8( KNSmlSUPWelcomeDataNodeName, "Data" );
_LIT8( KNSmlSUPWelcomeTextNodeName, "Text" );
_LIT8( KNSmlSUPStartupNodeName, "Startup" );
_LIT8( KNSmlCustomizationNodeName, "Customization" );
_LIT8( KNSmlSUPImage8, "WelcomeData" );
_LIT8( KNSmlSUPStartup8, "Startup" );
_LIT8( KNSmlCustomizationNodeName8, "Customization" );

//
// Policies:
//
#define SUPADAPTER_POLICY1_SUBJECT_P1 "urn:oasis:names:tc:xacml:2.0:subject:role_id"
#define SUPADAPTER_POLICY1_SUBJECT_P2 ""
#define SUPADAPTER_POLICY1_SUBJECT_P3 "http://www.w3.org/2001/XMLSchema#string"
#define SUPADAPTER_POLICY1_RESOURCE_P1 "urn:oasis:names:tc:xacml:2.0:resource:resource_id"
#define SUPADAPTER_POLICY1_RESOURCE_P2 "CustomizationManagement"
#define SUPADAPTER_POLICY1_RESOURCE_P3 "http://www.w3.org/2001/XMLSchema#string"
_LIT8( KSUPAdapterPolicy1SubjectParam1, SUPADAPTER_POLICY1_SUBJECT_P1 );
_LIT8( KSUPAdapterPolicy1SubjectParam2, SUPADAPTER_POLICY1_SUBJECT_P2 );
_LIT8( KSUPAdapterPolicy1SubjectParam3, SUPADAPTER_POLICY1_SUBJECT_P3 );
_LIT8( KSUPAdapterPolicy1ResourceParam1, SUPADAPTER_POLICY1_RESOURCE_P1 );
_LIT8( KSUPAdapterPolicy1ResourceParam2, SUPADAPTER_POLICY1_RESOURCE_P2 );
_LIT8( KSUPAdapterPolicy1ResourceParam3, SUPADAPTER_POLICY1_RESOURCE_P3 );


#ifndef MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64
#define MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64 21
#endif

// ------------------------------------------------------------------------------------------------
// CSupAdapter 
// ------------------------------------------------------------------------------------------------
class CSupAdapter : public CTARMDmStreamAdapter//, public MMsvSessionObserver
	{
private:
	enum TStartupAdapterLeafType
	{
		EText 		= 0,
		EImage 		= 1,
		EUnknown 	= 2	
	};

    enum TSupNodeIdentifier
        {
        ESupNodeCustomization = 0,
        ESupNodeStartup,
        ESupNodeWelcomeData,
        ESupNodeWelcomeText,
        ESupNodeNotUsedAndAlwaysLast
        };

public:
	CSupAdapter(TAny* aEcomArguments);

	static CSupAdapter* NewL( MSmlDmCallback* aDmCallback );
	static CSupAdapter* NewLC( MSmlDmCallback* aDmCallback );

	virtual ~CSupAdapter();

	// Pure virtual methods
	void DDFVersionL( CBufBase& aVersion );

	void DDFStructureL( MSmlDmDDFObject& aDDF );

	void UpdateLeafObjectL( CSmlDmAdapter::TError& aStatus, const TDesC8& aURI,
	                        const TDesC8& aLUID, const TDesC8& aObject, const TDesC8& aType );

	CSmlDmAdapter::TError FetchLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, TDes8& aNewMime, CBufFlat* object, TInt &aUncodedSize );

	void _UpdateLeafObjectL     ( const TDesC8& aURI, const TDesC8& aLUID,
	                              const TDesC8& aObject, const TDesC8& aType, TInt aStatusRef );
	void _UpdateLeafObjectL     ( const TDesC8& aURI, const TDesC8& aLUID,
	                              RWriteStream*& aStream, const TDesC8& aType, TInt aStatusRef );
	void _DeleteObjectL         ( const TDesC8& aURI, const TDesC8& aLUID, TInt aStatusRef );
	void _FetchLeafObjectL      ( const TDesC8& aURI, const TDesC8& aLUID,
	                              const TDesC8& aType, TInt aResultsRef, TInt aStatusRef );
	void _FetchLeafObjectSizeL  ( const TDesC8& aURI, const TDesC8& aLUID,
	                              const TDesC8& aType, TInt aResultsRef, TInt aStatusRef );
    void _ChildURIListL         ( const TDesC8& aURI, const TDesC8& aLUID,
                                  const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
                                  TInt aResultsRef, TInt aStatusRef );
	void _AddNodeObjectL        ( const TDesC8& aURI, const TDesC8& aParentLUID, TInt aStatusRef );
	void _ExecuteCommandL       ( const TDesC8& aURI, const TDesC8& aLUID,
	                              const TDesC8& aArgument, const TDesC8& aType, TInt aStatusRef );
    void _ExecuteCommandL       ( const TDesC8& aURI, const TDesC8& aLUID,
                                  RWriteStream*& aStream, const TDesC8& aType, TInt aStatusRef );
    void _CopyCommandL          ( const TDesC8& aTargetURI, const TDesC8& aTargetLUID,
                                  const TDesC8& aSourceURI, const TDesC8& aSourceLUID,
                                  const TDesC8& aType, TInt aStatusRef );

    void StartAtomicL();
    void CommitAtomicL();
    void RollbackAtomicL();

    TAdapterStreamType StreamType( const TDesC8& aURI );
    TBool StreamingSupport( TInt& aItemSize );
    void CompleteOutstandingCmdsL();

//#ifdef __SAP_POLICY_MANAGEMENT
    TInt CheckPolicyL();    
//#endif

private:
	void ConstructL(MSmlDmCallback* aDmCallback);
	
	void FillNodeInfoL( MSmlDmDDFObject& aNode,TSmlDmAccessTypes aAccTypes,MSmlDmDDFObject::TOccurence aOccurrence, MSmlDmDDFObject::TScope aScope, MSmlDmDDFObject::TDFFormat aFormat,const TDesC8& aDescription,const TDesC8& aMimeType);
	
	TStartupAdapterLeafType	GetObjectType(const TDesC8& aMime);
	TBool IsSupportedImageType(const TDesC8& aMime);
	TBool IsSupportedTextType(const TDesC8& aMime);

	void SetStartupTextL(CSmlDmAdapter::TError& aStatus, const TDesC16& aText);
	void SetStartupImageL(CSmlDmAdapter::TError& aStatus, const TDesC8& aObject, const TDesC8& aType);
	
	CBufFlat* GetStartupTextL(CSmlDmAdapter::TError& aStatus, const TDesC8& aType);
	CBufFlat* GetStartupImageL(CSmlDmAdapter::TError& aStatus, const TDesC8& aType, TInt &aImageSize);
	
	void CopyAndTrimMimeType(TDes8& aNewMime, const TDesC8& aType);
	
    CSupAdapter::TSupNodeIdentifier GetNodeIdentifier(const TDesC8& aURI);
	TStartupAdapterLeafType GetFetchTypeL(CSmlDmAdapter::TError& aStatus, const TDesC8& aType);
	TStartupAdapterLeafType GetSelectedTypeL(CSmlDmAdapter::TError& aStatus);	

//#ifdef __SAP_POLICY_MANAGEMENT
    TPtrC8 PolicyRequestResourceL( const TDesC8& aURI );
//#endif

#ifdef __WINS__	
public:	
	void DebugTestsL();
#endif


private:

//#ifdef __SAP_POLICY_MANAGEMENT
    RPolicyEngine   iPE;
    RPolicyRequest  iPR;
//#endif

	MSmlDmCallback* iCallBack;
	CBufBase *iUndoImage;
	CBufBase *iUndoText;
	TInt     iUndoNoteType;

    TFileName iTemporaryImageFile;
};

#endif // __NSMLSUPADAPTER_H__
