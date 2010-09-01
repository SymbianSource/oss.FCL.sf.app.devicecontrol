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
 
#ifndef __SCREENSAVER_CLIENTSERVER_H__
#define  __SCREENSAVER_CLIENTSERVER_H__

#include <e32std.h>
#include <e32base.h>

// server name

_LIT(KDMSSServerName,"ScreenSaverServer");
_LIT(KDMSSServerImg,"ScreenSaverServer");//can be removed

const TUid KDMSSServerUid={0x2000F8AB}; 
const TInt KSSInfoBufferLength = 1000;
enum TSSServerMessages
{
	EGetAvailableScreenSavers,
	EGetscreensaverInfo,
	EScreenSaversCount
};

#endif