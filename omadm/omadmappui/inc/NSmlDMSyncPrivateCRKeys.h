/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Device manager private CenRep key definitions
*
*/


#ifndef NSMLDMSYNCAPP_PRIVATE_CR_KEYS_H
#define NSMLDMSYNCAPP_PRIVATE_CR_KEYS_H

// CONSTANTS

// Device manager key UID
const TUid KCRUidNSmlDMSyncApp = {0x101f6de5};
//browser uid 
const TUid browseruid = {0x10008d39}; 
//java uid 
const TUid  javauid =   {0x2001FD68};
//flash uid 
const TUid  flashuid = {0x101FD693}; 

// CenRep key used in variation of the aspects of
// the firmware update feature. The flag values are
// commented below.
const TUint32 KNSmlDMFotaConfigurationKey = 0x00000001;

// Enables the menu option Check updates.
const TUint KNSmlDMFotaEnableManualCheckFlag         = 0x0001;
// Enables the menu option that allows the user to access the default
// profile used in FOTA sessions.
const TUint KNSmlDMFotaUnlockedDefaultProfileFlag    = 0x0002;
// Enables the FOTA view.
const TUint KNSmlDMFotaEnableFotaViewFlag            = 0x0004;

// CenRep keys for default dm profile used in the
// firmware update over the air.
const TUint32 KNSmlDMDefaultFotaProfileKey = 0x00000002;

// CenRep key that can be used to hide a DM profile from the
// profile list on the UI, but still retaining the UI functionality
// during server alerted session. The server identifier used
// to authenticate the server should be stored here.
const TUint32 KNSmlDMFotaHiddenProfileIdKey = 0x00000003;

// CenRep key that can be used to Enable/Disable the 
// Connect menu in servers view options.Default value is 1
// which Enables the Connect menu
const TUint32 KNSmlDMEnableConnectMenuKey = 0x00000004;

// CenRep key that can be used to Enable/Disable the 
// Servers menu in Main view options.Default value is 1
// which Enables the Servers menu
const TUint32 KNSmlDMServersMenuKey = 0x00000005;

// CenRep key that can be used to Enable/Disable the 
// Settings menu in Main view options.Default value is 1
// which Enables the Settings menu
const TUint32 KNSmlDMSettingsMenuKey = 0x00000006;

// CenRep key that can be used to allow manual check updates 
// or not.Default value is 1 which Enables the check updates 
// menu in the main view
const TUint32 KNsmlDmManualCheckAllowed = 0x00000007;

// CenRep key that can be used to allow to show runtime versions
// or not.Default value is 0 which makes it off 
const TUint32 KNsmlDmRuntimeVerSupport = 0x00000008;

#endif // NSMLDMSYNCAPP_PRIVATE_CR_KEYS_H
            
// End of File
