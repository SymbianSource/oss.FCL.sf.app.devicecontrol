/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  $Description: $
*
*/



// INCLUDE FILES
#include "FileCoderB64.h"
#include <f32file.h>
#include <imcvcodc.h>


// -----------------------------------------------------------------------------
// TFileCoderB64::EncodeL()
// Encode file content to memory
// -----------------------------------------------------------------------------
//
TInt TFileCoderB64::EncodeL( const TDesC8& aFileName, CBufFlat &aData ) const
    {
	TInt err( KErrNone );
	TInt size( 0 );

	RFs fs ;
	CleanupClosePushL( fs );

	RFile f ;
	CleanupClosePushL( f );

	User::LeaveIfError ( fs.Connect() );

	TFileName fileName;
	fileName.Copy( aFileName ); // Convert from TDesC8 to TDes16
	err = f.Open ( fs, fileName, EFileRead );
	User::LeaveIfError( err );
	if ( err == KErrNone )
		{
		err = f.Size( size );
		}
			
	if ( err == KErrNone ) 
		{
		aData.ResizeL( ( size * 4 + 2 ) / 3 );
		HBufC8 *data = HBufC8::NewLC( size );

		TPtr8 dataPtr( data->Des() );
		err = f.Read( dataPtr );

		TImCodecB64 B64Coder;
		
		TPtr8 targetPtr( aData.Ptr( 0 ) );
		B64Coder.Encode( *data, targetPtr ); //target->Des() );

		CleanupStack::PopAndDestroy( data ); // data
		}
			
	CleanupStack::PopAndDestroy( &f ); // f
	CleanupStack::PopAndDestroy( &fs ); // fs
	
	return err;
    }

TInt TFileCoderB64::EncodeL( const TDesC8& aFileName, HBufC8* &aData ) const
    {
	TInt err( KErrNone );
	TInt size( 0 );

	RFs fs ;
	CleanupClosePushL( fs );

	RFile f ;
	CleanupClosePushL( f );

	User::LeaveIfError ( fs.Connect() );

	TFileName fileName;
	fileName.Copy( aFileName ); // Convert from TDesC8 to TDes16
	err = f.Open ( fs, fileName, EFileRead );
	User::LeaveIfError( err );
	if ( err == KErrNone )
		{
		err = f.Size( size );
		}
			
	if ( err == KErrNone ) 
		{
		aData = HBufC8::NewLC( ( size * 4 + 2 ) / 3 );
		HBufC8 *data = HBufC8::NewLC( size );

		TPtr8 dataPtr( data->Des() );
		err = f.Read( dataPtr );

		TImCodecB64 B64Coder;

		TPtr8 targetPtr( aData->Des() );
		B64Coder.Encode( dataPtr, targetPtr ); //target->Des() );

		CleanupStack::PopAndDestroy( data ); // data
		CleanupStack::Pop( aData ); // aData
		}
			
	CleanupStack::PopAndDestroy( &f ); // f
	CleanupStack::PopAndDestroy( &fs ); // fs

	return err;
    }

// -----------------------------------------------------------------------------
// TFileCoderB64::DecodeL()
// Create temporay file and decode data to it
// -----------------------------------------------------------------------------
//


void TFileCoderB64::DecodeL( const TDesC8& aData, TDes& aFileName ) const
	{
	TInt err( KErrNone );

	RFs fs ;
	CleanupClosePushL( fs );

	RFile f ;
	CleanupClosePushL( f );

	User::LeaveIfError ( fs.Connect() );

	_LIT(KTmpFilePath, "c:\\system\\temp\\");

	err = fs.MkDirAll( KTmpFilePath );
//sf-	err = f.Temp(fs, KTmpFilePath, aFileName, EFileWrite);
	TFileName buf;
	err = f.Temp(fs, KTmpFilePath, buf, EFileWrite);
	User::LeaveIfError( err );
	aFileName = buf;
	
	if ( err == KErrNone )
		{
		}
		
	if ( err == KErrNone )
		{
		TImCodecB64 B64Coder;
		HBufC8 *target = HBufC8::NewLC( ( aData.Length() * 3 ) / 4 );
		TPtr8 targetPtr( target->Des() );
		TBool result = B64Coder.Decode( aData, targetPtr );
		if( result )
			{
			User::Leave( KErrNotSupported );
			}

		f.Write ( targetPtr );
		f.Flush();
		f.Close();

		CleanupStack::PopAndDestroy( target );
		}

	CleanupStack::PopAndDestroy( &f ); // f
	CleanupStack::PopAndDestroy( &fs ); // fs
	}


// -----------------------------------------------------------------------------
// TFileCoderB64::DecodeToL()
// Open/Create/Replace file and decode data to it
// -----------------------------------------------------------------------------
//
void TFileCoderB64::DecodeToL( const TDesC8& aData, const TDesC& aFileName ) const
	{
	TInt err( KErrNone );

	RFs fs ;
	CleanupClosePushL( fs );

	RFile f ;
	CleanupClosePushL( f );

	User::LeaveIfError ( fs.Connect() );

	err = fs.MkDirAll( aFileName );
	err = f.Replace( fs, aFileName, EFileWrite );
	User::LeaveIfError( err );
	
	if ( err == KErrNone )
		{
		TImCodecB64 B64Coder;
		HBufC8 *target = HBufC8::NewLC( ( aData.Length() * 3 ) / 4 );
		TPtr8 targetPtr( target->Des() );
		TBool result = B64Coder.Decode( aData, targetPtr );
		if( result )
			{
			User::Leave( KErrNotSupported );
			}

		f.Write ( targetPtr );
		f.Flush();
		f.Close();

		CleanupStack::PopAndDestroy( target );
		}

	CleanupStack::PopAndDestroy( &f ); // f
	CleanupStack::PopAndDestroy( &fs ); // fs
	}


TBool TFileCoderB64::CheckB64Encode( const TDesC8& aData )
	{
	TInt inputIndex = 0;
	TInt inputLen = aData.Length();
	
	while (inputIndex < inputLen)
		{
		const TUint8& p = aData[ inputIndex ];

		if ((p >= 48 && p <=  57) ||
			(p >= 65 && p <=  90) ||
			(p >= 97 && p <= 122) ||
			 p == 43 || p == 47 ||
			 p == 61 || p == 10 || p == 13 )
			{
			}
		else
			{
			return EFalse;
			}

		inputIndex++;
			
		}

	return ETrue;
	}
//  End of File  
