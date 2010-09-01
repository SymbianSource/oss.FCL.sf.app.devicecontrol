/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DCMO DM Adapter implementation
*
*/

// INCLUDE
#include <implementationproxy.h>
#include <smldmadapter.h>
#include <centralrepository.h>
#include <e32base.h>
#include <ecom.h>
#include <utf.h>
#include <LawmoAdapterCRKeys.h>
#include <DevManInternalCRKeys.h>
#include <SyncMLClientDM.h>  // syncmlclientapi.lib
#include <NSmlPrivateAPI.h> // Generic alert
#include <nsmlconstants.h>
#include <TerminalControl3rdPartyAPI.h>
#include <coreapplicationuisdomainpskeys.h>
#include <LAWMOInterface.h>
//User Include
#include "lawmodmadapter.h"
#include "nsmldebug.h"
#include "nsmldmuri.h"
#include "TPtrC8I.h"
#include "lawmodebug.h"
// CONSTANTS

#define MAXBUFLEN 255

// ============================= MEMBER FUNCTIONS =============================

// ----------------------------------------------------------------------------
// CLawmoDMAdapter::NewL
// Symbian 1st phase contructor
// (static, may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
CLawmoDMAdapter* CLawmoDMAdapter::NewL( MSmlDmCallback* aCallback )
    {
    RDEBUG( "CLawmoDMAdapter NewL: begin"  );
    CLawmoDMAdapter* self = NewLC( aCallback );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CLawmoDMAdapter::NewLC
// Symbian 1st phase contructor. Push object to cleanup-stack
// (static, may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
CLawmoDMAdapter* CLawmoDMAdapter::NewLC( MSmlDmCallback* aCallback )
    {
    	RDEBUG( "CLawmoDMAdapter NewLC : begin"  );
    	CLawmoDMAdapter* self = new ( ELeave ) CLawmoDMAdapter( aCallback );
    	CleanupStack::PushL( self );
    	self->ConstructL();
    	RDEBUG( "CLawmoDMAdapter NewLC : end"  );
    	return self;
    }

// ----------------------------------------------------------------------------
// CLawmoDMAdapter::CLawmoDMAdapter()
// C++ Constructor
// Status : Draft
// ----------------------------------------------------------------------------
CLawmoDMAdapter::CLawmoDMAdapter( TAny* aEcomArguments )
    : CSmlDmAdapter::CSmlDmAdapter( aEcomArguments )
    {
    	RDEBUG( "CLawmoDMAdapter default constructor"  );
    }

// ----------------------------------------------------------------------------
// CLawmoDMAdapter::ConstructL
// 2nd phase contructor
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CLawmoDMAdapter::ConstructL()
    {
    	RDEBUG( "CLawmoDMAdapter::ConstructL"  );
        User::LeaveIfError( ilawmoSession.Connect() );
        RDEBUG( "CLawmoDMAdapter::session connected"  );
        iRootNode.Zero();
        TBuf<MAXBUFLEN> temp; 
        CRepository *repository=CRepository::NewLC ( KCRUidLawmoAdapter );
        repository->Get(KLawmoRootNode,temp);
        iRootNode.Copy(temp);
        CleanupStack::PopAndDestroy();
    }

// ----------------------------------------------------------------------------
// CLawmoDMAdapter::~CLawmoDMAdapter()
// C++ Destructor
// Status : Draft
// ----------------------------------------------------------------------------
//
CLawmoDMAdapter::~CLawmoDMAdapter()
    {
    	RDEBUG( "CLawmoDMAdapter Destructor"  );
    	ilawmoSession.Close();
    }

// ----------------------------------------------------------------------------
// CLawmoDMAdapter::DDFVersionL
// Return DM plug-in version
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CLawmoDMAdapter::DDFVersionL( CBufBase& aDDFVersion )
    {
    // Insert version information
    RDEBUG( "CLawmoDMAdapter::DDFVersionL" );
    aDDFVersion.InsertL( 0, KLAWMODMAdapterVersion );
    }

// ----------------------------------------------------------------------------
// CLawmoDMAdapter::DDFStructureL
// Return DM plug-in structure
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CLawmoDMAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
    {
    // Declare accesses
    RDEBUG( "CLawmoDMAdapter::DDFStructureL begin" );
    TSmlDmAccessTypes accessTypes;
    accessTypes.SetGet();    
    
    // Create root node 
    MSmlDmDDFObject& root = aDDF.AddChildObjectL( iRootNode );
    FillNodeInfoL( root,
                   accessTypes,
                   MSmlDmDDFObject::EPermanent,
                   MSmlDmDDFObject::ENode,
                   MSmlDmDDFObject::EOne,
                   KNullDesC8(),
                   KMimeType );
                   
   	root.SetDFTitleL( KNSmlLAWMOAdapterTitle );
                   
   	FillLAWMODDFL(root);
   	RDEBUG( "CLawmoDMAdapter::DDFStructureL end" );

    }

// ----------------------------------------------------------------------------
// CLawmoDMAdapter::StreamingSupport
// Return streaming support status, set supported item size
// Status : Draft
// ----------------------------------------------------------------------------
//
TBool CLawmoDMAdapter::StreamingSupport( TInt& /* aItemSize */ )
    {
    // Not supported
    RDEBUG( "CLawmoDMAdapter::StreamingSupport"  );
    return EFalse;
    }
    
// ----------------------------------------------------------------------------
// CLawmoDMAdapter::StreamCommittedL
// Commit stream buffer
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CLawmoDMAdapter::StreamCommittedL()
    {        
    RDEBUG("CLawmoDMAdapter::StreamCommitted"  );
    // Intentionally left empty 
    }

// ----------------------------------------------------------------------------
// CLawmoDMAdapter::CompleteOutstandingCmdsL
// Complete outstanding commands
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CLawmoDMAdapter::CompleteOutstandingCmdsL()
    {
    RDEBUG( "CLawmoDMAdapter::CompleteOutstandingCmdsL"  );
    }

// ----------------------------------------------------------------------------
// CLawmoDMAdapter::FillNodeInfoL
// Fill node info
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CLawmoDMAdapter::FillNodeInfoL( MSmlDmDDFObject& aDDFObject, 
                                     TSmlDmAccessTypes& aAccessTypes, 
                                     MSmlDmDDFObject::TScope aScope,
                                     MSmlDmDDFObject::TDFFormat aFormat, 
                                     MSmlDmDDFObject::TOccurence aOccurence,
                                     const TDesC8& aDescription,
                                     const TDesC8& aMIMEType )
    {
    RDEBUG( "CLawmoDMAdapter::FillNodeInfoL - begin"  );
    aDDFObject.SetAccessTypesL( aAccessTypes );
    aDDFObject.SetScopeL( aScope );
    aDDFObject.SetOccurenceL( aOccurence );
    aDDFObject.SetDFFormatL( aFormat );
    aDDFObject.SetDescriptionL( aDescription );
    if ( aFormat != MSmlDmDDFObject::ENode )
        {
        aDDFObject.AddDFTypeMimeTypeL( aMIMEType );
        }
    RDEBUG( "CLawmoDMAdapter::FillNodeInfoL - end"  );    
    }

// ----------------------------------------------------------------------------
// CLawmoDMAdapter::CopyCommandL
// Copy object
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CLawmoDMAdapter::CopyCommandL( const TDesC8& /*aTargetURI*/, 
                                    const TDesC8& /*aTargetLUID*/,
                                    const TDesC8& /*aSourceURI*/, 
                                    const TDesC8& /*aSourceLUID*/,
                                    const TDesC8& /*aType*/, 
                                    TInt aStatusRef )
    {
    // Not supported
    RDEBUG( "CLawmoDMAdapter::CopyCommandL"  );
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    }

// ----------------------------------------------------------------------------
// DeleteObjectL
// Delete object
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CLawmoDMAdapter::DeleteObjectL( const TDesC8& /* aURI */, 
                                     const TDesC8& /* aLUID */,
                                     TInt aStatusRef )

    {
    // Not supported
    RDEBUG( "CLawmoDMAdapter::DeleteObjectL"  );
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    }
    
// ----------------------------------------------------------------------------
// CLawmoDMAdapter::FetchLeafObjectL
// Fetch leaf
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CLawmoDMAdapter::FetchLeafObjectL( const TDesC8& aURI, 
                                        const TDesC8& aLUID,
								        const TDesC8& aType, 
								        TInt aResultsRef,
								        TInt aStatusRef )
    {
    RDEBUG("CLawmoDMAdapter::FetchLeafObjectL() Begin");
    
    TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
    CBufFlat *lObject = CBufFlat::NewL( MAXBUFLEN );
    CleanupStack::PushL( lObject );
    lObject->Reset();
    
    TInt err = InternalFetchObjectL( uriPtrc, aLUID, aType, lObject, aStatusRef );
    
    if((err == ELawMoSuccess)||(err == ELawMoOk))
    Callback().SetResultsL( aResultsRef, *lObject, aType );
    
    CSmlDmAdapter::TError status =  MapErrorToStatus( err );

    Callback().SetStatusL( aStatusRef, status );
    CleanupStack::PopAndDestroy( lObject );
    RDEBUG("CLawmoDMAdapter::FetchLeafObjectL() End");
    }

// ----------------------------------------------------------------------------
//InternalFetchObjectL
// ----------------------------------------------------------------------------

TInt CLawmoDMAdapter::InternalFetchObjectL( const TDesC8& aURI, 
                                              const TDesC8& /*aLUID*/,
                                              const TDesC8& /*aType*/, 
                                              CBufFlat* aObject,
                                              TInt aStatusRef )
    {    
    RDEBUG("CLawmoDMAdapter::InternalFetchObjectL() Begin");
    CLawmoDMAdapter::TNodeIdentifier identifier = GetNodeIdentifier(aURI);
    TInt numOfSegs = NSmlDmURI::NumOfURISegs( aURI ) ;
    TInt err (ELawMoInvalid);

    switch(identifier)
       {
       
       case CLawmoDMAdapter::ENodeState:
            {
            RDEBUG("CLawmoDMAdapter::InternalFetchObjectL ENodeState");
            TInt state;
            err = GetStateL(state);
            TBuf8<MAXBUFLEN> tmp;
            tmp.Num(state);
            aObject->InsertL(aObject->Size(),tmp);
            }
       break;
        
       case CLawmoDMAdapter::ENodeItemName:
           {
           RDEBUG("CLawmoDMAdapter::InternalFetchObjectL ENodeItemName");
           TBuf<MAXBUFLEN> val;
           TBuf<MAXBUFLEN> property;
           TPtrC8 seg = NSmlDmURI::URISeg(aURI, numOfSegs-2);
		   property.Copy(seg);
           err = ilawmoSession.GetListItemName(property, val);
           TBuf8<MAXBUFLEN> tmp;
           tmp.Copy(val);
           aObject->InsertL(aObject->Size(),tmp);
           }
       break;
       
       case CLawmoDMAdapter::ENodeToBeWiped:
           {
           RDEBUG("CLawmoDMAdapter::InternalFetchObjectL ENodeToBeWiped");
           TBool val;
           TBuf<MAXBUFLEN> property;
           TPtrC8 seg = NSmlDmURI::URISeg(aURI, numOfSegs-2);
		   property.Copy(seg);
           err = ilawmoSession.GetToBeWipedValue(property, val);
           TBuf8<MAXBUFLEN> tmp;           
           if(val)
               tmp.Copy(KNSmlTrue);
           else
               tmp.Copy(KNSmlFalse);
           aObject->InsertL(aObject->Size(),tmp);
           }
       break;
       
       case CLawmoDMAdapter::ENodeWipe:
           {
           RDEBUG("CLawmoDMAdapter::InternalFetchObjectL ENodeWipe");
           err = ELawMoSuccess;
           TBuf8<MAXBUFLEN> tmp;
           tmp.Zero();
           RDEBUG("CLawmoDMAdapter::InternalFetchObjectL Null value return");
           aObject->InsertL(aObject->Size(),tmp);
           }
           break;
       default:
           err = ELawMoUnknown;
       break;
       
       }
     RDEBUG("CLawmoDMAdapter::InternalFetchObjectL() End");
     return err;     
    }
    
// ----------------------------------------------------------------------------
// CLawmoDMAdapter::FetchLeafObjectSizeL
// Calculate leaf object size
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CLawmoDMAdapter::FetchLeafObjectSizeL( const TDesC8& /* aURI */, 
                                            const TDesC8& /* aLUID */,
									        const TDesC8& /* aType */, 
									        TInt /* aResultsRef */,
									        TInt aStatusRef )
    {
    RDEBUG( "CLawmoDMAdapter::FetchLeafObjectSizeL"  );
    MSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;
    // Respond
    Callback().SetStatusL( aStatusRef, retValue );
    }    

// ----------------------------------------------------------------------------
// CLawmoDMAdapter::ChildURIListL
// Create child URI list
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CLawmoDMAdapter::ChildURIListL( const TDesC8& aURI, 
                                     const TDesC8& /* aLUID */,
					                 const CArrayFix<TSmlDmMappingInfo>& /* aPreviousURISegmentList */,
            					     TInt aResultsRef, 
            					     TInt aStatusRef )
    {
    RDEBUG( "CLawmoDMAdapter::ChildURIListL - begin"  );
    MSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;
    CBufBase *currentURISegmentList = CBufFlat::NewL( MAXBUFLEN );
	CleanupStack::PushL( currentURISegmentList );
	// Bcoz root node is configurable, remove the node for this operation.
    TPtrC8 uriPtrc = NSmlDmURI::RemoveFirstSeg(aURI);
    
    if( uriPtrc == KNullDesC8)
    {	
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KSegmLawmo );
        Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
        Callback().SetResultsL( aResultsRef, *currentURISegmentList, KNullDesC8 );
        CleanupStack::PopAndDestroy(  ); // currentURISegmentList
        RDEBUG( "CLawmoDMAdapter::ChildURIListL(): end" );
        return;
    }
    else if( ( aURI.Match( KLawmoAvWipe ) != KErrNotFound ) &&
            (aURI.Match( KLawmoAvWipe2 ) == KErrNotFound ))
        {
          currentURISegmentList->InsertL( currentURISegmentList->Size(), KSegmLawmo2 );
        }
    else if( ( aURI.Match( KLawmoAvWipe2 ) != KErrNotFound ))
        {
          currentURISegmentList->InsertL( currentURISegmentList->Size(), KSegmLawmo3 );
        }
    // Operations    
    else if(( aURI.Match( KLawmoOperationsMatch ) != KErrNotFound ))
        {
          currentURISegmentList->InsertL( currentURISegmentList->Size(), KSegmLawmoOperations );
        } 
    // Ext    
    else if(( aURI.Match(KLawmoOperationsExtMatch) != KErrNotFound))
        {
          currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDdfOpExtWipeAll );
        }        
    else
        {
        retValue = CSmlDmAdapter::ENotFound;
        }

    Callback().SetStatusL( aStatusRef, retValue );
    CleanupStack::PopAndDestroy(); // currentURISegmentList
    RDEBUG( "CLawmoDMAdapter::ChildURIListL - end"  );
    }    

// ----------------------------------------------------------------------------
// CLawmoDMAdapter::AddNodeObjectL
// Add node
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CLawmoDMAdapter::AddNodeObjectL( const TDesC8& /* aURI */,
                                     const TDesC8& /* aParentLUID */,
								     TInt aStatusRef )
    {
    // Not supported
    RDEBUG( "CLawmoDMAdapter::AddNodeObjectL - begin"  );
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    RDEBUG( "CLawmoDMAdapter::AddNodeObjectL - end"  );
    }    

// ----------------------------------------------------------------------------
// CLawmoDMAdapter::ExecuteCommandL
// Execute command
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CLawmoDMAdapter::ExecuteCommandL( const TDesC8&  aURI , 
                                      const TDesC8& /* aLUID */,
							          const TDesC8&  aArgument, 
							          const TDesC8& /* aType */,
								      TInt aStatusRef )
    {
    RDEBUG("CLawmoDMAdapter::ExecuteCommandL() Begin");
    CSmlDmAdapter::TError status;
    if(IsFactoryProfileL())
        {
        RDEBUG("CLawmoDMAdapter::ExecuteCommandL() its factory prof");
        SetCurrentServerIdL();
        TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
        CLawmoDMAdapter::TNodeIdentifier identifier = GetNodeIdentifier(uriPtrc);
        TLawMoStatus err(ELawMoSuccess);
        RDEBUG_2("ExecuteCommandL executing %d", identifier);
        switch(identifier)
            {
            case ENodePartLock:
            err = LockDeviceL();
            break;
    
            case ENodeUnlock:
            err = UnLockDeviceL();
            break;
            
            case ENodeWipe:
            {
            //Write correlator id into the cenrep            
            /********* disable this comment after correlator is added
            CRepository* crep = NULL;
            crep = CRepository::NewLC( KCRUidLawmoAdapter );
            RDEBUG("CLawmoDMAdapter::SetCurrentServerId() cenrep1");
            TInt reterr = crep->Set( KLawmoCorrelator, aArgument );
            CleanupStack::PopAndDestroy(crep);*/            
            err = ilawmoSession.DeviceWipe();
            }
            break;
            
            case ENodeWipeAll:
            err = ilawmoSession.DeviceWipeAll();
            break;  
            
            default:
            err = ELawMoUnknown;
            break;           
            }        
        if((identifier == ENodeWipe)||(identifier == ENodeWipeAll))
            {
            CRepository* crep = NULL;
            TInt reterr(KErrNone);
            TBuf<MAXBUFLEN> argument;
            TBuf<MAXBUFLEN> opURI;
            argument.Copy(aArgument);
            opURI.Copy(aURI);
            crep = CRepository::NewLC( KCRUidLawmoAdapter );
            if(argument.Length()!=0)
            reterr = crep->Set( KLawmoCorrelator, argument );
            RDEBUG_2("CLawmoDMAdapter write correlator %d", reterr);
            reterr = crep->Set( KLawmoSourceURI, opURI);
            RDEBUG_2("CLawmoDMAdapter write sourceURI %d", reterr);
            CleanupStack::PopAndDestroy(crep);
            }        
        else
            {        
            CRepository* crep = NULL;
            _LIT8(KNsmlNull,"null");
            crep = CRepository::NewLC( KCRUidDeviceManagementInternalKeys );
            RDEBUG("CLawmoDMAdapter::ExecuteCommandL Sourceref Crep");
            TInt reterr = crep->Set( KNSmlDMSCOMOTargetRef, KNsmlNull ); 
            RDEBUG_2("CLawmoDMAdapter::cenrep set for SourceRef, %d",reterr);
            CleanupStack::PopAndDestroy();
            }
        status = MapErrorToStatus( err );       //map lawmo/syncml error codes     
        }
    else
        {
        RDEBUG("CLawmoDMAdapter::ExecuteCommandL() NOT factory prof");
        status = CSmlDmAdapter::ENotAllowed; //405
        }
    Callback().SetStatusL( aStatusRef, status );
    RDEBUG( "CLawmoDMAdapter::ExecuteCommandL - end"  );
    }    

// ----------------------------------------------------------------------------
// CLawmoDMAdapter::ExecuteCommandL
// Execute command, streaming enabled
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CLawmoDMAdapter::ExecuteCommandL( const TDesC8& /* aURI */, 
                                      const TDesC8& /* aLUID */,
								      RWriteStream*& /* aStream */,
								      const TDesC8& /* aType */,
								      TInt aStatusRef )
    {
    // Not supported
    RDEBUG( "CLawmoDMAdapter::ExecuteCommandL Streaming"  );
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    }    
    
// ----------------------------------------------------------------------------
// CLawmoDMAdapter::UpdateLeafObjectL
// Update leaf object
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CLawmoDMAdapter::UpdateLeafObjectL( const TDesC8&  aURI,
                                        const TDesC8&   aLUID,
                                        const TDesC8&   aObject,
                                        const TDesC8&   aType,
                                        TInt aStatusRef )
    {
    RDEBUG( "CLawmoDMAdapter::UpdateLeafObjectL - begin"  );
    CSmlDmAdapter::TError status;
    if(IsFactoryProfileL())
       {
        RDEBUG("CLawmoDMAdapter::UpdateLeafObjectL() is factory prof");
        TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
        TInt err = InternalUpdateObjectL( uriPtrc, aLUID, aObject, aType, aStatusRef );        
        status = MapErrorToStatus( err );
       }
    else
        {
        RDEBUG("CLawmoDMAdapter::UpdateLeafObjectL() NOT factory prof");
        status = CSmlDmAdapter::ENotAllowed;//405
        }
    
    Callback().SetStatusL( aStatusRef, status );    
    RDEBUG( "CLawmoDMAdapter::UpdateLeafObjectL - end"  );
    }

	// ----------------------------------------------------------------------------
//InternalFetchObjectL
// ----------------------------------------------------------------------------

TInt CLawmoDMAdapter::InternalUpdateObjectL( const TDesC8& aURI, 
                                              const TDesC8& /*aLUID*/,
                                              const TDesC8& aObject, 
                                              const TDesC8& /*aType*/,
                                              TInt aStatusRef )
    { 		
      RDEBUG( "CLawmoDMAdapter::InternalUpdateObjectL - begin"  );
      CLawmoDMAdapter::TNodeIdentifier identifier = GetNodeIdentifier(aURI);
      TInt numOfSegs = NSmlDmURI::NumOfURISegs( aURI ) ;
      TInt err (ELawMoInvalid);
      RDEBUG_2("UpdateCommand updating %d", identifier);      
      switch(identifier)
         {         
         case CLawmoDMAdapter::ENodeToBeWiped:
           {
           TInt value;
           HBufC8 *object = HBufC8::NewLC( aObject.Length() );
           TPtr8 objPtr = object->Des();
           objPtr.Format( aObject );
           objPtr.LowerCase();
           if(objPtr.Compare(KNSmlTrue) == 0)
           value = 1;
           else if(objPtr.Compare(KNSmlFalse) == 0)
           value = 0;
           else
               {
                TLex8 lex( aObject );                
                RDEBUG( "CLawmoDMAdapter::InternalUpdateObjectL lexing"  );
                User::LeaveIfError( lex.Val( value ) );                
               }
           CleanupStack::PopAndDestroy(); // object

           if(value<0||value>1)
               {
               RDEBUG_2("value to set %d", value);
               return ELawMoInvalid;
               }
           
           RDEBUG_2("value to set %d", value);
           TPtrC8 seg = NSmlDmURI::URISeg(aURI, numOfSegs-2);
		   TBuf<MAXBUFLEN> property;
		   property.Copy(seg);
		   RDEBUG_2("category %s", property.PtrZ());
           err = ilawmoSession.SetToBeWipedValue(property, value);
           }
           break;
           
         default:
             err = ELawMoUnknown;
             break;
         
         }
    RDEBUG( "CLawmoDMAdapter::InternalUpdateObjectL - End"  );
	return err;
	}
// ----------------------------------------------------------------------------
// CLawmoDMAdapter::UpdateLeafObjectL
// Update leaf object, streaming enabled
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CLawmoDMAdapter::UpdateLeafObjectL( const TDesC8& /* aURI */, 
                                         const TDesC8& /* aLUID */,
									     RWriteStream*& /* aStream */, 
									     const TDesC8& /* aType */,
		 							     TInt aStatusRef )
    {
    
    // Not supported
    RDEBUG( "CLawmoDMAdapter::UpdateLeafObjectL - Streaming"  );
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );//500
    }
    
// ----------------------------------------------------------------------------
// CLawmoDMAdapter::StartAtomicL
// Start atomic
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CLawmoDMAdapter::StartAtomicL()
    {
    }
    
// ----------------------------------------------------------------------------
// CLawmoDMAdapter::CommitAtomicL
// Commit atomic commands
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CLawmoDMAdapter::CommitAtomicL()
    {
    }

// ----------------------------------------------------------------------------
// CLawmoDMAdapter::RollbackAtomicL
// Lose all modifications after 'StartAtomicL' command
// (may leave)
// Status : Draft
// ----------------------------------------------------------------------------
//
void CLawmoDMAdapter::RollbackAtomicL()
    {
    }    

// ---------------------------------------------------------------------------
// CNSmlDmFotaAdapter::MapStatusToError()
// Returns a status code corresponding to the system error code given 
// as parameter.
// ---------------------------------------------------------------------------
//
CSmlDmAdapter::TError CLawmoDMAdapter::MapErrorToStatus( TInt aError ) const
 {
    RDEBUG( "CLawmoDMAdapter::MapErrorToStatus - Begin"  );
    CSmlDmAdapter::TError err;
    
    switch( aError )
        {
        case ELawMoSuccess:
            err = CSmlDmAdapter::EExecSuccess; //1200
            break;
        case ELawMoOk:
            err = CSmlDmAdapter::EOk; //200
            break;
        case ELawMoFail:
            err = CSmlDmAdapter::EExecClientError; //1400
            break;    
        case ELawMoUserCancelled:
            err = CSmlDmAdapter::EExecUserCancelled; //1401
            break;
        case ELawMoFullyLockFailed:
            err = CSmlDmAdapter::EExecDownloadFailed; //1402
            break;
        case ELawMoPartialLockFailed:
            err = CSmlDmAdapter::EExecAltDwnldAuthFail; //1403
            break; 
        case ELawMoUnlockFailed:
            err = CSmlDmAdapter::EExecDownFailOOM; //1404
            break;            
        case ELawMoWipeFailed:
            err = CSmlDmAdapter::EExecInstallFailed; //1405
            break;    
        case ELawMoWipeNotPerformed:
            err = CSmlDmAdapter::EExecInstallOOM; //1406
            break; 
        case ELawMoWipeSuccess:
            err = CSmlDmAdapter::EExecDownFailOOM; //1201 ??????
            break;            
        case ELawMoAccepted:
            err = CSmlDmAdapter::EAcceptedForProcessing; //202
            break;
        case ELawMoUnknown:
            err = CSmlDmAdapter::ENotFound; //404
            break;
        case ELawMoInvalid:
            err = CSmlDmAdapter::EError; //500
            break;
        default :
        		err = CSmlDmAdapter::EExecClientError;//1400        
        }
    
    return err;
 }
      
// ========================= OTHER EXPORTED FUNCTIONS =========================

void CleanupEComArray(TAny* aArray)
	{
	(static_cast<RImplInfoPtrArray*> (aArray))->ResetAndDestroy();
	(static_cast<RImplInfoPtrArray*> (aArray))->Close();
	}

// ----------------------------------------------------------------------------
// CLawmoDMAdapter::FillLAWMODDFL
// This function constructs the DDFnode with the details
// ----------------------------------------------------------------------------
//	
void CLawmoDMAdapter::FillLAWMODDFL(MSmlDmDDFObject& aDDF)
{
	RDEBUG("CLawmoDMAdapter::FillDCMPDDFL(): begin");
		
	TSmlDmAccessTypes accessTypesG;
    accessTypesG.SetGet();
    
    MSmlDmDDFObject& nStateDDF = aDDF.AddChildObjectL( KNSmlDdfState );			// State
    FillNodeInfoL( nStateDDF, accessTypesG,  
                        MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::EInt, MSmlDmDDFObject::EOne,
                        KNSmlDdfStateDescription, KMimeType );

    FillDynamicNodeL(aDDF);
        
    MSmlDmDDFObject& nOperationsDDF = aDDF.AddChildObjectL( KNSmlDdfOperations );
    FillNodeInfoL( nOperationsDDF, accessTypesG,  
                        MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode, MSmlDmDDFObject::EOne,
                        KNSmlDdfOperationsDescription, KMimeType ); 
    
    TSmlDmAccessTypes accessTypesE;   
    accessTypesE.SetExec();
        
    MSmlDmDDFObject& nPartLockDDF = nOperationsDDF.AddChildObjectL( KNSmlDdfPartLock );     // Partial Lock
    FillNodeInfoL( nPartLockDDF, accessTypesE,  
                        MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENull, MSmlDmDDFObject::EOne,
                        KNSmlDdfPartLockDescription, KMimeType );
    
    MSmlDmDDFObject& nUnlockDDF = nOperationsDDF.AddChildObjectL( KNSmlDdfUnlock );     // Unlock
    FillNodeInfoL( nUnlockDDF, accessTypesE,  
                        MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENull, MSmlDmDDFObject::EOne,
                        KNSmlDdfUnlockDescription, KMimeType );

    TSmlDmAccessTypes accessTypesEG;   
    accessTypesEG.SetExec();
    accessTypesEG.SetGet();
    
    MSmlDmDDFObject& nWipeDDF = nOperationsDDF.AddChildObjectL( KNSmlDdfWipe );     // Wipe
    FillNodeInfoL( nWipeDDF, accessTypesEG,  
                        MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENull, MSmlDmDDFObject::EOne,
                        KNSmlDdfWipeDescription, KMimeType ); 
    
    MSmlDmDDFObject& nExtDDF = aDDF.AddChildObjectL( KNSmlDdfOpExt );       // Ext
    FillNodeInfoL( nExtDDF, accessTypesG,  
                        MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode, MSmlDmDDFObject::EOne,
                        KNSmlDdfExtDescription, KMimeType );
    
    MSmlDmDDFObject& nOpExtDDF = nExtDDF.AddChildObjectL( KNSmlDdfOpExtWipeAll );      // WipeAll
    FillNodeInfoL( nOpExtDDF, accessTypesE, 
                        MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENull, MSmlDmDDFObject::EOne, 
                        KNSmlDdfOpExtDescription, KMimeType ); 
    //Check for any generic alerts and send
    RDEBUG("CLawmoDMAdapter::FillDCMPDDFL(): send generic alert");
    SendGenericAlertL();
	RDEBUG("CLawmoDMAdapter::FillDCMPDDFL(): end");
}    

void CLawmoDMAdapter::FillDynamicNodeL(MSmlDmDDFObject& aDDF)
    {
    RDEBUG("CDCMOServer::FillDynamicNode(): start");
    RImplInfoPtrArray infoArray;
    // Note that a special cleanup function is required to reset and destroy
    // all items in the array, and then close it.
    TCleanupItem cleanup(CleanupEComArray, &infoArray);
    CleanupStack::PushL(cleanup);
    REComSession::ListImplementationsL(KLAWMOPluginInterfaceUid, infoArray);
    RDEBUG("CDCMOServer::GetPluginUids(): listImpl");
    
    TSmlDmAccessTypes accessTypesG;
    accessTypesG.SetGet();    
    
    TSmlDmAccessTypes accessTypesSR;
    accessTypesSR.SetGet();
    accessTypesSR.SetReplace();
    
    MSmlDmDDFObject& nAwlDDF = aDDF.AddChildObjectL( KNSmlDdfAvailWipeList );                   // AWL
    FillNodeInfoL( nAwlDDF, accessTypesG,  
                        MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode, MSmlDmDDFObject::EOne,
                        KNSmlDdfAWLDescription, KMimeType ); 
    
    // Loop through each info for each implementation
    for (TInt i=0; i< infoArray.Count(); i++)
        {
        RDEBUG("CDCMOServer::FillDynamicNode(): for loop");
        MSmlDmDDFObject& nDynamicNodeDDF = nAwlDDF.AddChildObjectL(infoArray[i]->OpaqueData());
        FillNodeInfoL( nDynamicNodeDDF, accessTypesG,  
                            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode, MSmlDmDDFObject::EOne,
                            KNSmlDdfDynNodeDescription, KMimeType ); 
        RDEBUG("CDCMOServer::FillDynamicNode(): fill dyn node");
        
        MSmlDmDDFObject& nListItemDDF = nDynamicNodeDDF.AddChildObjectL( KNSmlDdfListItemName );     // ListItemName
        FillNodeInfoL( nListItemDDF, accessTypesG, 
                            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::EChr, MSmlDmDDFObject::EOne, 
                            KNSmlDdfListDescription, KMimeType );
   
        
        MSmlDmDDFObject& nToWipeDDF = nDynamicNodeDDF.AddChildObjectL( KNSmlDdfToBeWiped );     // ToBeWiped
        FillNodeInfoL( nToWipeDDF, accessTypesSR, 
                            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::EBool, MSmlDmDDFObject::EOne, 
                            KNSmlDdfToWipeDescription, KMimeType );
        RDEBUG("CDCMOServer::FillDynamicNode(): loop done");
        }
    CleanupStack::PopAndDestroy(); //cleanup
    }

TBool CLawmoDMAdapter::IsFactoryProfileL()
    {
    RDEBUG("CLawmoDMAdapter::IsFactoryProfile() begin");
    TBool retval(EFalse);
    RSyncMLSession ses;
    ses.OpenL() ;
    CleanupClosePushL(ses);
    TSmlJobId jid;
    TSmlUsageType jtype;
    ses.CurrentJobL(jid, jtype);
    RSyncMLDevManJob job;
    job.OpenL(ses, jid);
    CleanupClosePushL(job) ;
    
    TSmlProfileId pid(job.Profile() ); 
    TInt fpid(-1);//factory profile id
    RDEBUG("CLawmoDMAdapter::IsFactoryProfile() before cenrep");
    
    CRepository* crep = NULL;
    crep = CRepository::NewLC( KCRUidDeviceManagementInternalKeys );
    RDEBUG("CLawmoDMAdapter::IsFactoryProfile() CRep1");
    TInt reterr = crep->Get( KMaxFactoryDMProfileId, fpid ); 
    RDEBUG("CLawmoDMAdapter::IsFactoryProfile() CRep2");
    fpid =fpid + KMaxDataSyncID; //fpid in cenrep needs to be updated
    RDEBUG_2("factprofidStored %d", fpid);
    
    if( pid <= fpid )
        {
        RDEBUG("CLawmoDMAdapter::IsFactoryProfile() pid is factory");
        retval = ETrue;
        }
    else
        {
        RDEBUG("CLawmoDMAdapter::IsFactoryProfile() pid not factory");
        }
    CleanupStack::PopAndDestroy(crep);  
    CleanupStack::PopAndDestroy( &job) ;
    CleanupStack::PopAndDestroy( &ses) ;
    
    RDEBUG("CLawmoDMAdapter::IsFactoryProfile() end");    
    return retval;
    }

void CLawmoDMAdapter::SetCurrentServerIdL()
    {
    RDEBUG("CLawmoDMAdapter::SetCurrentServerId() begin");
    TBuf<MAXBUFLEN> tsrvrid;
    TBuf<MAXBUFLEN> tsrvrname;
    RSyncMLSession ses;
    ses.OpenL() ;
    CleanupClosePushL(ses);
    
    TSmlJobId jid;
    TSmlUsageType jtype;
    ses.CurrentJobL(jid, jtype);
    
    RSyncMLDevManJob job;
    job.OpenL(ses, jid);
    CleanupClosePushL(job) ;
    TSmlProfileId pid(job.Profile() ); 
    
    RDEBUG("CLawmoDMAdapter::SetCurrentServerId() syncprofile");
    RSyncMLDevManProfile syncProfile;
    syncProfile.OpenL(ses, pid);    
    CleanupClosePushL(syncProfile);    
    tsrvrid.Copy(syncProfile.ServerId());
    tsrvrname.Copy(syncProfile.DisplayName());
    TInt tempPid = (TInt) syncProfile.Identifier();
    
    RDEBUG_2("CLawmoDMAdapter::SetCurrentServerId() ProfileId 1 %d",tempPid);
    RDEBUG_2("CLawmoDMAdapter::SetCurrentServerId() ProfileId 2 %d",(TInt) pid);
    RDEBUG_2("CLawmoDMAdapter::SetCurrentServerId() Srvrid %s",tsrvrid.PtrZ());  
    RDEBUG_2("CLawmoDMAdapter::SetCurrentServerId() srvrname %s",tsrvrname.PtrZ());
    
    CRepository* crep = NULL;
    crep = CRepository::NewLC( KCRUidDeviceManagementInternalKeys );
    TInt reterr = crep->Set( KLAWMOfactoryDmProfileID, (TInt)pid ); 
    RDEBUG_2("CLawmoDMAdapter::setProfile() %d",reterr);
    reterr = crep->Set( KLAWMOfactoryDmServerID, tsrvrid ); 
    RDEBUG_2("CLawmoDMAdapter::SetCurrentServerId() %d",reterr);
    reterr = crep->Set(KLAWMOfactoryDmServerName,tsrvrname);
    RDEBUG_2("CLawmoDMAdapter::servername() %d",reterr);
    
    CleanupStack::PopAndDestroy( &syncProfile);
    CleanupStack::PopAndDestroy( &job) ;
    CleanupStack::PopAndDestroy( &ses) ;
    CleanupStack::PopAndDestroy() ;//cenrep
    
    RDEBUG("CLawmoDMAdapter::SetCurrentServerId() end");
    }

CLawmoDMAdapter::TNodeIdentifier CLawmoDMAdapter::GetNodeIdentifier(const TDesC8& aURI)
    {
    RDEBUG("CLawmoDMAdapter::GetNodeIdentifier() begin");

    TInt    numOfSegs = NSmlDmURI::NumOfURISegs( aURI ) ;
    TPtrC8I seg1 = NSmlDmURI::URISeg(aURI, 0);

    if (seg1 == iRootNode)
        {
        if(numOfSegs == 1) return CLawmoDMAdapter::ENodeLawMo;

        TPtrC8I seg2 = NSmlDmURI::URISeg(aURI, 1);

        if (seg2 == KNSmlDdfState)
            {
                return CLawmoDMAdapter::ENodeState;
            }            
        else if (seg2 == KNSmlDdfAvailWipeList)
            {               
                if (numOfSegs == 2)
                {
                    return CLawmoDMAdapter::ENodeAvWipeList;
                }
                else if (numOfSegs == 3) 
                {            
                    // We are not using dynamic node name to do any decision making
                    return CLawmoDMAdapter::ENodeDynamic;
                }
                else if (numOfSegs == 4) 
                {
                    TPtrC8I seg4 = NSmlDmURI::URISeg(aURI, 3);
                    
                    if (seg4 == KNSmlDdfListItemName)
                        {
                        return CLawmoDMAdapter::ENodeItemName;
                        }
                    else if (seg4 == KNSmlDdfToBeWiped)
                        {
                        return CLawmoDMAdapter::ENodeToBeWiped;
                        }
                    else
                        {
                        return CLawmoDMAdapter::ENodeNotUsedAndAlwaysLast;
                        }
                }
                else
                {
                return CLawmoDMAdapter::ENodeNotUsedAndAlwaysLast;
                }
            }            
        else if (seg2 == KNSmlDdfOperations)                    
            {
                if(numOfSegs == 2)
                    {
                     return CLawmoDMAdapter::ENodeOperations;
                    }
                else if(numOfSegs == 3)
                    {
                    TPtrC8I seg3 = NSmlDmURI::URISeg(aURI, 2);
                
                    if(seg3 == KNSmlDdfPartLock)
                        {
                        return CLawmoDMAdapter::ENodePartLock;
                        }  
                    else if(seg3 == KNSmlDdfUnlock)
                        {
                        return CLawmoDMAdapter::ENodeUnlock;
                        } 
                    else if(seg3 == KNSmlDdfWipe)
                        {
                        return CLawmoDMAdapter::ENodeWipe;
                        } 
                    }
                else
                    { 
                    return CLawmoDMAdapter::ENodeNotUsedAndAlwaysLast;
                    }
            }
        else if (seg2 == KNSmlDdfOpExt)
            {
                if(numOfSegs == 2)
                {
                    return CLawmoDMAdapter::ENodeExt;
                }
                else if(numOfSegs == 3)
                {
                TPtrC8I seg3 = NSmlDmURI::URISeg(aURI, 2);

                if(seg3 == KNSmlDdfOpExtWipeAll)
                    {
                    return CLawmoDMAdapter::ENodeWipeAll;
                    } 
                else
                    {
                    return CLawmoDMAdapter::ENodeNotUsedAndAlwaysLast;
                    }
                }
                else
                {
                return CLawmoDMAdapter::ENodeNotUsedAndAlwaysLast;
                }
            }
        }
    RDEBUG("CLawmoDMAdapter::GetNodeIdentifier() End");
    return CLawmoDMAdapter::ENodeNotUsedAndAlwaysLast;
    }

void CLawmoDMAdapter::SendGenericAlertL()
{
    RDEBUG("CLawmoDMAdapter::SendGenericAlert(): begin");    
    // check if wipe generic alert needs to be sent ?
    TInt wipeStatus;
    CRepository *repository=CRepository::NewLC ( KCRUidLawmoAdapter );
    repository->Get(KLawmoWipeStatus,wipeStatus);
    RDEBUG("CDCMOServer::SendGenericAlert(): chkin wipestatus");
    if(wipeStatus!=-1)
        {
        //Wipe Alert needs tobe sent
        RNSmlPrivateAPI privateAPI;
        privateAPI.OpenL();
        RDEBUG("CLawmoDMAdapter::SendGenericAlert(): OpenL");
        CleanupClosePushL(privateAPI);
        RArray<CNSmlDMAlertItem> iItemArray;
        CNSmlDMAlertItem* item = new (ELeave) CNSmlDMAlertItem ;
        RDEBUG("CLawmoDMAdapter::SendGenericAlert(): alertItem");
        TBuf<MAXBUFLEN> targetURI;
        targetURI.Append(KNullDesC);
        TBuf<MAXBUFLEN> correlator;
        correlator.Append(KNullDesC);
        TBuf<MAXBUFLEN> sourceURI;
        sourceURI.Append(KNullDesC);
        RDEBUG("CLawmoDMAdapter::SendGenericAlert(): bfore cenrep");
        // Construct generic alert message
        TInt reterr = repository->Get(KLawmoCorrelator,correlator );
        reterr = repository->Get(KLawmoSourceURI,sourceURI);
        RDEBUG("CLawmoDMAdapter::SendGenericAlert(): after cenrep");
        
        HBufC8 *descorrelator = HBufC8::NewL(MAXBUFLEN);
        (descorrelator->Des()).Copy(correlator);
        
        HBufC8 *aSourceuri = HBufC8::NewL(MAXBUFLEN);
        (aSourceuri->Des()).Copy(sourceURI);   
        item->iSource = aSourceuri;    
        
        HBufC8 *aTargeturi = HBufC8::NewL(MAXBUFLEN);   
        (aTargeturi->Des()).Copy(targetURI);    
        item->iTarget = aTargeturi; 
        if(item->iTarget->Des().Compare(KNullDesC8)==0)
            RDEBUG("CLawmoDMAdapter::SendGenericAlert() targeturi is NULL");
        RDEBUG("CLawmoDMAdapter::SendGenericAlert(): targeturidone");
        
        HBufC8 *aMetatype = HBufC8::NewL(MAXBUFLEN); 
        (aMetatype->Des()).Copy(KLawmoMetatype);               
        item->iMetaType = aMetatype;     
        
        HBufC8 *aMetaformat = HBufC8::NewL(MAXBUFLEN); 
        (aMetaformat->Des()).Copy(KLawmoMetaformat);         
        item->iMetaFormat = aMetaformat;
        
        HBufC8 *aMetamark = HBufC8::NewL(MAXBUFLEN); 
        (aMetamark->Des()).Copy(KLawmoMetamark);               
        item->iMetaMark = aMetamark;
        
        //appending result code
        HBufC8 *data = HBufC8::NewL(MAXBUFLEN);
        (data->Des()).Num(wipeStatus);
        item->iData = data;
        
        repository->Set(KLawmoWipeStatus,-1);//reset the wipestatus
        
        TInt genericStatus(KErrNone);
        RDEBUG("CLawmoDMAdapter::SendGenericAlert(): Appending data");
        iItemArray.AppendL(*item);
        RDEBUG("CLawmoDMAdapter::SendGenericAlert(): Issuing request");
        TRAP_IGNORE(genericStatus=privateAPI.AddDMGenericAlertRequestL(*descorrelator, iItemArray ));
        RDEBUG_2("CLawmoDMAdapter::SendGenericAlert(): Api call done %d", genericStatus);
        delete data;    
        delete aMetamark;    
        delete aMetaformat;    
        delete aMetatype;  
		delete aTargeturi;
        delete aSourceuri;
        delete item;
        delete descorrelator;		
        iItemArray.Reset();         
        iItemArray.Close();
        CleanupStack::PopAndDestroy( &privateAPI);
        }
    
    CleanupStack::PopAndDestroy();
    RDEBUG("CLawmoDMAdapter::SendGenericAlert(): end");    
}

TLawMoStatus CLawmoDMAdapter::LockDeviceL()
    {
    RDEBUG("CLawmoDMAdapter::LockDevice(): begin");
    TInt lockValue(0);
    TInt currentLawmoState(0);
    TLawMoStatus lawmostatus(ELawMoSuccess);
    RDEBUG("CLawmoDMAdapter::LockDevice(): 1");
    CRepository* crep = NULL;
    crep = CRepository::NewLC( KCRUidDeviceManagementInternalKeys );
    RDEBUG("CLawmoDMAdapter::LockDevice(): 2");

    User::LeaveIfError(RProperty::Get(KPSUidCoreApplicationUIs, KCoreAppUIsAutolockStatus,lockValue));
    RDEBUG("CLawmoDMAdapter::LockDevice(): 3");  
    
    // If Locked already by Tarm/autolock, set LawmoState.
    if(lockValue>EAutolockOff)
        {
        RDEBUG("CLawmoDMAdapter::LockDevice(): 3.0");
        TInt err = crep->Set( KLAWMOPhoneLock, EPartialLocked );
        CleanupStack::PopAndDestroy(crep);  
        if(err==KErrNone)   return lawmostatus;
        else 
        return ELawMoPartialLockFailed;
        }

    TInt reterr = crep->Get( KLAWMOPhoneLock, currentLawmoState );     
    if(reterr!= KErrNone)
        lawmostatus = ELawMoPartialLockFailed;
    RDEBUG("CLawmoDMAdapter::LockDevice(): 3.1");
    
    if(currentLawmoState==EUnlocked)
        {
        RTerminalControl tc;
        RTerminalControl3rdPartySession ts;
        
        User::LeaveIfError( tc.Connect() );
        CleanupClosePushL( tc );

        User::LeaveIfError( ts.Open( tc ) );
        CleanupClosePushL( ts );
        RDEBUG("CLawmoDMAdapter::LockDevice(): 3.5");
        TBuf8<8> data;
        data.Copy(_L8("1"));
        TInt status = ts.SetDeviceLockParameter(RTerminalControl3rdPartySession::ELock, data);        
        RDEBUG_2("CLawmoDMAdapter::LockDevice() %d", status );        
        if (status == KErrNone)    
            {
            reterr = crep->Set( KLAWMOPhoneLock, EPartialLocked );
            if (reterr!= KErrNone)
                lawmostatus = ELawMoPartialLockFailed;
            }
        else if (status == KErrCancel)
            lawmostatus = ELawMoUserCancelled;
        else
            lawmostatus = ELawMoPartialLockFailed;
        
        ts.Close();
        tc.Close();
        RDEBUG("CLawmoDMAdapter::LockDevice(): set val to 20");       
        CleanupStack::PopAndDestroy( &ts );
        CleanupStack::PopAndDestroy( &tc );
        RDEBUG("CLawmoDMAdapter::LockDevice(): 5");
        }
    
    CleanupStack::PopAndDestroy(crep);  
    RDEBUG("CLawmoDMAdapter::LockDevice(): End");
    return lawmostatus;
    }

TLawMoStatus CLawmoDMAdapter::UnLockDeviceL()
    {
    RDEBUG("CDCMOServer::UnLockDevice(): begin");
    CRepository* crep = NULL;
    TInt currentLawmoState =0;
    TLawMoStatus lawmostat(ELawMoSuccess);    
    TBool currentLockState(EFalse);
    TInt lockValue(0);
    User::LeaveIfError(RProperty::Get(KPSUidCoreApplicationUIs, KCoreAppUIsAutolockStatus,lockValue));
    RDEBUG("CDCMOServer::unLockDevice(): 0");

    if(lockValue>EAutolockOff)
        currentLockState = ETrue;
    
    RDEBUG("CDCMOServer::UnLockDevice(): 1");
    crep = CRepository::NewLC( KCRUidDeviceManagementInternalKeys );
    RDEBUG("CDCMOServer::UnLockDevice(): 2");
    TInt reterr = crep->Get( KLAWMOPhoneLock, currentLawmoState ); 
    RDEBUG("CDCMOServer::UnLockDevice(): 2.1");
    
    if(reterr!= KErrNone)
        lawmostat = ELawMoUnlockFailed;
    
    if((currentLawmoState != EUnlocked) ||(currentLockState))
        {
        RTerminalControl tc;
        RTerminalControl3rdPartySession ts;
        RDEBUG("CDCMOServer::UnLockDevice(): 3");
        User::LeaveIfError( tc.Connect() );
        CleanupClosePushL( tc );  
        User::LeaveIfError( ts.Open( tc ) );
        CleanupClosePushL( ts );
        RDEBUG("CDCMOServer::UnLockDevice(): 4");
        TBuf8<8> data;
        data.Copy(_L8("0"));
        TInt status = ts.SetDeviceLockParameter(RTerminalControl3rdPartySession::ELock, data);       
        RDEBUG_2("CDCMOServer::UnLockDevice() %d", status );
        if (status == KErrNone)    
            {
            reterr = crep->Set( KLAWMOPhoneLock, EUnlocked );
            if (reterr!= KErrNone)
                lawmostat = ELawMoPartialLockFailed;
            }
        else if (status == KErrCancel)
            lawmostat = ELawMoUserCancelled;
        else
            lawmostat = ELawMoUnlockFailed;
        
        ts.Close();
        tc.Close();
        RDEBUG("CDCMOServer::UnLockDevice(): set to 30");
        CleanupStack::PopAndDestroy( &ts );
        CleanupStack::PopAndDestroy( &tc );
        }
    
    if(!currentLockState)
        {
        reterr = crep->Set( KLAWMOPhoneLock, EUnlocked );
        if(reterr!=KErrNone)   
            lawmostat = ELawMoUnlockFailed;
        }

    CleanupStack::PopAndDestroy(crep); 
    RDEBUG("CDCMOServer::UnLockDevice(): End");
    return lawmostat;    
    }

TLawMoStatus CLawmoDMAdapter::GetStateL(TInt& aState)
{
    RDEBUG("CDCMOServer::GetState(): begin");
    TLawMoStatus lawmostat(ELawMoOk);
    CRepository* crep = NULL;
    crep = CRepository::NewLC( KCRUidDeviceManagementInternalKeys );
    RDEBUG("CDCMOServer::GetState(): created cenrep");
    TInt reterr = crep->Get( KLAWMOPhoneLock, aState );
    RDEBUG("CDCMOServer::GetState(): get on cenrep");
    if(reterr!= KErrNone)
    {
            RDEBUG("CDCMOServer::GetLocalCategoryL(): centrep Get error");
            lawmostat = ELawMoInvalid;
    }
    RDEBUG("CDCMOServer::GetState(): writing the int val");
    CleanupStack::PopAndDestroy(crep); 
    RDEBUG("CDCMOServer::GetState(): End");
    return lawmostat;
}

// End of File
