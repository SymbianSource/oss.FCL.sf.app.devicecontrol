/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    DM browser Adapter
*
*/





//INCLUDE FILES
#include <implementationproxy.h> // For TImplementationProxy definition
#include "nsmldmimpluids.h"


#include "nsmldebug.h"
#include "nsmlconstants.h"
#include "nsmldmconst.h"
#include "nsmlbrowseradapter.h"
#include "nsmldmiapmatcher.h"

#include <centralrepository.h>
#include <browseruisdkcrkeys.h>
#include <commdb.h>
#include <cdbcols.h>             // CommsDB columname defs
#include <ApUtils.h>

#ifndef __WINS__
                                    // This lowers the unnecessary compiler warning (armv5) to remark.
                                    // "Warning: #174-D: expression has no effect..." is caused by 
                                    // DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif
const TInt KUserDefined = 0;


const TUint KNSmlDMBrowserAdapterImplUid = 0x1028335F;


// -----------------------------------------------------------------------------
// CNSmlDmBrowserAdapter* CNSmlDmBrowserAdapter::NewL( )
// -----------------------------------------------------------------------------
CNSmlDmBrowserAdapter* CNSmlDmBrowserAdapter::NewL(MSmlDmCallback* aDmCallback )
    {
    _DBG_FILE("CNSmlDmBrowserAdapter::NewL(): begin");
    CNSmlDmBrowserAdapter* self = NewLC( aDmCallback );
    CleanupStack::Pop();
    _DBG_FILE("CNSmlDmBrowserAdapter::NewL(): end");
    return self;
    }

// -----------------------------------------------------------------------------
// CNSmlDmBrowserAdapter* CNSmlDmBrowserAdapter::NewLC( )
// -----------------------------------------------------------------------------
CNSmlDmBrowserAdapter* CNSmlDmBrowserAdapter::NewLC(MSmlDmCallback* aDmCallback )
    {
    _DBG_FILE("CNSmlDmBrowserAdapter::NewLC(): begin");
    CNSmlDmBrowserAdapter* self = new (ELeave) CNSmlDmBrowserAdapter(aDmCallback);
    CleanupStack::PushL(self);
    self->iDmCallback = aDmCallback;
    _DBG_FILE("CNSmlDmBrowserAdapter::NewLC(): end");
    return self;
    }



// -----------------------------------------------------------------------------
// CNSmlDmBrowserAdapter::~CNSmlDmBrowserAdapter()
// -----------------------------------------------------------------------------
CNSmlDmBrowserAdapter::~CNSmlDmBrowserAdapter()
    {
    _DBG_FILE("CNSmlDmBrowserAdapter::~CNSmlDmBrowserAdapter(): begin");
    _DBG_FILE("CNSmlDmBrowserAdapter::~CNSmlDmBrowserAdapter(): end");
    }
    
// -----------------------------------------------------------------------------
// CNSmlDmBrowserAdapter::CNSmlDmBrowserAdapter()
// -----------------------------------------------------------------------------

CNSmlDmBrowserAdapter::CNSmlDmBrowserAdapter(TAny* aEcomArguments):CSmlDmAdapter(aEcomArguments)
    {
    _DBG_FILE("CNSmlDmBrowserAdapter::CNSmlDmBrowserAdapter(aEcomArguments): begin");
    _DBG_FILE("CNSmlDmBrowserAdapter::CNSmlDmBrowserAdapter(aEcomArguments): end"); 
    }

// -----------------------------------------------------------------------------
// CNSmlDmBrowserAdapter::SetLeafPropertiesL()
// -----------------------------------------------------------------------------
void CNSmlDmBrowserAdapter::SetLeafPropertiesL( MSmlDmDDFObject& aObject, 
                                                const TSmlDmAccessTypes& aAccessTypes, 
                                                const TDesC8& aDescription ) const
    {
    aObject.SetAccessTypesL( aAccessTypes );
    aObject.SetScopeL( MSmlDmDDFObject::EPermanent );
    aObject.SetDFFormatL( MSmlDmDDFObject::EChr );
    aObject.AddDFTypeMimeTypeL( KNSmlDMBrowserAdapterTextPlain );
    aObject.SetDescriptionL( aDescription );
    }


// -----------------------------------------------------------------------------
//  CNSmlDmBrowserAdapter::DDFVersionL()
// -----------------------------------------------------------------------------
void CNSmlDmBrowserAdapter::DDFVersionL(CBufBase& aDDFVersion)
    {
    _DBG_FILE("CNSmlDmBrowserAdapter::DDFVersionL(TDes& aDDFVersion): begin");
    aDDFVersion.InsertL(0,KNSmlDMBrowserAdapterDDFVersion);
    _DBG_FILE("CNSmlDmBrowserAdapter::DDFVersionL(TDes& aDDFVersion): end");
    }


// -----------------------------------------------------------------------------
//  CNSmlDmBrowserAdapter::DDFStructureL()
//
// -----------------------------------------------------------------------------
void CNSmlDmBrowserAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
    {
    _DBG_FILE("CNSmlDmBrowserAdapter::DDFStructureL(): begin");
    
    TSmlDmAccessTypes accessTypes;
    accessTypes.SetGet();
    
    TSmlDmAccessTypes accessSetTypes;
    accessSetTypes.SetGet();
    accessSetTypes.SetAdd();
    accessSetTypes.SetReplace();
    
    MSmlDmDDFObject& dmBrowserAdap = aDDF.AddChildObjectL( KNSmlDMBrowserAdapterNodeName );
    dmBrowserAdap.SetAccessTypesL( accessTypes ); 
    dmBrowserAdap.SetScopeL( MSmlDmDDFObject::EPermanent );
    dmBrowserAdap.SetDescriptionL( KNSmlDMBrowserAdapterDescription );
  
    MSmlDmDDFObject& toNapID = dmBrowserAdap.AddChildObjectL(KNSmlDMBrowserAdapterAPNode);
    toNapID.SetAccessTypesL( accessSetTypes );
    toNapID.SetScopeL( MSmlDmDDFObject::EDynamic );
    toNapID.SetDFFormatL( MSmlDmDDFObject::EChr );
    toNapID.AddDFTypeMimeTypeL( KNSmlDMBrowserAdapterTextPlain );
    toNapID.SetDescriptionL( KNSmlDMBrowserAdapterAPNodeDescription );
    
    MSmlDmDDFObject& startpageURL = dmBrowserAdap.AddChildObjectL(KNSmlDMBrowserAdapterStartpageURL);
    startpageURL.SetAccessTypesL( accessSetTypes );
    startpageURL.SetScopeL( MSmlDmDDFObject::EPermanent );
    startpageURL.SetDFFormatL( MSmlDmDDFObject::EChr );
    startpageURL.AddDFTypeMimeTypeL( KNSmlDMBrowserAdapterTextPlain );
    startpageURL.SetDescriptionL( KNSmlDMBrowserStartpageURLDescription );
    _DBG_FILE("CNSmlDmBrowserAdapter::DDFStructureL(): end");
    }

// -----------------------------------------------------------------------------
//  CNSmlDmBrowserAdapter::UpdateLeafObjectL()
// -----------------------------------------------------------------------------
void CNSmlDmBrowserAdapter::UpdateLeafObjectL( const TDesC8& aURI, 
                                               const TDesC8& aLUID, 
                                               const TDesC8& aObject, 
                                               const TDesC8& aType, 
                                               const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmBrowserAdapter::UpdateLeafObjectL(): begin");
    
     CSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;
    TInt ret = aURI.LocateReverse(KNSmlURISeparator()[0]);
    if ( ret == KErrNotFound ) 
        {
        retValue = CSmlDmAdapter::EError;
        iDmCallback->SetStatusL( aStatusRef, retValue );
        }
    else
        {
        iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
        TInt len = aURI.Length() - (ret + 1);
        TPtrC8 segment = aURI.Right( len );
        
    	DBG_ARGS8(_S8(" Value being set  URI - <%S> <%S> "), &aURI, &aObject );
                      
		if (segment == KNSmlDMBrowserAdapterAPNode )
            {
              // Get IAP ID and then set the browser setting 
            TInt err = KErrNone;  
            CNSmlDMIAPMatcher* iapmatch = CNSmlDMIAPMatcher::NewL( &Callback() );
            CleanupStack::PushL(iapmatch);
            
            TInt iap = iapmatch->IAPIdFromURIL( aObject );

            CleanupStack::PopAndDestroy(); // iapmatch
            
            if ( iap != KErrNotFound )
                {
                CCommsDatabase* commDb = CCommsDatabase::NewL();
                CleanupStack::PushL(commDb);
                CApUtils* aputils = CApUtils::NewLC(*commDb);
                TRAP(err, iap = aputils->WapIdFromIapIdL( iap ) );
                DBG_ARGS(_S("IAP = %d"), iap);

                CleanupStack::PopAndDestroy(2); //commdb,aputils

                if( err == KErrNone)
                    {
                    CRepository* repository = CRepository::NewLC( KCRUidBrowser );
    
				    // Force the browser to use the User Defined AP selection mode.
    				User::LeaveIfError( repository->Set( KBrowserAccessPointSelectionMode,
                                         KUserDefined ) );
    				// This sets the accesspoint default for browser.
    				User::LeaveIfError( repository->Set( KBrowserDefaultAccessPoint,
                                         iap ) );
				    CleanupStack::PopAndDestroy(); // repository
    
                    }
                else
                    {
                    _DBG_FILE("CNSmlDmMMSAdapter::UpdateDataL(): \
                    err!=KErrNone");
                    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject );
                     
                    }
                }
            else
                {
                _DBG_FILE("CNSmlDmMMSAdapter::UpdateDataL(): status = \
                CSmlDmAdapter::EInvalidObject");
              	iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject );
                }
            }
		else if(segment == KNSmlDMBrowserAdapterStartpageURL )
		    {
		    	// To Get the Browser Start page URL
		    TBuf<KMaxLengthStartpageName> aBuf;
		    TInt aErr;
		    aBuf.Zero();
		    aBuf.Copy(aObject);

            if(aBuf.Length() == 0 || aBuf.Length() > KMaxLengthStartpageName)
                {
                _DBG_FILE("CNSmlDmBrowserAdapter::UpdateLeafObjectL(): Error Zero Length String");
                iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject);
                return;
                }
                
		    
		    CRepository* repository = CRepository::NewLC( KCRUidBrowser );
		        
		    if(repository)
		        {
		        _DBG_FILE("CNSmlDmBrowserAdapter::UpdateLeafObjectL(): Before Set for StartPage");
		        User::LeaveIfError( aErr = repository->Set( KBrowserNGStartpageURL,aBuf));
		        if(aErr != KErrNone)
                    {
                    _DBG_FILE("CNSmlDmBrowserAdapter::UpdateLeafObjectL(): Error in Cenrep Set");
                    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError);
					return;
                    }
		        _DBG_FILE("CNSmlDmBrowserAdapter::UpdateLeafObjectL(): Before Set for StartPageType");
		        User::LeaveIfError( aErr = repository->Set( KBrowserNGHomepageType,KBrowserHomePageUDefined));
                if(aErr != KErrNone)
                    {
                    _DBG_FILE("CNSmlDmBrowserAdapter::UpdateLeafObjectL(): Error in Cenrep Set");
                    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError);
                    return;
                    }

		        }
		    _DBG_FILE("CNSmlDmBrowserAdapter::UpdateLeafObjectL(): Before Setting status to OK");
            CleanupStack::PopAndDestroy(); // repository
            iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
		        
		    }
        else
            {
            iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
            }   
        }
        
    _DBG_FILE("CNSmlDmBrowserAdapter::UpdateLeafObjectL(): end");
    }


// -----------------------------------------------------------------------------
// CNSmlDmBrowserAdapter::DeleteObjectL()
// -----------------------------------------------------------------------------
void CNSmlDmBrowserAdapter::DeleteObjectL( const TDesC8& /*aURI*/, 
                                           const TDesC8& /*aLUID*/, 
                                           const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmBrowserAdapter::DeleteLeafObjectL( ): begin");
    _DBG_FILE("CNSmlDmBrowserAdapter::DeleteLeafObjectL( ): end");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    }


// -----------------------------------------------------------------------------
//  CNSmlDmBrowserAdapter::FetchLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, 
// const TDesC8& aType, const TInt aResultsRef, const TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmBrowserAdapter::FetchLeafObjectL( const TDesC8& aURI, 
                                              const TDesC8& /*aLUID*/, 
                                              const TDesC8& aType, 
                                              const TInt aResultsRef, 
                                              const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmBrowserAdapter::FetchLeafObjectL(): begin");
    
    CBufBase *object = CBufFlat::NewL( 1 );
    CleanupStack::PushL( object );
    
    CSmlDmAdapter::TError retValue = FetchLeafObjectL( aURI, *object );
    
    iDmCallback->SetStatusL( aStatusRef, retValue );
    iDmCallback->SetResultsL( aResultsRef, *object, aType );
    CleanupStack::PopAndDestroy(); //object
    _DBG_FILE("CNSmlDmDevInfoAdapt+er::FetchLeafObjectL(): end");
    }


// -----------------------------------------------------------------------------
//  CNSmlDmBrowserAdapter::ChildURIListL( const TDesC& aURI, 
// const TDesC& aParentLUID, const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, 
// const TInt aResultsRef, const TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmBrowserAdapter::ChildURIListL( const TDesC8& aURI, 
                                           const TDesC8& /*aParentLUID*/, 
                                           const CArrayFix<TSmlDmMappingInfo>& /*aPreviousURISegmentList*/, 
                                           const TInt aResultsRef, 
                                           const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmBrowserAdapter::ChildURIListL(): begin");
    CSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;
    CBufBase* currentURISegmentList = CBufFlat::NewL( 1 );
    CleanupStack::PushL( currentURISegmentList );
    TInt ret = aURI.LocateReverse(KNSmlURISeparator()[0]);
    if ( ret == KErrNotFound ) 
        {
        ret = -1;
        }
    TInt len = aURI.Length() - ( ret + 1 );
    TPtrC8 segment = aURI.Right( len );

    if ( segment == KNSmlDMBrowserAdapterNodeName )
        {
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KNSmlDMBrowserAdapterBrowserNodes() );
        }
    
    else
        {
        retValue = CSmlDmAdapter::EError;
        }
    iDmCallback->SetStatusL( aStatusRef, retValue );
    iDmCallback->SetResultsL( aResultsRef, *currentURISegmentList, KNullDesC8 );
    CleanupStack::PopAndDestroy(); //currentURISegmentList
    _DBG_FILE("CNSmlDmBrowserAdapter::ChildURIListL(): end");
    }


// -----------------------------------------------------------------------------
// CNSmlDmBrowserAdapter::AddNodeObjectL( const TDesC8& aURI, 
// const TDesC8& aParentLUID, const TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmBrowserAdapter::AddNodeObjectL( const TDesC8& /*aURI*/, 
                                            const TDesC8& /*aParentLUID*/, 
                                            const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmBrowserAdapter::AddNodeObjectL(): begin");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    _DBG_FILE("CNSmlDmBrowserAdapter::AddNodeObjectL(): end");
    }

// -----------------------------------------------------------------------------
//  CNSmlDmBrowserAdapter::UpdateLeafObjectL()
// -----------------------------------------------------------------------------
void CNSmlDmBrowserAdapter::UpdateLeafObjectL( const TDesC8& /*aURI*/, 
                                               const TDesC8& /*aLUID*/, 
                                               RWriteStream*& /*aStream*/, 
                                               const TDesC8& /*aType*/, 
                                               const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmBrowserAdapter::UpdateLeafObjectL(): stream: begin");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    _DBG_FILE("CNSmlDmBrowserAdapter::UpdateLeafObjectL(): stream: end");
    }


// -----------------------------------------------------------------------------
//  CNSmlDmBrowserAdapter::FetchLeafObjectSizeL( const TDesC8& aURI, 
//  const TDesC8& aLUID, const TDesC8& aType, TInt aResultsRef, TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmBrowserAdapter::FetchLeafObjectSizeL( const TDesC8& aURI, 
                                                  const TDesC8& /*aLUID*/, 
                                                  const TDesC8& aType, 
                                                  const TInt aResultsRef, 
                                                  const TInt aStatusRef )
    {

    _DBG_FILE("CNSmlDmBrowserAdapter::FetchLeafObjectSizeL(): begin");
    
    CBufBase *object = CBufFlat::NewL( 1 );
    CleanupStack::PushL( object );
    CSmlDmAdapter::TError retValue = FetchLeafObjectL( aURI, *object );
        
    TInt objSizeInBytes = object->Size();
    TBuf8<2> stringObjSizeInBytes;
    stringObjSizeInBytes.Num( objSizeInBytes );
    object->Reset();
    object->InsertL( 0, stringObjSizeInBytes );
    
    iDmCallback->SetStatusL( aStatusRef, retValue );
    iDmCallback->SetResultsL( aResultsRef, *object, aType );
    CleanupStack::PopAndDestroy(); //object
    _DBG_FILE("CNSmlDmBrowserAdapter::FetchLeafObjectSizeL(): end");
    }
    

// -----------------------------------------------------------------------------
//  CNSmlDmBrowserAdapter::ExecuteCommandL( const TDesC8& aURI, 
//  const TDesC8& aParentLUID, TDesC8& aArgument, const TDesC8& aType, TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmBrowserAdapter::ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aArgument, const TDesC8& aType, const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmBrowserAdapter::ExecuteCommandL(): begin");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    _DBG_FILE("CNSmlDmBrowserAdapter::ExecuteCommandL(): end");
    }

// -----------------------------------------------------------------------------
//  CNSmlDmBrowserAdapter::ExecuteCommandL( const TDesC8& aURI, 
//  const TDesC8& aParentLUID, RWriteStream*& aStream, const TDesC8& aType, 
//  TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmBrowserAdapter::ExecuteCommandL( const TDesC8& /*aURI*/, 
                                             const TDesC8& /*aParentLUID*/, 
                                             RWriteStream*& /*aStream*/, 
                                             const TDesC8& /*aType*/, 
                                             const TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmBrowserAdapter::ExecuteCommandL(): stream: begin");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    _DBG_FILE("CNSmlDmBrowserAdapter::ExecuteCommandL(): stream: end");
    }

// -----------------------------------------------------------------------------
//  CNSmlDmBrowserAdapter::CopyCommandL( const TDesC8& aTargetURI, const TDesC8& 
//  aSourceURI, const TDesC8& aSourceLUID, const TDesC8& /*aType*/, TInt aStatusRef )
// -----------------------------------------------------------------------------
void CNSmlDmBrowserAdapter::CopyCommandL( const TDesC8& /*aTargetURI*/, const TDesC8& 
                                          /*aTargetLUID*/, const TDesC8& /*aSourceURI*/, 
                                          const TDesC8& /*aSourceLUID*/, 
                                          const TDesC8& /*aType*/, TInt aStatusRef )
    {
    _DBG_FILE("CNSmlDmBrowserAdapter::CopyCommandL(): begin");
    iDmCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    _DBG_FILE("CNSmlDmBrowserAdapter::CopyCommandL(): end");
    }

// --------------------------------------
//  CNSmlDmBrowserAdapter::StartAtomicL()
// --------------------------------------
void CNSmlDmBrowserAdapter::StartAtomicL()
    {
    _DBG_FILE("CNSmlDmBrowserAdapter::StartAtomicL(): begin");
    _DBG_FILE("CNSmlDmBrowserAdapter::StartAtomicL(): end");
    }
    
// ---------------------------------------
//  CNSmlDmBrowserAdapter::CommitAtomicL()
// ---------------------------------------
void CNSmlDmBrowserAdapter::CommitAtomicL()
    {
    _DBG_FILE("CNSmlDmBrowserAdapter::CommitAtomicL(): begin");
    _DBG_FILE("CNSmlDmBrowserAdapter::CommitAtomicL(): end");
    }

// -----------------------------------------
//  CNSmlDmBrowserAdapter::RollbackAtomicL()
// -----------------------------------------
void CNSmlDmBrowserAdapter::RollbackAtomicL()
    {
    _DBG_FILE("CNSmlDmBrowserAdapter::RollbackAtomicL(): begin");
    _DBG_FILE("CNSmlDmBrowserAdapter::RollbackAtomicL(): end");
    }

// -----------------------------------------------------------
//  CNSmlDmBrowserAdapter::StreamingSupport( TInt& aItemSize )
// -----------------------------------------------------------  
TBool CNSmlDmBrowserAdapter::StreamingSupport( TInt& /*aItemSize*/ )
    {
    _DBG_FILE("CNSmlDmBrowserAdapter::StreamingSupport(): begin");
    _DBG_FILE("CNSmlDmBrowserAdapter::StreamingSupport(): end");
    return EFalse;
    }

// ------------------------------------------
//  CNSmlDmBrowserAdapter::StreamCommittedL()
// ------------------------------------------
void CNSmlDmBrowserAdapter::StreamCommittedL()
    {
    _DBG_FILE("CNSmlDmBrowserAdapter::StreamCommittedL(): begin");
    _DBG_FILE("CNSmlDmBrowserAdapter::StreamCommittedL(): end");
    }

// --------------------------------------------------
//  CNSmlDmBrowserAdapter::CompleteOutstandingCmdsL()
// --------------------------------------------------   
void CNSmlDmBrowserAdapter::CompleteOutstandingCmdsL()
    {
    _DBG_FILE("CNSmlDmBrowserAdapter::CompleteOutstandingCmdsL(): begin");
    _DBG_FILE("CNSmlDmBrowserAdapter::CompleteOutstandingCmdsL(): end");    
    }

// -----------------------------------------------------------------------------
// CNSmlDmDevDetailAdapter::FetchLeafObjectL()
// -----------------------------------------------------------------------------
CSmlDmAdapter::TError CNSmlDmBrowserAdapter::FetchLeafObjectL( const TDesC8& aURI, 
                                                               CBufBase& aObject )
    {
    _DBG_FILE("CNSmlDmInfoAdapter::FetchLeafObjectL(): begin");
    CSmlDmAdapter::TError retValue = CSmlDmAdapter::EOk;
    TInt ret = aURI.LocateReverse(KNSmlURISeparator()[0]);
    TInt err = KErrNone;
    if ( ret == KErrNotFound ) 
        {
        retValue = CSmlDmAdapter::EError;
        }
    else
        {
        TInt len = aURI.Length() - (ret + 1);
        TPtrC8 segment = aURI.Right( len );
        
        if(segment == KNSmlDMBrowserAdapterAPNode)
            {
            TInt iap ;
            CRepository* repository = CRepository::NewLC( KCRUidBrowser );
    
		    // Force the browser to use the User Defined AP selection mode.
    		TInt selectionMode;
    		User::LeaveIfError( repository->Get( KBrowserAccessPointSelectionMode,
                                         selectionMode ) );
    		if (selectionMode != KUserDefined)                                          
        		{
         		retValue= CSmlDmAdapter::ENotFound;
		         
        		CleanupStack::PopAndDestroy(); // repository
        		}
       		else
        		{
        		// This sets the accesspoint default for browser.
    			User::LeaveIfError( repository->Get( KBrowserDefaultAccessPoint,
                                         iap ) );
    			CleanupStack::PopAndDestroy(); // repository
            
            	CCommsDatabase* commDb = CCommsDatabase::NewL();
                CleanupStack::PushL(commDb);
                CApUtils* aputils = CApUtils::NewLC(*commDb);
                TRAP(err, iap = aputils->IapIdFromWapIdL( iap ) );
                DBG_ARGS(_S("IAP = %d"), iap);

        
        	if( iap != -1 && err == KErrNone )
            	{
            	CNSmlDMIAPMatcher* iapmatch = CNSmlDMIAPMatcher::NewL( &Callback() );
            	CleanupStack::PushL(iapmatch);
    
            	HBufC8* uri8 = iapmatch->URIFromIAPIdL( iap );
                
            if( uri8 )
                {
                CleanupStack::PushL(uri8);
                aObject.InsertL(0,uri8->Des());
                retValue= CSmlDmAdapter::EOk;
                
                CleanupStack::PopAndDestroy(); // uri8
                }
            else
                {
                retValue= CSmlDmAdapter::ENotFound;
                
                }    
            CleanupStack::PopAndDestroy(); // iapMatch
            
            }
            else
                {
                retValue= CSmlDmAdapter::ENotFound;
                }
                CleanupStack::PopAndDestroy(); // apUrils
                CleanupStack::PopAndDestroy(); // commDb
            }
        }
        else if(segment == KNSmlDMBrowserAdapterStartpageURL)
            {
            	// To Set the Browser Start page URL
            TBuf8<KMaxLengthStartpageName> aName;
            CRepository* centrep( NULL);
            aName.Zero();
            centrep = CRepository::NewLC(KCRUidBrowser);

            if ( centrep )
                {
                TFullName temp;
                
                if (centrep->Get( KBrowserNGStartpageURL, temp )==KErrNone && temp.Length() )
                    {
                    _DBG_FILE("CNSmlDmBrowserAdapter::FetchLeafObjectL(): Got the value for start page");
                    temp.Trim();
                    aName.Copy(temp);
                    }
                CleanupStack::PopAndDestroy(centrep);
                }
            aObject.InsertL(0,aName);
            retValue= CSmlDmAdapter::EOk;   
            }
    
        }
    _DBG_FILE("CNSmlDmInfoAdapter::FetchLeafObjectL(): end");
    return retValue;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#ifndef IMPLEMENTATION_PROXY_ENTRY
#define IMPLEMENTATION_PROXY_ENTRY(aUid, aFuncPtr)  {{aUid},(aFuncPtr)}
#endif

const TImplementationProxy ImplementationTable[] = 
    {
        IMPLEMENTATION_PROXY_ENTRY(KNSmlDMBrowserAdapterImplUid, CNSmlDmBrowserAdapter::NewL)
    };

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    _DBG_FILE("ImplementationGroupProxy() for CNSmlDmBrowserAdapter: begin");
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    _DBG_FILE("ImplementationGroupProxy() for CNSmlDmBrowserAdapter: end");
    return ImplementationTable;
    }

// End of file

