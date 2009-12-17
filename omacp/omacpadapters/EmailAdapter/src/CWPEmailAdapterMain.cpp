/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DLL entry point
*
*/



// INCLUDE FILES
#include <e32std.h>
#include "EmailAdapter.pan"

// ========================== OTHER EXPORTED FUNCTIONS =========================

/**
* Panic handler
* @param aPanic reason for panic.
*/
GLDEF_C void Panic( TInt aPanic )
    {
    User::Panic( _L("WPEmail.dll"), aPanic );
    }
//  End of File  
