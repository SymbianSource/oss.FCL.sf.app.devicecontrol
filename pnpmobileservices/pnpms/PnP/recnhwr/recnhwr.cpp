/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of PnPMS components
*
*/


// INCLUDE FILES
#include <apmrec.h>
#include <apmstd.h>

#include <f32file.h>
#include <implementationproxy.h>

#include "recnhwr.h"

// CONSTANTS
const TInt KNhwrRecognizerValue=0x1020433B;
const TUid KUidNhwrRecognizer={KNhwrRecognizerValue};
_LIT8(KDataTypeNhwr, "application/vnd.nokia.headwrapper");
_LIT(KNhwrExtension,".wbf");
_LIT8(KHeader,"NHWR");
const TInt KHeaderLength = 4;

// Constructor
CApaNhwrRecognizer::CApaNhwrRecognizer()
    :CApaDataRecognizerType( KUidNhwrRecognizer,CApaDataRecognizerType::ENormal )
{
    //LOGSTRING( "Constructor" );
    iCountDataTypes = 1;
}

// Destructor
CApaNhwrRecognizer::~CApaNhwrRecognizer()
{
    //LOGSTRING( "Destructor" );
}

// -----------------------------------------------------------------------------
// CApaNhwrRecognizer::PreferredBufSize
// -----------------------------------------------------------------------------
//
TUint CApaNhwrRecognizer::PreferredBufSize()
{
    //LOGSTRING( "PreferredBufSize" );
    return KHeaderLength;
}

// -----------------------------------------------------------------------------
// CApaNhwrRecognizer::SupportedDataTypeL
// -----------------------------------------------------------------------------
//
TDataType CApaNhwrRecognizer::SupportedDataTypeL( TInt aIndex ) const
{
    //LOGSTRING( "SupportedDataTypeL" );
    __ASSERT_DEBUG( aIndex >= 0 && aIndex < 1, User::Invariant() );
    
     switch ( aIndex )
        {
        case 0:
            //LOGSTRING( "SupportedDataTypeL - done" );
            return TDataType(KDataTypeNhwr);
        default:
            //LOGSTRING( "SupportedDataTypeL - done" );
            return TDataType();
        }
}

// -----------------------------------------------------------------------------
// CApaNhwrRecognizer::DoRecognizeL
// -----------------------------------------------------------------------------
//
void CApaNhwrRecognizer::DoRecognizeL( const TDesC& aName, const TDesC8& aBuffer )
{
    //LOGSTRING( "DoRecognizeL" );

    if(aName.Length()>0)
        {
        TParse parse;
        parse.Set(aName,NULL,NULL);
        if(parse.Ext().Compare(KNhwrExtension)==0)
            {
            iDataType   = TDataType(KDataTypeNhwr);
            //LOGSTRING( "confidence EProbable" );
            iConfidence = EProbable;
            }
        }
    
    // File content
    if (aBuffer.Length() >= KHeaderLength)
        {
    
        if ( aBuffer.FindF(KHeader) == 0)
            {
            iDataType   = TDataType(KDataTypeNhwr);
            //LOGSTRING( "confidence ECertain" );
            iConfidence = ECertain;
            //LOGSTRING( "DoRecognizeL - done" );
            return;
            }
        }

    //LOGSTRING( "DoRecognizeL - done" );
    return;
}

// -----------------------------------------------------------------------------
// CApaNhwrRecognizer::CreateRecognizerL
// -----------------------------------------------------------------------------
//
CApaDataRecognizerType* CApaNhwrRecognizer::CreateRecognizerL()
    {
    //LOGSTRING( "CreateRecognizerL" );
    return new (ELeave) CApaNhwrRecognizer();
    }

// CApaNhwrRecognizer::ImplementationTable
// -----------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KNhwrRecognizerValue, CApaNhwrRecognizer::CreateRecognizerL)
    };

// -----------------------------------------------------------------------------
// CApaNhwrRecognizer::ImplementationGroupProxy
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    //LOGSTRING( "ImplementationGroupProxy" );
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
