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
* Description:  Active Object for wiping Contacts, will be called by lawmocontactsplugin.
*
*/

#ifndef ACTIVEWIPEOBJECT_H 
#define ACTIVEWIPEOBJECT_H

#include <e32base.h>
#include <f32file.h>
#include <lawmocallbackinterface.h>

#include <CPbkContactEngine.h>
#include <CPbkSingleEntryFetchDlg.h>
#include <rpbkviewresourcefile.h>
#include <cntdb.h>
#include "fdebug.h"
 
#include <w32std.h>


class CActiveWipeObject : public CActive
{
public:
     static CActiveWipeObject* NewL(MLawmoPluginWipeObserver* aObserver);

     ~CActiveWipeObject();
     void WipeAsyncL();
private: //Derived from CActive
     // Handle completion
     void RunL();

     // How to cancel me
     void DoCancel();

     // Override to handle leaves from RunL(). Default implementation causes
     // the active scheduler to panic.
     TInt RunError(TInt aError);

private: //All private functions of this class     
     // Deletes the Phone book contacts
     void DeletePhoneContactsL();
     // Deletes all the SIM contacts
     void DeleteSIMContactsL();
     // Wait Till PhoneBook Application EndTask();
     TBool WaitTillPhoneBookKilled();     
     
     /**
      * second phase constructor
      */
     void ConstructL();
     
     // Construction
     CActiveWipeObject(MLawmoPluginWipeObserver* aObserver);
    
private: //All private members of this class
	    	
     MLawmoPluginWipeObserver* iObserver; //Not owned
     CContactDatabase* iContactsDb;
     const CContactIdArray* iContacts;
     RTimer iTimer;
     RWsSession iWsSession;
} ;

#endif //ACTIVEWIPEOBJECT_H
