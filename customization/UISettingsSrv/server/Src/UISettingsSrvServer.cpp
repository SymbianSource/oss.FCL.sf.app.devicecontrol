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
#include "UISettingsSrv.h"
#include <e32svr.h>
#include <e32math.h>
#include <e32uid.h>

#include    <eikapp.h>
#include    <avkon.hrh>
#include    <akncontext.h>
#include    <aknnavi.h>
#include    <aknnavide.h>
#include    <akntabgrp.h>
#include    <akntitle.h>
#include    <aknnotedialog.h>
#include    <AknWaitDialog.h>
#include    <aknslider.h>
#include    <AknQueryDialog.h>
#include    <aknsettingpage.h>
#include    <aknradiobuttonsettingpage.h>
#include    <aknslidersettingpage.h>
#include    <akntextsettingpage.h>
#include    <AknDateFormatUtils.h>
#include    <AknUtils.h>
#include    <AknsConstants.h>    //for determining skin change
#include    <barsread.h>
#include    <hal.h>
#include <e32property.h>
#include <centralrepository.h>
#include <AknSkinsInternalCRKeys.h>
#include <AknIconSrvClient.h>
#include <startupdomaincrkeys.h>
	

// #include <ScShortcut.h>
// #include <ScShortcutUids.h>
// #include <ScShortcutBase.h>
// #include <ScShortcutEngine.h>
// #include <ScSelectableShortcutList.h>
// #include <ScShortcutCustomOperations.h>


#include <gsasyncimagehandling.h>
#include <backgroundimage.h>
#include <imageconversion.h>

#include "debug.h"

// Type defintions
typedef TInt TScBookmarkEngineId;	// NOTE ! This is copied from ScBookmarkDbManager.h
									// to avoid illegal userinclude

const TUid KUidIdleApp = { 0x101FD64C };
/******************************************************************************************

    Security policies of this server are defined here:   

*/

/**
    Ranges (inclusive):
        0: EUISettingsSrvSetStartupImage to (EUISettingsSrvLastCmd-1)
        1: EUISettingsSrvLastCmd to KMaxTInt
*/
const TInt CUISettingsSrvServer::iRanges[UISETTINGSSRV_POLICYCOUNT] =
                {EUISettingsSrvSetStartupImage, EUISettingsSrvLastCmd};

/**
    For range 0: Always pass
    For range 1: Not supported
*/
const TUint8 CUISettingsSrvServer::iElementsIndex[UISETTINGSSRV_POLICYCOUNT] = 
                {CPolicyServer::EAlwaysPass, CPolicyServer::ENotSupported};

/**
    The policy
*/
const CPolicyServer::TPolicy CUISettingsSrvServer::iConnectionPolicy =
    {
        CPolicyServer::ECustomCheck, // Check connection
        UISETTINGSSRV_POLICYCOUNT,
        iRanges,
        iElementsIndex,
        0
    };

// MACROS
#define WELCOME_NOTE_MAX_LENGTH 100
//_LIT( KTempFilePath, "c:\\system\\temp\\" );
_LIT( KUISSWelcomeImagePath,    "c:\\system\\data\\welcomedata.mbm");
//_LIT8( KMimeTypeImageMbm, "image/x-epoc-mbm");

// ----------------------------------------------------------------------------------------
// CShutDown
// ----------------------------------------------------------------------------------------

inline CShutdown::CShutdown()
	:CTimer(-1)
	{
	CActiveScheduler::Add(this);
	}

inline void CShutdown::ConstructL()
	{
	CTimer::ConstructL();
	}

inline void CShutdown::Start()
	{
	RDEBUG( "UiSettingsSrv: Starting shutdown timeout" );
	After( KUISettingsSrvShutdownDelay );
	}

void CShutdown::RunL()
	{
	RDEBUG( "UiSettingsSrv: Timeout ... closing" );
	CEikonEnv::Static()->EikAppUi()->HandleCommandL( EEikCmdExit );
	}

// ----------------------------------------------------------------------------------------
// CUISettingsSrvServer
// ----------------------------------------------------------------------------------------
CUISettingsSrvServer::CUISettingsSrvServer(TInt aPriority)
	: CPolicyServer (
	    aPriority, // Active object priority
	    iConnectionPolicy,
	    EUnsharableSessions
	  )
	, iActive( 0 )
	, iSessionCount( 0 )
//	, iShortcuts( 0 )
	, iImgHandling( 0 )
	, iAppList( 0 )
	, iIdleBitmapUid( KUidIdleApp )
	, iAIBitmapUid( KUidIdleApp ) 
	{
	}

// ----------------------------------------------------------------------------------------
// CTerminalControlServer::CustomSecurityCheckL
// ----------------------------------------------------------------------------------------
CPolicyServer::TCustomResult CUISettingsSrvServer::CustomSecurityCheckL(const RMessage2& aMsg, TInt& /*aAction*/, TSecurityInfo& /*aMissing*/)
	{
	RDEBUG("CUISettingsSrvServer::CustomSecurityCheckL");

    _LIT_SECURITY_POLICY_C7(
                KRequiredCapabilities1,
                ECapabilityNetworkServices,
                ECapabilityLocalServices,
                ECapabilityLocation,
                ECapabilityReadUserData,
                ECapabilityWriteUserData,
                ECapabilityReadDeviceData,
                ECapabilityWriteDeviceData
                );

    _LIT_SECURITY_POLICY_C3(
                KRequiredCapabilities2,
                ECapabilitySwEvent,
                ECapabilityUserEnvironment,
                ECapabilityNetworkControl
                );

		TInt policy1 = KRequiredCapabilities1().CheckPolicy( aMsg );
		if( !policy1 )
		    {
            RDEBUG("CUISettingsSrvServer::CustomSecurityCheckL Capabilities ERROR");
            return EFail;
		    }

		TInt policy2 = KRequiredCapabilities2().CheckPolicy( aMsg );
		if( !policy2 )
		    {
            RDEBUG("CUISettingsSrvServer::CustomSecurityCheckL Capabilities ERROR");
            return EFail;
		    }

        RDEBUG("CUISettingsSrvServer::CustomSecurityCheckL Capabilities OK");
        return EPass;
	}

CUISettingsSrvServer::~CUISettingsSrvServer()
	{
	iShutdownTimer.Cancel();

//    delete iAppList;

//	delete iShortcuts;

    if( iTemporaryImageFile.Length() > 0 )
        {
        RDEBUG( "CUISettingsSrvServer::~CUISettingsSrvServer() Deleting temporary file" );
        iFs.Delete( iTemporaryImageFile );
        iTemporaryImageFile.Zero();
        }

	iFs.Close();

	iIndeces.Reset();
	iNotifyChanges.Reset();

	delete iImgHandling;

	delete iStartupRep;
	}


void CUISettingsSrvServer::ConstructL()
	{
	RDEBUG( "CUISettingsSrvServer::ConstructL" );

	User::LeaveIfError( iFs.Connect() );
//    iShortcuts = CScShortcutEngine::NewL(iFs, *this);
//    RDEBUG_2( "CUISettingsSrvServer::ConstructL CScShortcutEngine ALLOC %x", iShortcuts );

	iShutdownTimer.ConstructL();
	iShutdownTimer.Start();
	StartL(KUISettingsSrvName);

    iImgHandling = CGSAsyncImageHandling::NewL(
                        iFs, this, KUISSWelcomeImagePath() );

    RDEBUG_2( "CUISettingsSrvServer::ConstructL CGSAsyncImageHandling ALLOC %x", iImgHandling );

    iStartupRep = CRepository::NewL( KCRUidStartupConf );
    RDEBUG_2( "CUISettingsSrvServer::ConstructL CRepository ALLOC %x", iStartupRep );
	}

// Create and start a new count server.
CUISettingsSrvServer* CUISettingsSrvServer::NewL()
	{
	RDEBUG( "CUISettingsSrvServer::NewL" );

	CUISettingsSrvServer *pS = 0;
	pS = new (ELeave) CUISettingsSrvServer( EPriorityStandard );
    RDEBUG_2( "CUISettingsSrvServer::NewL CUISettingsSrvServer ALLOC %x", pS );

	CleanupStack::PushL( pS );

	pS->ConstructL();

	CleanupStack::Pop( pS );
	return pS;
	}


// Create a new server session.
CSession2 *CUISettingsSrvServer::NewSessionL(const TVersion& aVersion,const RMessage2& /*aMessage*/) const
	{
	RDEBUG( "CUISettingsSrvServer::NewSessionL" );

	// check we're the right version
	TVersion v(KUISettingsSrvMajorVersionNumber,KUISettingsSrvMinorVersionNumber,KUISettingsSrvBuildVersionNumber);
	if (!User::QueryVersionSupported(v,aVersion))
		{
		User::Leave(KErrNotSupported);
		}

	// make new session
	return CUISettingsSrvSession::NewL( ( CUISettingsSrvServer*) this );
	}

void CUISettingsSrvServer::AddSession()
	{
	RDEBUG( "CUISettingsSrvServer::AddSession" );

	iSessionCount++;
	iShutdownTimer.Cancel();
	}

void CUISettingsSrvServer::RemoveSession()
	{
	RDEBUG( "CUISettingsSrvServer::RemoveSession" );

	iSessionCount--;
	if (iSessionCount <= 0)
		{
		iShutdownTimer.Start();
		}
	}

void CUISettingsSrvServer::Shutdown()
	{
	RDEBUG( "CUISettingsSrvServer::Shutdown" );	
	}

// From MScSelectableShortcutListObserver
void CUISettingsSrvServer::ScSSLOHandleListChangedL()
    {
	RDEBUG( "CUISettingsSrvServer::ScSSLOHandleListChangedL" );

	// Shortcut list has changed, notify sessions about the situation!
    for(TInt i=0; i<iNotifyChanges.Count(); i++)
        {
        TInt* flag = iNotifyChanges[ i ];
        if( flag != 0 )
            {
            *flag = 1;
            }
        }
    }

// From MScSelectableShortcutListObserver
void CUISettingsSrvServer::ScSSLOHandleListConstructionComplete(TInt /*aError*/)
    {
	RDEBUG( "CUISettingsSrvServer::ScSSLOHandleListConstructionComplete" );

    if( iASWait.IsStarted() )
    {
        iASWait.AsyncStop();
    }
    }

// From MScEngineObserver
// TBool CUISettingsSrvServer::MScEngineHandleShortcutEvent(
//                    TScShortcutEvent /*aEvent*/,
//                    TScActionType /*aType*/
//                    )
//    {
//	RDEBUG( "CUISettingsSrvServer::MScEngineHandleShortcutEvent" );

//    return EFalse;
//    } */

void CUISettingsSrvServer::GetShortcutTargetListL()
    {
/*	RDEBUG( "CUISettingsSrvServer::GetShortcutTargetListL" );

    if( iASWait.IsStarted() )
        {
        User::Leave( KErrServerBusy );
        }

    // Remember count of previous list
    TInt oldCount = 0;
    if( iAppList != 0 )
        {
        oldCount = iAppList->Count();
        }

    delete iAppList;
    iAppList = 0;

    delete iShortcuts;
    iShortcuts = 0;

    iShortcuts = CScShortcutEngine::NewL(iFs, *this);
    RDEBUG_2( "CUISettingsSrvServer::GetShortcutTargetListL CScShortcutEngine ALLOC %x", iShortcuts );

    const CScSelectableShortcutList::TCaptionType typeSettings = 
                CScSelectableShortcutList::ECaptionTypeSoftkey;
    iAppList = iShortcuts->SelectableApplicationListLC( typeSettings, *this );

    iASWait.Start();
    CleanupStack::Pop( iAppList );

    //
    // If list had already changed (and engine had not notified that to us)
    // set flag to indicate that clients data is most probably outdated
    //
    if( (oldCount > 0) && (oldCount != iAppList->Count()) )
        {
        ScSSLOHandleListChangedL();
        }
        
    */
    }

void CUISettingsSrvServer::SetShortcutListMaskL(CScSelectableShortcutList& aScAppList, TInt aMask)
    {
/*	RDEBUG( "CUISettingsSrvServer::SetShortcutListMaskL" );

    iIndeces.Reset();
    TInt i, count = aScAppList.Count();

    for(i=0; i<count; i++)
        {
        TScRtType rtType = aScAppList.At( i ).MScRtType();
        TInt rt = EUiSrvRtTypeNul;

        if( aMask == EUiSrvRtTypeNul)
            {
            iIndeces.AppendL( i );
            }
        else
            {
            switch( rtType.iUid )
                {
                case KScRtTypeUidValueNativeApp:
                case KScRtTypeUidValueMailbox:
                case KScRtTypeUidValueMsgEditor:
                case KScRtTypeUidValueNewMessage:
                case KScRtTypeUidValueNoEffect:
                default:
                    rt |= EUiSrvRtTypeApp;
                break;

                case KScRtTypeUidValueBookmark:
                    {
                    rt |= EUiSrvRtTypeBm;
                    TUid uid = TUid::Uid( KScCustomOpValueBookmarkGetURL );
                    HBufC* url = (HBufC*)
                             aScAppList.At( i ).MScCustomOperationL(
                                    uid,
                                    (TAny*)0,
                                    (TAny*)0
                                    );
                    delete url;               
                    }
                break;
                }

            if( (aMask & rt) == rt )
                {
                iIndeces.AppendL( i );
                }
            }
        }
        
    */
    }

void CUISettingsSrvServer::AddListChangeNotifierL(TInt *aFlag)
    {
	RDEBUG( "CUISettingsSrvServer::AddListChangeNotifierL" );

    iNotifyChanges.AppendL( aFlag );
    }

void CUISettingsSrvServer::RemoveListChangeNotifierL(TInt *aFlag)
    {
	RDEBUG( "CUISettingsSrvServer::RemoveListChangeNotifierL" );

    iNotifyChanges.Remove( iNotifyChanges.FindL(aFlag) );
    }

void CUISettingsSrvServer::GetShortcutTargetCountL(
                            const RMessage2 &aMessage )
    {
	RDEBUG( "CUISettingsSrvServer::GetShortcutTargetCountL" );

    SetShortcutListMaskL( *iAppList, aMessage.Int0() );

    TInt count = iIndeces.Count();
    TPckg<TInt> countp( count );
    aMessage.Write(1, countp);
    }

void CUISettingsSrvServer::GetShortcutTargetTypeL( const RMessage2 &aMessage )
    {
/*	RDEBUG( "CUISettingsSrvServer::GetShortcutTargetTypeL" );

    SetShortcutListMaskL( *iAppList, aMessage.Int0() );

    TInt index = aMessage.Int1();
    TInt type = iAppList->At( iIndeces[ index ] ).MScRtType().iUid;
    TPckg<TInt> typep( type );
    aMessage.Write(2, typep);

*/
    }

void CUISettingsSrvServer::GetShortcutTargetCaptionL( const RMessage2 &aMessage )
    {
/*	RDEBUG( "CUISettingsSrvServer::GetShortcutTargetCaptionL" );

    SetShortcutListMaskL( *iAppList, aMessage.Int0() );

    TInt index = aMessage.Int1();
    TPtrC caption( iAppList->At( iIndeces[ index ] ).MScCaption( MScShortcut::EScCaptionTypeSoftkey ) );
    aMessage.Write(2, caption);
*/
    }

void CUISettingsSrvServer::GetShortcutTargetAppUidL( const RMessage2 &aMessage )
    {
/*	RDEBUG( "CUISettingsSrvServer::GetShortcutTargetAppUidL" );

    SetShortcutListMaskL( *iAppList, aMessage.Int0() );

    TInt index = aMessage.Int1();
    
    TInt appUid = 0;
    
    const CScShortcutBase &sc = iAppList->At( iIndeces[ index ] );
    if( sc.MScRtType() == KScRtTypeUidBookmark )
        {
        appUid = (TInt)iAppList->At( iIndeces[ index ] ).CustomOperationL( KScCustomOpBookmarkGetBookmarkId, NULL, NULL );
        }
    else
        {
        appUid = (TInt)iAppList->At( iIndeces[ index ] ).CustomOperationL( KScCustomOpNativeAppGetUid, NULL, NULL );
        }
    TPckg<TInt> uidp( appUid );
    aMessage.Write(2, uidp);
    
*/
    }

void CUISettingsSrvServer::GetShortcutTargetIndexFromATL( const RMessage2 &aMessage )
    {
/*	RDEBUG( "CUISettingsSrvServer::GetShortcutTargetIndexFromATL" );

    SetShortcutListMaskL( *iAppList, aMessage.Int0() );

    TInt i, index = -1;
    TInt actionType = aMessage.Int1();
    const CScShortcutBase &sc = iShortcuts->ShortcutL( TUid::Uid( actionType ) );

    for(i=0; i<iIndeces.Count(); i++)
        {
        if( iAppList->At( iIndeces[ i ] ).IsEqual( sc ) )
            {
            index = i;
            break;
            }
        }

    TPckg<TInt> indexp( index );
    aMessage.Write(2, indexp);
    
*/

    }

void CUISettingsSrvServer::GetShortcutTargetIndexL( const RMessage2 &aMessage )
    {
/*	RDEBUG( "CUISettingsSrvServer::GetShortcutTargetIndexL" );

    TInt type = aMessage.Int0();

    SetShortcutListMaskL( *iAppList, type );

    TInt i, index = -1;
    TInt theUid = aMessage.Int1();

    for(i=0; i<iIndeces.Count(); i++)
        {
        if( (type & EUiSrvRtTypeBm) == EUiSrvRtTypeBm )
            {
                TScBookmarkEngineId bmUid = -1;

                TRAPD
                (
                 reason,
                 bmUid = (TScBookmarkEngineId)iAppList->At( iIndeces[ i ] ).CustomOperationL( KScCustomOpBookmarkGetBookmarkId, NULL, NULL )
                );

                if( (KErrNone == reason) && (bmUid == theUid) )
                    {
                    index = i;
                    break;
                    }
            }
        if( (type & EUiSrvRtTypeApp) == EUiSrvRtTypeApp )
            {
                TInt appUid = -1;

                TRAPD
                (
                 reason,
                 appUid = (TInt)iAppList->At( iIndeces[ i ] ).CustomOperationL( KScCustomOpNativeAppGetUid, NULL, NULL )
                );

                if( (KErrNone == reason) && (appUid == theUid) )
                    {
                    index = i;
                    break;
                    }
            }
        }

    TPckg<TInt> indexp( index );
    aMessage.Write(2, indexp);
*/
    }

void CUISettingsSrvServer::SetShortcutTargetAppIndexL(const RMessage2 &aMessage)
    {
/*	RDEBUG( "CUISettingsSrvServer::SetShortcutTargetAppIndexL >" );

    SetShortcutListMaskL( *iAppList, aMessage.Int0() );

    TInt actionType = aMessage.Int1();
    TInt index = aMessage.Int2();

    RDEBUG( "CUISettingsSrvServer::SetShortcutTargetAppIndexL get Shortcut index" );
    TInt scIndex = iIndeces[ index ];

    if( scIndex >= iAppList->Count() )
        {
        RDEBUG_3( "CUISettingsSrvServer::SetShortcutTargetAppIndexL Index out of bound! %d >= %d", scIndex, iAppList->Count() );
        User::Leave( KErrArgument );
        }

    RDEBUG( "CUISettingsSrvServer::SetShortcutTargetAppIndexL get Shortcut from the list" );
    const CScShortcutBase &newSc = iAppList->At( scIndex );

    RDEBUG( "CUISettingsSrvServer::SetShortcutTargetAppIndexL entering iShortcuts->SetShortcutL" );
    iShortcuts->SetShortcutL( TUid::Uid( actionType ), newSc );

    RDEBUG( "CUISettingsSrvServer::SetShortcutTargetAppIndexL <" );
*/
    }

void CUISettingsSrvServer::GetShortcutRtTypeL(const RMessage2 &aMessage)
    {
/*	RDEBUG( "CUISettingsSrvServer::GetShortcutRtTypeL" );


    TInt actionType = aMessage.Int0();
    const CScShortcutBase &sc = iShortcuts->ShortcutL( TUid::Uid( actionType ) );

    TInt rtType = sc.RtType().iUid;

    TPckg<TInt> rttypep( rtType );
    aMessage.Write(1, rttypep);
*/
    }

void CUISettingsSrvServer::SetShortcutTargetCaptionL(const RMessage2 &aMessage)
    {
/*	RDEBUG( "CUISettingsSrvServer::SetShortcutTargetCaptionL" );

    SetShortcutListMaskL( *iAppList, aMessage.Int0() );

    HBufC16 *caption = CUISettingsSrvSession::CopyParameter16L( aMessage, 2 );
    CleanupStack::PushL( caption );

    CleanupStack::PopAndDestroy( caption );
*/
    }

void CUISettingsSrvServer::SetShortcutTargetBitmapL(const RMessage2 &aMessage, TBool aIsMask/*=EFalse*/)
    {
/*	RDEBUG( "CUISettingsSrvServer::SetShortcutTargetBitmapL >" );

    SetShortcutListMaskL( *iAppList, aMessage.Int0() );

    TInt index  = aMessage.Int1();

    HBufC16 *filename = CUISettingsSrvSession::CopyParameter16L( aMessage, 2 );
    CleanupStack::PushL( filename );

    HBufC8 *mimetype = CUISettingsSrvSession::CopyParameter8L( aMessage, 3 );
    CleanupStack::PushL( mimetype );

    CFbsBitmap *bitmap = LoadBitmapL( *filename, *mimetype );
    CleanupStack::PushL( bitmap );

    CScShortcutBase &sc = (CScShortcutBase&)iAppList->At( iIndeces[ index ] );
    CScShortcutBitmapArray &scBitmapArray = sc.MScMultipleBitmapsL();

    CScShortcutBitmap *oldBitmap = 0;
    CScShortcutBitmap *newBitmap = 0;

    TRAP_IGNORE(
           oldBitmap = & scBitmapArray.BitmapL( iIdleBitmapUid ) );

    if( 0 == oldBitmap )
        {
        if( aIsMask )
            {
            RDEBUG( "CUISettingsSrvServer::SetShortcutTargetBitmapL ERROR Trying to set mask when there is no image");
            User::Leave( KErrNotSupported );
            }
        newBitmap = CScShortcutBitmap::NewL( iIdleBitmapUid );
        RDEBUG_2( "CUISettingsSrvServer::SetShortcutTargetBitmapL CScShortcutBitmap::NewL ALLOC %x", newBitmap );
        }
    else
        {
        newBitmap = CScShortcutBitmap::NewCopyL( *oldBitmap );
        RDEBUG_2( "CUISettingsSrvServer::SetShortcutTargetBitmapL CScShortcutBitmap::NewCopyL ALLOC %x", newBitmap );
        }

    CleanupStack::PushL( newBitmap );

    // Set image or mask
    if( ! aIsMask )
        {
        RDEBUG( "CUISettingsSrvServer::SetShortcutTargetBitmapL Setting BITMAP" );
        newBitmap->SetImageL( bitmap );
        }
    else
        {
        if( !newBitmap->HasBitmap() )
            {
            RDEBUG( "CUISettingsSrvServer::SetShortcutTargetBitmapL ERROR Trying to set mask when there is no image 2");
            User::Leave( KErrNotSupported );
            }
        RDEBUG( "CUISettingsSrvServer::SetShortcutTargetBitmapL Setting MASK" );
        newBitmap->SetMaskL( bitmap );
        }

    RDEBUG( "CUISettingsSrvServer::SetShortcutTargetBitmapL Updating shortcut bitmaps" );
    scBitmapArray.SetBitmapL( iIdleBitmapUid, newBitmap );

    CleanupStack::Pop( newBitmap );
    CleanupStack::Pop( bitmap );
    CleanupStack::PopAndDestroy( mimetype );
    CleanupStack::PopAndDestroy( filename );

    RDEBUG( "CUISettingsSrvServer::SetShortcutTargetBitmapL <" );
*/
    }

void CUISettingsSrvServer::GetShortcutTargetBitmapL(const RMessage2 &aMessage, TBool aIsMask/*=EFalse*/)
    {
/*	RDEBUG( "CUISettingsSrvServer::GetShortcutTargetBitmapL >" );

    SetShortcutListMaskL( *iAppList, aMessage.Int0() );
    TInt index  = aMessage.Int1();

    CScShortcutBase &sc = (CScShortcutBase&)iAppList->At( iIndeces[ index ] );
    CScShortcutBitmapArray &scBitmapArray = sc.MScMultipleBitmapsL();
    CScShortcutBitmap *scBitmap = 0;

    TRAP_IGNORE(
           scBitmap = & scBitmapArray.BitmapL( iIdleBitmapUid ) );

    if( 0 == scBitmap )
        {
        User::Leave( KErrNotFound );
        }

    CFbsBitmap *bitmap = 0;

    if( !aIsMask )
        {
        bitmap = &scBitmap->ImageL();
        }
    else
        {
        bitmap = &scBitmap->MaskL();
        }

    CleanupStack::PushL( bitmap );

    //
    // Delete previous temp file
    //
    if( iTemporaryImageFile.Length() > 0 )
        {
        iFs.Delete( iTemporaryImageFile );
        iTemporaryImageFile.Zero();
        }

    //
    // Create new temp file
    //
    RFile file;
    User::LeaveIfError( file.Temp( iFs, KTempFilePath, iTemporaryImageFile, EFileWrite ) );
    CleanupClosePushL( file );

    bitmap->Save( file );

    // Close file
    CleanupStack::PopAndDestroy( &file );

    CleanupStack::PopAndDestroy( bitmap );

    RDEBUG( "CUISettingsSrvServer::GetShortcutTargetBitmapL <" );
    
*/
    }

void CUISettingsSrvServer::SetSoftkeyBitmapL(const RMessage2 &aMessage, TBool aIsMask/*=EFalse*/)
    {
/*	RDEBUG( "CUISettingsSrvServer::SetSoftkeyBitmapL >" );

    TUid imageUid ( iIdleBitmapUid );
    TInt softkeyUid = aMessage.Int0();
    switch( softkeyUid )
        {
        default:
        case KScActionTypeUidValueNull:
            User::Leave( KErrArgument );
        break;

        case KScActionTypeUidValueSoftkeyLeft:
        case KScActionTypeUidValueSoftkeyRight:
        case KScActionTypeUidValueScrollkeyLeft:
        case KScActionTypeUidValueScrollkeyRight:
        case KScActionTypeUidValueScrollkeyUp:
        case KScActionTypeUidValueScrollkeyDown:
        case KScActionTypeUidValueScrollkeySelect:
        imageUid = iIdleBitmapUid;
        break;

        case KActiveIdleScUidValueItem1:
        case KActiveIdleScUidValueItem2:
        case KActiveIdleScUidValueItem3:
        case KActiveIdleScUidValueItem4:
        case KActiveIdleScUidValueItem5:
        imageUid = iAIBitmapUid;
        break;
        }

    HBufC16 *filename = CUISettingsSrvSession::CopyParameter16L( aMessage, 1 );
    CleanupStack::PushL( filename );

    HBufC8 *mimetype = CUISettingsSrvSession::CopyParameter8L( aMessage, 2 );
    CleanupStack::PushL( mimetype );

    CFbsBitmap *bitmap = LoadBitmapL( *filename, *mimetype );
    CleanupStack::PushL( bitmap );

    CScShortcutBase& shortcut = iShortcuts->ShortcutL( TUid::Uid( softkeyUid ) );
    CScShortcutBitmapArray &scBitmapArray = shortcut.MScMultipleBitmapsL();

    CScShortcutBitmap *oldBitmap = 0;
    CScShortcutBitmap *newBitmap = 0;

    TRAP_IGNORE(
           oldBitmap = & scBitmapArray.BitmapL( imageUid ) );

    if( 0 == oldBitmap )
        {
        if( aIsMask )
            {
            RDEBUG( "CUISettingsSrvServer::SetSoftkeyBitmapL ERROR Trying to set mask when there is no image");
            User::Leave( KErrNotSupported );
            }
        newBitmap = CScShortcutBitmap::NewL( imageUid );
        RDEBUG_2( "CUISettingsSrvServer::SetSoftkeyBitmapL CScShortcutBitmap::NewL ALLOC %x", newBitmap );
        }
    else
        {
        newBitmap = CScShortcutBitmap::NewCopyL( *oldBitmap );
        RDEBUG_2( "CUISettingsSrvServer::SetSoftkeyBitmapL CScShortcutBitmap::NewCopyL ALLOC %x", newBitmap );
        }

    CleanupStack::PushL( newBitmap );

    // Set image or mask
    if( ! aIsMask )
        {
        RDEBUG( "CUISettingsSrvServer::SetSoftkeyBitmapL Setting BITMAP" );
        newBitmap->SetImageL( bitmap );
        }
    else
        {
        if( !newBitmap->HasBitmap() )
            {
            RDEBUG( "CUISettingsSrvServer::SetSoftkeyBitmapL ERROR Trying to set mask when there is no image 2");
            User::Leave( KErrNotSupported );
            }
        RDEBUG( "CUISettingsSrvServer::SetSoftkeyBitmapL Setting MASK" );
        newBitmap->SetMaskL( bitmap );
        }

    RDEBUG( "CUISettingsSrvServer::SetSoftkeyBitmapL Updating shortcut bitmaps" );
    scBitmapArray.SetBitmapL( imageUid, newBitmap );

    // Shortcut is updated ...
    iShortcuts->SetShortcutL( TUid::Uid( softkeyUid ), shortcut );

    CleanupStack::Pop( newBitmap );
    CleanupStack::Pop( bitmap );
    CleanupStack::PopAndDestroy( mimetype );
    CleanupStack::PopAndDestroy( filename );

	RDEBUG( "CUISettingsSrvServer::SetSoftkeyBitmapL <" );
*/
    }

void CUISettingsSrvServer::GetSoftkeyBitmapL(const RMessage2 &aMessage, TBool aIsMask/*=EFalse*/)
    {
/*	RDEBUG( "CUISettingsSrvServer::GetSoftkeyBitmapL" );

    TUid imageUid ( iIdleBitmapUid );
    TInt softkeyUid = aMessage.Int0();
    switch( softkeyUid )
        {
        default:
        case KScActionTypeUidValueNull:
            User::Leave( KErrArgument );
        break;

        case KScActionTypeUidValueSoftkeyLeft:
        case KScActionTypeUidValueSoftkeyRight:
        case KScActionTypeUidValueScrollkeyLeft:
        case KScActionTypeUidValueScrollkeyRight:
        case KScActionTypeUidValueScrollkeyUp:
        case KScActionTypeUidValueScrollkeyDown:
        case KScActionTypeUidValueScrollkeySelect:
        imageUid = iIdleBitmapUid;
        break;

        case KActiveIdleScUidValueItem1:
        case KActiveIdleScUidValueItem2:
        case KActiveIdleScUidValueItem3:
        case KActiveIdleScUidValueItem4:
        case KActiveIdleScUidValueItem5:
        imageUid = iAIBitmapUid;
        break;
        }

    CScShortcutBase& shortcut = iShortcuts->ShortcutL( TUid::Uid( softkeyUid ) );
    CScShortcutBitmapArray &scBitmapArray = shortcut.MScMultipleBitmapsL();
    CScShortcutBitmap *scBitmap = 0;

    TRAP_IGNORE(
           scBitmap = & scBitmapArray.BitmapL( imageUid ) );

    if( 0 == scBitmap )
        {
        User::Leave( KErrNotFound );
        }

    CFbsBitmap *bitmap = 0;

    if( !aIsMask )
        {
        bitmap = &scBitmap->ImageL();
        }
    else
        {
        bitmap = &scBitmap->MaskL();
        }

    CleanupStack::PushL( bitmap );

    //
    // Delete previous temp file
    //
    if( iTemporaryImageFile.Length() > 0 )
        {
        iFs.Delete( iTemporaryImageFile );
        iTemporaryImageFile.Zero();
        }

    //
    // Create new temp file
    //
    RFile file;
    User::LeaveIfError( file.Temp( iFs, KTempFilePath, iTemporaryImageFile, EFileWrite ) );
    CleanupClosePushL( file );

    bitmap->Save( file );

    aMessage.Write(1, iTemporaryImageFile);
    aMessage.Write(2, KMimeTypeImageMbm);

    // Close file
    CleanupStack::PopAndDestroy( &file );

    CleanupStack::PopAndDestroy( bitmap );
*/
    }

void CUISettingsSrvServer::SetStartupImageL( const TDesC &aFilePath )
    {
	RDEBUG( "CUISettingsSrvServer::SetStartupImageL" );

    if( iASWait.IsStarted() )
        {
        User::Leave( KErrServerBusy );
        }

    if( iImgHandling->IsActive() )
        {
        iASWait.Start();
        }

    CGSBackgroundImage *bgImage = CGSBackgroundImage::NewL();
    RDEBUG_2( "CUISettingsSrvServer::SetShortcutTargetBitmapL CGSBackgroundImage::NewL ALLOC %x", bgImage );

    CleanupStack::PushL( bgImage );
    bgImage->SetWelcomeNoteImageL( aFilePath, *iImgHandling );
    iImageHandlingError = KErrNone;
    User::LeaveIfError( iImageHandlingError );

    User::LeaveIfError( iStartupRep->Set( KStartupWelcomeNoteImage, KUISSWelcomeImagePath ) );

    CleanupStack::PopAndDestroy( bgImage );
    }

void CUISettingsSrvServer::SetStartupImageL(const RMessage2 &aMessage)
    {
	RDEBUG( "CUISettingsSrvServer::SetStartupImageL" );

    HBufC16* text = CUISettingsSrvSession::CopyParameter16L( aMessage, 0 );
    CleanupStack::PushL( text );

    SetStartupImageL( *text );

    CleanupStack::PopAndDestroy( text );
    }

void CUISettingsSrvServer::GetStartupImagePathL(const RMessage2 &aMessage)
    {
	RDEBUG( "CUISettingsSrvServer::GetStartupImagePathL" );

    TFileName imageLocation;

    CRepository* rep = CRepository::NewLC( KCRUidStartupConf );
    RDEBUG_2( "CUISettingsSrvServer::GetStartupImagePathL CRepository::NewLC ALLOC %x", rep );

    User::LeaveIfError( rep->Get( KStartupWelcomeNoteImage, imageLocation ) );

    CleanupStack::PopAndDestroy( rep );

    aMessage.Write(0, imageLocation);
    }

void CUISettingsSrvServer::SetStartupTextL(const RMessage2 &aMessage)
    {
	RDEBUG( "CUISettingsSrvServer::SetStartupTextL" );

    HBufC16* text = CUISettingsSrvSession::CopyParameter16L( aMessage, 0 );
    CleanupStack::PushL( text );

    User::LeaveIfError( iStartupRep->Set( KStartupWelcomeNoteText, *text ) );

    CleanupStack::PopAndDestroy( text );
    }

void CUISettingsSrvServer::GetStartupTextL(const RMessage2 &aMessage)
    {
	RDEBUG( "CUISettingsSrvServer::GetStartupTextL" );

    TBuf<WELCOME_NOTE_MAX_LENGTH> welcomeText;

    User::LeaveIfError( iStartupRep->Get( KStartupWelcomeNoteText, welcomeText ) );

    aMessage.Write(0, welcomeText);
    }

void CUISettingsSrvServer::GetStartupTextSizeL(const RMessage2 &aMessage)
    {
	RDEBUG( "CUISettingsSrvServer::GetStartupTextSizeL" );

    TBuf<WELCOME_NOTE_MAX_LENGTH> welcomeText;

    User::LeaveIfError( iStartupRep->Get( KStartupWelcomeNoteText, welcomeText ) );

    TInt len = welcomeText.Length();
    TPckg<TInt> lenp( len );
    aMessage.Write(0, lenp);
    }

void CUISettingsSrvServer::SetStartupNoteTypeL(const RMessage2 &aMessage)
    {
	RDEBUG( "CUISettingsSrvServer::SetStartupNoteTypeL" );

    TInt type = aMessage.Int0();

    User::LeaveIfError( iStartupRep->Set( KStartupWelcomeNoteType, type ) );
    }

void CUISettingsSrvServer::GetStartupNoteTypeL(const RMessage2 &aMessage)
    {
	RDEBUG( "CUISettingsSrvServer::GetStartupNoteTypeL" );

    TInt type = 0;

    User::LeaveIfError( iStartupRep->Get( KStartupWelcomeNoteType, type ) );

    TPckg<TInt> typep( type );
    aMessage.Write(0, typep);
    }

// -------------------------------------------------------------------------------------
// CUISettingsSrvServer::LoadBitmapL
// -------------------------------------------------------------------------------------
CFbsBitmap* CUISettingsSrvServer::LoadBitmapL( const TDesC& aFileName, const TDesC8& aType )
    {
	RDEBUG( "CUISettingsSrvServer::LoadBitmapL" );

    if( aType.Length() == 0 )
        {
    	RDEBUG( "CUISettingsSrvServer::LoadBitmapL ERROR Mime type is missing!" );
        User::Leave( KErrArgument );
        }

	RDEBUG_2( "CUISettingsSrvServer::LoadBitmapL Filename:  %S", &aFileName );
	RDEBUG8_2( "CUISettingsSrvServer::LoadBitmapL Mime type: %S", &aType );

    //
    // Convert to CFbsImages
    //
    const CImageDecoder::TOptions options = CImageDecoder::EOptionAlwaysThread;

    CImageDecoder* img = CImageDecoder::FileNewL( 
                            iFs,
                            aFileName,
                            aType,
                            options);

    RDEBUG_2( "CUISettingsSrvServer::LoadBitmapL CImageDecoder::FileNewL ALLOC %x", img );

    CleanupStack::PushL( img );
    
    CFbsBitmap* bitmap = 0;

    if( img != 0 )
        {
        TFrameInfo frameInfo = img->FrameInfo();
        TRequestStatus rs( KErrNone );

        bitmap = new (ELeave) CFbsBitmap();
        RDEBUG_2( "CUISettingsSrvServer::LoadBitmapL CFbsBitmap bitmap ALLOC %x", bitmap );

        CleanupStack::PushL( bitmap );
        bitmap->Create( frameInfo.iOverallSizeInPixels, frameInfo.iFrameDisplayMode );

        CFbsBitmap* mask   =  new (ELeave) CFbsBitmap();
        RDEBUG_2( "CUISettingsSrvServer::LoadBitmapL CFbsBitmap mask ALLOC %x", mask );

        CleanupStack::PushL( mask );
        mask->Create( frameInfo.iOverallSizeInPixels, frameInfo.iFrameDisplayMode );

        mask->SetDisplayMode( EGray2 );

        img->Convert( &rs, *bitmap, *mask );
        User::WaitForRequest( rs );

        User::LeaveIfError( rs.Int() );

        CleanupStack::PopAndDestroy( mask );
        CleanupStack::Pop( bitmap );
        }

    CleanupStack::PopAndDestroy( img );

    return bitmap;
    }

void CUISettingsSrvServer::ImageHandlingCompleteL( TInt aError )
    {
    if( iASWait.IsStarted() )
    {
        iASWait.AsyncStop();
    }
    iImageHandlingError = aError;
	RDEBUG( "CUISettingsSrvServer::ImageHandlingCompleteL" );
    }

// ----------------------------------------------------------------------------------------
// CUISettingsSrvSession
// ----------------------------------------------------------------------------------------


// constructor
CUISettingsSrvSession::CUISettingsSrvSession(CUISettingsSrvServer * aServer)
	: iServer( aServer )
	, iListChanged( 0 )
	{
	RDEBUG( "CUISettingsSrvSession::CUISettingsSrvSession" );

	}

void CUISettingsSrvSession::ConstructL()
	{
	iServer->AddSession();
	iServer->AddListChangeNotifierL( &iListChanged );
	}

CUISettingsSrvSession::~CUISettingsSrvSession()
	{
	RDEBUG( "CUISettingsSrvSession::~CUISettingsSrvSession" );

	if( iServer != NULL )
		{
		TInt err = KErrNone;
		TRAP( err, iServer->RemoveListChangeNotifierL( &iListChanged ) );
		iServer->RemoveSession();
		}
	}

CUISettingsSrvSession* CUISettingsSrvSession::NewL(CUISettingsSrvServer* aServer)
	{
	RDEBUG( "CUISettingsSrvSession::NewL" );

	CUISettingsSrvSession* self =  new (ELeave) CUISettingsSrvSession( aServer );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop();

    return self;	
	}


void CUISettingsSrvSession::ServiceL(const RMessage2& aMessage)
	{
	RDEBUG( "CUISettingsSrvSession::ServiceL" );

	TBool complete = ETrue;
	TRAPD(err,complete = DispatchMessageL(aMessage));
	if( complete )
	    {
	    aMessage.Complete(err);
	    }
	}



// service a client request; test the opcode and then do appropriate servicing
TBool CUISettingsSrvSession::DispatchMessageL(const RMessage2& aMessage)
	{
	RDEBUG( "CUISettingsSrvSession::DispatchMessageL" );

	switch (aMessage.Function())
        {
    case EUISettingsSrvSetStartupImage:
        SetStartupImageL( aMessage );
        return ETrue;
    case EUISettingsSrvGetStartupImagePath:
        GetStartupImagePathL( aMessage );
        return ETrue;
    case EUISettingsSrvSetStartupText:
        SetStartupTextL( aMessage );
        return ETrue;
	case EUISettingsSrvGetStartupText:
        GetStartupTextL( aMessage );
        return ETrue;
	case EUISettingsSrvGetStartupTextSize:
        GetStartupTextSizeL( aMessage );
        return ETrue;
	case EUISettingsSrvSetStartupNoteType:
        SetStartupNoteTypeL( aMessage );
        return ETrue;
	case EUISettingsSrvGetStartupNoteType:
        GetStartupNoteTypeL( aMessage );
        return ETrue;
    case EUISettingsSrvUpdateShortcutList:
        GetShortcutTargetListL( );
        return ETrue;
	case EUISettingsSrvGetShortcutCount:
	    GetShortcutTargetCountL( aMessage );
        return ETrue;
    case EUISettingsSrvGetShortcutType:
	    GetShortcutTargetTypeL( aMessage );
        return ETrue;    
	case EUISettingsSrvGetShortcutTargetCaption:
	    GetShortcutTargetCaptionL( aMessage );
        return ETrue;
    case EUISettingsSrvSetShortcutTargetCaption:
        SetShortcutTargetCaptionL( aMessage );
        return ETrue;
    case EUISettingsSrvGetShortcutAppUid:
	    GetShortcutTargetAppUidL( aMessage );
        return ETrue;
    case EUISettingsSrvGetShortcutIndexFromAT:
	    GetShortcutTargetIndexFromATL( aMessage );
        return ETrue;
    case EUISettingsSrvGetShortcutIndex:
	    GetShortcutTargetIndexL( aMessage );
        return ETrue;
    case EUISettingsSrvSetShortcutTargetAppIndex:
	    SetShortcutTargetAppIndexL( aMessage );
        return ETrue;
    case EUISettingsSrvGetShortcutRtType:
        GetShortcutRtTypeL( aMessage );
        return ETrue;
    case EUISettingsSrvSetShortcutTargetBitmap:    
        SetShortcutTargetBitmapL( aMessage );
        return ETrue;
    case EUISettingsSrvSetShortcutTargetMask:
        SetShortcutTargetBitmapL( aMessage, ETrue );
        return ETrue;
    case EUISettingsSrvGetShortcutTargetBitmap:    
        GetShortcutTargetBitmapL( aMessage );
        return ETrue;
    case EUISettingsSrvGetShortcutTargetMask:
        GetShortcutTargetBitmapL( aMessage, ETrue );
        return ETrue;
    case EUISettingsSrvGetSoftkeyBitmap:
        GetSoftkeyBitmapL( aMessage );
        return ETrue;
    case EUISettingsSrvGetSoftkeyMask:
        GetSoftkeyBitmapL( aMessage, ETrue );
        return ETrue;
    case EUISettingsSrvSetSoftkeyBitmap:
        SetSoftkeyBitmapL( aMessage );
        return ETrue;
    case EUISettingsSrvSetSoftkeyMask:
        SetSoftkeyBitmapL( aMessage, ETrue );
        return ETrue;

	// requests we don't understand at all are a different thing,
	// so panic the client here, this function also completes the message
	default:
		PanicClient(aMessage, EBadRequest);
		return ETrue;
        }
	}

void CUISettingsSrvSession::SetStartupImageL(const RMessage2 &aMessage)
    {
	RDEBUG( "CUISettingsSrvSession::SetStartupImageL" );
    ((CUISettingsSrvServer*)Server())->SetStartupImageL( aMessage );
    }

void CUISettingsSrvSession::GetStartupImagePathL(const RMessage2 &aMessage)
    {
	RDEBUG( "CUISettingsSrvSession::GetStartupImagePathL" );
    ((CUISettingsSrvServer*)Server())->GetStartupImagePathL( aMessage );
    }

void CUISettingsSrvSession::SetStartupTextL(const RMessage2 &aMessage)
    {
	RDEBUG( "CUISettingsSrvSession::SetStartupTextL" );
    ((CUISettingsSrvServer*)Server())->SetStartupTextL( aMessage );
    }

void CUISettingsSrvSession::GetStartupTextL(const RMessage2 &aMessage)
    {
	RDEBUG( "CUISettingsSrvSession::GetStartupTextL" );
    ((CUISettingsSrvServer*)Server())->GetStartupTextL( aMessage );
    }

void CUISettingsSrvSession::GetStartupTextSizeL(const RMessage2 &aMessage)
    {
	RDEBUG( "CUISettingsSrvSession::GetStartupTextSizeL" );
    ((CUISettingsSrvServer*)Server())->GetStartupTextSizeL( aMessage );
    }

void CUISettingsSrvSession::SetStartupNoteTypeL(const RMessage2 &aMessage)
    {
	RDEBUG( "CUISettingsSrvSession::SetStartupNoteTypeL" );
    ((CUISettingsSrvServer*)Server())->SetStartupNoteTypeL( aMessage );
    }

void CUISettingsSrvSession::GetStartupNoteTypeL(const RMessage2 &aMessage)
    {
	RDEBUG( "CUISettingsSrvSession::GetStartupNoteTypeL" );
    ((CUISettingsSrvServer*)Server())->GetStartupNoteTypeL( aMessage );
    }

void CUISettingsSrvSession::GetShortcutTargetListL( )
    {
	RDEBUG( "CUISettingsSrvSession::GetShortcutTargetListL" );
    ((CUISettingsSrvServer*)Server())->GetShortcutTargetListL( );

    // If short list had changed, server has notified it by settings flag
    // Clear flag of this session, other currently connected clients will still get notified
    // NOTE: There is no way to notify future clients about changes, there will be errors ...
    // 
    iListChanged = 0;
    }

void CUISettingsSrvSession::GetShortcutTargetCountL( const RMessage2 &aMessage )
    {
	RDEBUG( "CUISettingsSrvSession::GetShortcutTargetCountL" );

    AppListCheckL();   
    ((CUISettingsSrvServer*)Server())->GetShortcutTargetCountL( aMessage );
    }

void CUISettingsSrvSession::GetShortcutTargetTypeL( const RMessage2 &aMessage )
    {
	RDEBUG( "CUISettingsSrvSession::GetShortcutTargetTypeL" );

    AppListCheckL();
    ((CUISettingsSrvServer*)Server())->GetShortcutTargetTypeL( aMessage );
    }

void CUISettingsSrvSession::GetShortcutTargetCaptionL( const RMessage2 &aMessage )
    {
	RDEBUG( "CUISettingsSrvSession::GetShortcutTargetCaptionL" );

    AppListCheckL();
    ((CUISettingsSrvServer*)Server())->GetShortcutTargetCaptionL( aMessage );
    }

void CUISettingsSrvSession::GetShortcutTargetAppUidL( const RMessage2 &aMessage )
    {
	RDEBUG( "CUISettingsSrvSession::GetShortcutTargetAppUidL" );

    AppListCheckL();
    ((CUISettingsSrvServer*)Server())->GetShortcutTargetAppUidL( aMessage  );
    }

void CUISettingsSrvSession::GetShortcutTargetIndexL( const RMessage2 &aMessage )
    {
	RDEBUG( "CUISettingsSrvSession::GetShortcutTargetIndexL" );

    AppListCheckL();
    ((CUISettingsSrvServer*)Server())->GetShortcutTargetIndexL( aMessage );
    }

void CUISettingsSrvSession::GetShortcutTargetIndexFromATL( const RMessage2 &aMessage )
    {
	RDEBUG( "CUISettingsSrvSession::GetShortcutTargetIndexFromATL" );

    AppListCheckL();
    ((CUISettingsSrvServer*)Server())->GetShortcutTargetIndexFromATL( aMessage );
    }

void CUISettingsSrvSession::SetShortcutTargetAppIndexL(const RMessage2 &aMessage)
    {
	RDEBUG( "CUISettingsSrvSession::SetShortcutTargetAppIndexL" );

    AppListCheckL();
    ((CUISettingsSrvServer*)Server())->SetShortcutTargetAppIndexL( aMessage );    
    }

void CUISettingsSrvSession::GetShortcutRtTypeL(const RMessage2 &aMessage)
    {
	RDEBUG( "CUISettingsSrvSession::GetShortcutRtTypeL" );

    AppListCheckL();
    ((CUISettingsSrvServer*)Server())->GetShortcutRtTypeL( aMessage );    
    }

void CUISettingsSrvSession::SetShortcutTargetCaptionL(const RMessage2 &aMessage)
    {
	RDEBUG( "CUISettingsSrvSession::SetShortcutTargetCaptionL" );

    AppListCheckL();
    ((CUISettingsSrvServer*)Server())->SetShortcutTargetCaptionL( aMessage );
    }

void CUISettingsSrvSession::SetShortcutTargetBitmapL(const RMessage2 &aMessage, TBool aIsMask/*=EFalse*/)
    {
	RDEBUG( "CUISettingsSrvSession::SetShortcutTargetBitmapL" );

    AppListCheckL();
    ((CUISettingsSrvServer*)Server())->SetShortcutTargetBitmapL( aMessage, aIsMask );
    }

void CUISettingsSrvSession::GetShortcutTargetBitmapL(const RMessage2 &aMessage, TBool aIsMask/*=EFalse*/)
    {
	RDEBUG( "CUISettingsSrvSession::GetShortcutTargetBitmapL" );

    AppListCheckL();
    ((CUISettingsSrvServer*)Server())->GetShortcutTargetBitmapL( aMessage, aIsMask );
    }

void CUISettingsSrvSession::SetSoftkeyBitmapL(const RMessage2 &aMessage, TBool aIsMask/*=EFalse*/)
    {
	RDEBUG( "CUISettingsSrvSession::SetSoftkeyBitmapL" );

    AppListCheckL();
    ((CUISettingsSrvServer*)Server())->SetSoftkeyBitmapL( aMessage, aIsMask );
    }

void CUISettingsSrvSession::GetSoftkeyBitmapL(const RMessage2 &aMessage, TBool aIsMask/*=EFalse*/)
    {
	RDEBUG( "CUISettingsSrvSession::GetSoftkeyBitmapL" );

    AppListCheckL();
    ((CUISettingsSrvServer*)Server())->GetSoftkeyBitmapL( aMessage, aIsMask );
    }

void CUISettingsSrvSession::AppListCheckL()
    {
	RDEBUG( "CUISettingsSrvSession::AppListCheckL" );

    if( iListChanged == 1 )
        {
        // Shortcut list has changed during the session
        // This session is therefore invalid and all following commands will fail
    	RDEBUG( "CUISettingsSrvSession::AppListCheckL Invalidate session!" );
        User::Leave( KErrCorrupt );
        }
    }

HBufC8* CUISettingsSrvSession::CopyParameter8L( const RMessage2 &aMsg, TInt aIndex )
    {
	RDEBUG( "CUISettingsSrvSession::CopyParameter8L" );

    TInt length = aMsg.GetDesLengthL( aIndex );
    
    HBufC8* buffer = HBufC8::NewL( length );
    RDEBUG_2( "CUISettingsSrvSession::CopyParameter8L HBufC8::NewL ALLOC %x", buffer );

    TPtr8 ptr( buffer->Des() );
    aMsg.ReadL( aIndex, ptr );

    return buffer;
    }

HBufC16* CUISettingsSrvSession::CopyParameter16L( const RMessage2 &aMsg, TInt aIndex )
    {
	RDEBUG( "CUISettingsSrvSession::CopyParameter16L" );

    TInt length = aMsg.GetDesLengthL( aIndex );

    HBufC16* buffer = HBufC16::NewL( length );
    RDEBUG_2("CUISettingsSrvSession::CopyParameter16L HBufC16::NewL ALLOC %x", buffer );

    TPtr16 ptr( buffer->Des() );
    aMsg.ReadL( aIndex, ptr );

    return buffer;
    }

// RMessagePtr2::Panic() also completes the message. This is:
// (a) important for efficient cleanup within the kernel
// (b) a problem if the message is completed a second time
void CUISettingsSrvSession::PanicClient(const RMessagePtr2& aMessage,TInt aPanic)
	{
	_LIT(KTxtServer,"CountServ server");
	aMessage.Panic(KTxtServer,aPanic);
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================

// ----------------------------------------------------------------------------------------
// Global functions
// ----------------------------------------------------------------------------------------



// Panic the server
GLDEF_C void PanicServer(TUISettingsSrvPanic aPanic)
	{
	_LIT(KTxtServerPanic,"UISettingsSrv server panic");
	User::Panic(KTxtServerPanic,aPanic);
	}


//  End of File  
