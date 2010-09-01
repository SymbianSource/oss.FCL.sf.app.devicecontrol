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


#ifndef DIAGAUDIORECORDERPRIVATECRKEYS_H
#define DIAGAUDIORECORDERPRIVATECRKEYS_H

//  INCLUDES

#include <e32std.h>

// =============================================================================
// Audio Recorder 
// =============================================================================

const TUid KCRUidDiagVoiceRecorderPlugin = { 0x2000E58C }; 
	

/**
 *
 * The value assigned to this key is the recording time in second
 * Available options:   
 * Default value: 5
*/
const TUint32 KAudioRecorderRecordingTime = 0x00000001;

/**
 *
 * The value assigned to this key is the full path where the audio file is stored
 * Available options:   
 * Default value: c:\system\temp\record.wav
*/
const TUint32 KAudioRecorderFilePath = 0x00000002;




#endif      // DIAGAUDIORECORDERPRIVATECRKEYS_H

// End of File



