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
* Description: Implementation of customization components
*
*/



// INCLUDE FILES

#include    "TestDmCallback.h"
#include "StifLogger.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestDmCallback::CTestDmCallback
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTestDmCallback::CTestDmCallback( CStifLogger *aLog )
	: iLog( aLog )
	, iMappingURI( 16 )
	, iMappingLuid( 16 )
//	RPointerArray<CBufBase> iResultObjects;
	, iResultTypes( 16 )
//	RArray<TInt> iStatusArray;
    {
    }

// -----------------------------------------------------------------------------
// CTestDmCallback::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTestDmCallback::ConstructL()
    {
//	CDesC8ArrayFlat *iMappingURI;
//	CDesC8ArrayFlat:: *iMappingURI;
//	CDesC8ArrayFlat *iMappingLuid;

	/* Containers for results */
//	RPointerArray<CBufBase> iResultObjects;
//	CDesC8ArrayFlat *iResultTypes;

	/* Container for status */
//	RArray<TInt> iStatusArray;
    }

// -----------------------------------------------------------------------------
// CTestDmCallback::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTestDmCallback* CTestDmCallback::NewL( CStifLogger *aLog )
    {
    CTestDmCallback* self = new( ELeave ) CTestDmCallback( aLog );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CTestDmCallback::~CTestDmCallback()
    {
    
    }


// -----------------------------------------------------------------------------
// CTestDmCallback::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*?type CTestDmCallback::?member_function(
    ?arg_type arg,
    ?arg_type arg )
    {
    
    ?code
    
    }
*/

void CTestDmCallback::SetResultsL( 
	TInt aResultsRef, 
	CBufBase& aObject,
	const TDesC8& aType )
	{
	TPtrC8 obj = aObject.Ptr( 0 ).Left( 120 );
	iLog->Log( _L8( "SetResults, ref=%d, object='%S', type='%S'" ), aResultsRef, &obj, &aType );
	if (aResultsRef < 0 || aResultsRef >= iResultObjects.Count() || aResultsRef >= iResultTypes.Count())
		{
		User::Leave( KErrArgument );
		}
		
//sf-	iResultObjects[ aResultsRef ]->Copy( aObject );
	iResultObjects[ aResultsRef ]->Reset();
//sf-	iResultObjects[ aResultsRef ]->InsertL( 0, aObject.Ptr( 0 ) );
	for (TInt i = 0 ; i < aObject.Size() ; )
		{
		TPtrC8 ptr = aObject.Ptr( i );
		iResultObjects[ aResultsRef ]->InsertL( i, ptr );
		i += ptr.Length();
		}
//sf-	iResultTypes[ aResultsRef ] = aType;
	iResultTypes.Delete( aResultsRef );
	iResultTypes.InsertL( aResultsRef, aType );
	}

void CTestDmCallback::SetStatusL( TInt aStatusRef,
							 MSmlDmAdapter::TError aErrorCode ) 
	{
	iLog->Log( _L8( "SetStatusL, ref=%d, code=%d" ), aStatusRef, aErrorCode );
	if (aStatusRef < 0 || aStatusRef >= iStatusArray.Count() )
		{
		User::Leave( KErrArgument );
		}

	iStatusArray[ aStatusRef ] = aErrorCode;
	}

void CTestDmCallback::SetMappingL( const TDesC8& aURI, const TDesC8& aLUID ) 
	{
	TPtrC8 ptrUri = aURI;
	TPtrC8 ptrLuid = aLUID.Left( 80 );
	iLog->Log( _L8( "SetMappingL, aURI='%S', aLUID='%S'" ), &ptrUri, &ptrLuid );

	TInt idx = FindMappingURI( aURI );
	if (idx < 0)
		{
		iMappingURI.AppendL( aURI );
		iMappingLuid.AppendL( aLUID );
		}
	else
		{
//sf-		iMappingLuid[ idx ] = aLUID;
		iMappingLuid.Delete( idx );
		iMappingLuid.InsertL( idx, aLUID );
		}
	}

HBufC8* CTestDmCallback::GetLuidAllocL( const TDesC8& aURI ) 
	{
	iLog->Log( _L8( "GetLuidAllocL, aURI='%S'" ), &aURI );
	HBufC8* ptr = GetLuidL( aURI ).AllocL();
	return ptr;
/*	HBufC8* ptr = 0;
	TInt idx = FindMappingURI( aURI );
	if (idx < 0)
		{
		ptr = HBufC8::NewL(2);
		}
	else
		{
		ptr = iMappingLuid[ idx ].AllocL();
		}

	return ptr;
*/	}

TPtrC8 CTestDmCallback::GetLuidL( const TDesC8& aURI ) 
	{
	iLog->Log( _L8( "GetLuidL, aURI='%S'" ), &aURI );
	TPtrC8 ptr;
	TInt idx = FindMappingURI( aURI );
	if (idx < 0)
		{
		ptr.Set( KNullDesC8 );
		}
	else
		{
		ptr.Set( iMappingLuid[ idx ] );
		}

	return ptr;
	}


/* New functions */

TInt CTestDmCallback::FindMappingURI( const TDesC8& aURI )
	{
	TInt idx = iMappingURI.Count() - 1;
	while (idx >= 0 && iMappingURI[ idx ] != aURI)
		{
		idx--;
		}
	return idx;
	}

TInt CTestDmCallback::GetNewResultsRefL( )
	{
	CBufBase* obj = CBufFlat::NewL( 100 );
	obj->Reset();

	CleanupStack::PushL( obj );
	User::LeaveIfError( iResultObjects.Append( obj ) );
	CleanupStack::Pop( obj );
	iResultTypes.AppendL( KNullDesC8 );
	
	TInt ref = iResultObjects.Count() - 1;
	return ref;
	}

TInt CTestDmCallback::GetNewStatusRefL( )
	{
	User::LeaveIfError( iStatusArray.Append( -1 ) );

	TInt ref = iStatusArray.Count() - 1;
	return ref;
	}
/*TInt CTestDmCallback::GetResultsL( TInt aResultsRef, )
	CBufBase& aObject,
	const TDesC8& aType )
	{
	
	}
*/

CBufBase& CTestDmCallback::GetResultsObjectL( TInt aResultsRef )
	{
	if (aResultsRef < 0 || aResultsRef >= iResultObjects.Count() )
		{
		User::Leave( KErrArgument );
		}
	return *iResultObjects[ aResultsRef ];
	}

TPtrC8 CTestDmCallback::GetResultsTypeL( TInt aResultsRef )
	{
	if (aResultsRef < 0 || aResultsRef >= iResultTypes.Count() )
		{
		User::Leave( KErrArgument );
		}
	return iResultTypes[aResultsRef];
	}

MSmlDmAdapter::TError CTestDmCallback::GetStatusL( TInt aStatusRef )
	{
	if (aStatusRef < 0 || aStatusRef >= iStatusArray.Count() )
		{
		User::Leave( KErrArgument );
		}
	return (MSmlDmAdapter::TError ) iStatusArray[aStatusRef];
	}

void CTestDmCallback::FreeResultsRefsL( )
	{
	iResultObjects.ResetAndDestroy();
	iResultTypes.Reset();
	}
	
void CTestDmCallback::FreeStatusRefsL( )
	{
	iStatusArray.Reset();
	}


/*
// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ?function_name implements...
// ?implementation_description.
// Returns: ?value_1: ?description
//          ?value_n: ?description
//                    ?description
// -----------------------------------------------------------------------------
//
?type  ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg )  // ?description
    {

    ?code

    }
*/
//  End of File  
