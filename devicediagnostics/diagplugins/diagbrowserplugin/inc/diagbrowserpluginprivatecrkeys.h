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
* Description:  Private Central Repository keys.
*
*/


#ifndef DIAGBROWSERPLUGINPRIVATECRKEYS_H
#define DIAGBROWSERPLUGINPRIVATECRKEYS_H

//  INCLUDES
#include <e32std.h>

// ===========================================================================
// Browser Test Plugin
// ===========================================================================

const TUid KCRUidDiagBrowserTestPlugin = { 0x2000E59A };

/**
 *
 * The value assigned to this key is the name of the preferred (primary)
 * access point to use for the test.
 *
 * Available options:   
*/
const TUint32 KDiagBrowserApnPrimary = 0x00000001;

/**
 *
 * The value assigned to this key is the name of the fallback (secondary)
 * access point to use for the test.
 *
 * Available options:   
*/
const TUint32 KDiagBrowserApnSecondary = 0x00000002;

/**
 *
 * The value assigned to this key is the address of the web page to retrieve
 * for the test.
 *
 * Available options:   
 * Default value: http://www.wireless.att.com/phone-inspector/index.html
*/
const TUint32 KDiagBrowserUri = 0x00000003;

/**
 *
 * The value assigned to this key is the timeout value (in milliseconds) to
 * wait for the connection to the access point, and to wait for the page to be
 * retrieved.
 *
 * Available options:   
 * Default value: 30000
*/
const TUint32 KDiagBrowserResponseTimeout = 0x00000004;

/**
 *
 * The value assigned to this key is the address of the proxy server to use
 * for the test.
 *
 * Available options:   
*/
const TUint32 KDiagBrowserProxyAddress = 0x00000005;

/**
 *
 * The value assigned to this key is the port of the proxy server to use for
 * the test.
 *
 * Available options:   
 * Default value: 80
*/
const TUint32 KDiagBrowserProxyPort = 0x00000006;

#endif // DIAGBROWSERPLUGINPRIVATECRKEYS_H
