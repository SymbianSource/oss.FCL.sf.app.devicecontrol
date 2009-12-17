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
* Description:  Synchronous Image Operations
*
*/


#include "SyncImageOperations.h"

// ------------------------------------------------------------------------------------------------
// CSyncDither* CSyncDither::NewL()
// ------------------------------------------------------------------------------------------------
CSyncDither* CSyncDither::NewL()
	{
	CSyncDither* self = NewLC();
	CleanupStack::Pop();
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CSyncDither* CSyncDither::NewLC()
// ------------------------------------------------------------------------------------------------
CSyncDither* CSyncDither::NewLC()
	{
	CSyncDither* self = new (ELeave) CSyncDither();
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}


// ------------------------------------------------------------------------------------------------
// CSyncDither::CSyncDither()
// ------------------------------------------------------------------------------------------------
CSyncDither::CSyncDither(  )
	: iDitherer( 0 )
	, iScheduler()
	, iError( KErrNone )
	{
	}

// ------------------------------------------------------------------------------------------------
// CSyncDither::ConstructL()
// ------------------------------------------------------------------------------------------------
void CSyncDither::ConstructL()
	{
	iDitherer = CMdaImageBitmapToBitmapUtility::NewL( *this );
	}

// ------------------------------------------------------------------------------------------------
// CSyncDither::~CSyncDither()
// ------------------------------------------------------------------------------------------------
CSyncDither::~CSyncDither()
	{
	delete iDitherer;
	}

// ------------------------------------------------------------------------------------------------
//  CSyncDither::
// ------------------------------------------------------------------------------------------------
TInt CSyncDither::DitherL( CFbsBitmap& aBitmap, TDisplayMode aDisplayMode )
	{
	TInt err = KErrNone;


	CSyncDither* obj = CSyncDither::NewLC();
	err = obj->DoDitherL( aBitmap, aDisplayMode );

	CleanupStack::PopAndDestroy( obj );
	return err;
	}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Internal methods
//
///////////////////////////////////////////////////////////////////////////////////////////////////

TInt CSyncDither::DoDitherL( CFbsBitmap& aBitmap, TDisplayMode aDisplayMode )
	{
	iError = KErrNone;

	CFbsBitmap* tmpBitmap = new (ELeave) CFbsBitmap();
	CleanupStack::PushL( tmpBitmap );

	TSize bmpSize = aBitmap.SizeInPixels();
	User::LeaveIfError( tmpBitmap->Create( bmpSize, aDisplayMode ) );

	if(iError == KErrNone)
		{
		// Open original bitmap
	    iDitherer->OpenL( aBitmap );
	    iScheduler.Start();
        }

    if(iError == KErrNone)
        {
	    // Dither bitmap
	    iDitherer->ConvertL( *tmpBitmap );
	    iScheduler.Start();
        }

	// Replace original bitmap with the new one.
    if(iError == KErrNone)
        {
		aBitmap.Reset();
		aBitmap.Duplicate( tmpBitmap->Handle() );
        }

	// Cleanup
	CleanupStack::PopAndDestroy( tmpBitmap );

    return iError;
    }

// ------------------------------------------------------------------------------------------------
//  CSyncDither::
// ------------------------------------------------------------------------------------------------
void CSyncDither::MiuoCreateComplete(TInt aError)
	{
	iError = aError;
	iScheduler.AsyncStop();
	}
void CSyncDither::MiuoOpenComplete(TInt aError)
	{
	iError = aError;
	iScheduler.AsyncStop();
	}
void CSyncDither::MiuoConvertComplete(TInt aError)
	{
	iError = aError;
	iScheduler.AsyncStop();
	}



#if 0
// -----------------------------------------------------------------------------
// RAknsSrvSession::EnumerateSkinPackages
// -----------------------------------------------------------------------------
//
EXPORT_C CArrayPtr<CAknsSrvSkinInformationPkg>* 
    RAknsSrvSession::EnumerateSkinPackagesL( 
        TAknSkinSrvSkinPackageLocation aLocation )
    {
    AKNS_TRACE1("RAknsSrvSession::EnumerateSkinPackagesL %i", aLocation);




    CArrayPtrFlat<CAknsSrvSkinInformationPkg>* array = 
        new (ELeave) CArrayPtrFlat<CAknsSrvSkinInformationPkg>(5);
    CleanupStack::PushL( TCleanupItem(
        AknsSrvCArrayPtrCAknsSrvSkinInformationPkgCleanup, array ) );
    
    CDesC16ArrayFlat* skinfiles = new (ELeave) CDesC16ArrayFlat(5);
    CleanupStack::PushL(skinfiles);

    switch (aLocation)
        {
        // Drives z and c
        case EAknsSrvPhone: 
            AknsSrvUtils::SearchDirectoriesL(KAknSkinSrvSystemSkinPath, 
                skinfiles );
            AknsSrvUtils::SearchDirectoriesL(KAknSkinSrvSkinPathLFFS, 
                skinfiles );
            break;
        // drive e
        case EAknsSrvMMC: 
            AknsSrvUtils::SearchDirectoriesL(KAknSkinSrvSkinPathMMC, 
                skinfiles );
            break;
        // all drives
        case EAknsSrvAll: 
#endif


// ------------------------------------------------------------------------------------------------
// EOF
// ------------------------------------------------------------------------------------------------
