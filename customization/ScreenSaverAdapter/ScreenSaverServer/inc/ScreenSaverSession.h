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
* Description: Implementation of customization components
*
*/
 

#ifndef __SCREENSAVER_SESSION_H__
#define __SCREENSAVER_SESSION_H__

#include "ScreenSaverServer.h"
#include <s32mem.h>

class CScreenSaverSession: public CSession2
{
	friend class CScreenSaverServer;
	public:
		~CScreenSaverSession();
	void CreateL();
	
	private:
		CScreenSaverSession();
		CScreenSaverServer& Server();
		
		void ServiceL(const RMessage2& aMessage);
		void DispatchMessageL(const RMessage2& aMessage);
		void ServiceError(const RMessage2 &aMessage, TInt aError);
};


#endif