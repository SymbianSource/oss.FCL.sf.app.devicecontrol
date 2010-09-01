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
* Description: Implementation of customization components
*
*/



// INCLUDE FILES
#include <Stiftestinterface.h>
#include "ThemesTest.h"
#include "TestDmCallback.h"
//#include "TestDmDDFObject.h"
//#include "FileCoderB64.h"
#include "TestParams.h"


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

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CThemesTest::CThemesTest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CThemesTest::CThemesTest( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
	, iCallback( 0 )
	, iAdapter( 0 )
//	, iParams( )
//	, iDDFParams( )
        
    {
    }

// -----------------------------------------------------------------------------
// CThemesTest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CThemesTest::ConstructL()
    {
    iLog = CStifLogger::NewL( KThemesTestLogPath, 
                          KThemesTestLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );

    }

// -----------------------------------------------------------------------------
// CThemesTest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CThemesTest* CThemesTest::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CThemesTest* self = new (ELeave) CThemesTest( aTestModuleIf );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    
    }
    
// Destructor
CThemesTest::~CThemesTest()
    {
	delete iAdapter;
	delete iCallback;

    REComSession::FinalClose();

	iParams.ResetAndDestroy();
	iDDFParams.ResetAndDestroy();

    // Delete resources allocated from test methods
    Delete();
    
    // Delete logger
    delete iLog;  
    
    }
    
/*
void CThemesTest::AddParamsL( XTestParams* aParams )
	{
	CleanupStack::PushL( aParams );
	User::LeaveIfError( iParams.Append( aParams ) );
	CleanupStack::Pop( aParams );
	}

TInt CThemesTest::CheckAllL( )
	{
	TInt ret = KErrNone;
	
	for (TInt i = 0 ; i < iParams.Count() ; i++)
		{
		TInt result = iParams[i]->CheckL();
		if (result != KErrNone && ret == KErrNone)
			{
			ret = result;
			}
		}
		
	return ret;
	}
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( 
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {
    
    return ( CScriptBase* ) CThemesTest::NewL( aTestModuleIf );
        
    }


    
//  End of File
