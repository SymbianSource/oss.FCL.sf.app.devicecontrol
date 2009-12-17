/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Device Management IMPS Adapter
*
*/



// TaH 3.6.2005 Added constants for parameter lenghts (KNSmlDMMaxNameLen, KNSmlDMMaxAddrLen). 
// TaH 13.6.2005 Added comments for functions.
// TaH 27.9.2005 Constansts for dynamic node names changed.

#ifndef __NSMLDMIMPSADAPTER_H__
#define __NSMLDMIMPSADAPTER_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <e32base.h>
#include <smldmadapter.h>
#include <msvapi.h>

// CONSTANTS
// Defined in nsmldmimpluids.h file
const TUint KNSmlDMImpsAdapterImplUid = 0x101F9A2B;
const TInt KNSmlDMSAPSettings = 200;
const TInt KNSmlDMMaxNameLen = 50;
const TInt KNSmlDMMaxAddrLen = 255;
const TInt KNSmlDMReservingData = 16;
const TInt KNSmlDMReservingMaxData = 100;

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

// the DDF version must be changed if any changes in DDF structure ( built in DDFStructureL() function )
_LIT8( KNSmlDMImpsDDFVersion, "1.0" ); 

_LIT8( KNSmlDMImpsTextPlain, "text/plain" );
_LIT8( KNSmlDMImpsDynamicNode, "" );
_LIT8( KNSmlDMImpsNodeName, "IMPS" );
_LIT8( KNSmlDMImpsAppId, "AppID" );
_LIT8( KNSmlDMImpsName, "Name" );
_LIT8( KNSmlDMImpsPrefConRef, "PrefConRef" );
_LIT8( KNSmlDMImpsPrefAddr, "PrefAddr" );
_LIT8( KNSmlDMImpsAppAuth, "AppAuth" );
_LIT8( KNSmlDMImpsAAuthLevel, "AAuthLevel" );
_LIT8( KNSmlDMImpsAAuthName, "AAuthName" );
_LIT8( KNSmlDMImpsAAuthSecret, "AAuthSecret" );
_LIT8( KNSmlDMImpsServices, "Services" );
_LIT8( KNSmlDMImpsExtNode, "Ext" );
_LIT8( KNSmlDMImpsPepCompliant, "PEPCompliant" );
_LIT( KNSmlDMImpsPepCompliant16, "PEPCompliant" );

_LIT8( KNSmlDMImpsDescr, "The interior object holds all IMPS objects" );
_LIT8( KNSmlDMDynamicNodeDescr, "Placeholder for one or more objects" );
_LIT8( KNSmlDMImpsAppIDDescr, "Identifier for the application service available at the described SAP" );
_LIT8( KNSmlDMImpsNameDescr, "Logical and user readable identity of the SAP" );
_LIT8( KNSmlDMImpsPrefConRefDescr, "Indicator for preferred linkage to connectivity parameters" );
_LIT8( KNSmlDMImpsPrefAddrDescr, "Addres of the application server" );
_LIT8( KNSmlDMImpsAppAuthDescr, "Container of authentication information to be used with the SAP" );
_LIT8( KNSmlDMImpsAAuthLevelDescr, "Tells how the provided authentication credentials are to be applied" );
_LIT8( KNSmlDMImpsAAuthNameDescr, "Indicator of the ID" );
_LIT8( KNSmlDMImpsAAuthSecretDescr, "Indicator of the authentication secret" );
_LIT8( KNSmlDMImpsServicesDescr, "List of supported high-level services" );
_LIT8( KNSmlDMImpsExtNodeDescr, "Container of vendor specific information" );
_LIT8( KNSmlDMImpsPepCompliantDescr, "Indicator of the PEP Compliantly" );

_LIT8( KNSmlDMImpsDefaultAppID, "wA" );
_LIT8( KNSmlDMImpsDefaultAAuthLevel, "APPSRV" );
_LIT8( KNSmlDMImpsDefaultPEC, "TRUE" );
_LIT8( KNSmlDMImpsDefaultIM, "FALSE" );
_LIT8( KNSmlDMImpsDefPEC, "T" );
_LIT8( KNSmlDMImpsDefIM, "F" );
_LIT( KNSmlDMImpsNotDefined, "" );
_LIT8( KNSmlDMImpsNotDefined8, "" );
_LIT8( KNSmlDMImpsIAPUri, "AP" );
_LIT8( KNSmlDMImpsServiceIM, "IM" );
_LIT8( KNSmlDMImpsServicePS, "PS" );
_LIT8( KNSmlDMImpsServiceGR, "GR" );
_LIT8( KNSmlDMImpsConnMOIAPUri, "NAP" );

_LIT8( KNSmlDMImpsRtNode, "AppID/Name/PrefConRef/PrefAddr/Services/AppAuth/Ext");
_LIT8( KNSmlDMAppAuthRtNode, "AAuthLevel/AAuthName/AAuthSecret");
_LIT8( KNSmlDMRtNode8, "AAId1");
_LIT( KNSmlDMRtNode, "AAId1");

_LIT8( KNSmlDMImpsSeparator8, "/" );
_LIT( KNSmlDMImpsSeparator, "/" );
_LIT8( KNSmlImpsUriDotSlash, "./");
_LIT8( KNSmlDMImpsSemiColon, ";");
_LIT( KNSmlDMImpsDefaultName, "default" );

_LIT8( KNSmlDMImpsRootAndDyn, "IMPS/*" );
_LIT8( KNSmlDMImpsWholeAppAuthUri, "IMPS/*/AppAuth" );
_LIT8( KNSmlDMImpsWholeAppAuthDynUri, "IMPS/*/AppAuth/*" );
_LIT8( KNSmlDMImpsWholeExtUri, "IMPS/*/Ext" );

_LIT8( KNSMLDMImpsDynNodeName, "ImpsId" );
_LIT8( KNSmlImpsLine, "-");

// ------------------------------------------------------------------------------------------------
// CNSmlDmImpsAdapter 
// ------------------------------------------------------------------------------------------------
class CNSmlDmImpsAdapter : public CSmlDmAdapter, public MMsvSessionObserver
	{
public: // Constructors and destructor

        /**
        * Two-phased constructors.
        */	
    static CNSmlDmImpsAdapter* NewL( MSmlDmCallback* aDmCallback );
    static CNSmlDmImpsAdapter* NewLC( MSmlDmCallback* aDmCallback );

	    /**
        * Destructor.
        */
    virtual ~CNSmlDmImpsAdapter();

        /**
        * Requests DDF Version
        * @param aVersion, Reference for version
        */
    void DDFVersionL( CBufBase& aVersion );	

        /**
        * Requests DDF Structure
        * @param aDDF, Reference to root node
        */
    void DDFStructureL( MSmlDmDDFObject& aDDF );

        /**
        * Updates leaf object value
        * @param aURI, The uri of parameter
        * @param aLUID, The Luid of Uri
        * @param aObject, The value for parameter
        * @param aType, Not used at the moment
        * @param aStatusRef, Status reference of command
        */
    void UpdateLeafObjectL( const TDesC8& aURI, 
	                        const TDesC8& aLUID, 
	                        const TDesC8& aObject, 
	                        const TDesC8& aType, 
	                        TInt aStatusRef );

        /**
        * Updates leaf object value
        * Not used at the moment
        */
    void UpdateLeafObjectL( const TDesC8& aURI, 
	                        const TDesC8& aLUID,
	                        RWriteStream*& aStream, 
	                        const TDesC8& aType,
	                        TInt aStatusRef );

        /**
        * Fetches the leaf object size
        * @param aURI, The uri of parameter
        * @param aLUID, The Luid of Uri
        * @param aType, Not used at the moment
        * @param aResultsRef, Result reference of command
        * @param aStatusRef, Status reference of command
        */
    void FetchLeafObjectSizeL( const TDesC8& aURI, 
	                           const TDesC8& aLUID, 
	                           const TDesC8& aType, 
	                           const TInt aResultsRef, 
	                           const TInt aStatusRef );
	
        /**
        * Fetches the leaf object value
        * @param aURI, The uri of parameter
        * @param aLUID, The Luid of Uri
        * @param aType, Not used at the moment
        * @param aResultsRef, Result reference of command
        * @param aStatusRef, Status reference of command
        */
    void FetchLeafObjectL( const TDesC8& aURI, 
	                       const TDesC8& aLUID, 
	                       const TDesC8& aType, 
	                       TInt aResultsRef, 
	                       TInt aStatusRef );

        /**
        * Deletes the node or leaf 
        * @param aURI, The uri of parameter
        * @param aLUID, The Luid of Uri
        * @param aType, Not used at the moment
        * @param aResultsRef, Result reference of command
        * @param aStatusRef, Status reference of command
        */
    void DeleteObjectL( const TDesC8& aURI, 
	                    const TDesC8& aLUID, 
	                    TInt aStatusRef );
	
	
        /**
        * Requests the child uri list
        * @param aURI, The uri of parameter
        * @param aLUID, The Luid of Uri
        * @param aPreviousURISegmentList, list of previous Uri Segments
        * @param aResultsRef, Result reference of command
        * @param aStatusRef, Status reference of command
        */
    void ChildURIListL( const TDesC8& aURI, 
	                    const TDesC8& aLUID, 
	                    const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, 
	                    TInt aResultsRef, 
	                    TInt aStatusRef );
	
        /**
        * Adds new node
        * @param aURI, The uri of parameter
        * @param aParentLUID, The Luid of ParentUri
        * @param aStatusRef, Status reference of command
        */
    void AddNodeObjectL( const TDesC8& aURI, 
	                     const TDesC8& aParentLUID, 
	                     TInt aStatusRef );

        /**
        * Following methods are not used at the moment
        */
	void CompleteOutstandingCmdsL();
	void ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aArgument, const TDesC8& aType, const TInt aStatusRef );
	void ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, RWriteStream*& aStream, const TDesC8& aType, const TInt aStatusRef );
	void CopyCommandL( const TDesC8& aTargetURI, const TDesC8& aTargetLUID, const TDesC8& aSourceURI, const TDesC8& aSourceLUID, const TDesC8& aType, TInt aStatusRef );
	void StartAtomicL();
	void CommitAtomicL();
	void RollbackAtomicL();
	TBool StreamingSupport( TInt& aItemSize );
	void StreamCommittedL();
	
protected:	
	
        /**
        * From MMsvSessionObserver
        */
    void HandleSessionEventL( TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3 );

private:

        /**
        * Constructors.
        */
	CNSmlDmImpsAdapter();
	CNSmlDmImpsAdapter( TAny* aEcomArguments );//new
	
        /**
        * Symbian 2nd phase constructor.
        */
    void ConstructL( MSmlDmCallback* aDmCallback );

        /**
        * Change Descriptor to Int
        * @param aLuid, Descriptor
        * @return Integer value
        */
    TInt DesToInt( const TDesC& aLuid );
	TInt DesToInt( const TDesC8& aLuid );

        /**
        * Change Int to descriptor
        * @param aLuid, Integer value
        * @return Descriptor
        */
	HBufC* IntToDesLC( const TInt aLuid );
	HBufC8* IntToDes8LC( const TInt aLuid );

        /**
        * Converts 8 bit descriptor to 16 bit descriptor 
        * @param aSource, source descriptor
        * @return Destination descriptor
        */
    TDesC8&  ConvertTo8LC( const TDesC& aSource );

        /**
        * Converts 16 bit descriptor to 8 bit descriptor 
        * @param aSource, source descriptor
        * @return Destination descriptor
        */
	TDesC16& ConvertTo16LC( const TDesC8& aSource );
	
        /**
        * Fill node info
        * @param aNode, node name
        * @param aAccTypes, access types
        * @param aOccurence, occurrence
        * @param aScope, Dynamic or permanent
        * @param aFormat, type of leaf
        * @param aDescription, description of node/leaf
        */
	void FillNodeInfoL( MSmlDmDDFObject& aNode, 
	                    const TSmlDmAccessTypes aAccTypes, 
	                    MSmlDmDDFObject::TOccurence aOccurrence, 
	                    MSmlDmDDFObject::TScope aScope, 
	                    MSmlDmDDFObject::TDFFormat aFormat,
	                    const TDesC8& aDescription );
	                    
        /**
        * Find correct SAP from DB and read the data
        * @param aLUID, SAP uid
        * @param aSap, place for reading SAP data
        * @param aStore, variable for rading SAP data
        * @return error status
        */
    TInt GetCorrectSapL( const TDesC8& aLUID, 
                         CIMPSSAPSettings* aSap, 
                         CIMPSSAPSettingsStore* aStore );

        /**
        * Reset Sap with empty data
        * @param aSap, includes the SAP data
        */
    void ResetSapL( CIMPSSAPSettings* aSap );

        /**
        * Update value to SAP data
        * @param aUri, parameter
        * @param aObject, parameter value
        * @param aSap, variable for SAP data
        * @return error status
        */
    CSmlDmAdapter::TError UpdateValueL( const TDesC8& aUri, 
                                        const TDesC8& aObject, 
                                        CIMPSSAPSettings* aSap );
                                        
        /**
        * Update PEP value to SAP data
        * @param aUri, parameter
        * @param aObject, parameter value
        * @param aLUID, SAP uid
        * @param aSap, variable for reading SAP data
        * @param aStore, used for rading SAP data
        * @return error status
        */
	CSmlDmAdapter::TError UpdatePEPValueL( const TDesC8& aURI, 
	                                       const TDesC8& aObject, 
	                                       const TDesC8& aLUID, 
	                                       CIMPSSAPSettings* aSap, 
	                                       CIMPSSAPSettingsStore* aStore );
	                                       
        /**
        * Delete value from SAP data
        * @param aUri, parameter
        * @param aSap, variable for reading SAP data
        * @return error status
        */
    CSmlDmAdapter::TError DeleteValueL( const TDesC8& aUri, 
                                        CIMPSSAPSettings* aSap );

        /**
        * Fetch object data 
        * @param aUri, parameter
        * @param aLUID, SAP uid
        * @param aObject, variable for returning parameter data
        * @return error status
        */
    CSmlDmAdapter::TError FetchObjectL( const TDesC8& aUri, 
                                        const TDesC8& aLUID, 
                                        CBufBase& aObject );
                                        
        /**
        * Get Connection Reference 
        * @return accesspoint reference 
        */
    const TDesC8& GetConRef() ; 


private: // Data
	MSmlDmCallback* iCallBack;
	/**
		* Boolean to check FeatMgr Initialization
		*/
		TBool iFeatMgrInitialized; 
	};

#endif // __NSMLDMIMPSADAPTER_H__
