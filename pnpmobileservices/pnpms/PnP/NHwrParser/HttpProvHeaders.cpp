/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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


#include "HttpProvHeaders.h"
#include "NHwrParserLogger.h"


CHttpProvHeaders* CHttpProvHeaders::NewL( const TDesC8& aHeaders )
    {
    CHttpProvHeaders* self = NewLC( aHeaders );
    CleanupStack::Pop(self);
    return self;
    }

    
CHttpProvHeaders* CHttpProvHeaders::NewLC( const TDesC8& aHeaders )
    {
    CHttpProvHeaders* self = new (ELeave) CHttpProvHeaders();
    CleanupStack::PushL(self);
    self->ConstructL( aHeaders );
    return self;
    }


CHttpProvHeaders::CHttpProvHeaders() :
    iReportUrl( KNullDesC8 ),
    iSignatureValue( KNullDesC8 ),
    iDigestValue( KNullDesC8 ),
    iProvisioningActivation( KNullDesC8 ),
    iUserInteraction( KNullDesC8 )
    {
    }


CHttpProvHeaders::~CHttpProvHeaders()
    {
    LOGSTRING( "~CHttpProvHeaders - done" );
    }


void CHttpProvHeaders::ConstructL( const TDesC8& aHeaders )
    {
    ParseL( aHeaders );
    }

void CHttpProvHeaders::ParseL( const TDesC8& aHeaders  )
    {
    LOGSTRING( "CHttpProvHeaders::ParseL" );
    TPtrC8 work( aHeaders );
    TPtrC8 line( aHeaders );
    TInt nextLinePos = 0;
    TBool firstLine = ETrue;

    TInt err;
    FOREVER
        {
        TRAP( err, GetLineL(work, line, nextLinePos) );
        LOGSTRING2( "got line with len:%i", line.Length() );
        if( err == KErrNotFound )
            {
            // if thre is a single line in the headers it currently does not end with CRLF
            //   as COULD be assumed from the specs
            //   so continue to the length check...
            if( !firstLine )
                {
                break;
                }
            firstLine = EFalse;
            }
        if( line.Length() == 0 )
            {
            break;
            }

        LOGSTRING( "line:" );
        LOGTEXT( line );

        HandleHeaderLineL( line );
            
        if( nextLinePos == KErrNotFound )
            {
            break;
            }
        LOGSTRING( "work.Set( work.Mid(nextLinePos) )" );
LOGSTRING2( "nextLinePos: %i", nextLinePos );
        work.Set( work.Mid(nextLinePos) );
        }
LOGSTRING( "end" );
    }

void CHttpProvHeaders::GetLineL( const TDesC8& aBuffer, TPtrC8& aLine, TInt& aNextLinePos )
    {
LOGSTRING("*1");
    TInt newlinePos = aBuffer.Locate( '\n' );
    if( newlinePos == KErrNotFound )
        {
LOGSTRING("*2");
        aNextLinePos = KErrNotFound;
        if( aBuffer.Length() )
            aLine.Set( aBuffer );
        User::Leave( KErrNotFound );
        }
    if( aBuffer.Length() > (newlinePos+1) )
        {    
LOGSTRING("*3");
        aNextLinePos = newlinePos + 1;
        }
    else
        {
LOGSTRING("*4");
        aNextLinePos = KErrNotFound;
        }
    if( newlinePos > 0 ) // 
        {
LOGSTRING("*5");
        if( aBuffer[newlinePos-1] == '\r' )
            {
            LOGSTRING( "aLine.Set( aBuffer.Left(newlinePos-1) )" );
            aLine.Set( aBuffer.Left(newlinePos-1) );
            return;
            }
        }
    LOGSTRING( "aLine.Set( aBuffer.Left(newlinePos) )" );
    aLine.Set( aBuffer.Left(newlinePos) );
    }

void CHttpProvHeaders::HandleHeaderLineL( const TDesC8& aLine )
    {
    LOGSTRING( "HandleHeaderLineL" );

    TBuf8<1> empty;
    TPtrC8 left( empty );
    TPtrC8 right( empty );

    LOGSTRING( "splitting" );    
    SplitLineL(aLine, ':', left, right );
    
    LOGSTRING( "left:" );
    LOGTEXT( left );
    LOGSTRING( "right:" );
    LOGTEXT( right );

    while( left[left.Length()-1] == ' ' )
        {
        left.Set( left.Left(left.Length()-1) );
        }
        
    if( right.Length() > 1 )
        {
        right.Set( right.Mid(0) ); // no space in this spec
        }
    SetParamL( left, right );
    }

void CHttpProvHeaders::SetParamL( const TDesC8& aName, const TPtrC8& aVal )
    {
    LOGSTRING( "SetParamL" );
    LOGSTRING( "aName:" );
    LOGTEXT( aName );
    LOGSTRING( "aVal:" );
    LOGTEXT( aVal );

    if( aName.Compare( KReportUrl ) == 0 ) 
        iReportUrl.Set( aVal );
    else if( aName.Compare( KSignatureValue ) == 0 ) 
        iSignatureValue.Set( aVal );
    else if( aName.Compare( KDigestValue ) == 0 )
        iDigestValue.Set( aVal );
    else if( aName.Compare( KProvisioningActivation ) == 0 )
        iProvisioningActivation.Set( aVal );
    else if( aName.Compare( KUserInteraction ) == 0 )
        iUserInteraction.Set( aVal );
    }

const TDesC8& CHttpProvHeaders::GetParamValL( const TDesC8& aName ) const
    {
    LOGSTRING( "GetParamValL" );
    if( aName.Compare( KReportUrl ) == 0 ) 
        {
        if( iReportUrl.Length() == 0 ) User::Leave( KErrNotFound );
        return( iReportUrl);
        }
    else if( aName.Compare( KSignatureValue ) == 0 ) 
        {
        if( iSignatureValue.Length() == 0 ) User::Leave( KErrNotFound );
        return iSignatureValue;
        }
    else if( aName.Compare( KDigestValue ) == 0 )
        {
        if( iDigestValue.Length() == 0 ) User::Leave( KErrNotFound );
        return iDigestValue;
        }
    else if( aName.Compare( KProvisioningActivation ) == 0 )
        {
        LOGTEXT( KProvisioningActivation );
        if( iProvisioningActivation.Length() == 0 ) User::Leave( KErrNotFound );
        LOGTEXT( iProvisioningActivation );
        return iProvisioningActivation;
        }
    else if( aName.Compare( KUserInteraction ) == 0 )
        {
        LOGTEXT( KUserInteraction );
        if( iUserInteraction.Length() == 0 ) User::Leave( KErrNotFound );
        return iUserInteraction;
        }
    else 
        {
        LOGSTRING( "GetParamValL error" );
        return KNullDesC8;
        }
    }

void CHttpProvHeaders::SplitLineL( const TDesC8& aLine, TChar aSplitBy, TPtrC8& aLeft, TPtrC8& aRight )
    {
    TInt splitIdx = aLine.Locate( aSplitBy );
    if( splitIdx == KErrNotFound )
        {
        User::Leave( splitIdx );
        }
    aLeft.Set( aLine.Left(splitIdx) );
    if( aLine.Length() >= splitIdx+2 )
        {
        aRight.Set( aLine.Mid(splitIdx+1) );
        }
    }
