/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Media plug-in for LAWMO
*
*/

#ifndef _MEDIAPLUGIN__
#define _MEDIAPLUGIN__

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib
#include <mdesession.h>
#include <mdequery.h>
#include <mdeobjectdef.h>
#include <mdeobjectquery.h>
#include <mdeconstants.h>
#include <mdeitem.h>
#include <f32file.h>
#include <flogger.h>
#include <lawmointerface.h>
#include <lawmocallbackinterface.h>

// An implementation of the CLAWMOPluginInterface definition
class CLAWMOMediaPlugin : public CLAWMOPluginInterface,
						  public MMdESessionObserver,
						  public MMdEQueryObserver
	{
public:
	// Standardised safe construction which leaves nothing the cleanup stack.
    static CLAWMOMediaPlugin* NewL(MLawmoPluginWipeObserver* aObserver);
	// Destructor	
	~CLAWMOMediaPlugin();
	
	void WipeL ();
	// Initiate and start a MdsQuery
	void StartMdsQueryL();
public: // from MMdESessionObserver
    
    void HandleSessionOpened( CMdESession& aSession, TInt aError );
    void HandleSessionError( CMdESession& aSession, TInt aError );
		
public: // from MMdEQueryObserver
    void HandleQueryNewResults(CMdEQuery& aQuery,
                                   TInt aFirstNewItemIndex,
                                   TInt aNewItemCount);
                                   
    void HandleQueryCompleted(CMdEQuery& aQuery, TInt aError);

private:
    enum TMdsQueryState
        {
        EUninitialized, // Uninitialized
        EInitialized, // Initalized
        EImage,
        EVideo,
        ESession,
        EError
        // Error condition
        };
		
private:
	// Construction
	CLAWMOMediaPlugin();
	/**
	 * second phase constructor
	 */
	void ConstructL(MLawmoPluginWipeObserver* aObserver);

private:
	TInt iState; // State of the active object
	CMdESession* iMdeSession; // session to MdE, created before trying to do the query
	CMdEObjectQuery* iQuery;
	RFs iRfs;
	MLawmoPluginWipeObserver* iObserver;
	};  

#endif // _MEDIAPLUGIN__
