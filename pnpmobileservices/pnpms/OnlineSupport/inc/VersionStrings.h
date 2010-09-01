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
* Description:  A helper class for reading Series 60 and Symbian OS versions
*
*/



#ifndef VERSION_STRINGS_H
#define VERSION_STRINGS_H

//  INCLUDES
#include <e32base.h>
#include <sysutil.h>

#include "OnlineSupportLogger.h"

// CONSTANTS
const TInt KMaxVersionBufLength(10);

// MACROS
// None

// DATA TYPES
typedef TBuf8<KMaxVersionBufLength> TVersionBuf;

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None
    
// CLASS DECLARATION

/**
*  CMiniBrowser class
*
*  @since 3.1?
*/
class TVersionStrings
    {
public:
    /*!
    Get the beging index and length of a token in the descriptor.
    @param    aDescriptor
    @param    aSeparator Token separator, for example 0xA for a line feed.
    @param    aToken The token number, zero is the index of the first token.
    @param    aMaxLength The maximum length of the token. If the token length is bigger
            than aMaxLength the function leaves with KErrTooBig. Give KErrNotFound
            to not to check the maximum length.
    @param    aBegin On return contains the begin index of the token.
            The value is unspecified if the function leaves.
    @param    aLength On return contains the length of the token.
            The value is unspecified if the function leaves.
    */
    static void TokenLocationL(
        const TDesC& aDescriptor, const TChar aSeparator, const TInt aToken,
        const TInt aMaxLength, TInt& aBegin, TInt& aLength )
        {
        LOGSTRING( "TokenLocationL" );

        TInt locate = aDescriptor.Locate( aSeparator );
        if( locate == KErrNotFound )
            {
            User::Leave( KErrNotFound );
            }
        
        if( aToken )
            {
            // not the last row
            TokenLocationL( aDescriptor.Right( aDescriptor.Length() - locate - 1 ),
                aSeparator, aToken - 1, aMaxLength, aBegin, aLength );
            aBegin = aBegin + locate + 1;
            }
        else
            {
            if( aMaxLength != KErrNotFound && locate > aMaxLength )
                {
                User::Leave( KErrTooBig );
                }
            // the last row
            aBegin = 0;
            aLength = locate;
            }
        LOGSTRING( "TokenLocationL - done" );
        };

    /*!
    Gives Series 60 version and SymbianOS version based on product model.
    Leaves with KErrCorrupt if the version strings could not be read.
    @aS60Version On return contains Series 60 version, for example 2.6
    @aSymbianVersion On return contains Symbian OS version, for example 8.1
    */
    static void GetUserAgentVersionsL( TVersionBuf& aS60Version, TVersionBuf& aSymbianVersion )
        {
        LOGSTRING( "GetUserAgentVersionsL" );

        TBuf<KSysUtilVersionTextLength> versions;
        TInt err = SysUtil::GetSWVersion( versions );
        if( err != KErrNone )
            {
            User::Leave( KErrCorrupt );
            }
        LOGSTRING( "SW:" );
        LOGTEXT( versions );


        const TChar lineFeedChar( 0xA );
        TInt begin(0);
        TInt len(0);
        // Since the Series 60 3.0 the Series 60 version is assumed to be in the SW version string
        TRAP( err, TVersionStrings::TokenLocationL( versions, lineFeedChar, 2,
            aS60Version.MaxLength(), begin, len ) );
        if( err != KErrNone )
            {
            User::Leave( KErrCorrupt );
            }
        aS60Version.Copy( versions.Mid( begin, len ) );

        GetSymbianOsVersionL( aS60Version, aSymbianVersion );
        LOGSTRING( "S60:" );
        LOGTEXT( aS60Version );
        LOGSTRING( "Symbian OS:" );
        LOGTEXT( aSymbianVersion );
        LOGSTRING( "GetUserAgentVersionsL - done" );
        }

    /*!
    Gives Series 60 version and SymbianOS version based on product model.
    @aProduct Product, for example RM-1.
    @aS60Version Series 60 version, for example 2.6
    @aSymbianVersion On return contains Symbian OS version, for example 8.1
    */
    static void GetSymbianOsVersionL( const TDesC8& aS60Version, TVersionBuf& aSymbianVersion )
        {
        _LIT8( KSeries6020, "2.0");
        _LIT8( KSeries6021, "2.1");
        _LIT8( KSeries6026, "2.6");
        _LIT8( KSeries6026m, "2.6m");
        _LIT8( KSeries6028, "2.8");
        _LIT8( KSeries6030, "S60-30");
        _LIT8( KSeries6031, "S60-31");
        _LIT8( KSeries6040, "S60-40");

        _LIT8( KSymbianOS70s, "7.0s");
        _LIT8( KSymbianOS80a, "8.0a");
        _LIT8( KSymbianOS81a, "8.1a");
        _LIT8( KSymbianOS91, "9.1");
        _LIT8( KSymbianOS92, "9.2");
        _LIT8( KSymbianOS9x, "9.x");

        if( aS60Version.Compare( KSeries6040 ) == 0 )
            {
            aSymbianVersion.Copy( KSymbianOS9x );
            }
        else if( aS60Version.Compare( KSeries6031 ) == 0 )
            {
            aSymbianVersion.Copy( KSymbianOS92 );
            }
        else if( aS60Version.Compare( KSeries6030 ) == 0 )
            {
            aSymbianVersion.Copy( KSymbianOS91 );
            }
        else if( aS60Version.Compare( KSeries6028 ) == 0 )
            {
            aSymbianVersion.Copy( KSymbianOS81a );
            }
        else if( aS60Version.Compare( KSeries6026 ) == 0 || aS60Version.Compare( KSeries6026m ) == 0 )
            {
            aSymbianVersion.Copy( KSymbianOS80a );
            }
        else if( aS60Version.Compare( KSeries6021 ) == 0 || aS60Version.Compare( KSeries6020 ) == 0 )
            {
            aSymbianVersion.Copy( KSymbianOS70s );
            }
        else
            {
            User::Leave( KErrNotFound );
            }
        };

    };

#endif  // VERSION_STRINGS_H

// End of File
