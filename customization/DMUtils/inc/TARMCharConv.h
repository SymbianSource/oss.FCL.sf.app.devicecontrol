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
* Description:  TARM character conversion methods
*
*/


// Link _your_ project with CharConv.lib if you use these methods

#ifndef __TARMCharConv_H__
#define __TARMCharConv_H__

#include <e32base.h>
#include "nsmldmuri.h" // For export macros

class CTARMCharConv : public CBase
    {
    private:
    CTARMCharConv();
    
    public:
    NSMLDMURI_IMPORT_C static HBufC8*  ConvertToUtf8LC(const TDesC16& aText);
    NSMLDMURI_IMPORT_C static HBufC16* ConvertFromUtf8LC(const TDesC8& aText);
    };

#endif //__TARMCharConv_H__
