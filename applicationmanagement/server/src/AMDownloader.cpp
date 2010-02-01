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
 * Description:  Implementation of applicationmanagement components
 *
 */


// INCLUDE FILES
//#include <BrowserUiInternalCRKeys.h>
#include <DownloadMgrClient.h>

#include "AMDownloader.h"
//#include "aminstalloptions.h"
#include "debug.h"
#include <e32property.h>
#include <aknnotewrappers.h> 
#include <AknNotify.h>
#include <AknNotifyStd.h>
#include <AknGlobalNote.h>
#include "ampskeys.h"
#include "ApplicationManagementConst.h"
#include <applicationmanagement.rsg>
#include <StringLoader.h>
#include <etelpckt.h>
#include <exterror.h>
#include <wlanerrorcodes.h>
#include <CoreApplicationUIsSDKCRKeys.h>// for KCRUidCoreApplicationUIs

using namespace NApplicationManagement;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMDownloader::CAMDownloader
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAMDownloader::CAMDownloader(RHttpDownload& aDownload,
        MAMDownloaderObserver& aObserver) :
        iDownload(aDownload), iObserver(aObserver)
        {
        }

// -----------------------------------------------------------------------------
// CAMDownloader::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAMDownloader::ConstructL()
    {
    iProgStarted=0;
    iSetFinalValue =0;
    iProgressCancelled = EFalse;
    iCurrentDownloadProgState = EHttpProgNone;
    iProgressNote= new(ELeave)CAppMgmtProgDialog(this);
    }

// -----------------------------------------------------------------------------
// CAMDownloader::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAMDownloader* CAMDownloader::NewL(RHttpDownload& aDownload,
        MAMDownloaderObserver& aObserver)
    {
    CAMDownloader* self = new( ELeave ) CAMDownloader( aDownload, aObserver );

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CAMDownloader::~CAMDownloader()
    {
    RDEBUG_3( "CAMDownloader::~CAMDownloader 0x%X - 0x%X", reinterpret_cast<TUint>( this ),
            reinterpret_cast<TUint>(this)+sizeof( CAMDownloader ) );

    if (iProgressNote)
        delete iProgressNote;

    if (iFileName)
        {
        delete iFileName;
        iFileName = NULL;
        }

    if (iContentType)
        {
        delete iContentType;
        iContentType = NULL;
        }

    delete iURI;
    iURI = NULL;
    }

// ---------------------------------------------------------------------------
// StartDownload()
// ---------------------------------------------------------------------------
TInt CAMDownloader::StartDownload()
    {
    RDEBUG("CAMDownloader::StartDownload");
    TInt err(iDownload.SetIntAttribute(EDlAttrAction, EMove) );
    RDEBUG_2("CAMDownload::StartDownload: response to EDlAttrAction set to true: (%d)", err );

    err = iDownload.Start();
    if (err == KErrNone)
        {
        RDEBUG( "CAMDownload::StartDownloadL: Started download successfully!!" );
        }
    else
        {
        RDEBUG_2( "CAMDownload::StartDownloadL: ERROR Failed to start downlad '%d'", err );
        }
    return err;
    }

// ---------------------------------------------------------------------------
// StopDownload()
// ---------------------------------------------------------------------------
TInt CAMDownloader::StopDownload()
    {
    RDEBUG("CAMDownloader::StopDownload");
    TInt result(KErrNone);
    if (!iProgressCancelled)
        {
        TRAP_IGNORE(iProgressNote->ProgressCompletedL());
        }
    result = iDownload.Delete();
    RDEBUG_2("CAMDownloader::StopDownload: result (%d)", result);
    return result;
    }

// ---------------------------------------------------------------------------
// ContinueDownload()
// ---------------------------------------------------------------------------
TInt CAMDownloader::ContinueDownload()
    {
    RDEBUG("CAMDownloader::ContinueDownload");
    TInt result(KErrNone);
    result = iDownload.Start();
    RDEBUG_2("CAMDownloader::ContinueDownload: result (%d)", result);
    return result;
    }

void CAMDownloader::CreateAndUpdateProgressDialogL(TInt32 aDownloadedSize,
        TInt32 aContentSize, THttpProgressState aState)
    {

    if (iProgStarted && aState == EHttpProgConnectionSuspended)    
    {
     if (!iProgressCancelled)
        {
        TRAP_IGNORE(iProgressNote->ProgressCompletedL());
        }
     iProgressCancelled = ETrue;
    
    TInt UIdisabled=0;
    RProperty::Set(KUidPSApplicationManagementKeys, KAMServerUIEnabled,
                UIdisabled);

     TApaTaskList taskList(CEikonEnv::Static()->WsSession() );
            TApaTask task = taskList.FindApp(TUid::Uid(KAppMgmtServerUid));

            task.SetWgId(CEikonEnv::Static()->RootWin().Identifier());

            if (task.Exists())
                task.SendToBackground();



    iCurrentDownloadProgState = EHttpProgConnectionSuspended;          

     RDEBUG( "CreateAndUpdateProgressDialogL : CAMDownloader Connection Failed" );  
        HBufC *netlossnote = StringLoader::LoadLC(R_DOWNLOADFAILED_CONNECTION);

        CAknGlobalNote* globalNote = CAknGlobalNote::NewL();
        CleanupStack::PushL(globalNote);
        
        globalNote->ShowNoteL(EAknGlobalErrorNote, *netlossnote);

        CleanupStack::PopAndDestroy( 2);// globalNote, dialognote

    
    }

    if (!iProgStarted)
        {

        RDEBUG( "CAMDownloadManager::HandleAlreadyRunningL - StartProgressNoteL" );
        iProgressNote->StartProgressNoteL();

        // UI in progress whenever switch btween application occur AM server will be moved 
        // to foreground

        //Define P&Skey


        TInt UIenabled = 1;
        TInt err = RProperty::Set(KUidPSApplicationManagementKeys,
                KAMServerUIEnabled, UIenabled);
        User::LeaveIfError(err); // invalid


        //Check if operation is silent or Non Silent

        TInt operNonSilent = -1;

        RProperty::Get(KUidPSApplicationManagementKeys,
                KAMOperationNonSilent, operNonSilent);

         RDEBUG_2( "CAMDownloader::Value of operNonSilent '%d'", operNonSilent );

        if (operNonSilent==1 || operNonSilent==KErrNotFound)
            {
            TApaTaskList taskList(CEikonEnv::Static()->WsSession() );
            TApaTask task = taskList.FindApp(TUid::Uid(KAppMgmtServerUid));

            task.SetWgId(CEikonEnv::Static()->RootWin().Identifier());

            if (task.Exists())
                task.BringToForeground();

            }
         else
            {
			RDEBUG( "CAMDownloadManager::HandleAlreadyRunningL - CreateAndUpdateProgressDialogL UI enabled = 0" );

		    TInt UIenabled = 0;
			TInt err = RProperty::Set(KUidPSApplicationManagementKeys,
			                KAMServerUIEnabled, UIenabled);
        	User::LeaveIfError(err); // invalid
			}

        iProgStarted = 1;
        }

    if (!iSetFinalValue && aContentSize>=0)
        {
        RDEBUG( "CAMDownloadManager::HandleAlreadyRunningL - SetFinalValue" );
        if (!iProgressCancelled)
        {
        iProgressNote->SetFinalValueL(aContentSize);
        iSetFinalValue = 1;
        }
        }
    else
        if (aContentSize>=0)
         {
            RDEBUG( "CAMDownloadManager::HandleAlreadyRunningL - UpdateProcessL" );
            if (!iProgressCancelled)
            {
             iProgressNote->UpdateProcessL(aDownloadedSize);
             }
          }

    RDEBUG( "CAMDownloadManager::HandleAlreadyRunningL - END" );

    }

// ---------------------------------------------------------------------------
// CAMDownloader::FileNameFromDownloadL
// ---------------------------------------------------------------------------
void CAMDownloader::FileNameFromDownloadL()
    {
    RFile f;
    TInt err(iDownload.GetFileHandleAttribute(f));
    if (err == KErrNone)
        {
        TFileName fn;
        err = f.Name(fn);
        if (err == KErrNone)
            {
            if (iFileName)
                {
                delete iFileName;
                iFileName = NULL;
                }
            iFileName = fn.AllocL();

            }
        else
            {

            }
        }
    else
        {
        RDEBUG_2( "CAMDownloader::GetFileNameL: WARNING Failed to get download file handle '%d'", err );
        RDEBUG("CAMDownloader::GetFileNameL: Using filename instead");
        TBuf<KMaxPath> file;
        err = iDownload.GetStringAttribute(EDlAttrDestFilename, file);
        if (err == KErrNone)
            {
            if (iFileName)
                {
                delete iFileName;
                iFileName = NULL;
                }
            iFileName = file.AllocL();

            }
        else
            {

            }
        }
    }

// ---------------------------------------------------------------------------
// CAMDownloader::ContentTypeFromDownloadL
// ---------------------------------------------------------------------------
void CAMDownloader::ContentTypeFromDownloadL()
    {
    TBuf8<KMaxContentTypeLength> contype;
    TInt err(iDownload.GetStringAttribute(EDlAttrContentType, contype) );
    if (err != KErrNone)
        {
        RDEBUG_2( "CAMDownloader::ContentTypeFromDownloadL: Failed to get content type: %d", err );
        }
    else
        {
        if (iContentType)
            {
            delete iContentType;
            iContentType = NULL;
            }
        iContentType = contype.AllocL();
        RDEBUG( "CAMDownloader::ContentTypeFromDownloadL: ");RDEBUG_HEX8( iContentType->Ptr(), iContentType->Size() );
        }
    }

// ---------------------------------------------------------------------------
// CAMDownloader::DownloadURIFromDownloadL
// ---------------------------------------------------------------------------
void CAMDownloader::DownloadURIFromDownloadL()
    {
    TBuf8<KMaxUrlLength> uri;
    TInt err(iDownload.GetStringAttribute(EDlAttrReqUrl, uri) );
    if (err != KErrNone)
        {
        RDEBUG_2( "CAMDownloader::DownloadURIFromDownloadL: Failed to get uri: %d", err );
        }
    else
        {
        if (iURI)
            {
            delete iURI;
            iURI = NULL;
            }
        iURI = uri.AllocL();
        RDEBUG( "CAMDownloader::DownloadURIFromDownloadL: ");RDEBUG_HEX8( iURI->Ptr(), iURI->Size() );
        }
    }

// ---------------------------------------------------------------------------
// CAMDownloader::FileName
// ---------------------------------------------------------------------------
const TDesC& CAMDownloader::FileName()
    {
    if (iFileName)
        {
        return *iFileName;
        }
    else
        {
        TRAPD( err, FileNameFromDownloadL() )
        ;
        if (err == KErrNone)
            {
            return *iFileName;
            }
        }

    return KNullDesC();
    }

// ---------------------------------------------------------------------------
// CAMDownloader::MimeType
// ---------------------------------------------------------------------------
const TDesC8& CAMDownloader::MimeType()
    {
    if (iContentType)
        {
        return *iContentType;
        }
    else
        {
        TRAPD( err, ContentTypeFromDownloadL() )
        ;
        if (err == KErrNone)
            {
            return *iContentType;
            }
        }
    return KNullDesC8();
    }

// ---------------------------------------------------------------------------
// CAMDownloader::DownloadURI
// ---------------------------------------------------------------------------
const TDesC8& CAMDownloader::DownloadURI()
    {
    if (iURI)
        {
        return *iURI;
        }
    else
        {
        TRAPD( err, DownloadURIFromDownloadL() )
        ;
        if (err == KErrNone)
            {
            return *iURI;
            }
        }
    return KNullDesC8();
    }

// ---------------------------------------------------------------------------
// CompareDownloadURI
// ---------------------------------------------------------------------------
TBool CAMDownloader::CompareDownloadURI(const TDesC8& aURI)
    {
    TBool match(EFalse);
    if (DownloadURI() == aURI)
        {
        match = ETrue;
        }
    return match;
    }

// ---------------------------------------------------------------------------
// DownloadStatusCode
// ---------------------------------------------------------------------------
TInt32 CAMDownloader::DownloadStatusCodeL(THttpDownloadState aDownloadState)
    {
    RDEBUG_2( "CAMDownloader::DownloadStatusCodeL: State '%d'", aDownloadState );	
    
    TInt32 code;
    User::LeaveIfError(iDownload.GetIntAttribute(EDlAttrStatusCode, code) );
    RDEBUG_2( "CAMDownloader::DownloadStatusCodeL: ERROR download status code '%d'", code );

    THttpDownloadMgrError errx;
    User::LeaveIfError(iDownload.GetIntAttribute(EDlAttrErrorId,
            (TInt32&)errx ) );
    RDEBUG_2( "CAMDownloader::DownloadStatusCodeL: ERROR download error id '%d'", errx );

    TInt32 gerr;
    User::LeaveIfError(iDownload.GetIntAttribute(EDlAttrGlobalErrorId, gerr) );
    RDEBUG_2( "CAMDownloader::DownloadStatusCodeL: ERROR global error id '%d'", gerr );

    TInt32 res(KErrNone);
    if (code == 0 || code == 200)
        {
        res = errx;
        
        }
    else
        {
        res = code;
        }

    if (errx == EHttpUnhandled || errx == EGeneral || errx
            == EConnectionFailed || errx == ETransactionFailed)
        {
        res = gerr;
        }
    if (errx == EDiskFull)
        res = KStatusDowloadFailedOOM;

    if(errx == ETransactionFailed && gerr == -7376)
        res = KStatusAltDowldUnavailable;

    if (errx == EHttpAuthenticationFailed)
        res = KStatusAlternateDownldAuthFail;

    RDEBUG( "Result code visited" ); 

    if(iCurrentDownloadProgState == EHttpProgConnectionSuspended && aDownloadState == EHttpDlPaused)
    {
      RDEBUG( "Connection lost and  suspended" ); 
      res = KStatusDownloadFailed;
      
    }

    // Display note in case of download failure
    // if download fails because of network loss then display note specific to Networkloss
    // else display note for general
    if(errx != ENoError )
    TRAP_IGNORE(DisplayDownloadFailedNoteL(errx,gerr));
    
    if (!iProgressCancelled)
            {
            RDEBUG( "iProgressNote->ProgressCompletedL() Start" ); 
            TRAP_IGNORE(iProgressNote->ProgressCompletedL());
            RDEBUG( "iProgressNote->ProgressCompletedL() End" ); 
            }
    
    iProgressCancelled = ETrue;
    
    RDEBUG_2( "CAMDownloader::DownloadStatusCodeL: Status code (%d)", res );
    return res;
    }

// ---------------------------------------------------------------------------
// DisplayDownloadFailedNoteL
// ---------------------------------------------------------------------------

void CAMDownloader::DisplayDownloadFailedNoteL(THttpDownloadMgrError aError, TInt32 aGlobalErr)
    {

    TBool iOffline = EFalse;
    TInt allowed(0);
    CRepository* coreAppUisCr = CRepository::NewLC( KCRUidCoreApplicationUIs );
    coreAppUisCr->Get( KCoreAppUIsNetworkConnectionAllowed, allowed ) ;
    CleanupStack::PopAndDestroy( coreAppUisCr );
    if( allowed != ECoreAppUIsNetworkConnectionAllowed )
        {
        iOffline = ETrue;
        }

    // current phone in offline mode or connection failure display note to user that connection is lost

    if(aError == EConnectionFailed || iOffline)
        {
         RDEBUG( "DisplayDownloadFailedNoteL : CAMDownloader Connection Failed" );  
        HBufC *netlossnote = StringLoader::LoadLC(R_DOWNLOADFAILED_CONNECTION);

        CAknGlobalNote* globalNote = CAknGlobalNote::NewL();
        CleanupStack::PushL(globalNote);
        
        globalNote->ShowNoteL(EAknGlobalErrorNote, *netlossnote);

        CleanupStack::PopAndDestroy( 2);// globalNote, dialognote
        }
    else
        {
        RDEBUG( "DisplayDownloadFailedNoteL : CAMDownloader" );       

        HBufC *generrornote = StringLoader::LoadLC(R_DOWNLOADFAILED_GENERAL);
        TRequestStatus status;              

        CAknGlobalNote* dialog = CAknGlobalNote::NewLC();
        dialog->SetSoftkeys(R_AVKON_SOFTKEYS_OK_EMPTY);
        
        dialog->ShowNoteL(status, EAknGlobalErrorNote, *generrornote);
        User::WaitForRequest(status);

        // ignore the softkey status..

        CleanupStack::PopAndDestroy(2);         // pop dialog, generrornote
        }    

    }

// ---------------------------------------------------------------------------
// HandleDLProgressDialogExitL
// ---------------------------------------------------------------------------

void CAMDownloader::HandleDLProgressDialogExitL(TInt aButtonId)
    {

    if (aButtonId == EAknSoftkeyCancel)
        {
        iProgressCancelled = ETrue;

        HBufC* note = StringLoader::LoadLC( R_DOWNLOAD_CANCELLED);

        CAknGlobalNote* globalNote = CAknGlobalNote::NewL();
        CleanupStack::PushL(globalNote);
        globalNote->ShowNoteL(EAknGlobalInformationNote, *note);

        CleanupStack::PopAndDestroy( 2);// globalNote, note


        TInt UIdisabled=0;
        RProperty::Set(KUidPSApplicationManagementKeys, KAMServerUIEnabled,
                UIdisabled);

        iObserver.SetStatus(KStatusUserCancelled);
        }

    }
//  End of File
