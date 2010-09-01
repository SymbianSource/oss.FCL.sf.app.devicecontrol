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



#ifndef __SyncImageOperations_H__
#define __SyncImageOperations_H__



#define __WITHIN_MCI_LIBRARY	// Needed because of conflicting typedef
#include <mdaimageconverter.h>


// ------------------------------------------------------------------------------------------------
// CSyncDither
// ------------------------------------------------------------------------------------------------
class CSyncDither : public CBase, public MMdaImageUtilObserver
	{
public:

	// Other
	static TInt DitherL( CFbsBitmap& aBitmap, TDisplayMode aDisplayMode );

protected:
	static CSyncDither* NewL( );
	static CSyncDither* NewLC( );

	virtual ~CSyncDither();

	TInt DoDitherL( CFbsBitmap& aBitmap, TDisplayMode aDisplayMode );

private:
	CSyncDither();
	void ConstructL();

private: // from MMdaImageUtilObserver
	void MiuoCreateComplete(TInt aError);
	void MiuoOpenComplete(TInt aError);
	void MiuoConvertComplete(TInt aError);

private: // new
	void BeginWait();
	void EndWait();

private:
	CMdaImageBitmapToBitmapUtility* iDitherer;
	CActiveSchedulerWait iScheduler;
	TInt iError;

	};


#endif // __SyncImageOperations_H__
