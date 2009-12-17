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


#ifndef DIAGNETCOVERAGEPRIVATECRKEYS_H
#define DIAGNETCOVERAGEPRIVATECRKEYS_H

//  INCLUDES

#include <e32std.h>

// =============================================================================
// Network Coverage Plug-in
// =============================================================================

const TUid KCRUidNetworkCoverage = { 0x2000E596 };

// Minimum threshold for signal strength on the GSM 850 band
const TUint32 KPhoneDoctorGSM850SigStrThreshold     = 0x00000001;
	
// Minimum threshold for signal strength on the GSM 1900 band	
const TUint32 KPhoneDoctorGSM1900SigStrThreshold	= 0x00000002;

// Minimum threshold for signal strength on the UMTS 850 band
const TUint32 KPhoneDoctorUMTS850SigStrThreshold	= 0x00000003;
	
// Minimum threshold for signal strength on the UMTS 1900 band	
const TUint32 KPhoneDoctorUMTS1900SigStrThreshold	= 0x00000004;


#endif      // DIAGNETCOVERAGEPRIVATECRKEYS_H

// End of File



