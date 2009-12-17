/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DM Idle SoftKey Adapter
*
*/


#ifndef __IsAdapterConstants_H__
#define __IsAdapterConstants_H__

//#include <ScShortcutUids.h>

_LIT(KTempFilePath, "c:\\system\\temp\\");
_LIT8(KDefaultApName8, "Default");


#define KScActionTypeUidValueNull                  0x0
#define KScActionTypeUidValueSoftkeyLeft           0x01000100
#define KScActionTypeUidValueSoftkeyRight          0x01000101
#define KScActionTypeUidValueScrollkeyLeft         0x01000000
#define KScActionTypeUidValueScrollkeyRight        0x01000001
#define KScActionTypeUidValueScrollkeyUp           0x01000002
#define KScActionTypeUidValueScrollkeyDown         0x01000003
#define KScActionTypeUidValueScrollkeySelect       0x01000004

// Active Idle
#define KActiveIdleScUidValueItem1                 0x00000001
#define KActiveIdleScUidValueItem2                 0x00000002
#define KActiveIdleScUidValueItem3                 0x00000003
#define KActiveIdleScUidValueItem4                 0x00000004
#define KActiveIdleScUidValueItem5                 0x00000005
#define KActiveIdleScUidValueItem6                 0x00000006

#define KScRtTypeUidValueBookmark	1

const TInt CIsAdapter::iSoftkeyList[] =
    {
    
#ifdef KScActionTypeUidValueSoftkeyLeft
    KScActionTypeUidValueSoftkeyLeft,
#endif
#ifdef KScActionTypeUidValueSoftkeyRight
    KScActionTypeUidValueSoftkeyRight,
#endif
#ifdef KScActionTypeUidValueScrollkeyLeft
    KScActionTypeUidValueScrollkeyLeft,
#endif
#ifdef KScActionTypeUidValueScrollkeyRight
    KScActionTypeUidValueScrollkeyRight,
#endif
#ifdef KScActionTypeUidValueScrollkeyUp
    KScActionTypeUidValueScrollkeyUp,
#endif
#ifdef KScActionTypeUidValueScrollkeyDown
    KScActionTypeUidValueScrollkeyDown,
#endif
#ifdef KScActionTypeUidValueScrollkeySelect
    KScActionTypeUidValueScrollkeySelect,
#endif
#ifdef KActiveIdleScUidValueItem1
    KActiveIdleScUidValueItem1,
#endif
#ifdef KActiveIdleScUidValueItem2
    KActiveIdleScUidValueItem2,
#endif
#ifdef KActiveIdleScUidValueItem3
    KActiveIdleScUidValueItem3,
#endif
#ifdef KActiveIdleScUidValueItem4
    KActiveIdleScUidValueItem4,
#endif
#ifdef KActiveIdleScUidValueItem5
    KActiveIdleScUidValueItem5,
#endif
#ifdef KActiveIdleScUidValueItem6
    KActiveIdleScUidValueItem6
#endif

    };

#define NumberOfSoftkeys (sizeof(iSoftkeyList) / sizeof(TInt32))
//#define NumberOfSoftkeys 6

#endif //__IsAdapterConstants_H__