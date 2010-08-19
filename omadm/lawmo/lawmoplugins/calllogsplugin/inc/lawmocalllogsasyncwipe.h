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
* Description:  Active object for wiping CallLogs, called by lawmocalllogsplugin.
*
*/

#ifndef LAWMOCALLLOGSAYNCWIPE_H 
#define LAWMOCALLLOGSAYNCWIPE_H

#include <e32base.h>
#include <f32file.h>
#include <lawmocallbackinterface.h>
#include <logcli.h>
#include <w32std.h>
#include "fdebug.h"


// Time Max constant
_LIT( KMaxTime, "99991130:235959.999999");
// Call Logs App UId
#define KCallLogsAppUid   0x101F4CD5

class CActiveWipeObject : public CActive
{
public:
     static CActiveWipeObject* NewL(MLawmoPluginWipeObserver* aObserver);

     ~CActiveWipeObject();
     /*
      * Name: WipeAsyncL
      * Input args: None
      * Output args: None
      * Description: Wipes the call-logs - including master logs.
      */
     void WipeAsyncL();
     
private: // Derived from CActive
     // Handle completion
     void RunL();

     // How to cancel me
     void DoCancel();

     // Override to handle leaves from RunL(). Default implementation causes
     // the active scheduler to panic.
     TInt RunError(TInt aError);
     
private: //Private functions for this class

     /**
      * second phase constructor
      */
     void ConstructL();
     
     // Construction
     CActiveWipeObject(MLawmoPluginWipeObserver* aObserver);
     
     // Wait Till Call-Logs Application EndTask();
     TBool WaitTillLogsAppKilled();
     
     // Clears the logs
     void ClearLogsL();

private: //Private member variables
	
		// State of the active object
	   TInt iState; 
	   
     // Observer member for the callback. Not owned
     MLawmoPluginWipeObserver* iObserver;
     
     // Call-log client member
     CLogClient* ilogClient;
     
     // RFs for Call-logs     
     RFs ifs;
     
     // Timer
     RTimer iTimer;
     
     //The windows server session
     RWsSession iWsSession;
     
     //Tells whether timer is started or not
     TBool iTimerStarted;
} ;

#endif //LAWMOCALLLOGSAYNCWIPE_H
