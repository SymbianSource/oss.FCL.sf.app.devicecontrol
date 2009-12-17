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
* Description: 
*     Panic codes for E-Mail Adapter
*
*/


#ifndef __MSGMAILADAPTER_PAN__
#define __MSGMAILADAPTER_PAN__

// DATA TYPES
enum TMsgMailAdapterPanic // Panic codes
    {
    EMailAdapterOutOfRange,
    EMailAdapterNullPointer,
    EMailAdapterArgument,
	EMailAdapterInvalidId
    };

// FUNCTION PROTOTYPES

// ----------------------------------------------------
// Panic handler
// ----------------------------------------------------
//
GLREF_C void Panic(TInt aPanic);

#endif //__MSGMAILADAPTERR_PAN__     
            
// End of File
