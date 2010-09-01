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
* Description:  This is the header for the PDP Test Engine
*
*/


#ifndef DIAGPDPTESTENGINE_H 
#define DIAGPDPTESTENGINE_H

// INCLUDES
#include <e32base.h>
#include <commdb.h>
#include <cmmanager.h>                      // RCmManager
#include <cmdestination.h>                  // RCmDestination
#include <cmconnectionmethod.h>             // RCmConnectionMethod
#include <es_sock.h> // for RSocketServ
#include <es_enum.h> //for TConnectionInfoV2
#include <commdbconnpref.h> //for TCommDbConnPref 
#include <ApDataHandler.h> 

//CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// CLASS DEFINITION
class CDiagPDPTestEngine : public CActive
    {
    protected:

        // Enumerations
            
        /**
        * This enum is used to identify primary IAP array or secondary IAP array
        */    
        enum TIapArray
            {
            EPrimaryAPNIapArray,
            ESecondaryAPNIapArray        
            };
            
        /**
        * C++ default constructor.
        */    
        CDiagPDPTestEngine();
        
        /**
        * C++ default virtual destructor.
        */    
        virtual ~CDiagPDPTestEngine();        
        
        /**
        * Any derived PDP engine must provide a concrete implementation of this function
        * and read the APNs from the cenrep in this function.
        * If function is able to read APNs successfully, argument aError is set as KErrNone
        */       
        virtual void ReadAPNsFromCenrepL(TInt& aError) = 0;
        
        /**
        * This function checks if the APNs read from the cenrep are visible and available to the 
        * Connection manager. After calling this function, the arrays iPrimaryAPNIapIdArray and
        * iSecondaryAPNIapIdArray contain the IAP Ids of Access Points which match the primary and
        * secondary APNs. It returns EFalse, if both the arrays are still empty, else it returns ETrue.
        */ 
        TBool CheckAPNsInConnectionManagerL();
        
        /**
        * This function checks the provided connection methods and checks if it matches the primary or
        * secondary APN. If so it adds the IAP ids of the connection method to the Primary or secondary 
        * IAP array.
        */
        void  CheckConnectionMatchL( const RCmConnectionMethod& aMethod);
        
        /**
        * This function returns ETrue if an already active connection is found using the IAP ids which
        * match with the APNs in primary or secondary IAP array, else return EFalse
        */
        TBool FindActiveConnectionsL();
    
    protected:    // from CActive
        /**
        * Handle active object completion event
        */
        void RunL();
    
        /**
        * Handle cancel
        */
        void DoCancel();
            
    protected:               
               
        // buffer to read the APNs from cenrep
        TBuf<KCommsDbSvrMaxFieldLength> iCenrepPrimaryAPN;
        TBuf<KCommsDbSvrMaxFieldLength> iCenrepSecondaryAPN;
        
        // arrays to hold the IAP Ids which have APNs as specified in the cenrep
        RArray<TUint32> iPrimaryAPNIapIdArray;
        RArray<TUint32> iSecondaryAPNIapIdArray;
        TUint32 iIAPiddefault;
        // used for iterating through active connections and establishing PDP context
        RSocketServ iSocketServ;
        RConnection iConnection;
    
    };

#endif //DIAGPDPTESTENGINE_H

// End of File
