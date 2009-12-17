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


#ifndef DEVDIAGPRIVATECRKEYS_H
#define DEVDIAGPRIVATECRKEYS_H

//  INCLUDES
#include <e32std.h>

// =============================================================================
// Phone Doctor
// =============================================================================

const TUid KCRUidDevDiag = { 0x2000E542 };

/**
 *
 * The value assigned to this key is the minimum battery level required to run
 * tests.
 *
 * Available options: N/A
 * Default value: 15
*/
const TUint32 KDevDiagAppMinBatteryLevel = 0x00000001;

/**
 *
 * The value assigned to this key is the minimum disk space that will be
 * required to run Device Diagnostics Application.
 *
 * Available options: N/A
 * Default value: 100000
*/
const TUint32 KDevDiagAppMinDiskSpace = 0x00000002;

#endif // DEVDIAGPRIVATECRKEYS_H
