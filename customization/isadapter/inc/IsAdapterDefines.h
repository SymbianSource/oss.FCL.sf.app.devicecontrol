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


#ifndef __IsAdapterDefines_H__
#define __IsAdapterDefines_H__

#define KNSmlDMISAdapterImplUidValue     0x10207832
#define KNSmlDMISAdapterCentRepUidValue  0x10207831
#define KNSmlDMISAdapterImplUid          TUid::Uid( KNSmlDMISAdapterImplUidValue )
#define KNSmlDMISAdapterCentRepUid       TUid::Uid( KNSmlDMISAdapterCentRepUidValue )

#define KRepositoryEntryMaxLength  512
#define KRepositoryIdBookmarkCount 1
#define KRepositoryIdBookmarkFirst 2
#define ISADAPTER_SHORTCUT_CAPTION_MAXLENGTH 128
#define KISTBufMaxLength 100
#ifndef MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64
#define MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64 21
#endif

#endif