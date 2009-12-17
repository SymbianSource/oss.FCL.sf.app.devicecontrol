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
* Description:  Dll entry point
*
*/


#include <e32std.h>
#include <ecom/implementationproxy.h>

#include "PnpPaosFilter.h"
#include "PnpPaosLogger.h"


const TImplementationProxy KImplementationTable[] =
    {
        { {0x10272D33}, (TProxyNewLPtr) CPnpPaosFilter::InstantiateL }
    };


EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    LOGSTRING("ImplementationGroupProxy");
    aTableCount = sizeof(KImplementationTable) / sizeof(TImplementationProxy);
    return KImplementationTable;
    }
