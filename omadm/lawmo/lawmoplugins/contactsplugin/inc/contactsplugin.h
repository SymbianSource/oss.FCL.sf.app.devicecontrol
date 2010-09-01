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
* Description:  Contacts plug-in for LAWMO
*
*/

#ifndef _CONTACTPLUGIN__
#define _CONTACTPLUGIN__

//#include "lawmoplugininterface.h"
#include <lawmointerface.h>
#include <lawmocallbackinterface.h>
#include "lawmocontactsasyncwipe.h"

// An implementation of the CLAWMOPluginInterface definition
class CLAWMOContactsPlugin : public CLAWMOPluginInterface
	{
public:
	// Standardised safe construction which leaves nothing the cleanup stack.
	static CLAWMOContactsPlugin* NewL(MLawmoPluginWipeObserver* aObserver);
	// Destructor	
	~CLAWMOContactsPlugin();
	
	/**
	 * Wipe the contacts
	 * @param None
	 * @return None
	 */
	void WipeL ();
	
private:
	// Construction
	CLAWMOContactsPlugin();
	
	/**
	 * second phase constructor
	 */
	void ConstructL(MLawmoPluginWipeObserver* aObserver);

private:	
    CActiveWipeObject *asyncOp;
	};  

#endif // _CONTACTPLUGIN__
