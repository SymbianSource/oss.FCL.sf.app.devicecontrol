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


#include "TARMCharConv.h"

#include <utf.h>

CTARMCharConv::CTARMCharConv()
	{
	
	}

// ------------------------------------------------------------------------------------------------
// TInt CTARMCharConv::ConvertToUtf8LC
// Encodes from Unicode UCS-2 to UTF-8
// ------------------------------------------------------------------------------------------------
NSMLDMURI_EXPORT_C HBufC8* CTARMCharConv::ConvertToUtf8LC(const TDesC16& aText)
    {
    TPtrC16    remainder( aText );
    TBuf8<20>  utfBuffer;
    HBufC8     *ret    = 0;
    CBufFlat   *buffer = CBufFlat::NewL( 128 );
    CleanupStack::PushL( buffer );

    TBool finish = EFalse;
    while( !finish )
        {
        utfBuffer.Zero();
        TInt unconverted = CnvUtfConverter::ConvertFromUnicodeToUtf8( utfBuffer, remainder );
        if( unconverted >= 0 )
            {
            remainder.Set( remainder.Right( unconverted ) );
            buffer->InsertL( buffer->Size(), utfBuffer );
            finish = (unconverted == 0);
            }
        else
            {
            User::Leave( unconverted );
            }
        }

    buffer->Compress();
    ret = buffer->Ptr( 0 ).Alloc();
    CleanupStack::PopAndDestroy( buffer );
    CleanupStack::PushL( ret );
    return ret;
    }


// ------------------------------------------------------------------------------------------------
// TInt CTARMCharConv::ConvertFromUtf8LC
// Decodes from UTF-8 to Unicode UCS-2
// ------------------------------------------------------------------------------------------------
NSMLDMURI_EXPORT_C HBufC16* CTARMCharConv::ConvertFromUtf8LC(const TDesC8& aText)
    {
    TPtrC8     remainder( aText );
    TBuf16<20> unicodeBuffer;
    HBufC16    *ret    = 0;
    CBufFlat   *buffer = CBufFlat::NewL( 128 );
    CleanupStack::PushL( buffer );

    TBool finish = EFalse;
    while( !finish )
        {
        unicodeBuffer.Zero();
        TInt unconverted = CnvUtfConverter::ConvertToUnicodeFromUtf8( unicodeBuffer, remainder );
        if( unconverted >= 0 )
            {
            remainder.Set( remainder.Right( unconverted ) );
            TPtrC8 ptr( reinterpret_cast<const unsigned char*>(unicodeBuffer.Ptr()), unicodeBuffer.Length()*2 );
            buffer->InsertL( buffer->Size(), ptr );
            finish = (unconverted == 0);
            }
        else
            {
            User::Leave( unconverted );
            }
        }

    buffer->Compress();
    TPtrC16 ptr16( reinterpret_cast<const unsigned short*>(buffer->Ptr( 0 ).Ptr()), buffer->Size() / 2 );
    ret = ptr16.Alloc();
    CleanupStack::PopAndDestroy( buffer );
    CleanupStack::PushL( ret );
    return ret;
    }

// End of file
