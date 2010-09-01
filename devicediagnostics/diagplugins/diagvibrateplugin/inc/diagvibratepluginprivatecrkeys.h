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
* Description:  Vibrate plugin private Central Repository keys.
*
*/


#ifndef __DIAGVIBRATEPLUGINPRIVATECRKEYS_H__
#define __DIAGVIBRATEPLUGINPRIVATECRKEYS_H__

//  INCLUDES

#include <e32std.h>

// =============================================================================
// Vibra Test 
// =============================================================================

const TUid KCRUidDevDiagVibraTestPlugin = { 0x2000E586 };


/**
 *
 * The value assigned to this key is the time in second for the vibra to vibrate
 * Available options:   
 * Default value: 3
*/
const TUint32 KDevDiagVibraTestVibrateTime = 0x00000001;

#endif // __DIAGVIBRATEPLUGINPRIVATECRKEYS_H__

// End of File