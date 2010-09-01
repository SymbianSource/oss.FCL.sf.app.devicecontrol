/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of DM UI test component
* 	This is part of omadmappui test application.
*
*/




// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <W32STD.H> 
#include <COEMAIN.H> 
#include <APGCLI.H> 
#include <APGTASK.H> 
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "ui_dmui.h"

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
// Cui_dmui::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Cui_dmui::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Cui_dmui::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cui_dmui::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", Cui_dmui::ExampleL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Cui_dmui::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cui_dmui::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI

    /*
    TestModuleIf().Printf( 0, Kui_dmui, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, Kui_dmui, 
                                KParam, i, &string );
        i++;
        }
*/

RDebug::Print( _L("stiff "));
	                                                
const TUid KDeviceManagerUid = {0x101F6DE5};        // Device manager application 
RWsSession sess = CCoeEnv::Static()->WsSession();
RDebug::Print( 
	     _L("stiff after sess"));
    RApaLsSession apaLsSession;        
  
    TApaTaskList appList( sess );
    TApaTask bring = appList.FindApp( KDeviceManagerUid );
RDebug::Print( 
	     _L("stiff after app list"));
    if ( bring.Exists() )
        {
        	RDebug::Print( 
	     _L("stiff 1 exists"));
        bring.BringToForeground();
        }
    else
        {
        	RDebug::Print( 
	     _L("stiff 1 new app launch"));
        if( !apaLsSession.Handle() )
            {
            User::LeaveIfError(apaLsSession.Connect());
            RDebug::Print( 
	     _L("stiff 1 new app launch connect"));
            }
        CleanupClosePushL( apaLsSession );
        TThreadId thread;
        RDebug::Print( 
	     _L("stiff 1 new app thread creation"));
	     TInt t = apaLsSession.StartDocument(KNullDesC, KDeviceManagerUid, thread);
	     TBuf <20> err;
	     err.Num(t);
	     err.Append(_L("err"));
	     RDebug::Print(err );
        User::LeaveIfError( t );
         RDebug::Print( 
	     _L("stiff 1 new app thread created"));
        CleanupStack::PopAndDestroy( &apaLsSession );  
        RDebug::Print( 
	     _L("stiff 1 new destroy app ls session"));
          TApaTaskList appList( CCoeEnv::Static()->WsSession());
    TApaTask bring = appList.FindApp( KDeviceManagerUid );

    /*if ( bring.Exists() )
        {
        	RDebug::Print( 
	     _L("stiff 1 app send to bg"));
	     User::After(3000000);
        bring.SendToBackground();
        //bring.BringToForeground();
        //bring.SendToBackground();
        }*/
      }
      RDebug::Print( 
	     _L("stiff 1 return ke errnone"));
    return KErrNone;

    }


TInt Cui_dmui::LaunchserverviewL( CStifItemParser& aItem )
    {

       return KErrNone;

    }
// -----------------------------------------------------------------------------
// Cui_dmui::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt Cui_dmui::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
