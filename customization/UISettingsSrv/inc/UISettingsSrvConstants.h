/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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

#include <e32base.h>


// server name

_LIT(KUISettingsSrvName,"UISettingsSrv");
_LIT(KUISettingsSrvImg,"UISettingsSrv");

const TUid KUISettingsSrvUid = { 0x10207839 };


// KUISettingsSrvStackSize
// KUISettingsSrvInitHeapSize
// KUISettingsSrvMaxHeapSize

// A version must be specifyed when creating a session with the server

const TUint KUISettingsSrvMajorVersionNumber=0;
const TUint KUISettingsSrvMinorVersionNumber=1;
const TUint KUISettingsSrvBuildVersionNumber=1;

//sf- IMPORT_C TInt StartThread();

const int koje=33;

// opcodes used in message passing between client and server
enum TUISettingsSrvRqst
	{
	EUISettingsSrvSetStartupImage = 0,
	EUISettingsSrvGetStartupImagePath,
	EUISettingsSrvSetStartupText,
	EUISettingsSrvGetStartupText,
	EUISettingsSrvGetStartupTextSize,
	EUISettingsSrvSetStartupNoteType,
	EUISettingsSrvGetStartupNoteType,
	
	EUISettingsSrvUpdateShortcutList,
	EUISettingsSrvGetShortcutCount,
	EUISettingsSrvGetShortcutType,
	EUISettingsSrvGetShortcutAppUid,
	EUISettingsSrvGetShortcutIndexFromAT,
	EUISettingsSrvGetShortcutIndex,
	EUISettingsSrvSetShortcutTargetAppIndex,
	EUISettingsSrvGetShortcutRtType,

	EUISettingsSrvGetShortcutTargetCaption,
	EUISettingsSrvSetShortcutTargetCaption,

	EUISettingsSrvSetShortcutTargetBitmap,
	EUISettingsSrvSetShortcutTargetMask,
	EUISettingsSrvGetShortcutTargetBitmap,
	EUISettingsSrvGetShortcutTargetMask,

	EUISettingsSrvGetSoftkeyBitmap,
	EUISettingsSrvGetSoftkeyMask,

	EUISettingsSrvSetSoftkeyBitmap,
	EUISettingsSrvSetSoftkeyMask,

	EUISettingsSrvLastCmd
	};

enum TUISettingsSrvLeave
{
	ENonNumericString
};

enum EUiSrvScRtType
    {
    EUiSrvRtTypeNul = 0,
    EUiSrvRtTypeApp = 1,
    EUiSrvRtTypeBm  = 2
    };

