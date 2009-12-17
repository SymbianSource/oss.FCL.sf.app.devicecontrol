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



// TaH 3.6.2005 Added the checking of Name, PrefAddr, AAuthName and AAuthSecret lenght. 
//              and corrected the fetching of PrefConRef parameter.
// TaH 16.6.2005 Some minor changes done according the code review comments.
// TaH 8.7.2005 Corrected errors ESBA-6DYMFL, ESBA-6DYNMK and ESBA-6DYMNV
// TaH 27.9.2005 Dynamic node names changed, 
//				 reduced number of warnings caused by debug logs writtings.

// INCLUDE FILES


#include <msvids.h>
#include <implementationproxy.h> // For TImplementationProxy definition
#include <utf.h>
#include <iapprefs.h>
#include <commdb.h>
#include <cdbcols.h>			 // CommsDB columname defs
#include <sysutil.h>
#include <e32des8.h>
#include <cimpssapsettings.h>         //CIMPSSAPSettings
#include <cimpssapsettingsstore.h>    //CWWVSAPSettingsStore
#include <cimpssapsettingslist.h>     //CIMPSSAPSettingsList
#include <featmgr.h>
#include "nsmldmimpluids.h"
#include "nsmldmimpsadapter.h"
#include "nsmldebug.h"
#include "nsmlconstants.h"
#include "nsmldmconst.h"
#include "nsmldmiapmatcher.h"
#include "nsmldmuri.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif

// ============================= LOCAL FUNCTIONS ==============================

// ============================ MEMBER FUNCTIONS ==============================
	
// ----------------------------------------------------------------------------
// CNSmlDmImpsAdapter::CNSmlDmImpsAdapter()
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------

CNSmlDmImpsAdapter::CNSmlDmImpsAdapter(TAny* aEcomArguments) : 
        CSmlDmAdapter(aEcomArguments)
	{
	_DBG_FILE( "CNSmlDmImpsAdapter::CNSmlDmImpsAdapter(): begin" );
	_DBG_FILE( "CNSmlDmImpsAdapter::CNSmlDmImpsAdapter(): end" );
	}


// ----------------------------------------------------------------------------
// CNSmlDmImpsAdapter::ConstructL()
// Symbian 2nd phase constructor can leave. 
// The global variables are created here.
// ----------------------------------------------------------------------------
void CNSmlDmImpsAdapter::ConstructL( MSmlDmCallback *aDmCallback )
	{
	iCallBack = aDmCallback;
	FeatureManager::InitializeLibL();
  iFeatMgrInitialized = ETrue;
	}

// ----------------------------------------------------------------------------
// CNSmlDmImpsAdapter* CNSmlDmImpsAdapter::NewL( )
// Two-phased constructor.
// ----------------------------------------------------------------------------
CNSmlDmImpsAdapter* CNSmlDmImpsAdapter::NewL( MSmlDmCallback* aDmCallback )
	{
	_DBG_FILE( "CNSmlDmImpsAdapter::NewL(): begin" );
	CNSmlDmImpsAdapter* self = NewLC( aDmCallback );
	CleanupStack::Pop();
	_DBG_FILE( "CNSmlDmImpsAdapter::NewL(): end" );	
	return self;
	}

// ----------------------------------------------------------------------------
// CNSmlDmImpsAdapter* CNSmlDmImpsAdapter::NewLC( )
// Two-phased constructor.
// ----------------------------------------------------------------------------
CNSmlDmImpsAdapter* CNSmlDmImpsAdapter::NewLC( MSmlDmCallback* aDmCallback )
	{
	_DBG_FILE( "CNSmlDmImpsAdapter::NewLC(): begin" );
	CNSmlDmImpsAdapter* self = new( ELeave ) CNSmlDmImpsAdapter( aDmCallback );
	CleanupStack::PushL( self );
	self->ConstructL( aDmCallback );
	_DBG_FILE( "CNSmlDmImpsAdapter::NewLC(): end" );
	return self;
	}


// ----------------------------------------------------------------------------
// CNSmlDmImpsAdapter::~CNSmlDmImpsAdapter()    
// Destructor, the global variables are deleted here.
// ----------------------------------------------------------------------------
CNSmlDmImpsAdapter::~CNSmlDmImpsAdapter()
	{
	_DBG_FILE( "CNSmlDmImpsAdapter::~CNSmlDmImpsAdapter(): begin" );
	if(iFeatMgrInitialized)
    		{
    			FeatureManager::UnInitializeLib();
    		}

    _DBG_FILE( "CNSmlDmImpsAdapter::~CNSmlDmImpsAdapter(): end" );
	}

// ----------------------------------------------------------------------------
//  CNSmlDmImpsAdapter::DDFVersionL()
//  The method returns the DDFVersion number.
// ----------------------------------------------------------------------------
void CNSmlDmImpsAdapter::DDFVersionL( CBufBase& aDDFVersion )
	{
	_DBG_FILE( "CNSmlDmImpsAdapter::DDFVersionL( TDes& aDDFVersion ): begin" );
	aDDFVersion.InsertL( 0,KNSmlDMImpsDDFVersion );
	_DBG_FILE( "CNSmlDmImpsAdapter::DDFVersionL( TDes& aDDFVersion ): end" );
	}


// ----------------------------------------------------------------------------
//  CNSmlDmImpsAdapter::DDFStructureL()
//  The method creates the DDFStructure
// ----------------------------------------------------------------------------
void CNSmlDmImpsAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
	{
	_DBG_FILE( "CNSmlDmImpsAdapter::DDFStructureL(): begin" );
	
	TSmlDmAccessTypes allAccessTypes;
    allAccessTypes.SetAdd();  
	allAccessTypes.SetDelete();
	allAccessTypes.SetGet();
	allAccessTypes.SetReplace();

	TSmlDmAccessTypes accessTypesAddGet;
    accessTypesAddGet.SetAdd();  
	accessTypesAddGet.SetGet();
    
	TSmlDmAccessTypes accessTypesGet;
	accessTypesGet.SetGet();
	
	TSmlDmAccessTypes accessTypesNoDel;
    accessTypesNoDel.SetAdd();  
	accessTypesNoDel.SetGet();
	accessTypesNoDel.SetReplace();

	TSmlDmAccessTypes accessTypesASecret;
	accessTypesASecret.SetAdd();
	accessTypesASecret.SetDelete();
	accessTypesASecret.SetReplace();
	
    
  // IMPS
    MSmlDmDDFObject& imps = aDDF.AddChildObjectL( KNSmlDMImpsNodeName );
    FillNodeInfoL( imps, 
                    accessTypesGet, 
                    MSmlDmDDFObject::EOne,
                    MSmlDmDDFObject::EPermanent, 
                    MSmlDmDDFObject::ENode, KNSmlDMImpsDescr );

    // run-time node under IMPS
    // IMPS/<X>
    MSmlDmDDFObject& rtNode = imps.AddChildObjectGroupL(); 
    FillNodeInfoL( rtNode, 
                allAccessTypes,
                MSmlDmDDFObject::EZeroOrMore, 
                MSmlDmDDFObject::EDynamic, 
                MSmlDmDDFObject::ENode, 
                KNSmlDMDynamicNodeDescr );
    
    // AppID
    MSmlDmDDFObject& appId = rtNode.AddChildObjectL( KNSmlDMImpsAppId );
    FillNodeInfoL( appId, 
                   accessTypesAddGet, 
                   MSmlDmDDFObject::EZeroOrOne, 
                   MSmlDmDDFObject::EDynamic, 
                   MSmlDmDDFObject::EChr, 
                   KNSmlDMImpsAppIDDescr );

    // Name
    MSmlDmDDFObject& name = rtNode.AddChildObjectL( KNSmlDMImpsName );
    FillNodeInfoL( name, 
                   allAccessTypes, 
                   MSmlDmDDFObject::EZeroOrOne, 
                   MSmlDmDDFObject::EDynamic, 
                   MSmlDmDDFObject::EChr, 
                   KNSmlDMImpsNameDescr );

    // PrefConRef
    MSmlDmDDFObject& prefConRef = rtNode.AddChildObjectL( KNSmlDMImpsPrefConRef );
    FillNodeInfoL( prefConRef, 
                   allAccessTypes, 
                   MSmlDmDDFObject::EZeroOrOne, 
                   MSmlDmDDFObject::EDynamic, 
                   MSmlDmDDFObject::EChr, 
                   KNSmlDMImpsPrefConRefDescr );

    // PrefAddr
    MSmlDmDDFObject& prefAddr = rtNode.AddChildObjectL( KNSmlDMImpsPrefAddr );
    FillNodeInfoL( prefAddr, 
                   allAccessTypes, 
                   MSmlDmDDFObject::EZeroOrOne, 
                   MSmlDmDDFObject::EDynamic, 
                   MSmlDmDDFObject::EChr, 
                   KNSmlDMImpsPrefAddrDescr );

    // AppAuth
    MSmlDmDDFObject& appAuth = rtNode.AddChildObjectL( KNSmlDMImpsAppAuth );
    FillNodeInfoL( appAuth, 
                   allAccessTypes, 
                   MSmlDmDDFObject::EZeroOrOne,
                   MSmlDmDDFObject::EDynamic,
                   MSmlDmDDFObject::ENode,
                   KNSmlDMImpsAppAuthDescr );
	
    // Run-time node under AppAuth
    MSmlDmDDFObject& rtNodeAppAuth = appAuth.AddChildObjectGroupL(); 
    FillNodeInfoL( rtNodeAppAuth, 
                   allAccessTypes, 
                   MSmlDmDDFObject::EZeroOrMore, 
                   MSmlDmDDFObject::EDynamic, 
                   MSmlDmDDFObject::ENode, 
                   KNSmlDMDynamicNodeDescr );

    // AAuthLevel
    MSmlDmDDFObject& AAuthLevel = rtNodeAppAuth.AddChildObjectL( KNSmlDMImpsAAuthLevel );
    FillNodeInfoL( AAuthLevel,
                   allAccessTypes,
                   MSmlDmDDFObject::EOne,
                   MSmlDmDDFObject::EDynamic,
                   MSmlDmDDFObject::EChr,
                   KNSmlDMImpsAAuthLevelDescr );

    // AAuthName
    MSmlDmDDFObject& AAuthName = rtNodeAppAuth.AddChildObjectL( KNSmlDMImpsAAuthName );
    FillNodeInfoL( AAuthName,
                   allAccessTypes,
                   MSmlDmDDFObject::EZeroOrOne,
                   MSmlDmDDFObject::EDynamic,
                   MSmlDmDDFObject::EChr,
                   KNSmlDMImpsAAuthNameDescr );

    // AAuthSecret
    MSmlDmDDFObject& AAuthSecret = rtNodeAppAuth.AddChildObjectL( KNSmlDMImpsAAuthSecret );
    FillNodeInfoL( AAuthSecret,
                   accessTypesASecret,
                   MSmlDmDDFObject::EZeroOrOne,
                   MSmlDmDDFObject::EDynamic,
                   MSmlDmDDFObject::EChr,
                   KNSmlDMImpsAAuthSecretDescr );

    // Services
    MSmlDmDDFObject& service = rtNode.AddChildObjectL( KNSmlDMImpsServices );
    FillNodeInfoL( service, 
                   accessTypesNoDel, 
                   MSmlDmDDFObject::EZeroOrOne, 
                   MSmlDmDDFObject::EDynamic, 
                   MSmlDmDDFObject::EChr, 
                   KNSmlDMImpsServicesDescr );

    // Ext
    MSmlDmDDFObject& extNode = rtNode.AddChildObjectL( KNSmlDMImpsExtNode );
    FillNodeInfoL( extNode, 
                   allAccessTypes, 
                   MSmlDmDDFObject::EZeroOrOne,
                   MSmlDmDDFObject::EDynamic,
                   MSmlDmDDFObject::ENode, 
                   KNSmlDMImpsExtNodeDescr );

    // PEPCompliant
    MSmlDmDDFObject& pepCompliant = extNode.AddChildObjectL( KNSmlDMImpsPepCompliant );
    FillNodeInfoL( pepCompliant, 
                   allAccessTypes, 
                   MSmlDmDDFObject::EOne, 
                   MSmlDmDDFObject::EDynamic, 
                   MSmlDmDDFObject::EBool, 
                   KNSmlDMImpsPepCompliantDescr );
            
	_DBG_FILE( "CNSmlDmImpsAdapter::DDFStructureL(): end" );
	}

// ----------------------------------------------------------------------------
// CNSmlDmImpsAdapter::AddNodeObjectL()
// The method adds new node.
// - Check number of uri segments
// - If number of segements = 2
//  	 - Read all dynamic nodes under IMPS from the DMTree
//  	 - Check that the new node name does not already exist
//	   - If exists
//	    --> return error status "AlreadyExists"
//	   - If does not exist, create new SAP to the database 
//              and use the node name as temporary SAP name
//	   - Map the received index to the URI.
// - If number of segments = 3 or 4
//     - Check that there exists a SAP with given LUID value in db
//	   - If does not exist
//		  --> return error status "NotFound"
//	   - If exists
//	    --> if number of segments = 4
//		    - check that there does not already exist the dynamic node 
//                  ( only one dynamic allowed )
//			  - if exists
// 			   --> Return error status invalid object
//	   - Set mapping for DB index and URI.
// ----------------------------------------------------------------------------
void CNSmlDmImpsAdapter::AddNodeObjectL( const TDesC8& aURI, 
        const TDesC8& aParentLUID, const TInt aStatusRef )
	{
	_DBG_FILE( "CNSmlDmImpsAdapter::AddNodeObjectL(): begin" );

    CIMPSSAPSettings* ownSap = CIMPSSAPSettings::NewL();
	CleanupStack::PushL( ownSap );
    CIMPSSAPSettingsStore* ownStore = CIMPSSAPSettingsStore::NewL();
	CleanupStack::PushL( ownStore );
    CIMPSSAPSettingsList* ownList = CIMPSSAPSettingsList::NewL();   
	CleanupStack::PushL( ownList );

    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;
    TInt numOfSegs = NSmlDmURI::NumOfURISegs( aURI );
    TPtrC8 checkedNode = NSmlDmURI::LastURISeg( aURI );
    
    if ( ( numOfSegs == 2 ) && 
         ( aURI.Match( KNSmlDMImpsRootAndDyn ) != KErrNotFound ) )
        {   
        status = CSmlDmAdapter::EOk;

        // Read all dynamic nodes under IMPS from tree
        CSmlDmAdapter::TError stat;
        CBufBase *allNodes = CBufFlat::NewL( KNSmlDMReservingData );
    	CleanupStack::PushL( allNodes );
	    iCallBack->FetchLinkL( KNSmlDMImpsNodeName, *allNodes, stat );

        // Check if the second node is found
        TInt startIndex = 0;        
        for( TInt i = 0; i < allNodes->Size(); i++ )
            {
            if( allNodes->Ptr(0)[i] == '/' || i == allNodes->Size()-1 )
                {
                TPtrC8 uriSeg8Ptr = allNodes->Ptr( startIndex ).Mid( 0, i-startIndex );    			                
                if ( uriSeg8Ptr.Compare( checkedNode ) == 0 )        
                    {
                    status = EAlreadyExists;
                    break;
                    }
                startIndex = i + 1;
                }  // if ( allNodes->Ptr )            
            } // for
        CleanupStack::PopAndDestroy(); // allNodes

        if  ( status == CSmlDmAdapter::EOk )
            {
            // the item does not exist - create it to the db    
            RFs fs;
            User::LeaveIfError( fs.Connect() );
            CleanupClosePushL( fs );
            if ( SysUtil::FFSSpaceBelowCriticalLevelL( &fs, 
                                                       KNSmlDMSAPSettings ) )
	            {
                _DBG_FILE( "CNSmlDmImpsAdapter::AddNodeObjectL(): DISK FULL end" );
                status = CSmlDmAdapter::EDiskFull; // Disk full
                }
            else
                {
                ResetSapL( ownSap );
                
                TBufC8<5> tmpName( KNSmlDMImpsName ); 
                status = UpdateValueL( tmpName, checkedNode, ownSap );
				if ( status == CSmlDmAdapter::EOk  )
    				{ 		        			    
					TUint32 uid32 = ownStore->StoreNewSAPL( ownSap, EIMPSIMAccessGroup );
					TInt uid( uid32 );
				    _DBG_FILE( "CNSmlDmImpsAdapter::AddNodeObjectL(): New SAP added" );
      		    	HBufC8 *luidDes = IntToDes8LC( uid );
	            	iCallBack->SetMappingL( aURI, *luidDes );
       		    	CleanupStack::PopAndDestroy(); // luidDes
    				}
                }  //disk size
            CleanupStack::PopAndDestroy(); // fs
            }
        }
	// AppAuth nodes        
    else if ( ( ( numOfSegs == 3 ) && 
                ( aURI.Match( KNSmlDMImpsWholeAppAuthUri ) != KErrNotFound ) )
           || ( ( numOfSegs == 4 ) && 
                ( aURI.Match( KNSmlDMImpsWholeAppAuthDynUri ) != KErrNotFound ) ) )
        {
    	  if( aParentLUID.Length() > 0 )
		    {
        // Check that Store is not empty
        // causes leaving if empty db is tried to read
        	TInt count = 0;
        	count = ownStore->SAPCountL( EIMPSAccessFilterAll );
        	DBG_ARGS8(_S8("IMPS::Count of SAPs in store = %d \n"), count );
            if ( count > 0 )
                {
    	        TInt uid = DesToInt( aParentLUID );
    	        TUint32 uid32( uid );
                ownStore->PopulateSAPSettingsListL( *ownList, 
                                                    EIMPSAccessFilterAll );
                _DBG_FILE( "CNSmlDmImpsAdapter::AddNodeObjectL(): SAP settings list" );
               	TBool found = EFalse;
                for ( TInt i = 0; i < count; i++ )
                    {
                    TUint32 id32 = ownList->UidForIndex( i );
                    if ( id32 == uid32 )
                        {
                        found = ETrue;
                        if ( numOfSegs == 4 )
                            {
                            // There can be only one dynamic node, 
                            // check that there is not one
                            TPtrC8 ptrToParentUri = NSmlDmURI::RemoveLastSeg( aURI );
	                            // Read dynamic nodes under AppAuth from tree
                            CSmlDmAdapter::TError stat;
                            CBufBase *dynNodes = CBufFlat::NewL( KNSmlDMReservingData );
    	                    CleanupStack::PushL( dynNodes );
	                        iCallBack->FetchLinkL( ptrToParentUri, *dynNodes, stat );
	                        TInt len = dynNodes->Size();	                        
                            if ( len > 0 )                             
                                {
                                status = EInvalidObject;
                                }
                            else
                                {
                                iCallBack->SetMappingL( aURI, aParentLUID );
                                }
                            CleanupStack::PopAndDestroy(); // dynNodes
                            }
                        else
                            {
            	    	    iCallBack->SetMappingL( aURI, aParentLUID );
                            }
                        break;
                        }
                    } // for
		                    
                if ( !found )
                    {
   	                status = CSmlDmAdapter::ENotFound;
       	            }            
                } // count
            }  // aParentLUID length
        else
        	{
	        status = CSmlDmAdapter::ENotFound;
            }  
        } // aParentLUID length
        
	// Ext node
   	else if ( ( numOfSegs == 3 ) && 
   	          ( aURI.Match( KNSmlDMImpsWholeExtUri ) != KErrNotFound ) )
		{
       	if( aParentLUID.Length() > 0 )
		    {
        // Check that Store is not empty
        // causes leaving if empty db is tried to read
        	TInt count = 0;
            count = ownStore->SAPCountL( EIMPSAccessFilterAll );
            DBG_ARGS8(_S8("IMPS::Count of SAPs in store = %d \n"), count );
            if ( count > 0 )
                {
   	        	TInt uid = DesToInt( aParentLUID );
   	        	TUint32 uid32( uid );
                ownStore->PopulateSAPSettingsListL( *ownList, EIMPSAccessFilterAll );
                _DBG_FILE( "CNSmlDmImpsAdapter::AddNodeobjectL(): SAP settings list" );
                TBool found = EFalse;
                for ( TInt i = 0; i < count; i++ )
                    {
                    TUint32 id32 = ownList->UidForIndex( i );
                    if ( id32 == uid32 )
                        {
                        found = ETrue;
          	    	    iCallBack->SetMappingL( aURI, aParentLUID );
                        break;
                        }
                    } // for
                if ( !found )
                    {
                    status = CSmlDmAdapter::ENotFound;
                    }            
	            } // count            	
            }  // aParentLUID length
        else
        	{
	        status = CSmlDmAdapter::ENotFound;
            }  
        } // aParentLUID length
        
    else 
        {
        status = CSmlDmAdapter::EInvalidObject;
        }

    CleanupStack::PopAndDestroy( 3 ); // ownList, ownStore, ownSap

    iCallBack->SetStatusL( aStatusRef, status );
    _DBG_FILE( "CNSmlDmImpsAdapter::AddNodeObjectL(): end" );

    return;
	}


// ----------------------------------------------------------------------------
//  CNSmlDmImpsAdapter::UpdateLeafObjectL()
//  The method updates the leaf value.
//  - Check that LUID length > 0
//	- Check that there exists SAP with uid which is same as received LUID value.
//  - If exists
//	  --> update received parameter value to db
//  - else
//	  --> return error status
// ----------------------------------------------------------------------------
void CNSmlDmImpsAdapter::UpdateLeafObjectL( const TDesC8& aURI, 
                                            const TDesC8& aLUID, 
                                            const TDesC8& aObject, 
											const TDesC8& /*aType*/, 
											TInt aStatusRef )	
    {
    _DBG_FILE( "CNSmlDmImpsAdapter::UpdateLeafObjectL(): begin" );

// Used only in Test purpose
//   TBuf8<50> tmpUriBuf = aURI;
//   TBuf8<50> tmpLuidBuf = aLUID;   
//   DBG_ARGS8(_S8("IMPS Luid / Uri : Luid = %S  Uri = %S \n"), &tmpLuidBuf, &tmpUriBuf );     

    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;

    CIMPSSAPSettings* ownSap = CIMPSSAPSettings::NewL();
    CleanupStack::PushL( ownSap );
    CIMPSSAPSettingsStore* ownStore = CIMPSSAPSettingsStore::NewL();
    CleanupStack::PushL( ownStore );

    TPtrC8 lastSeg = NSmlDmURI::LastURISeg( aURI );

    if( aLUID.Length() > 0 )
        {	    
        // Check that Store is not empty and find the correct SAP
        TInt sapFound = GetCorrectSapL( aLUID, 
                                        ownSap, 
                                        ownStore );                    	  	
        if ( sapFound == KErrNone )                    	  
            {        
            if ( lastSeg.Compare( KNSmlDMImpsPepCompliant ) == 0 )
                {
                TInt stat = UpdatePEPValueL( aURI, 
                                             aObject, 
                                             aLUID, 
                                             ownSap, 
                                             ownStore );
                if ( stat != KErrNone )
                    { 
                    status = CSmlDmAdapter::EError;
                    }
                }
            else if ( lastSeg.Compare( KNSmlDMImpsAppId ) == 0 )
                {
                // Only value "wA" is accepted for AppID parameter
                if ( aObject.Compare( KNSmlDMImpsDefaultAppID ) != 0 )
                    {
                    status = CSmlDmAdapter::EInvalidObject;
                    }	  		
                }			  				  	
            else
                {
                TInt uid = DesToInt( aLUID );
                TUint32 Uid32( uid );
                status = UpdateValueL( lastSeg, aObject, ownSap );        
                if ( status == KErrNone )
                    {    
                    ownStore->UpdateOldSAPL( ownSap, Uid32 );					
                    _DBG_FILE( "CNSmlDmImpsAdapter::UpdateLeafObjectL(): SAP updated" );
                    }		  	
                }
            }	// sapFound
    		  			  	
        else if ( sapFound == KErrNotFound )                    	 
            {      		
            status = CSmlDmAdapter::ENotFound;
            }  
        else
            {
            status = CSmlDmAdapter::EError;
            }
        } // aLUID length
    else
        {
        status = CSmlDmAdapter::ENotFound;
        }  

    iCallBack->SetStatusL( aStatusRef, status );

    CleanupStack::PopAndDestroy( 2 ); // ownStore, ownSap

    _DBG_FILE( "CNSmlDmImpsAdapter::UpdateLeafObjectL( ): end" );
    	
    return;
    }

// ----------------------------------------------------------------------------
//  CNSmlDmImpsAdapter::UpdateLeafObjectL()
//  Method not used. Used if streaming in use.
// ----------------------------------------------------------------------------
void CNSmlDmImpsAdapter::UpdateLeafObjectL( const TDesC8& /*aURI*/, 
                                            const TDesC8& /*aLUID*/, 
                                            RWriteStream*& /*aStream*/, 
                                            const TDesC8& /*aType*/, 
                                            TInt aStatusRef )
    {
    _DBG_FILE( "CNSmlDmImpsAdapter::UpdateLeafObjectL(): begin" );
    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
    _DBG_FILE( "CNSmlDmImpsAdapter::UpdateLeafObjectL( ): end" );
    }

// ----------------------------------------------------------------------------
// void CNSmlDmImpsAdapter::FetchLeafObjectSizeL()
// Fetches data object and returns its size.
// - Take last URI segment and request the value for requested parameter 
//      by using method FetchObjectL.
// - Take the size of received parameter value and change it to descriptor
// - Return the size and status
// ----------------------------------------------------------------------------
void CNSmlDmImpsAdapter::FetchLeafObjectSizeL( const TDesC8& aURI, 
                                               const TDesC8& aLUID, 
                                               const TDesC8& aType, 
                                               const TInt aResultsRef, 
                                               const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmImpsAdapter::FetchLeafObjectSizeL(): begin");

    CBufBase *currentURISegmentList = CBufFlat::NewL( KNSmlDMReservingMaxData );
    CleanupStack::PushL( currentURISegmentList );
    TPtrC8 fetchedLeaf = NSmlDmURI::LastURISeg( aURI );    

    CSmlDmAdapter::TError status = FetchObjectL( fetchedLeaf, 
                                                 aLUID, 
                                                 *currentURISegmentList );

    if ( status == CSmlDmAdapter::EOk )
        {
        TInt objSizeInBytes = currentURISegmentList->Size();
        TBuf8<16> stringObjSizeInBytes;
        stringObjSizeInBytes.Num( objSizeInBytes );
        currentURISegmentList->Reset();
        currentURISegmentList->InsertL( 0, stringObjSizeInBytes );

        iCallBack->SetResultsL( aResultsRef, *currentURISegmentList, aType );
        }

    iCallBack->SetStatusL( aStatusRef, status );
    CleanupStack::PopAndDestroy();  //currentURISegmentList
	
    _DBG_FILE("CNSmlDmImpsAdapter::FetchLeafObjectSizeL(): end");
    }
    
// ----------------------------------------------------------------------------
// CNSmlDmImpsAdapter::DeleteObjectL()
// The method deletes the requested parameter values.
// - Check number of segments
// - If number of segments > 2
// 	 --> check that the SAP which the deletion involves exists in DB 
//          and get SAP to ownSap
//   - If number of segments = 2
//	   --> delete whole SAP from the DB
//	 - If number of segments > 2 and <= 5
//	   --> check the requested parameter and delete the value from it in DB
// - If number of segments < 1 or > 5 or given SAP is not found from DB
//	 --> return error status
// ----------------------------------------------------------------------------
void CNSmlDmImpsAdapter::DeleteObjectL( const TDesC8& aURI, 
                                        const TDesC8& aLUID, 
                                        TInt aStatusRef )
    {
    _DBG_FILE( "CNSmlDmImpsAdapter::DeleteLeafObjectL( ): begin" );

    CIMPSSAPSettings* ownSap = CIMPSSAPSettings::NewL();
    CleanupStack::PushL( ownSap );
    CIMPSSAPSettingsStore* ownStore = CIMPSSAPSettingsStore::NewL();
    CleanupStack::PushL( ownStore );

    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;

    // Check the number of uri segments to find out is a node or a leaf deleted 
    TInt numOfSegs = NSmlDmURI::NumOfURISegs( aURI );

    if ( numOfSegs < 2 || numOfSegs > 5 )
        {
        // Sap does not exist, return error status
        status = CSmlDmAdapter::EInvalidObject;
        iCallBack->SetStatusL( aStatusRef, status );
        return;
        }

    if( aLUID.Length() > 0 )
        {
        // Check that Store is not empty, 
        // causes leaving if empty db is tried to read
        TInt sapFound = KErrNotFound;
        sapFound = GetCorrectSapL( aLUID, 
                                   ownSap, 
                                   ownStore ); 
        if ( sapFound == KErrNone )
            {   		
            TInt uid = DesToInt( aLUID );
            TUint32 Uid32( uid );
            if ( numOfSegs == 2 )
                {
                // delete SAP
                ownStore->DeleteSAPL( Uid32 );
                _DBG_FILE( "CNSmlDmImpsAdapter::DeleteObjectL(): SAP deleted" );
                }  // if numOfSegs
            else 
                {            
                TPtrC8 deletedLeaf = NSmlDmURI::LastURISeg( aURI );    
                if ( ( deletedLeaf.Compare( KNSmlDMImpsPepCompliant ) == 0 ) ||              
                   ( deletedLeaf.Compare( KNSmlDMImpsExtNode ) == 0 ) )
                    {
                    TBuf8<5> tmpPep( KNSmlDMImpsDefaultIM );  //False		    	    
                    TInt stat = EOk;
                    if ( deletedLeaf.Compare( KNSmlDMImpsExtNode ) == 0 )
                        {
                        HBufC8 *tmpUri = HBufC8::NewLC( aURI.Length() + 14 );
                        TPtr8 tmpUriPtr = tmpUri->Des();

                        // URI: IMPS/<x>/PEPCompliant
                        tmpUriPtr.Format( aURI );
                        tmpUriPtr.Append( KNSmlDMImpsSeparator8 );
                        tmpUriPtr.Append( KNSmlDMImpsPepCompliant );
                        stat = UpdatePEPValueL( tmpUriPtr, 
                                                tmpPep, 
                                                aLUID, 
                                                ownSap, 
                                                ownStore );
                        CleanupStack::PopAndDestroy();  // tmpUri
                        }
                    else
                        {
                        stat = UpdatePEPValueL( aURI, 
                                                tmpPep, 
                                                aLUID, 
                                                ownSap, 
                                                ownStore );                    
                        }
                    if ( stat != KErrNone )
                        { 
                        status = CSmlDmAdapter::EError;
                        }
                    }  // stat
                else
                    {
                    status  = DeleteValueL( aURI, ownSap );
                    if ( status == EOk )
                        {                        
                        // update the SAP to db 
                        ownStore->UpdateOldSAPL( ownSap, Uid32 );
                        _DBG_FILE( "CNSmlDmImpsAdapter::DeleteObjectL(): SAP updated" );
                        }			  		                	
                    }  // status 
                } // numOfSegs
            }  // sapFound
        else if ( sapFound == KErrNotFound )                    	 
            {      		
            status = CSmlDmAdapter::ENotFound;
            }  
        else
            {
            status = CSmlDmAdapter::EError;
            }
        }  // LUID length
    else
        {
        status = CSmlDmAdapter::ENotFound;
        }
    
    iCallBack->SetStatusL( aStatusRef, status );

    CleanupStack::PopAndDestroy( 2 );  // ownSap, ownStore

    _DBG_FILE( "CNSmlDmImpsAdapter::DeleteLeafObjectL( ): end" );

    return;
    }
    
// ----------------------------------------------------------------------------
//  CNSmlDmImpsAdapter::FetchLeafObjectL()
// The method returns the requested parameter value from DB
// - Take last URI segment and request the value for requested parameter 
//          by using method FetchObjectL.
// - Return the parameter value and status
// ----------------------------------------------------------------------------
void CNSmlDmImpsAdapter::FetchLeafObjectL( const TDesC8& aURI, 
                                           const TDesC8& aLUID, 
                                           const TDesC8& aType, 
                                           TInt aResultsRef, 
                                           TInt aStatusRef )
    {
    _DBG_FILE( "CNSmlDmImpsAdapter::FetchLeafObjectL(): begin" );    

    CBufBase *currentURISegmentList = CBufFlat::NewL( KNSmlDMReservingMaxData );
    CleanupStack::PushL( currentURISegmentList );
    TPtrC8 fetchedLeaf = NSmlDmURI::LastURISeg( aURI );    
    CSmlDmAdapter::TError status = FetchObjectL( fetchedLeaf, 
                                                 aLUID, 
                                                 *currentURISegmentList );

    if ( status == CSmlDmAdapter::EOk )
        {
        iCallBack->SetResultsL( aResultsRef, *currentURISegmentList, aType );
        }

    iCallBack->SetStatusL( aStatusRef, status );
    CleanupStack::PopAndDestroy();  //currentURISegmentList

    _DBG_FILE( "CNSmlDmImpsAdapter::FetchLeafObjectL(): end" );

    return;
    }


// ----------------------------------------------------------------------------
//  void CNSmlDmImpsAdapter::ChildURIListL()
//  The method returns the child nodes or leafs under requested node. 
//  - Check number of URI segments. 
//  - If number of segments = 1
//    --> check if the dynamic node name is found by setting the DB uid
//              against the LUID value in aPreviousURISegmentList 
// 	      - If the name is found, it is taken to the result list, 
//		  - else the index number of SAP is converted to the name 
//              and returned as node name
//  - If number of segments > 1 and < 5
//	  --> check that given LUID is found as index in DB 
//              and get the SAP data to ownSap
//      - if number of segments = 2 or 4
//		  --> return parameter under dynamic node
// 	    - if number of segments = 3
//		  --> return the name of dynamic node under AppAuth node.
//  - Return the results and status
//  - In error case return error status
// ----------------------------------------------------------------------------
void CNSmlDmImpsAdapter::ChildURIListL( const TDesC8& aURI, 
                                        const TDesC8& aLUID, 
                                        const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, 
                                        TInt aResultsRef, 
                                        TInt aStatusRef )
    {
    _DBG_FILE( "CNSmlDmImpsAdapter::ChildURIListL(): begin" );
        
// Used only in Test purpose
//   TBuf8<50> tmpUriBuf = aURI;
//   TBuf8<50> tmpLuidBuf = aLUID;   
//   DBG_ARGS8(_S8("IMPS Luid / Uri : Luid = %S  Uri = %S \n"), &tmpLuidBuf, &tmpUriBuf );     
        
    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;    
                        
    CIMPSSAPSettings* ownSap = CIMPSSAPSettings::NewL();
    CleanupStack::PushL( ownSap );
    CIMPSSAPSettingsStore* ownStore = CIMPSSAPSettingsStore::NewL();
    CleanupStack::PushL( ownStore );
    CIMPSSAPSettingsList* ownList = CIMPSSAPSettingsList::NewL();   
    CleanupStack::PushL( ownList );

    TInt numOfSegs = NSmlDmURI::NumOfURISegs( aURI );

    CBufBase *currentURISegmentList = CBufFlat::NewL( KNSmlDMReservingMaxData );
    CleanupStack::PushL( currentURISegmentList );

    if ( numOfSegs == 1 )
        {
        if ( aURI.Compare( KNSmlDMImpsNodeName ) == 0 )
            {   
            ownStore->PopulateSAPSettingsListL( *ownList, EIMPSAccessFilterAll );
            _DBG_FILE( "CNSmlDmImpsAdapter::ChildURIListL(): SAP settings list" );
            TInt32 id32; 
            TInt count = 0;
            count = ownStore->SAPCountL( EIMPSAccessFilterAll );			
            DBG_ARGS8(_S8("IMPS::Count of SAPs in store = %d "), count );
            TInt prevUriSegListCount = aPreviousURISegmentList.Count();
            DBG_ARGS8(_S8("IMPS::Count of luids in prevUriSegList = %d "), 
                           prevUriSegListCount );
			
            for ( TInt i = 0; i < count; i++ )
                {
                id32 = ownList->UidForIndex( i );
                TInt id( id32 );
                DBG_ARGS8(_S8("IMPS::Sap id = %d"), id );
		
                HBufC8* name=0;
                TBool found = EFalse;
                // Go through the previousUriSegmentList
			 
                for( TInt prevListIx = 0; 
                     prevListIx < aPreviousURISegmentList.Count(); 
                     prevListIx++ )
                    {
                     // Check if the Sap id is found from segment list
                    if( id == DesToInt( aPreviousURISegmentList.At( prevListIx ).iURISegLUID ) )
                        {
                        found = ETrue;
                        // If id is found, take the urisegment from segment list
                        name = aPreviousURISegmentList.At( prevListIx ).iURISeg.AllocLC();
                        DBG_ARGS8(_S8("InList: Id = %d Seg %S"), id, 
                                  &aPreviousURISegmentList.At( prevListIx ).iURISeg);                                            
                        break;
                        }
                    } // for prevListIx
                if( !found )
                    {
                    // id not found from previousUriSegmentList
                    // change the id to segment name

                    name = HBufC8::NewLC( 20 ); 
                    TPtr8 namePtr = name->Des();
                    namePtr = KNSMLDMImpsDynNodeName;
                    namePtr.AppendNum( i+1 );

                    HBufC8 *uri = HBufC8::NewLC( aURI.Length() + name->Length() + 1 );
                    TPtr8 uriPtr = uri->Des();
                    // Format the whole URI
                    uriPtr.Format( aURI );
                    uriPtr.Append( KNSmlDMImpsSeparator );
                    uriPtr.Append( name->Des() );
                    HBufC8 *luid = IntToDes8LC( id );
                    // Map the received uri to id
                    iCallBack->SetMappingL( uriPtr, *luid );
    	    		
// Used only in Test purpose
//    	    		TBuf8<50> uriBuf = uriPtr;
//                    DBG_ARGS8(_S8("NotInList, mapping done: Luid = %d  Uri = %S "), 
//                                   id, &uriBuf );
      	    		   		
                    CleanupStack::PopAndDestroy( 2 ); //uri,luid
                    }                    
                TPtr8 namePtr = name->Des();
                currentURISegmentList->InsertL( currentURISegmentList->Size(), 
                                                namePtr );
                if( i + 1 < count )
                    {
                    currentURISegmentList->InsertL( currentURISegmentList->Size(), 
                                                    KNSmlDMImpsSeparator8 );
                    }
                CleanupStack::PopAndDestroy( ); // name              
                } // for i < count
              
            iCallBack->SetResultsL( aResultsRef, 
                                    *currentURISegmentList, 
                                    KNullDesC8 );	            			                
            }  // aUri
        else 
            {
            status = CSmlDmAdapter::EInvalidObject;
            }  // aUri
        } 
    else if ( ( numOfSegs > 1 ) && ( numOfSegs < 5 ) )
        {
        if( aLUID.Length() > 0 )
            {                   
            // Check that Store is not empty
            // causes leaving if empty db is tried to read
            TInt sapFound = KErrNotFound;
            sapFound = GetCorrectSapL( aLUID, ownSap, ownStore );
            if ( sapFound == KErrNone)
                {
                if ( ( numOfSegs == 2 ) && 
                     ( aURI.Match( KNSmlDMImpsRootAndDyn ) != KErrNotFound ) )
                    {
                    // List leafs
                    currentURISegmentList->InsertL( currentURISegmentList->Size(), 
                                                    KNSmlDMImpsRtNode() );
                                                                        
                    iCallBack->SetResultsL( aResultsRef, 
                                            *currentURISegmentList, 
                                            KNullDesC8 );
                    }  // numOfSegs
                else if ( ( numOfSegs == 3 ) && 
                          ( aURI.Match( KNSmlDMImpsWholeExtUri ) != KErrNotFound ) )
                    {
                    currentURISegmentList->InsertL( currentURISegmentList->Size(), 
                                                    KNSmlDMImpsPepCompliant() );            
                    iCallBack->SetResultsL( aResultsRef, 
                                            *currentURISegmentList, 
                                            KNullDesC8 );
                    }
                else if ( ( numOfSegs == 3 ) && 
                          ( aURI.Match( KNSmlDMImpsWholeAppAuthUri ) != KErrNotFound ) )
                    {
                    HBufC8* name=0;
                    TInt count = aPreviousURISegmentList.Count();
                    if ( count > 0 ) 
                        {
                        for( TInt prevListIx = 0; prevListIx < count; prevListIx++ )
                            {
                            name = aPreviousURISegmentList.At( prevListIx ).iURISeg.AllocLC();
                            TPtr8 namePtr = name->Des();
                            currentURISegmentList->InsertL( currentURISegmentList->Size(), 
                                                            namePtr );                 
                            if( prevListIx + 1 < count )
                                {
                                currentURISegmentList->InsertL( currentURISegmentList->Size(), 
                                                                KNSmlDMImpsSeparator8 );
                                }
                            CleanupStack::PopAndDestroy();  // name
                            } // for
                        }  // if count > 0
                    else
                        {
                        if ( ( ownSap->SAPUserId().Compare(KNSmlDMImpsNotDefined ) == 0 ) &&
                             ( ownSap->SAPUserPassword().Compare(KNSmlDMImpsNotDefined ) == 0 ) )
                            {                    
                            }
                        else
                            {
                            HBufC8 *uri = HBufC8::NewLC( aURI.Length() + 9 );
                            TPtr8 uriPtr = uri->Des();
                            uriPtr.Format( aURI );
                            uriPtr.Append( KNSmlDMImpsSeparator );
                            uriPtr.Append( KNSmlDMRtNode );
                            iCallBack->SetMappingL( uriPtr, aLUID );
                            currentURISegmentList->InsertL( currentURISegmentList->Size(), 
                                                            KNSmlDMRtNode8() );
                            CleanupStack::PopAndDestroy(); //uri
                            }
                        }
                    iCallBack->SetResultsL( aResultsRef, 
                                            *currentURISegmentList, 
                                            KNullDesC8 );                              
                    }
                else if ( ( numOfSegs == 4 ) && 
                          ( aURI.Match( KNSmlDMImpsWholeAppAuthDynUri ) != KErrNotFound ) )
                    {
                    currentURISegmentList->InsertL( currentURISegmentList->Size(), 
                                                    KNSmlDMAppAuthRtNode() );            
                    iCallBack->SetResultsL( aResultsRef, 
                                            *currentURISegmentList, 
                                            KNullDesC8 );
                    }                                                              
                else
                    {
                    status = CSmlDmAdapter::EInvalidObject;
                    }
                }  // sapFound
            else if ( sapFound == KErrNotFound )                    	 
                {      		
                status = CSmlDmAdapter::ENotFound;
                }  
            else
                {
                status = CSmlDmAdapter::EError;
                }
            }  // luid len
        else  
            {
            status = CSmlDmAdapter::ENotFound;
            }
        } // numOfSegs
    else 
        {
        status = CSmlDmAdapter::EInvalidObject;
        }

// Used only in Test purpose
    DBG_ARGS8(_S8("Status = %d \n"), status );       
        
    iCallBack->SetStatusL( aStatusRef, status );
    
    CleanupStack::PopAndDestroy( 4 ); // currentUriSegmentList, ownSap, ownStore, ownList

    _DBG_FILE( "CNSmlDmImpsAdapter::ChildURIListL(): end" );
    return;
    }

// ----------------------------------------------------------------------------
// CNSmlDmImpsAdapter::CompleteOutstandingCmdsL()
// ----------------------------------------------------------------------------
void CNSmlDmImpsAdapter::CompleteOutstandingCmdsL()
    {
    _DBG_FILE( "CNSmlDmImpsAdapter::EndMessageL(): begin" );
    _DBG_FILE( "CNSmlDmImpsAdapter::EndMessageL(): end" );
    }

//==============================================================================
// CNSmlDmImpsAdapter::ExecuteCommandL()
//==============================================================================
void CNSmlDmImpsAdapter::ExecuteCommandL( const TDesC8& /*aURI*/, 
                                          const TDesC8& /*aLUID*/, 
                                          const TDesC8& /*aArgument*/, 
                                          const TDesC8& /*aType*/, 
                                          const TInt aStatusRef )
    {
    //Not supported
    _DBG_FILE( "CNSmlDmImpsAdapter::ExecuteCommandL(): begin" );
    iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    _DBG_FILE( "CNSmlDmImpsAdapter::ExecuteCommandL(): end" );
    }

//==============================================================================
// CNSmlDmImpsAdapter::ExecuteCommandL()
//==============================================================================
void CNSmlDmImpsAdapter::ExecuteCommandL( const TDesC8& /*aURI*/, 
                                          const TDesC8& /*aLUID*/, 
                                          RWriteStream*& /*aStream*/, 
                                          const TDesC8& /*aType*/, 
                                          const TInt aStatusRef )
    {
    //Not supported
    _DBG_FILE( "CNSmlDmImpsAdapter::ExecuteCommandL(): begin" );
    iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    _DBG_FILE( "CNSmlDmImpsAdapter::ExecuteCommandL(): end" );
    }

//==============================================================================
// CNSmlDmImpsAdapter::CopyCommandL()
//==============================================================================
void CNSmlDmImpsAdapter::CopyCommandL( const TDesC8& /*aTargetURI*/, 
                                       const TDesC8& /*aTargetLUID*/, 
                                       const TDesC8& /*aSourceURI*/, 
                                       const TDesC8& /*aSourceLUID*/, 
                                       const TDesC8& /*aType*/, 
                                       TInt aStatusRef )
    {    
    //Not supported
    _DBG_FILE( "CNSmlDmImpsAdapter::CopyCommandL(): begin" );
    iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    _DBG_FILE( "CNSmlDmImpsAdapter::CopyCommandL(): end" );
    }

//==============================================================================
// CNSmlDmImpsAdapter::StartAtomicL()
//==============================================================================
void CNSmlDmImpsAdapter::StartAtomicL()
    {
    //Not supported
    _DBG_FILE( "CNSmlDmImpsAdapter::StartAtomicL(): begin" );
    _DBG_FILE( "CNSmlDmImpsAdapter::StartAtomicL(): end" );
    }

//==============================================================================
// CNSmlDmImpsAdapter::CommitAtomicL()
//==============================================================================
void CNSmlDmImpsAdapter::CommitAtomicL()
    {
    //Not supported
    _DBG_FILE( "CNSmlDmImpsAdapter::CommitAtomicL(): begin" );
    _DBG_FILE( "CNSmlDmImpsAdapter::CommitAtomicL(): end" );
    }
    
//==============================================================================
// CNSmlDmImpsAdapter::RollbackAtomicL()
//==============================================================================
void CNSmlDmImpsAdapter::RollbackAtomicL()
    {
    //Not supported
    _DBG_FILE( "CNSmlDmImpsAdapter::RollbackAtomicL(): begin" );
    _DBG_FILE( "CNSmlDmImpsAdapter::RollbackAtomicL(): end" );
    }

//==============================================================================
// CNSmlDmImpsAdapter::StreamingSupport()
//==============================================================================
TBool CNSmlDmImpsAdapter::StreamingSupport( TInt& /*aItemSize*/ )
    {
    _DBG_FILE( "CNSmlDmImpsAdapter::StreamingSupport(): begin" );
    _DBG_FILE( "CNSmlDmImpsAdapter::StreamingSupport(): end" );
    return EFalse;
    }

//==============================================================================
// CNSmlDmImpsAdapter::StreamCommittedL()
//==============================================================================
void CNSmlDmImpsAdapter::StreamCommittedL()
    {
    //Not supported
    _DBG_FILE( "CNSmlDmImpsAdapter::StreamCommittedL(): begin" );
    _DBG_FILE( "CNSmlDmImpsAdapter::StreamCommittedL(): end" );
    }


// ----------------------------------------------------------------------------
// Converts 16 bit descriptor to integer value.
// ----------------------------------------------------------------------------
TInt CNSmlDmImpsAdapter::DesToInt( const TDesC& aLuid )
    {
    TLex16 lex( aLuid );
    TInt value = 0;
    lex.Val( value );
    return value;
    }

// ----------------------------------------------------------------------------
// Converts 8 bit descriptor to integer value.
// ----------------------------------------------------------------------------
TInt CNSmlDmImpsAdapter::DesToInt( const TDesC8& aLuid )
    {
    TLex8 lex( aLuid );
    TInt value = 0;
    lex.Val( value );
    return value;
    }

// ----------------------------------------------------------------------------
// Converts integer value to 8 bit descriptor.
// ----------------------------------------------------------------------------
HBufC8* CNSmlDmImpsAdapter::IntToDes8LC( const TInt aLuid )
    {
    HBufC8* buf = HBufC8::NewLC( 10 ); //10 = max length of 32bit integer
    TPtr8 ptrBuf = buf->Des();
    ptrBuf.Num( aLuid );
    return buf;
    }

// ----------------------------------------------------------------------------
// Converts integer value to 16 bit descriptor.
// ----------------------------------------------------------------------------
HBufC* CNSmlDmImpsAdapter::IntToDesLC( const TInt aLuid )
    {
    HBufC* buf = HBufC::NewLC( 10 ); //10 = max length of 32bit integer
    TPtr ptrBuf = buf->Des();
    ptrBuf.Num( aLuid );
    return buf;
    }

// ----------------------------------------------------------------------------
// CNSmlDmImpsAdapter::FillNodeInfoL()
// Fills the node info in ddf structure
// ----------------------------------------------------------------------------
void CNSmlDmImpsAdapter::FillNodeInfoL( MSmlDmDDFObject& aNode, 
                                        const TSmlDmAccessTypes aAccTypes, 
                                        MSmlDmDDFObject::TOccurence aOccurrence,
                                        MSmlDmDDFObject::TScope aScope, 
                                        MSmlDmDDFObject::TDFFormat aFormat, 
                                        const TDesC8& aDescription )
    {
    aNode.SetAccessTypesL( aAccTypes );
    aNode.SetOccurenceL( aOccurrence );
    aNode.SetScopeL( aScope );
    aNode.SetDFFormatL( aFormat );
    if( aFormat != MSmlDmDDFObject::ENode )
        {
        aNode.AddDFTypeMimeTypeL( KNSmlDMImpsTextPlain );
        }
    aNode.SetDescriptionL( aDescription );
    }

// ----------------------------------------------------------------------------
// CNSmlDmImpsAdapter::ConvertTo8LC()
// Converts string value to 8-bit
// ----------------------------------------------------------------------------
TDesC8& CNSmlDmImpsAdapter::ConvertTo8LC( const TDesC& aSource )
    {    
    HBufC8* buf = HBufC8::NewLC( aSource.Length()*2 );
    TPtr8 bufPtr = buf->Des();
    CnvUtfConverter::ConvertFromUnicodeToUtf8( bufPtr, aSource );

    return *buf;
    }

// ----------------------------------------------------------------------------
// CNSmlDmImpsAdapter::ConvertTo16LC()
// Converts string value to 16-bit
// ----------------------------------------------------------------------------
TDesC16& CNSmlDmImpsAdapter::ConvertTo16LC( const TDesC8& aSource )
    {
    HBufC16* buf16 = HBufC16::NewLC( aSource.Length() );
    TPtr bufPtr16 = buf16->Des();

    CnvUtfConverter::ConvertToUnicodeFromUtf8( bufPtr16, aSource );

    return *buf16;
    }

// ----------------------------------------------------------------------------
// CNSmlDmImpsAdapter::ResetSapL();
// Initializes all fields with empty string.
// ----------------------------------------------------------------------------
void CNSmlDmImpsAdapter::ResetSapL( CIMPSSAPSettings* aSap )
    {
    aSap->SetSAPNameL( KNSmlDMImpsNotDefined );
    aSap->SetSAPAddressL( KNSmlDMImpsNotDefined );
    aSap->SetSAPUserIdL( KNSmlDMImpsNotDefined );
    aSap->SetSAPUserPasswordL( KNSmlDMImpsNotDefined );
    aSap->SetAccessPoint( 0 );
    aSap->SetHighLevelServices( EIMPSServicesUnknown ); 
    return;
    }

// ----------------------------------------------------------------------------
// CNSmlDmImpsAdapter::UpdateValueL();
// Updates the given leaf value. 
// Checks that the object is not too long and returns error status if is.
// ----------------------------------------------------------------------------
CSmlDmAdapter::TError CNSmlDmImpsAdapter::UpdateValueL( const TDesC8& aUpdatedLeaf, 
                                                        const TDesC8& aObject, 
                                                        CIMPSSAPSettings* aSap )
    {
    _DBG_FILE( "CNSmlDmImpsAdapter::UpdateValueL(): begin" );		
    
    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;

    if ( aUpdatedLeaf.Compare( KNSmlDMImpsName ) == 0 )
        {
        // Check the length of SapName
        if ( aObject.Length() <= KNSmlDMMaxNameLen )
            {
            aSap->SetSAPNameL( ConvertTo16LC( aObject ) );
            CleanupStack::PopAndDestroy();  // ConvertTo16LC            
            }
        else
            {
            // length of SapName is too long
            status = CSmlDmAdapter::ETooLargeObject; 
            }
        }    
    else if ( aUpdatedLeaf.Compare( KNSmlDMImpsPrefAddr ) == 0 )
        {
        // Check the length of PrefAddr
        if ( aObject.Length() <= KNSmlDMMaxAddrLen )
            {
            aSap->SetSAPAddressL( ConvertTo16LC( aObject ) );
            CleanupStack::PopAndDestroy(); // ConvertTo16LC
            }
        else
            {
            // length of PrefAddr is too long
            status = CSmlDmAdapter::ETooLargeObject; 
            }                
        }
    else if ( aUpdatedLeaf.Compare( KNSmlDMImpsAAuthName ) == 0 )
        {
        // Check the length of AAuthName
        if ( aObject.Length() <= KNSmlDMMaxNameLen )
            {
            aSap->SetSAPUserIdL( ConvertTo16LC( aObject ) );
            CleanupStack::PopAndDestroy(); // ConvertTo16LC
            }
        else
            {
            // length of AuthName is too long
            status = CSmlDmAdapter::ETooLargeObject; 
            }        
        }
    else if ( aUpdatedLeaf.Compare( KNSmlDMImpsAAuthSecret ) == 0 )
        {
        // Check the length of AAuthSecret
        if ( aObject.Length() <= KNSmlDMMaxNameLen )
            {
            aSap->SetSAPUserPasswordL( ConvertTo16LC( aObject ) );
            CleanupStack::PopAndDestroy(); // ConvertTo16LC
            }
        else
            {
            // length of AuthSecret is too long
            status = CSmlDmAdapter::ETooLargeObject; 
            }        
        }        
    else if ( aUpdatedLeaf.Compare( KNSmlDMImpsPrefConRef ) == 0 )
        {
        // find out the access point link        
        TPtrC8 objectPtr= NSmlDmURI::RemoveDotSlash( aObject );

// Used only in Test purpose
//   TBuf8<50> tmpObjBuf = objectPtr;   
//   DBG_ARGS8(_S8("IMPS : AP = %S \n"), &tmpObjBuf );     

        HBufC8* luid = iCallBack->GetLuidAllocL( objectPtr );
        CleanupStack::PushL( luid );

// Used only in Test purpose
//        DBG_ARGS8(_S8("Luid length = %d \n"), luid->Length() );     

        if( luid->Length() > 0 )
            {
            TInt uid = DesToInt( *luid );
            TUint32 uid32( uid );
            aSap->SetAccessPoint( uid32 );  
            }
        else
            {
            status = CSmlDmAdapter::ENotFound;
            }

        CleanupStack::PopAndDestroy(); // luid        
        }
            
    else if ( aUpdatedLeaf.Compare( KNSmlDMImpsAAuthLevel ) == 0 )
        {
        if ( ( aObject.Length() > 0 ) && 
             ( aObject.Compare( KNSmlDMImpsDefaultAAuthLevel ) != 0 ) )
            {
            status = CSmlDmAdapter::EInvalidObject;
            }
        }       
    else if ( aUpdatedLeaf.Compare( KNSmlDMImpsServices ) == 0 )
        {
        // Change the text to int value    
        TUint32 serv = EIMPSServicesUnknown;

        TBool imFlag = EFalse;
        TBool psFlag = EFalse;
        TBool grFlag = EFalse;
        
        TInt len = aObject.Length();
        TInt startIndex = 0;  // tells the start point of value
        TInt endIndex = 1;  // tells the end point of value
        while ( endIndex < len )
            {
//            TPtrC8 objPtr = aObject.Mid( startIndex, 2 );
            TBufC8<2> str = aObject.Mid( startIndex, 2 );
            if ( str.Match( KNSmlDMImpsServiceIM ) != KErrNotFound )
                {
                if ( imFlag == EFalse )
                    {
                    // IM service found	
                    serv |= EIMPSServicesIM;
                    imFlag = ETrue;
                    }                    	    					                
                else
                    {
                    status = CSmlDmAdapter::EInvalidObject;
                    break;
                    }            
                }
            else if ( str.Match( KNSmlDMImpsServicePS ) != KErrNotFound )
                {
                if ( psFlag == EFalse )
                    {
                    // PS service found	
                    serv |= EIMPSServicesPEC;
                    psFlag = ETrue;
                    }                    	    					                
                else
                    {
                    status = CSmlDmAdapter::EInvalidObject;
                    break;
                    }            
                }
            else if ( str.Match( KNSmlDMImpsServiceGR ) != KErrNotFound )
                {
                if ( grFlag == EFalse )
                    {
                    // GR service found	
                    serv |= EIMPSServicesGroups;
                    grFlag = ETrue;
                    }                    	    					                
                else
                    {
                    status = CSmlDmAdapter::EInvalidObject;
                    break;
                    }            
                }      	    	          
            else
                {
                status = CSmlDmAdapter::EInvalidObject;
                break;
                }            
            startIndex = endIndex + 1;
            if ( startIndex < len )
                {
                TBufC8<1> chr = aObject.Mid( startIndex, 1 );
                if ( chr.Match( KNSmlDMImpsSemiColon ) == KErrNotFound )
                    {
                    status = CSmlDmAdapter::EInvalidObject;
                    break;                    
                    }
                }
            startIndex++;
            endIndex = startIndex + 1;
            }  // while

            // Set Services
        if ( status == KErrNone )
            {
            aSap->SetHighLevelServices( serv );	    	                    
            }	
        }  // KNSmlDMImpsServices
    else  // Unknown leaf
        {
        status = CSmlDmAdapter::EInvalidObject;
        }        

    _DBG_FILE( "CNSmlDmImpsAdapter::UpdateValueL(): end" );		
            
    return status;
    }

// ----------------------------------------------------------------------------
// CNSmlDmImpsAdapter::UpdatePEPValueL();
// If PEP value is changed, deletes the existing SAP and creates new one with 
// received access group and sets new mappings to all nodes and leafs
// ----------------------------------------------------------------------------
CSmlDmAdapter::TError CNSmlDmImpsAdapter::UpdatePEPValueL( const TDesC8& aURI, 
                                                           const TDesC8& aObject, 
                                                           const TDesC8& aLUID, 
                                                           CIMPSSAPSettings* aSap, 
                                                           CIMPSSAPSettingsStore* aStore )
    {	
    _DBG_FILE( "CNSmlDmImpsAdapter::UpdatePEPValueL(): begin" );		

    HBufC8 *object = HBufC8::NewLC( aObject.Length() );
    TPtr8 objPtr = object->Des();
    objPtr.Format( aObject );
    objPtr.UpperCase();
    
    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;
    TInt uid = DesToInt ( aLUID );
    TUint32 uid32( uid );
    
    TIMPSAccessGroup newAccGroup = EIMPSIMAccessGroup;
    if ( ( objPtr.Compare( KNSmlDMImpsDefaultPEC ) == 0 ) 
        || ( objPtr.Compare( KNSmlDMImpsDefPEC ) == 0 ) )
        {
        newAccGroup = EIMPSPECAccessGroup;
        }
    else if ( ( objPtr.Compare( KNSmlDMImpsDefaultIM ) == 0 ) 
        || ( objPtr.Compare( KNSmlDMImpsDefIM ) == 0 ) )
        {
        newAccGroup = EIMPSIMAccessGroup;
        }
    else
        {
        status = CSmlDmAdapter::EInvalidObject;
        }        

    CleanupStack::PopAndDestroy(); // object
 
    TIMPSAccessGroup oldAccGroup = aSap->AccessGroup();
    if ( oldAccGroup != newAccGroup )
        {	        
        // The AccessGroup should be changed.    	        
        // Check mapping first
        
        TPtrC8 ptrToExtUri = NSmlDmURI::RemoveLastSeg( aURI ); // IMPS/<x>/Ext            
        TPtrC8 ptrToDynNode = NSmlDmURI::RemoveLastSeg( ptrToExtUri ); // IMPS/<x>

        HBufC8 *appAuthUri = HBufC8::NewLC( ptrToDynNode.Length() + 
                                     KNSmlDMMaxNameLen );
        TPtr8 appAuthPtr= appAuthUri->Des();

        // URI: IMPS/<x>/AppAuth
        appAuthPtr.Format( ptrToDynNode );
        appAuthPtr.Append( KNSmlDMImpsSeparator8 );
        appAuthPtr.Append( KNSmlDMImpsAppAuth );
		
// Used only in Test purpose
//        TBuf8<50> tmpUriBuf = appAuthPtr;
//        DBG_ARGS8(_S8("Fetched Uri = %S"), &tmpUriBuf );     

        // Get dynamic node under AppAuth
        CBufBase *allNodes = CBufFlat::NewL( KNSmlDMReservingData );
        CleanupStack::PushL( allNodes );
        iCallBack->FetchLinkL( appAuthPtr, *allNodes, status );
		    
// Used only in Test purpose
        DBG_ARGS8(_S8("Status = %d \n"), status );     
		    
        if( status == CSmlDmAdapter::EOk )
            {               	        	
            // Delete existing SAP
            aStore->DeleteSAPL( uid32 );
            _DBG_FILE( "CNSmlDmImpsAdapter::UpdatePEPValueL(): SAP deleted" );
        
            // Create new with correct AccessGroup        
            uid32 = aStore->StoreNewSAPL( aSap, newAccGroup );
            uid = TInt( uid32 );
            _DBG_FILE( "CNSmlDmImpsAdapter::UpdatePEPValueL(): New SAP added" );
		
            // Set mappings to new UID
            HBufC8 *luidDes = IntToDes8LC( TInt( uid ) );
        	
            // URI: IMPS/<x>/Ext/PEPCompliant
            iCallBack->SetMappingL( aURI, *luidDes );

            // URI: IMPS/<x>/Ext        	
            iCallBack->SetMappingL( ptrToExtUri, *luidDes );

            // URI: IMPS/<x>
            iCallBack->SetMappingL( ptrToDynNode, *luidDes );
        
            // URI: IMPS/<x>/AppAuth
            iCallBack->SetMappingL( appAuthPtr, *luidDes );		

       	    if ( allNodes->Size() > 0 )
                {
                TPtrC8 uriSeg8Ptr = allNodes->Ptr( 0 );

                // URI: IMPS/<x>/AppAuth/<x>
                HBufC8 *authUri = HBufC8::NewLC( appAuthPtr.Length() + 
                                                 uriSeg8Ptr.Length() );				
                TPtr8 authUriPtr= authUri->Des();
                authUriPtr.Format( appAuthPtr );
                authUriPtr.Append( KNSmlDMImpsSeparator8 );
                authUriPtr.Append( uriSeg8Ptr );
                iCallBack->SetMappingL( authUriPtr, *luidDes );
                
                CleanupStack::PopAndDestroy(); // authUri
                }
            CleanupStack::PopAndDestroy(); // luidDes
            }
		
        CleanupStack::PopAndDestroy( 2 ); // appAuthUri, allNodes
        } // oldValue != newValue  	

    _DBG_FILE( "CNSmlDmImpsAdapter::UpdatePEPValueL(): end" );		

    return status;
    }

// ----------------------------------------------------------------------------
// CNSmlDmImpsAdapter::DeleteValueL();
// Initializes the given leaf field with empty string.
// ----------------------------------------------------------------------------
CSmlDmAdapter::TError CNSmlDmImpsAdapter::DeleteValueL( const TDesC8& aUri, 
                                                        CIMPSSAPSettings* aSap )
    {
    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;

    TPtrC8 deletedLeaf = NSmlDmURI::LastURISeg( aUri );
    TPtrC8 ptrToParentUri = NSmlDmURI::RemoveLastSeg( aUri );
    TPtrC8 parentSeg = NSmlDmURI::LastURISeg( ptrToParentUri );

    if ( deletedLeaf.Compare( KNSmlDMImpsName ) == 0)
        {
        aSap->SetSAPNameL( ConvertTo16LC( parentSeg ) );
        CleanupStack::PopAndDestroy();
        }
    else if ( deletedLeaf.Compare( KNSmlDMImpsPrefAddr ) == 0)
        {
        aSap->SetSAPAddressL( KNSmlDMImpsNotDefined );
        }
    else if ( deletedLeaf.Compare( KNSmlDMImpsAAuthName ) == 0)
        {
        aSap->SetSAPUserIdL( KNSmlDMImpsNotDefined );
        }
    else if ( deletedLeaf.Compare( KNSmlDMImpsAAuthSecret ) == 0)
        {
        aSap->SetSAPUserPasswordL( KNSmlDMImpsNotDefined );
        }
    else if ( deletedLeaf.Compare( KNSmlDMImpsPrefConRef ) == 0 )
        {
        aSap->SetAccessPoint( 0 );  
        }
    else if ( deletedLeaf.Compare( KNSmlDMImpsAAuthLevel ) == 0 )
        {
        // AAuthLevel is not stored anywhere so it can not be deleted either
        }
    else if ( ( aUri.Match( KNSmlDMImpsWholeAppAuthUri ) != KErrNotFound ) ||
            ( aUri.Match( KNSmlDMImpsWholeAppAuthDynUri ) != KErrNotFound ) )
        {
        aSap->SetSAPUserIdL( KNSmlDMImpsNotDefined );
        aSap->SetSAPUserPasswordL( KNSmlDMImpsNotDefined );
        }
    else if ( deletedLeaf.Compare( KNSmlDMImpsServices ) == 0 )
        {
        status = CSmlDmAdapter::EError;
        }
    else  // Unknown leaf
        {
        status = CSmlDmAdapter::EInvalidObject;
        }
                
    return status;
    }

// ----------------------------------------------------------------------------
// CNSmlDmImpsAdapter::FetchObjectL();
// The method reads the requested parameter value from DB
// - Check that the requested SAP exists in DB by checking that 
//      the uid with LUID value exists.
// - Get SAP data to aSap
// - Read the requested parameter value
// - In error case return error status
// ----------------------------------------------------------------------------
CSmlDmAdapter::TError CNSmlDmImpsAdapter::FetchObjectL( const TDesC8& aUri, 
                                                        const TDesC8& aLUID, 
                                                        CBufBase& aObject )
    {	
    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;
    
    CIMPSSAPSettings* ownSap = CIMPSSAPSettings::NewL();
    CleanupStack::PushL( ownSap );
    CIMPSSAPSettingsStore* ownStore = CIMPSSAPSettingsStore::NewL();
    CleanupStack::PushL( ownStore );
    
    if( aLUID.Length() > 0 )
        {
        // Check that Store is not empty
        // causes leaving if empty db is tried to read
        TInt sapFound = KErrNotFound;
        sapFound = GetCorrectSapL( aLUID, ownSap, ownStore );
        if ( sapFound == KErrNone )
            {
            // Sap exist, read value of correct leaf
            if ( aUri.Compare( KNSmlDMImpsName ) == 0 )
                {
                aObject.InsertL( 0, ConvertTo8LC( ownSap->SAPName() ) );
                CleanupStack::PopAndDestroy();  // ConvertTo8LC
                }
            else if ( aUri.Compare( KNSmlDMImpsPrefAddr ) == 0 )
                {
                aObject.InsertL( 0, ConvertTo8LC( ownSap->SAPAddress() ) );
                CleanupStack::PopAndDestroy();  // ConvertTo8LC
                }
            else if ( aUri.Compare( KNSmlDMImpsAAuthName ) == 0 )
                {
                aObject.InsertL( 0, ConvertTo8LC( ownSap->SAPUserId() ) );
                CleanupStack::PopAndDestroy();  // ConvertTo8LC
                }
            else if ( aUri.Compare( KNSmlDMImpsAAuthSecret ) == 0 )
                {
                // Requesting password not allowed.
                status = EError;
                }
            else if ( aUri.Compare( KNSmlDMImpsPepCompliant ) == 0 )
                {
                TIMPSAccessGroup accGroup = ownSap->AccessGroup();
                if ( accGroup == EIMPSPECAccessGroup )	    	
                    { 
                    aObject.InsertL( 0, KNSmlDMImpsDefaultPEC );  		
                    }
                else
                    {
                    aObject.InsertL( 0, KNSmlDMImpsDefaultIM );  						  					  	
                    }
                }  // PEPCompliant
            else if ( aUri.Compare( KNSmlDMImpsPrefConRef ) == 0 )
                {            
                HBufC8* buf = IntToDes8LC( ownSap->AccessPoint() );
                TUint idFromDb = DesToInt( buf->Des() );
                CleanupStack::PopAndDestroy( ); // buf
                
                CBufBase *allIAPs = CBufFlat::NewL( KNSmlDMReservingData );
                CleanupStack::PushL( allIAPs );
                iCallBack->FetchLinkL( GetConRef(), *allIAPs, status );
		    
                TBool found = EFalse;
                if( status == CSmlDmAdapter::EOk )
                    {
                    TInt startIndex = 0;
        
                    for( TInt i = 0; i < allIAPs->Size(); i++ )
                        {
                        if( allIAPs->Ptr(0)[i] == '/' || i == allIAPs->Size()-1 )
                            {
                            HBufC8 *uriSegment=0;
                            TPtrC8 uriSeg8Ptr = allIAPs->Ptr( startIndex ).Mid( 0, i-startIndex );                				
                            uriSegment = uriSeg8Ptr.AllocLC();
                            HBufC8 *uri = HBufC8::NewLC( GetConRef().Length() + 1 + uriSegment->Length() );
                            TPtr8 uriPtr= uri->Des();
                            uriPtr.Format( GetConRef());
                            uriPtr.Append( KNSmlDMImpsSeparator );
                            uriPtr.Append( *uriSegment );
                            HBufC8* luid = iCallBack->GetLuidAllocL( uriPtr );
                            CleanupStack::PushL( luid );

                            if( luid->Length() > 0 )
                                {
                                TInt iapId = DesToInt( luid->Des() );
                                if( iapId == idFromDb )
                                    {
                                    found = ETrue;
                                    aObject.InsertL( 0, uriPtr );
                                    CleanupStack::PopAndDestroy( 3 ); // uriSegment, uri, luid
                                    break;
                                    }                                
                                }  // if luid
					        
                            startIndex = i + 1;
                            CleanupStack::PopAndDestroy( 3 ); // uriSegment, uri, luid
                            } //end if allIaps...
                        } //end for
                    }
                CleanupStack::PopAndDestroy(); //allIaps
                if( !found )
                    {
                    status = CSmlDmAdapter::ENotFound;
                    }
                } // PrefConRef

            else if ( aUri.Compare( KNSmlDMImpsAppId ) == 0 )
                {
                // AppID is not used yet so return default value
                aObject.InsertL( 0, KNSmlDMImpsDefaultAppID ); 
                }
            else if ( aUri.Compare( KNSmlDMImpsAAuthLevel ) == 0 )
                {
                // Check from the database if the AAuthName or AAuthSecret are defined
                // if they are, return default value
                if ( ( ownSap->SAPUserId().Compare(KNSmlDMImpsNotDefined ) == 0 ) &&
                     ( ownSap->SAPUserPassword().Compare(KNSmlDMImpsNotDefined ) == 0 ) )
                    // AAuthName and AAuthSecret are not defined 
                    {
                    aObject.InsertL( 0, KNSmlDMImpsNotDefined8 ); 
                    }
                else
                    {
                    aObject.InsertL( 0, KNSmlDMImpsDefaultAAuthLevel ); 
                    }
                }   // AppID                 
            else if ( aUri.Compare( KNSmlDMImpsServices ) == 0 )
                {
                HBufC8 *services=0;
                TUint32 serv = EIMPSServicesUnknown;
                services = HBufC8::NewLC( 11 ); 
                TPtr8 servicesPtr = services->Des();
            	
                serv = ownSap->HighLevelServices();
                switch ( serv )
                    {
                    case ( EIMPSServicesIM ):
                        {
                        servicesPtr.Format( KNSmlDMImpsServiceIM );
                        break;
                        }
                    case ( EIMPSServicesPEC ):
                        {
                        servicesPtr.Format( KNSmlDMImpsServicePS );            			
                        break;
                        }
                    case ( EIMPSServicesIM | EIMPSServicesPEC ):
                        {
                        servicesPtr.Format( KNSmlDMImpsServiceIM );
                        servicesPtr.Append( KNSmlDMImpsSemiColon );
                        servicesPtr.Append( KNSmlDMImpsServicePS );
                        break;
                        }
                    case ( EIMPSServicesGroups ):
                        {
                        servicesPtr.Format( KNSmlDMImpsServiceGR );           			
                        break;
                        }
                    case ( EIMPSServicesIM | EIMPSServicesGroups ):
                        {
                        servicesPtr.Format( KNSmlDMImpsServiceIM );
                        servicesPtr.Append( KNSmlDMImpsSemiColon );
                        servicesPtr.Append( KNSmlDMImpsServiceGR );
                        break;
                        }
                        case ( EIMPSServicesPEC | EIMPSServicesGroups ):
                        {
                        servicesPtr.Format( KNSmlDMImpsServicePS );
                        servicesPtr.Append( KNSmlDMImpsSemiColon );
                        servicesPtr.Append( KNSmlDMImpsServiceGR );
                        break;
                        }
            		case ( EIMPSServicesIM | EIMPSServicesPEC | EIMPSServicesGroups ):
            			{
						servicesPtr.Format( KNSmlDMImpsServiceIM );
						servicesPtr.Append( KNSmlDMImpsSemiColon );
                        servicesPtr.Append( KNSmlDMImpsServicePS );
                        servicesPtr.Append( KNSmlDMImpsSemiColon );
                        servicesPtr.Append( KNSmlDMImpsServiceGR );

						break;
            			}
            		default:
        	    		{
                        servicesPtr.Format( KNSmlDMImpsNotDefined8 );	            		
                        break;
                        }
                    }  // Service      		            		            	
                    aObject.InsertL( 0, servicesPtr );
                    CleanupStack::PopAndDestroy(); // services     		        
                } 
            else  
                {
                status = CSmlDmAdapter::EInvalidObject;
                }
            }  // sapFound
        else if ( sapFound == KErrNotFound )                    	 
            {      		
            status = CSmlDmAdapter::ENotFound;
            }  
        else
            {
            status = CSmlDmAdapter::EError;
            }

        }  // aLUID length
    else  
        {
        status = CSmlDmAdapter::ENotFound;
        }

    CleanupStack::PopAndDestroy( 2 ); // ownSap, ownStore
        
    return status;
    }

// ----------------------------------------------------------------------------
// CNSmlDmImpsAdapter::GetCorrectSapL();
// The method reads the requested parameter value from DB
// - Check that the requested SAP exists in DB by checking 
//      that the index with LUID value exists.
// - Get SAP data to aSap
// - Read the requested parameter value
// - In error case return error status
// ----------------------------------------------------------------------------
TInt CNSmlDmImpsAdapter::GetCorrectSapL( const TDesC8& aLUID, 
                                         CIMPSSAPSettings* aSap, 
                                         CIMPSSAPSettingsStore* aStore )
    {	
    // If value of Stat is KErrNone, the Sap has been found. 
    // If it is KErrNotFound, the Sap does not exist.
    // Else the stat returns error status.
    CIMPSSAPSettingsList* ownList = CIMPSSAPSettingsList::NewL();   
    CleanupStack::PushL( ownList );
	
    TInt stat = KErrNotFound;
	
        // Check that Store is not empty
        // causes leaving if empty db is tried to read
    TInt count = 0;
    count = aStore->SAPCountL( EIMPSAccessFilterAll ) ;
    DBG_ARGS8(_S8("IMPS::Count of SAPs in store = %d \n"), count );
    if ( count > 0 )
        {
        TInt uid = DesToInt( aLUID );
        TUint32 uid32( uid );
        aStore->PopulateSAPSettingsListL( *ownList, EIMPSAccessFilterAll );
        _DBG_FILE( "CNSmlDmImpsAdapter::GetCorrectSapL(): SAP settings list" );
        for ( TInt i = 0; i < count; i++ )
            {
            TUint32 id32 = ownList->UidForIndex( i );
            if ( id32 == uid32 )
                {
                aStore->GetSAPL( uid32, aSap );
                _DBG_FILE( "CNSmlDmImpsAdapter::GetCorrectSapL(): SAP requested" );
                stat = KErrNone;
                break;
                }
            } // for        	   
        } // if count

    CleanupStack::PopAndDestroy(); // ownList

    return stat;
    }

// ----------------------------------------------------------------------------
// TImplementationProxy ImplementationTable[]
// Needed because of ECOM architecture
// ----------------------------------------------------------------------------
#ifndef IMPLEMENTATION_PROXY_ENTRY
#define IMPLEMENTATION_PROXY_ENTRY( aUid, aFuncPtr ) { {aUid}, ( aFuncPtr ) }
#endif
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( KNSmlDMImpsAdapterImplUid, 
                                CNSmlDmImpsAdapter::NewL )
    };

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    _DBG_FILE( "ImplementationGroupProxy() for CNSmlDmImpsAdapter: begin" );
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    _DBG_FILE( "ImplementationGroupProxy() for CNSmlDmImpsAdapter: end" );
    return ImplementationTable;
    }


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void CNSmlDmImpsAdapter::HandleSessionEventL( TMsvSessionEvent /*aEvent*/, 
                                              TAny* /*aArg1*/, 
                                              TAny* /*aArg2*/, 
                                              TAny* /*aArg3*/ )
    {
    _DBG_FILE( "CNSmlDmImpsAdapter::HandleSessionEventL(): begin" );

    _DBG_FILE( "CNSmlDmImpsAdapter::HandleSessionEventL(): end" );
    }
    
// ------------------------------------------------------------------------------------------------
// CNSmlDmImpsAdapter::const TDesC8& GetConRef()
// Returns the connection reference
// ------------------------------------------------------------------------------------------------
const TDesC8& CNSmlDmImpsAdapter::GetConRef()
	{
	if(FeatureManager::FeatureSupported(KFeatureIdFfDmConnmoAdapter))
		{
			return KNSmlDMImpsConnMOIAPUri();
		}
	else
		{
			return KNSmlDMImpsIAPUri();
		}	
  }	    


// End of file

