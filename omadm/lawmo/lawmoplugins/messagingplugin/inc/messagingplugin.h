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
* Description: Messaging plug-in for LAWMO.
*
*/

#ifndef _MESSAGINGPLUGIN__
#define _MESSAGINGPLUGIN__


#include <f32file.h>
#include <lawmointerface.h>
#include <lawmocallbackinterface.h>
#include "fdebug.h"
#include "lawmomessagingasyncwipe.h"

// An implementation of the CLAWMOPluginInterface definition
class CLAWMOMessagingplugin : public CLAWMOPluginInterface
{
public:
	// Standardised safe construction which leaves nothing the cleanup stack.
	static CLAWMOMessagingplugin* NewL(MLawmoPluginWipeObserver* aObserver);
	// Destructor	
	~CLAWMOMessagingplugin();
	
	/**
	 * Wipe the Messages
	 * @param ENUM for which category of Messages to wipe
	 * @return None
	 */
	void WipeL ();
	
private:
	// Construction
	CLAWMOMessagingplugin();
	
	/**
	 * second phase constructor
	 */
	void ConstructL(MLawmoPluginWipeObserver* aObserver);
    
private:
	 CActiveWipeObject* iAsyncOp;
};  

#endif // _MESSAGINGPLUGIN__
