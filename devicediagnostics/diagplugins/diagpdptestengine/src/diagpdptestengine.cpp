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
* Description:  This is the class module for the PDP Test Engine
*
*/


// INCLUDE FILES
#include <DiagFrameworkDebug.h>                 // debug macros
#include "diagpdptestengine.h"
#include <cmpluginpacketdatadef.h>
//#include <AknLists.h>
//#include <BADESCA.H >
//#include <aknPopup.h> 
// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
//const TInt KConnArrayGranularity = ( 3 );

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ==============================

// ========================= MEMBER FUNCTIONS ================================
  
 
// ----------------------------------------------------------------------------
// CDiagPDPTestEngine::CDiagPDPTestEngine()
//
// C++ default constructor
// ----------------------------------------------------------------------------    
CDiagPDPTestEngine::CDiagPDPTestEngine() 
    : CActive(CActive::EPriorityUserInput)
    {
    // no implementation needed 
    }
   
// ----------------------------------------------------------------------------
// CDiagPDPTestEngine::~CDiagPDPTestEngine()
//
// C++ default destructor
// ----------------------------------------------------------------------------    
CDiagPDPTestEngine::~CDiagPDPTestEngine()
    {
    // no implementation needed
    }   
   
// ----------------------------------------------------------------------------
// CDiagPDPTestEngine::RunL()
//
// Handle asynchronous requests
// ----------------------------------------------------------------------------     
void CDiagPDPTestEngine::RunL()
    {
    // no implementation needed 
    } 
    
    
// ----------------------------------------------------------------------------
// CDiagPDPTestEngine::DoCancel()
//
// Handle cancellation of asynchronous requests
// ----------------------------------------------------------------------------    
void CDiagPDPTestEngine::DoCancel()
    {
    // no implementation needed 
    }
    
// ----------------------------------------------------------------------------
// CDiagPDPTestEngine::CheckAPNsInConnectionManagerL()
//
// 
// ----------------------------------------------------------------------------    
TBool CDiagPDPTestEngine::CheckAPNsInConnectionManagerL()
    {
    LOGSTRING( "CDiagPDPTestEngine::CheckAPNsInConnectionManagerL()" );
    /*RArray<TUint32> idArray;
    CleanupClosePushL( idArray );

    CDesCArrayFlat* nameArray = new (ELeave) CDesCArrayFlat(16);
    CleanupStack::PushL( nameArray );
    
    // Get IAP names and ids from the database
    CCommsDatabase* TheDb = CCommsDatabase::NewL( EDatabaseTypeIAP );
    CleanupStack::PushL( TheDb );

    TheDb->ShowHiddenRecords();

    CCommsDbTableView* view = TheDb->OpenTableLC( TPtrC(IAP) );

    TBuf<40> name;
    TUint32 id,aId;
    TInt res = view->GotoFirstRecord();
    while( res == KErrNone )
    {
        view->ReadTextL( TPtrC(COMMDB_NAME), name );
        view->ReadUintL( TPtrC(COMMDB_ID), id );
        
        idArray.Insert( id, 0 );
        nameArray->InsertL( 0, name );

        res = view->GotoNextRecord();
        RDebug::Print(_L("IAP name, id: %S, %d"), &name, id);
    }
    
    CleanupStack::PopAndDestroy( view ); // view
    CleanupStack::PopAndDestroy( TheDb ); // TheDb
    
    // Create listbox and PUSH it.
    CEikTextListBox* list = new(ELeave) CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL( list );
    
    // Create popup list and PUSH it.
    CAknPopupList* popupList = CAknPopupList::NewL(
        list, R_AVKON_SOFTKEYS_OK_CANCEL,
        AknPopupLayouts::EMenuWindow);
    CleanupStack::PushL( popupList );
    
    // initialize listbox.
    list->ConstructL( popupList, CEikListBox::ELeftDownInViewRect );
    list->CreateScrollBarFrameL( ETrue );
    list->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff,
        CEikScrollBarFrame::EAuto );
    
    // Set listitems.
    CTextListBoxModel* model = list->Model();
    model->SetItemTextArray( nameArray );
    model->SetOwnershipType( ELbmDoesNotOwnItemArray );
    
    // Set title
    popupList->SetTitleL( _L("Select IAP:") );
    
    // Show popup list.
    TInt popupOk = popupList->ExecuteLD();
    CleanupStack::Pop( popupList ); // popupList
    if ( popupOk )
        {
        TInt index = list->CurrentItemIndex();
        aId = idArray[index];
        }
    
    CleanupStack::PopAndDestroy( 3, &idArray );  // list, nameArray, idArray
    
    if(popupOk != EFalse)
					{
					iIAPiddefault = aId;
					}
		return popupOk;*/
		
		
       /*CCommsDatabase* cdb = CCommsDatabase::NewL(EDatabaseTypeIAP);
       CleanupStack::PushL(cdb);
       LOGSTRING("CCommsDatabase::NewL");
       CApDataHandler* apDataHandler = CApDataHandler::NewLC(*cdb);
       LOGSTRING("CApDataHandler::NewLC");
       iIAPiddefault = apDataHandler->DefaultL(EFalse);
       LOGSTRING2( "CApDataHandler->Default %d",iIAPiddefault );
       CleanupStack::PopAndDestroy(2);
       LOGSTRING( "pop/destroy 2");
       return ETrue;*/
/*    LOGSTRING( "CDiagPDPTestEngine::CheckAPNsInConnectionManagerL()" )

    // Reset the arrays to hold valid IAP Ids
    iPrimaryAPNIapIdArray.Reset();;
    iSecondaryAPNIapIdArray.Reset();    
    
    // Opens a session with the connection manager
    RCmManager connManager; 
    connManager.OpenLC();
    LOGSTRING( "CDiagPDPTestEngine session with connection manager opened." )

    // create a conneciton array to hold the Ids of connections    
    RArray< TUint32 > connArray = RArray< TUint32 >( KConnArrayGranularity );
    CleanupClosePushL( connArray );
    
    // Get a list of all the destinations
    LOGSTRING( "CDiagPDPTestEngine Get a list of all the destinations." )
    connManager.AllDestinationsL( connArray );    
    TInt numberOfDestinations = connArray.Count();    
    LOGSTRING2( "CDiagPDPTestEngine number of destinations = %d", numberOfDestinations )
    
      
    // Look through all the destinations' connection methods and see if any of
    // them match the CenRep data.
    for ( TInt i = 0; i < numberOfDestinations; i++ )
        {
        RCmDestination destination = connManager.DestinationL( connArray[ i ] );
        CleanupClosePushL( destination );

        // Don't use any hidden destinations, because they are not visible to
        // the user.
        if ( destination.IsHidden() )
            {
            LOGSTRING2( "CDiagPDPTestEngine: Destination number %d is hidden, hence ignoring it", i)
            CleanupStack::PopAndDestroy(); // destination
            continue;
            }

        // get the number of connection methods with this destination
        TInt numberOfConnectionMethods = destination.ConnectionMethodCount();
        LOGSTRING3( "CDiagPDPTestEngine For destination %d, number of connection methods are %d", i, numberOfConnectionMethods )
        
        for ( TInt j = 0; j < numberOfConnectionMethods; j++ )
            {
            RCmConnectionMethod method = destination.ConnectionMethodL( j );
            CleanupClosePushL( method );
            
            // This call is trapped because not all connection methods support
            // the data we're querying, and the leave should not be propagated
            // up because that would not be a reason for test failure.
            TRAP_IGNORE( CheckConnectionMatchL( method ) )
            CleanupStack::PopAndDestroy(); // method
            }

        CleanupStack::PopAndDestroy(); // destination
        }        


    // Get a list of all the uncategorized connection methods.
    LOGSTRING( "CDiagPDPTestEngine : Get a list of all the uncategorized connection methods." )
    connArray.Reset();
    connManager.ConnectionMethodL( connArray );
    
    TInt numberOfUncategorizedConnectionMethods = connArray.Count();    
    LOGSTRING2( "CDiagPDPTestEngine : number of uncategorized connection methods = %d", numberOfUncategorizedConnectionMethods)

    // Look through all the uncategorized connection methods and see if any
    // of them match match the CenRep data.
    for ( TInt i = 0; i < numberOfUncategorizedConnectionMethods; i++ )
        {
        RCmConnectionMethod method = connManager.ConnectionMethodL( connArray[ i ] );
        CleanupClosePushL( method );

        // This call is trapped because not all connection methods support
        // the data we're querying, and the leave should not be propagated
        // up because that would not be a reason for test failure.
        TRAP_IGNORE( CheckConnectionMatchL( method) )
        CleanupStack::PopAndDestroy(); // method
        }

    CleanupStack::PopAndDestroy(); // connArray
    CleanupStack::PopAndDestroy(); // connManager
    
    // check if atleast one occurrence each of the primary APN or the secondary APN was found in CommsDB
    if( (iPrimaryAPNIapIdArray.Count() > 0) || (iSecondaryAPNIapIdArray.Count() > 0) )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    */
    return ETrue; //compiler warning
    }


// ----------------------------------------------------------------------------
// CDiagPDPTestEngine::CheckConnectionMatchL()
//
//
// ----------------------------------------------------------------------------  

void CDiagPDPTestEngine::CheckConnectionMatchL(const RCmConnectionMethod& aMethod)
    {
    LOGSTRING( "CDiagBrowserPluginModel::CheckConnectionMatchL()" )

    HBufC* name = aMethod.GetStringAttributeL(
        CMManager::ECmName );
    CleanupStack::PushL( name );

    HBufC* apn = aMethod.GetStringAttributeL(
        CMManager::EPacketDataAPName );

    CleanupStack::PushL( apn );

    TInt type = aMethod.GetIntAttributeL(
        CMManager::ECmBearerType );

    TBool hidden = aMethod.GetBoolAttributeL(
        CMManager::ECmHidden );

    TUint32 apId = aMethod.GetIntAttributeL(
        CMManager::ECmIapId );   

    LOGSTRING5(
        "CDiagBrowserPluginModel::CheckConnectionMatchL() "
        L" Name '%S', APN '%S' ,Type '0x%08x', Hidden '%d'", name, apn, type, hidden )


    if ( ( apn->Compare( iCenrepPrimaryAPN ) == 0 ) && 
         type == KUidPacketDataBearerType &&
         !hidden )
        {
        LOGSTRING( "CDiagBrowserPluginModel connection matches primary APN, adding to primary IAP array" )
        iPrimaryAPNIapIdArray.Append(apId);
        }
        
    if ( ( apn->Compare( iCenrepSecondaryAPN ) == 0 ) && 
         type == KUidPacketDataBearerType &&
         !hidden )
        {
        LOGSTRING( "CDiagBrowserPluginModel connection matches secondary APN, adding to secondary IAP array" )
        iSecondaryAPNIapIdArray.Append(apId);
        }        

    CleanupStack::PopAndDestroy( apn );
    CleanupStack::PopAndDestroy( name );
    }

    
// ----------------------------------------------------------------------------
// CDiagPDPTestEngine::FindActiveConnectionsL()
//
// this function returns ETrue if an already active connection is found using 
// the IAP ids which match with the specified APNs, else it returns EFalse
// ----------------------------------------------------------------------------       
TBool CDiagPDPTestEngine::FindActiveConnectionsL() 
    {
    LOGSTRING( "CDiagPDPTestEngine::FindActiveConnectionsL");
    TBool result = EFalse;   
    TUint connectionCount = 0;
    TPckgBuf<TConnectionInfo> connectionInfo;
    User::LeaveIfError(
        iConnection.EnumerateConnections(connectionCount));
    LOGSTRING("iConnection.EnumerateConnections");
    if(connectionCount > 0)
        {
        // there is atleast one active connection

        // iterate through each active connection
        for(TUint count = 1 ; count <= connectionCount; count++)
            {
            User::LeaveIfError(
                iConnection.GetConnectionInfo(count, connectionInfo));               
            
            // test if the IAP Id of this active connection matches with one of the IAP ids in 
            // primary array or secondary array
            /*if( ((iPrimaryAPNIapIdArray.Find( connectionInfo().iIapId)) != KErrNotFound ) || 
                ((iSecondaryAPNIapIdArray.Find( connectionInfo().iIapId)) != KErrNotFound ) )*/
	        if (iIAPiddefault == connectionInfo().iIapId)
                {
                // the IAP ID of the current active connection has matched with the one of the IAPs in primary array or secondary array
                // data connection has PASSED
                LOGSTRING("IAP ID of the current active connection has matched");
                result = ETrue;
                break; //break from the for loop
                }
                else
                {
                // do nothing, look for next active connection
                }
            }
        }
    else
        {
        // no active connection found
        result = EFalse;        
        }

    return result;
    }
    
    

// End of File
