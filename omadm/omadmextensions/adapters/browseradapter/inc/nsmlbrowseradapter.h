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
* Description:    DM Browser Adapter
*
*/






#ifndef __NSMLDMDEVINFOADAPTER_H__
#define __NSMLDMDEVINFOADAPTER_H__

// INCLUDES
#include <e32base.h>
#include <smldmadapter.h>
#include "nsmldmconstants.h"

// MACROS

// the DDF version must be changed if any changes in DDF structure 
// ( built in DDFStructureL() function )
_LIT8( KNSmlDMBrowserAdapterDDFVersion, "1.0" ); 

_LIT8 (KNSmlURISeparator ,"/");

_LIT8( KNSmlDMBrowserAdapterTextPlain, "text/plain" );
_LIT8( KNSmlDMBrowserAdapterNodeName, "Browser" );
_LIT8( KNSmlDMBrowserAdapterDescription, "The parent object holding all Browser objects" );
_LIT8( KNSmlDMBrowserStartpageURLDescription, "The default URL that is connected when the browser opens" );

_LIT8( KNSmlDMBrowserAdapterAPNode, "ToConRef" );
_LIT8( KNSmlDMBrowserAdapterBrowserNodes, "ToConRef/StartPage" );
_LIT8( KNSmlDMBrowserAdapterStartpageURL, "StartPage" );
_LIT8( KNSmlDMBrowserAdapterAPNodeDescription, "To set the default AP of browser" );
_LIT8( KNSmlDMBrowserAdapterAPvalue,"");

_LIT8( KNSmlDMBrowserAdapterAPDefaultvalue,"AP/APId001");


const TInt KMaxLengthStartpageName = 255;

const TInt KBrowserHomePageUDefined = 1;
const TInt KMaxLengthStartpageBookmark = 3;

const TUint32 KBrowserNGStartpageURL = 0x00000030;
//const TUint32 KBrowserNGHomepageType = 0x00000031;



// CLASS DECLARATION
class CNSmlDmBrowserAdapter : public CSmlDmAdapter
    {
public: // constructors and destructor
    static CNSmlDmBrowserAdapter* NewL(MSmlDmCallback* aDmCallback );
    static CNSmlDmBrowserAdapter* NewLC(MSmlDmCallback* aDmCallback );

    virtual ~CNSmlDmBrowserAdapter();

private: ///new functions

    void SetLeafPropertiesL( MSmlDmDDFObject& aObject, 
                 const TSmlDmAccessTypes& aAccessTypes, 
                 const TDesC8& aDescription ) const;
    CNSmlDmBrowserAdapter(TAny* aEcomArguments);
    CSmlDmAdapter::TError FetchLeafObjectL( const TDesC8& aURI, CBufBase& aObject );
    
private: // from base classes
    // Adapter interface from CSmlDmAdapter
    void DDFVersionL( CBufBase& aDDFVersion );
    void DDFStructureL( MSmlDmDDFObject& aDDF );
    void UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, 
                const TDesC8& aObject, const TDesC8& aType, 
                const TInt aStatusRef );
    void DeleteObjectL( const TDesC8& aURI, const TDesC8& aLUID, TInt aStatusRef ); 
    void FetchLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, 
                   const TDesC8& aType, const TInt aResultsRef, 
                   const TInt aStatusRef ); 
    void ChildURIListL( const TDesC8& aURI, const TDesC8& aLUID, 
                const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, 
                const TInt aResultsRef, const TInt aStatusRef );  
    void AddNodeObjectL( const TDesC8& aURI, const TDesC8& aParentLUID, 
                 const TInt aStatusRef );
    
    void UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, 
                RWriteStream*& aStream, const TDesC8& aType, 
                const TInt aStatusRef );
    void FetchLeafObjectSizeL( const TDesC8& aURI, const TDesC8& aLUID, 
                   const TDesC8& aType, const TInt aResultsRef, 
                   const TInt aStatusRef );
     
    void ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID,
                          const TDesC8& aArgument, 
                          const TDesC8& aType,
                          const TInt aStatusRef );
                                  
    void ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID,
                          RWriteStream*& aStream, 
                          const TDesC8& aType,
                          const TInt aStatusRef );
    
    void CopyCommandL( const TDesC8& aTargetURI, const TDesC8& aTargetLUID,
                       const TDesC8& aSourceURI, 
                       const TDesC8& aSourceLUID,
                       const TDesC8& aType, TInt aStatusRef );
    void StartAtomicL();
    void CommitAtomicL();
    void RollbackAtomicL();
    TBool StreamingSupport( TInt& aItemSize );
    void StreamCommittedL();
    void CompleteOutstandingCmdsL();
    
    
private:
    MSmlDmCallback* iDmCallback;
    };

#endif // __NSMLDMDEVINFOADAPTER_H__
