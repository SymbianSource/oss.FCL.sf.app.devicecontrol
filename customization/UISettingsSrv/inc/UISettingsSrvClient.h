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
* Description: Implementation of customization components
*
*/

#if !defined(__UISettingsSrvClient_H__)
#define __UISettingsSrvClient_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

// #include "ScShortcutUids.h"

//**********************************
//RUISettingsSrv
//**********************************
// The client class. An RSessionBase sends messages to the server with the function 
// RSessionBase::SendReceive(); specifying an opcode (TCountServRqst) and and array of argument pointers. 
// Functions in derived classes, such as RUISettingsSrv::Increase(), are wrappers for different calls to 
// SendReceive().
// Most of the functions here return void because if they fail the server will panic the client.
// If they return it can be assumed that there is no error.

class RUISettingsSrv : public RSessionBase
	{

public:

	IMPORT_C RUISettingsSrv();

	IMPORT_C TInt Connect();
	IMPORT_C TVersion Version() const;

	IMPORT_C TInt SetStartupImage( const TDesC& aImagePath );
	IMPORT_C TInt GetStartupImagePath( TDes& aImagePath );

	IMPORT_C TInt   SetStartupText( const TDesC& aText );
	IMPORT_C HBufC* GetStartupTextL( );

	IMPORT_C TInt SetStartupNoteType( TInt aType );
	IMPORT_C TInt GetStartupNoteType( TInt &aType );

/*	IMPORT_C TInt UpdateShortcutTargetList();
	IMPORT_C TInt GetShortcutTargetCount( TInt aRtMask, TInt &aCount );
	IMPORT_C TInt GetShortcutTargetType( TInt aRtMask, TInt aIndex, TInt& aType );
    IMPORT_C TInt GetShortcutTargetCaption( TInt aRtMask, TInt aIndex, TDes& aCaption );
	IMPORT_C TInt GetShortcutTargetAppUid( TInt aRtMask, TInt aIndex, TInt& aUid );
	IMPORT_C TInt GetShortcutTargetIndex( TInt aRtMask, TScActionType aActionType, TInt& aUid );
	IMPORT_C TInt GetShortcutTargetIndex( TInt aRtMask, TInt aIndex, TInt& aUid );
	IMPORT_C TInt SetShortcutTargetAppIndex( TInt aActionType, TInt aRtType, TInt aIndex );

	IMPORT_C TInt GetShortcutRtType( TInt aUid, TInt &aRtType );
	IMPORT_C TInt SetShortcutTargetCaption( TInt aRtMask, TInt aIndex, const TDesC& aCaption);

	IMPORT_C TInt SetShortcutTargetBitmap   ( TInt aRtMask, TInt aIndex,
	                                          const TDesC& aFileName,
	                                          const TDesC8& aMimeType );

	IMPORT_C TInt SetShortcutTargetMask     ( TInt aRtMask, TInt aIndex,
	                                          const TDesC& aFileName,
	                                          const TDesC8& aMimeType );

	IMPORT_C TInt GetShortcutTargetBitmap   ( TInt aRtMask, TInt aIndex,
	                                          TDes& aFileName, TDes8& aMimeType );

	IMPORT_C TInt GetShortcutTargetMask     ( TInt aRtMask, TInt aIndex,
	                                          TDes& aFileName, TDes8& aMimeType );

*/
	IMPORT_C TInt GetSoftkeyBitmap          ( TInt aSoftkeyId,
	                                          TDes& aFileName, TDes8& aMimeType );

	IMPORT_C TInt GetSoftkeyMask            ( TInt aSoftkeyId,
	                                          TDes& aFileName, TDes8& aMimeType );

	IMPORT_C TInt SetSoftkeyBitmap          ( TInt aSoftkeyId,
	                                          const TDesC& aFileName,
	                                          const TDesC8& aMimeType );

	IMPORT_C TInt SetSoftkeyMask            ( TInt aSoftkeyId,
	                                          const TDesC& aFileName,
	                                          const TDesC8& aMimeType );

private:
		/**
		* Starts UISettingsSrv.
		* @param aServerExeName. The name of server.
		* @return TInt. KErrNone or one of the system wide error codes.
		*/
		TInt LaunchServer( const TDesC& aServerName );

	};


#endif

