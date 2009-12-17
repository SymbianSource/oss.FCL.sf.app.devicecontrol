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
* Description: Implementation of customization components
*
*/



// INCLUDE FILES
#include "UISettingsSrvConstants.h"
#include "UISettingsSrvClient.h"

#include "UISettingsSrv.h"
#include "debug.h"

#include <eikdll.h>
#include <apacmdln.h>

#include <apgcli.h> // apgrfx.lib


// Create the server thread
// This function is exported from the DLL and called from the client 
LOCAL_C TInt StartServerL()
    {
	RDEBUG("UISettingsSrv: Starting server...");

	const TUidType serverUid(KNullUid,KNullUid,KUISettingsSrvUid);

	// EPOC and EKA 2 is easy, we just create a new server process. Simultaneous
	// launching of two such processes should be detected when the second one
	// attempts to create the server object, failing with KErrAlreadyExists.
	RProcess server;
	//TInt r=server.Create(KHelloWorldServerImg,KNullDesC,serverUid);
	TInt r = 0;
//sf	TInt r=server.Create( KUISettingsSrvImg, KNullDesC );

	CApaCommandLine * cmd = CApaCommandLine::NewLC();
#if (defined(SYMBIAN_SUPPORT_UI_FRAMEWORKS_V1) || !defined(SYMBIAN_HIDE_UI_FRAMEWORKS_V1)) && !defined(SYMBIAN_REMOVE_UI_FRAMEWORKS_V1)
	// cmd->SetLibraryNameL( KUISettingsSrvImg ); // kpo not available in w20
#else
	cmd->SetExecutableNameL( KUISettingsSrvImg );
#endif
	cmd->SetDocumentNameL( KNullDesC );
//sf-	cmd->SetCommandL( EApaCommandRun );
	cmd->SetCommandL( EApaCommandBackground );

#if (defined(SYMBIAN_SUPPORT_UI_FRAMEWORKS_V1) || !defined(SYMBIAN_HIDE_UI_FRAMEWORKS_V1)) && !defined(SYMBIAN_REMOVE_UI_FRAMEWORKS_V1)
	r = server.Create( KUISettingsSrvImg, cmd->FullCommandLine() );  // kpo not available in w20
#else
	r = server.Create( KUISettingsSrvImg, KNullDesC() );
#endif


	cmd->SetProcessEnvironmentL( server );



	CleanupStack::PopAndDestroy( cmd );


	if ( r != KErrNone )
		{
		RDEBUG_2( "UISettingsSrv: server start failed %d",r);
		return r;
		}
	TRequestStatus stat;
	server.Rendezvous( stat );
	if (stat!=KRequestPending)
		{
		server.Kill(0);		// abort startup
		}
	else
		{
		server.Resume();	// logon OK - start the server
		}
	RDEBUG("UISettingsSrv: Started");

	User::WaitForRequest( stat );		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	r = (server.ExitType() == EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
    }




	



// ============================ MEMBER FUNCTIONS ===============================

//**********************************
//RUISettingsSrv
//**********************************

EXPORT_C RUISettingsSrv::RUISettingsSrv()
	{
	}


// This is the standard retry pattern for server connection
EXPORT_C TInt RUISettingsSrv::Connect()
	{
	TInt retry=2;
	for (;;)
		{
		TInt r=CreateSession( KUISettingsSrvName, TVersion(0,0,0), 1 );
		if ( r != KErrNotFound && r != KErrServerTerminated )
			{
			return r;
			}
		if ( --retry == 0 )
			{
			return r;
			}
		TRAPD( err, r = StartServerL() );
		if( err != KErrNone )
			{
			return err;
			}
		if ( r != KErrNone && r != KErrAlreadyExists )
			{
			return r;
			}
		}
	}

// Return the client side version number.
EXPORT_C TVersion RUISettingsSrv::Version(void) const
	{
	return(TVersion(KUISettingsSrvMajorVersionNumber,KUISettingsSrvMinorVersionNumber,KUISettingsSrvBuildVersionNumber));
	}

EXPORT_C TInt RUISettingsSrv::SetStartupImage( const TDesC& aImagePath )
	{
	TIpcArgs args( &aImagePath );
	return SendReceive( EUISettingsSrvSetStartupImage, args );
	}

EXPORT_C TInt RUISettingsSrv::GetStartupImagePath( TDes& aImagePath )
	{
	TIpcArgs args( &aImagePath );
	return SendReceive( EUISettingsSrvGetStartupImagePath, args );
	}

EXPORT_C TInt RUISettingsSrv::SetStartupText( const TDesC& aText )
    {
	TIpcArgs args( &aText );
	return SendReceive( EUISettingsSrvSetStartupText, args );
    }

EXPORT_C HBufC* RUISettingsSrv::GetStartupTextL( )
    {
	TInt len = 0;
	TPckg<TInt> lenp( len );
	TIpcArgs args1( &lenp );
	
	User::LeaveIfError( SendReceive( EUISettingsSrvGetStartupTextSize, args1 ) );
	
	HBufC* buf = HBufC::NewLC( len );
	TPtr ptr( buf->Des() );
	TIpcArgs args2( &ptr );
	User::LeaveIfError( SendReceive( EUISettingsSrvGetStartupText, args2 ) );
	CleanupStack::Pop( buf );

	return buf;
    }

EXPORT_C TInt RUISettingsSrv::SetStartupNoteType( TInt aType )
    {
	TIpcArgs args( aType );
	return SendReceive( EUISettingsSrvSetStartupNoteType, args );
    }

EXPORT_C TInt RUISettingsSrv::GetStartupNoteType( TInt &aType )
    {
	TPckg<TInt> typep( aType );
	TIpcArgs args( &typep );
	return SendReceive( EUISettingsSrvGetStartupNoteType, args );
    }

/* EXPORT_C TInt RUISettingsSrv::UpdateShortcutTargetList()
    {
    return SendReceive( EUISettingsSrvUpdateShortcutList );
    }

EXPORT_C TInt RUISettingsSrv::GetShortcutTargetCount( TInt aRtMask, TInt &aCount )
    {
	TPckg<TInt> countp( aCount );
	TIpcArgs args( aRtMask, &countp );
	return SendReceive( EUISettingsSrvGetShortcutCount, args );
    }

EXPORT_C TInt RUISettingsSrv::GetShortcutTargetType( TInt aRtMask, TInt aIndex, TInt& aType )
    {
	TPckg<TInt> typep( aType );
	TIpcArgs args( aRtMask, aIndex, &typep );
	return SendReceive( EUISettingsSrvGetShortcutType, args );
    }

EXPORT_C TInt RUISettingsSrv::GetShortcutTargetCaption( TInt aRtMask, TInt aIndex, TDes& aCaption )
    {
	TIpcArgs args( aRtMask, aIndex, &aCaption );
	return SendReceive( EUISettingsSrvGetShortcutTargetCaption, args );
    }

EXPORT_C TInt RUISettingsSrv::GetShortcutTargetAppUid( TInt aRtMask, TInt aIndex, TInt& aUid )
    {
    TPckg<TInt> uidp( aUid );
	TIpcArgs args( aRtMask, aIndex, &uidp );
	return SendReceive( EUISettingsSrvGetShortcutAppUid, args );
    }

EXPORT_C TInt RUISettingsSrv::GetShortcutTargetIndex( TInt aRtMask, TScActionType aActionType, TInt& aIndex )
    {
    TPckg<TInt> indexp( aIndex );
	TIpcArgs args( aRtMask, aActionType.iUid, &indexp );
	return SendReceive( EUISettingsSrvGetShortcutIndexFromAT, args );
    }

EXPORT_C TInt RUISettingsSrv::GetShortcutTargetIndex( TInt aRtMask, TInt aUid, TInt& aIndex )
    {
    TPckg<TInt> indexp( aIndex );
	TIpcArgs args( aRtMask, aUid, &indexp );
	return SendReceive( EUISettingsSrvGetShortcutIndex, args );
    }

EXPORT_C TInt RUISettingsSrv::SetShortcutTargetAppIndex( TInt aActionType, TInt aRtType, TInt aIndex )
    {
	TIpcArgs args( aRtType, aActionType, aIndex );
	return SendReceive( EUISettingsSrvSetShortcutTargetAppIndex, args );
    }

EXPORT_C TInt RUISettingsSrv::GetShortcutRtType( TInt aActionType, TInt &aRtType )
    {
    TPckg<TInt> rttypep( aRtType );
	TIpcArgs args( aActionType, &rttypep );
	return SendReceive( EUISettingsSrvGetShortcutRtType, args );
    }

EXPORT_C TInt RUISettingsSrv::SetShortcutTargetCaption( TInt aRtMask, TInt aIndex, const TDesC& aCaption)
    {
	TIpcArgs args( aRtMask, aIndex, &aCaption );
	return SendReceive( EUISettingsSrvSetShortcutTargetCaption, args );	
    }

EXPORT_C TInt RUISettingsSrv::SetShortcutTargetBitmap( TInt aRtMask, TInt aIndex, const TDesC& aFileName, const TDesC8& aMimeType )
    {
	TIpcArgs args( aRtMask, aIndex, &aFileName, &aMimeType );
	return SendReceive( EUISettingsSrvSetShortcutTargetBitmap, args );
    }

EXPORT_C TInt RUISettingsSrv::SetShortcutTargetMask( TInt aRtMask, TInt aIndex, const TDesC& aFileName, const TDesC8& aMimeType )
    {
	TIpcArgs args( aRtMask, aIndex, &aFileName, &aMimeType );
	return SendReceive( EUISettingsSrvSetShortcutTargetMask, args );
    }

EXPORT_C TInt RUISettingsSrv::GetShortcutTargetBitmap( TInt aRtMask, TInt aIndex, TDes& aFileName, TDes8& aMimeType )
    {
	TIpcArgs args( aRtMask, aIndex, &aFileName, &aMimeType );
	return SendReceive( EUISettingsSrvGetShortcutTargetBitmap, args );
    }

EXPORT_C TInt RUISettingsSrv::GetShortcutTargetMask( TInt aRtMask, TInt aIndex, TDes& aFileName, TDes8& aMimeType )
    {
	TIpcArgs args( aRtMask, aIndex, &aFileName, &aMimeType );
	return SendReceive( EUISettingsSrvGetShortcutTargetMask, args );
    }
*/
EXPORT_C TInt RUISettingsSrv::GetSoftkeyBitmap( TInt aSoftkeyId,
                                                TDes& aFileName,
                                                TDes8& aMimeType )
    {
	TIpcArgs args( aSoftkeyId, &aFileName, &aMimeType );
	return SendReceive( EUISettingsSrvGetSoftkeyBitmap, args );
    }

EXPORT_C TInt RUISettingsSrv::GetSoftkeyMask( TInt aSoftkeyId,
                                              TDes& aFileName,
                                              TDes8& aMimeType )
    {
	TIpcArgs args( aSoftkeyId, &aFileName, &aMimeType );
	return SendReceive( EUISettingsSrvGetSoftkeyMask, args );
    }

EXPORT_C TInt RUISettingsSrv::SetSoftkeyBitmap (
                                          TInt aSoftkeyId,
                                          const TDesC& aFileName,
                                          const TDesC8& aMimeType )
    {
	TIpcArgs args( aSoftkeyId, &aFileName, &aMimeType );
	return SendReceive( EUISettingsSrvSetSoftkeyBitmap, args );
    }

EXPORT_C TInt RUISettingsSrv::SetSoftkeyMask (
                                          TInt aSoftkeyId,
                                          const TDesC& aFileName,
                                          const TDesC8& aMimeType )
    {
	TIpcArgs args( aSoftkeyId, &aFileName, &aMimeType );
	return SendReceive( EUISettingsSrvSetSoftkeyMask, args );
    }



//  End of File  
