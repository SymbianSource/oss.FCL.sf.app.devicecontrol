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
//#include <DownloadMgrClient.h>

#include "AMDownloadManager.h"
//#include "AMDownload.h"
//#include "amdeploymentcomponent.h"
#include "amsmlhelper.h"
#include "debug.h"
#include "AppMgmtSrvApp.h"
#include <e32property.h>
#include <aknnotewrappers.h>
#include "ampskeys.h"
#include "SyncService.h"
#include "amprivateCRKeys.h"

using namespace NApplicationManagement;

// from THttpDownloadState
const TText * KDownloadStates[] =
    {
    _S( "Empty" ), _S( "EHttpDlCreated" ), _S( "EHttpDlInprogress" ), _S( "EHttpDlPaused" ), _S( "EHttpDlCompleted" ), _S( "EHttpDlFailed" ),

    // The following states are temporary.
            // e.g. download adopt EHttpDlMoved state,
            // but right after that, it returns to the
            // original EHttpDlCompleted state

            // download is moved from one client instance to another one.
            _S( "EHttpDlMoved" ),
            // MMC card or other storage media is removed from the phone.
            _S( "EHttpDlMediaRemoved" ),
            // MMC card or other storage media inserted and 
            // downloaded content file found on it. 
            // If MMC card inserted, but (partially) downloaded content file 
            // is not found on it, download is failed with error reason
            // EContentFileIntegrity.
            _S( "EHttpDlMediaInserted" ),
            // Download process can be paused again. This event only occurs after
            // EHttpDlNonPausable.
            _S( "EHttpDlPausable" ),
            // Download process cannot be paused, or the content will be lost.
            _S( "EHttpDlNonPausable" ),
            // Download is delete from another client instance.
            _S( "EHttpDlDeleted" ),
            // Download is started when it's already progressing
            _S( "EHttpDlAlreadyRunning" ),
            // Download is going to be deleted.
            _S( "EHttpDlDeleting" ),

            // Internally used download event. Do NOT deal with them
            _S( "EHttpDlCancelTransaction" )
    };

// These enumarated values come with EHttpDlInprogress
// The only exception is EHttpContentTypeReceived that pauses
// the download until client app accepts and resumes, or
// deletes/resets it.
const TText * ProgStates(THttpProgressState state)
    {
    switch (state)
        {
        case EHttpProgNone:
            return _S( "EHttpProgNone" );
        case EHttpStarted:
            return _S("EHttpStarted");
        case EHttpProgCreatingConnection:
            return _S("EHttpProgCreatingConnection");
        case EHttpProgConnectionNeeded:
            return _S("EHttpProgConnectionNeeded");

        case EHttpProgConnected:
            return _S("EHttpProgConnected");
        case EHttpProgConnectionSuspended:
            return _S("EHttpProgConnectionSuspended");
        case EHttpProgDisconnected:
            return _S("EHttpProgDisconnected");
        case EHttpProgDownloadStarted:
            return _S("EHttpProgDownloadStarted" );
        case EHttpContentTypeRequested:
            return _S("EHttpContentTypeRequested" );
        case EHttpContentTypeReceived:
            // Download status is EHttpDlPaused!
            // Application can check the received content type here
            // and decide whether to accept it or not.
            // Call Start again to carry on download, or
            // Delete to delete this download
            return _S("EHttpContentTypeReceived" );
        case EHttpProgSubmitIssued:
            return _S("EHttpProgSubmitIssued" );
        case EHttpProgResponseHeaderReceived:
            return _S("EHttpProgResponseHeaderReceived" );
        case EHttpProgResponseBodyReceived:
            return _S("EHttpProgResponseBodyReceived" );
        case EHttpProgRedirectedPermanently:
            return _S("EHttpProgRedirectedPermanently" );
        case EHttpProgRedirectedTemporarily:
            return _S("EHttpProgRedirectedTemporarily" );
        case EHttpProgMovingContentFile:
            return _S("EHttpProgMovingContentFile" );
        case EHttpProgContentFileMoved:
            return _S("EHttpProgContentFileMoved" );
        default:
            return _S( "Unknown" );
        }
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMDownloadManager::CAMDownloadManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAMDownloadManager::CAMDownloadManager(MDownloadMngrObserver& aObserver) :
    iObserver(aObserver)
    {
    }

// -----------------------------------------------------------------------------
// CAMDownloadManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAMDownloadManager::ConstructL()
    {
    iOMASCOMOEnabled = ETrue;
    iNoteDisabled = EFalse;
    
    }

// -----------------------------------------------------------------------------
// CAMDownloadManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAMDownloadManager* CAMDownloadManager::NewL(MDownloadMngrObserver& aObserver)
    {
    CAMDownloadManager* self = new( ELeave ) CAMDownloadManager( aObserver );

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CAMDownloadManager::~CAMDownloadManager()
    {
    RDEBUG_3( "CAMDownloadManager::~CAMDownloadManager 0x%X - 0x%X", reinterpret_cast<TUint>( this ),
            reinterpret_cast<TUint>(this)+sizeof( CAMDownloadManager ) );

    iDownloads.ResetAndDestroy();
    
    iPrevDownloads.ResetAndDestroy();

    RDEBUG( "CAMDownloadManager::~CAMDownloadManager() - closing downloadmgr" );
    if (iDownloadMgr.Handle())
            iDownloadMgr.Close();


    if (iServerId)
        delete iServerId;
    
    
    RDEBUG( "CAMDownloadManager::~CAMDownloadManager() - closed downloadmgr" );
    }

// ---------------------------------------------------------------------------
// CancelDownloads
// ---------------------------------------------------------------------------
/*
 void CAMDownloadManager::CancelDownloads()
 {
 TInt err = KErrNone;
 TRAP( err, CancelDownloadsL() );
 RDEBUG_2("CAMDownloadManager::Cancel() ERR(%d)", err);
 User::LeaveIfError( err );
 }
 */

// ---------------------------------------------------------------------------
// CancelDownloads
// ---------------------------------------------------------------------------
/*
 void CAMDownloadManager::CancelDownloadsL()
 {
 }
 */

// ---------------------------------------------------------------------------
// CancelDownloadL
// ---------------------------------------------------------------------------
void CAMDownloadManager::CancelDownloadL(CAMDownload& aDownload)
    {
    RDEBUG("CAMDownloadManager::CancelDownloadL()");
    ConnectHttpDownloadMgrL();

    TInt delres(aDownload.StopDownload() );
    if (delres != KErrNone)
        {
        RDEBUG8_3( "CAMDownloadManager::CancelDownloadL: ERROR Failed to delete download '%S': '%d'",
                &aDownload.DownloadURI(), delres );
        }
    }

// ------------------------------------------------------------------------------------------------
//  CAMDownloadManager::ConnectHttpDownloadMgrL()
// ------------------------------------------------------------------------------------------------
void CAMDownloadManager::ConnectHttpDownloadMgrL()
    {
    if ( !iDownloadMgrConnected)
        {
        const TUid KUid =
            {
            0x1020781C
            // ApplicationManagementServer
                };

        iDownloadMgr.ConnectL(KUid, *this, EFalse);
        iDownloadMgrConnected = ETrue;
        RDEBUG( "CAMDownloadManager::ConnectHttpDownloadMgrL(): Connected to download manager" );
        }
    }

// ----------------------------------------------------------------------------------------
// CAMDownloadManager::AddDownloadL
// ----------------------------------------------------------------------------------------	
void CAMDownloadManager::AddDownloadL(CDeploymentComponent *aComponent)
    {
    RDEBUG("CAMDownloadManager::AddDownloadL Add new Download: ");
    TPtrC8 uriPtr(aComponent->DownloadURI() );
    RDEBUG_HEX8(uriPtr.Ptr(), uriPtr.Size());
    aComponent->SetCallback( this);
    CAMDownload* download = 0;
    download = FindDownload(aComponent->DownloadURI() );
    if (download)
        {
        RDEBUG8_2("CAMDownloadManager::AddDownloadL Download already exist, cancelling: (%S)", &aComponent->DownloadURI());
        CancelDownloadL( *download);
        }

    iNoteDisabled = EFalse;

    // DM session alive. Check the type of Server Alert
    // Silent or NOn-Silent


    // PubSub key used to share information about the
    // type of the server alert for device management session.
    // Contains the identifier of server alert type
    // started from DM Server


    TInt nonSilentOperation = KErrNotFound;
    RProperty::Get(KPSUidNSmlDMSyncApp, KSilentSession, nonSilentOperation);
    
    iOMASCOMOEnabled = IsOMASCOMOEnabledL();
    
    // If OMA SCOMO is not enabled then default make the download operation silent
    if (!iOMASCOMOEnabled)
        nonSilentOperation = 0;

    // Get the type of session(silent or non-silent) from DM Session and store in AM server
    // P&S key since DM Session is terminated as soon as download starts.

    RProperty::Set(KUidPSApplicationManagementKeys, KAMOperationNonSilent,
            nonSilentOperation);

    // Set this P&S key so that DM UI will not be closed in case of server initiated session
    // applicable only for Interactive, Informative or Non specified

    TInt dmuinotclosed = 1;
    RProperty::Set(KUidPSApplicationManagementKeys, KDMUINotClosed,
            dmuinotclosed);

    RHttpDownload& httpdownload(CreateHttpDownloadL(aComponent) );
    
   
    CAMDownload* newDL = CAMDownload::NewL(httpdownload, aComponent);
    CleanupStack::PushL(newDL);
    iDownloads.Append(newDL);
    
    // keep track of all the downloads in another array
    // to ensure that downloads are not missed from actual array
    // during cleanup
    
    iPrevDownloads.Append(newDL);
    
    // start the newly created download
    newDL->StartDownload();
    aComponent->SetStatusNode(EDownload_DownloadProgress);
    TSmlProfileId ownerProfile;
    SyncMLCurrentProfileIdL(ownerProfile);
    aComponent->SetOwnerProfile(ownerProfile);
    CleanupStack::Pop(newDL);
    
    
    RDEBUG("CAMDownloadManager::AddDownloadL end");
    }

// ------------------------------------------------------------------------------------------------
// CAMDownloadManager::IsOMASCOMOEnabledL()
// ------------------------------------------------------------------------------------------------

TBool CAMDownloadManager::IsOMASCOMOEnabledL()
    {
    TInt adapterValue;
    
    CRepository *repository = CRepository::NewLC ( KCRUidPrivateApplicationManagementKeys ) ;
    repository->Get(KAMAdapterValue, adapterValue);
    CleanupStack::PopAndDestroy();
    
    if(adapterValue ==0 || adapterValue == 1)
     return EFalse;
    else
     return ETrue;
    
    }


// ----------------------------------------------------------------------------------------
// CAMDownloadManager::CreateHttpDownloadL
// ----------------------------------------------------------------------------------------	
RHttpDownload& CAMDownloadManager::CreateHttpDownloadL(
        CDeploymentComponent *aComponent)
    {
    RDEBUG( "CAMDownloadManager::CreateHttpDownload: start" );
    ConnectHttpDownloadMgrL();
    TBool dlCreated(EFalse);
    RHttpDownload &dl = iDownloadMgr.CreateDownloadL(
            aComponent->DownloadURI(), dlCreated);
    iServerId = SmlHelper::GetCurrentServerIDL();
    if (dlCreated)
        {
        RDEBUG( "CAMDownloadManager::CreateHttpDownload: Created Download" );
        dl.SetBoolAttribute(EDlAttrNoContentTypeCheck, ETrue);
        iDownloadMgr.SetBoolAttribute(EDlMgrSilentMode, ETrue);
        SetIAPL(aComponent);
        SetUserNamePassword(dl, aComponent);
        }
    else
        {
        RDEBUG( "CAMDownloadManager::CreateHttpDownload: Download already exists!" );
        THttpDownloadState dlst;
        dl.GetIntAttribute(EDlAttrState, (TInt32&)dlst );
        THttpProgressState prst;
        dl.GetIntAttribute(EDlAttrProgressState, (TInt32&)prst );

        SetIAPL(aComponent);
				RDEBUG_3( "CAMDownloadManager::CreateHttpDownload: DownloadState: '%s', progstate: '%s'", KDownloadStates[dlst], ProgStates( prst ) );

        if (dlst == EHttpDlPaused)
            {
            RDEBUG( "CAMDownloadManager::CreateHttpDownload: download paused" );
            }
        else
            {
            RDEBUG( "CAMDownloadManager::CreateHttpDownload: resetting download!" );
            dl.Reset();
            }
        }
    RDEBUG( "CAMDownloadManager::CreateHttpDownload: end" );
    return dl;
    }

// ----------------------------------------------------------------------------------------
// CAMDownloadManager::SetIAPL
// ----------------------------------------------------------------------------------------	
void CAMDownloadManager::SetIAPL(CDeploymentComponent *aComponent)
    {
    RDEBUG( "CAMDownloadManager::SetIAPL: start" );
    TInt iap(KErrNotFound);

    const TAMInstallOptions &opts = aComponent->InstallOpts();
    RDEBUG_2( "CAMDownloadManager::SetIAPL : Setting Iap Id to %d ", opts.iIAP );

    if (opts.iIAP == -1)
        {
        TRAPD( err, SmlHelper::GetDefaultIAPFromDMProfileL( iap ) )
        ;
        if (err != KErrNone)
            {
            RDEBUG_2("CAMDownloadManager::SetIAPL: ERROR when trying to fetch DM profile default IAP: %d", err );
            }
        }
    else
        {
        iap = opts.iIAP;
        }
    RDEBUG_2( "CAMDownloadManager::SetIAPL: Set IAP Id value to (%d) ", iap);
    
    if (iap <= -1)
        {
        iap = 0; //as DL mgr treats 0 (zero) as "Always Ask"
        }
    iapid = iap;
    TInt errDL(iDownloadMgr.SetIntAttribute(EDlMgrIap, iap) );
    if (errDL != KErrNone)
        {
        RDEBUG_2("CAMDownloadManager::SetIAPL: ERROR when trying to set DownloadManager IAP: %d", errDL );
        }
    else
        {
        RDEBUG_2( "CAMDownloadManager::SetIAPL: Set IAP Id to (%d) ", iap);
        }

#ifdef _DEBUG
    TInt err(iDownloadMgr.SetIntAttribute(EDlMgrIap, 0) );
    RDEBUG_2( "CAMDownloadManager::SetIAPL (debug only): WARNING Overriding IAP Id to 11 results %d", err );
#endif		
    }

// ----------------------------------------------------------------------------------------
// CAMDownloadManager::SetUserNamePassword
// ----------------------------------------------------------------------------------------	
void CAMDownloadManager::SetUserNamePassword(RHttpDownload& aDownload,
        CDeploymentComponent *aComponent)
    {
    RDEBUG( "CAMDownloadManager::SetUserNamePassword: start" );
    const TAMInstallOptions &opts = aComponent->InstallOpts();
    if (opts.iOptions.iPassword.Length() && opts.iOptions.iLogin.Length() )
        {
        TBuf8<KMaxPath> password;
        TBuf8<KMaxPath> login;
        password.Copy(opts.iOptions.iPassword);
        login.Copy(opts.iOptions.iLogin);

        TInt err(aDownload.SetStringAttribute(EDlAttrPassword, password) );
        RDEBUG8_3( "CAMDownloadManager::SetUserNamePassword: response to EDlAttrPassword set to '%S': %d", &password, err );
        err = aDownload.SetStringAttribute(EDlAttrUsername, login);
        RDEBUG8_3( "CAMDownloadManager::SetUserNamePassword: response to EDlAttrUsername set to '%S': %d", &login, err );
        }
    RDEBUG( "CAMDownloadManager::SetUserNamePassword: end" );
    }

// ----------------------------------------------------------------------------------------
// CAMDownloadManager::SyncMLCurrentProfileIdL
// ----------------------------------------------------------------------------------------	
void CAMDownloadManager::SyncMLCurrentProfileIdL(TSmlProfileId& aOwnerProfile)
    {
    RDEBUG( "CAMDownloadManager::SyncMLCurrentProfileIdL: start" );
#ifdef _DEBUG
    TRAPD( erx, SmlHelper::GetCurrentProfileIdL( aOwnerProfile ) )
    ;
    if (erx != KErrNone)
        {
        RDEBUG_2( "CAMDownloadManager::SyncMLCurrentProfileIdL (debug): Failed to get profile id: %d", erx );
        }
    else
        {
        RDEBUG_2( "CAMDownloadManager::SyncMLCurrentProfileIdL (debug): Using profile id: %d", aOwnerProfile );
        }
#else
    SmlHelper::GetCurrentProfileIdL( aOwnerProfile );
    RDEBUG_2( "CAMDownloadManager::SyncMLCurrentProfileIdL: Find profile id: %d", aOwnerProfile );
#endif
    }

// ----------------------------------------------------------------------------------------
// CAMDownloadManager::FindDownload
// ----------------------------------------------------------------------------------------	
CAMDownload* CAMDownloadManager::FindDownload(const TDesC8& aURI)
    {
    CAMDownload* download = 0;

    //find corresponding download from list
    for (TInt i(0); i < iDownloads.Count(); i++)
        {
        download = iDownloads[i];
        if (download->DownloadURIMatch(aURI) )
            {
            RDEBUG("CAMDownloadManager::FindDownload: Found valid download");
            break;
            }
        download = 0;
        }
    return download;
    }

// ----------------------------------------------------------------------------------------
// CAMDownloadManager::FindDownload
// ----------------------------------------------------------------------------------------	
CAMDownload* CAMDownloadManager::FindDownload(RHttpDownload& aDownload)
    {
    CAMDownload* download = 0;
    TBuf8<KMaxUrlLength> uri;
    DownloadURI(aDownload, uri);
    download = FindDownload(uri);
    return download;
    }

// ----------------------------------------------------------------------------------------
// CAMDownloadManager::FindDownload
// ----------------------------------------------------------------------------------------	
CAMDownload* CAMDownloadManager::FindDownload(
        CDeploymentComponent *aComponent)
    {
    CAMDownload* download = 0;
    download = FindDownload(aComponent->DownloadURI() );
    return download;
    }

// ----------------------------------------------------------------------------------------
// CAMDownloadManager::DownloadURI
// ----------------------------------------------------------------------------------------	
void CAMDownloadManager::DownloadURI(RHttpDownload& aDownload, TDes8& aURI)
    {
    TInt err(aDownload.GetStringAttribute(EDlAttrReqUrl, aURI) );
    RDEBUG_2("CAMDownloadManager::DownloadURI err: (%d)", err);
    }

// ----------------------------------------------------------------------------------------
// CAMDownloadManager::HandleDMgrEventL
// ----------------------------------------------------------------------------------------	
void CAMDownloadManager::HandleDMgrEventL(RHttpDownload& aDownload,
        THttpDownloadEvent aEvent)
    {
    //RDEBUG_3( "CAMDownloadManager::HandleDMgrEventL: DownloadState: '%s', progstate: '%s'", 
    //	KDownloadStates[aEvent.iDownloadState], ProgStates( aEvent.iProgressState ) );

    TInt32 contentsize;
    TInt32 downloadedsize;

    User::LeaveIfError(aDownload.GetIntAttribute(EDlAttrDownloadedSize,
            downloadedsize) );
    User::LeaveIfError(aDownload.GetIntAttribute(EDlAttrLength, contentsize) );

    CAMDownload* download = FindDownload(aDownload);

    if ( !download)
        {
        TBuf8<KMaxUrlLength> uri;
        DownloadURI(aDownload, uri);
        RDEBUG8_2("CAMDownloadManager::HandleDMgrEventL: Could not find download: (%S)", &uri);
        //User::Leave( KErrNotFound );	// Don't do this! DownloadMgrClient::HandleDMgrEventL does not handle leave correctly.
        }
    else
        {
        switch (aEvent.iDownloadState)
            {
           /* case EHttpDlPaused:
                HandlePausedEventL( *download);
                break;*/
            case EHttpDlCreated:
                {
                if (aEvent.iProgressState == EHttpProgNone)
                    {
                    HandleNonProgEventL( *download);
                    }
                }
                break;
            case EHttpDlInprogress:
                {
                HandleAlreadyRunningL(downloadedsize, contentsize, *download, aEvent.iProgressState);

                if (!iNoteDisabled)
                    {
                    CSyncService *syncService =
                            CSyncService::NewL(NULL, KDevManServiceStart);
                    if (syncService)
                        {
                        syncService->EnableProgressNoteL(EFalse);
                        }

                    delete syncService;
                    iNoteDisabled = ETrue;
                    }
                }
                break;
            case EHttpContentTypeReceived:
                HandleContentTypeRecEventL( *download);
                break;
            case EHttpDlCompleted:
                HandleDlCompletedEventL( *download);
                break;
            case EHttpDlPaused:
            case EHttpDlFailed:
                HandleDlFailedEventL( *download, aEvent.iDownloadState);
                break;
            case EHttpDlDeleting:
            case EHttpDlDeleted:
                HandleDlDeleteEventL(download);
                break;
            default:
                /*			RDEBUG8_2( "CAMDownloadManager::HandleDMgrEventL: Not handled download event: (%S)",
                 KDownloadStates[aEvent.iDownloadState] ); 
                 
                 */
                break;
            }
        }
    RDEBUG( "CAMDownloadManager::HandleDMgrEventL: end" );
    }

// ----------------------------------------------------------------------------------------
// CAMDownloadManager::HandlePausedEventL
// ----------------------------------------------------------------------------------------	
void CAMDownloadManager::HandlePausedEventL(CAMDownload& aDownload)
    {
    RDEBUG( "CAMDownloadManager::HandlePausedEventL" );
    aDownload.ContinueDownload();
    }

// ----------------------------------------------------------------------------------------
// CAMDownloadManager::HandleNonProgEventL
// ----------------------------------------------------------------------------------------	
void CAMDownloadManager::HandleNonProgEventL(CAMDownload& aDownload)
    {
    RDEBUG( "CAMDownloadManager::HandleNonProgEventL" );
    aDownload.ContinueDownload();
    }

// ----------------------------------------------------------------------------------------
// CAMDownloadManager::HandleContentTypeRecEventL
// ----------------------------------------------------------------------------------------	
void CAMDownloadManager::HandleContentTypeRecEventL(CAMDownload& aDownload)
    {
    RDEBUG( "CAMDownloadManager::HandleContentTypeRecEventL: EHttpContentTypeReceived " );
    const TDesC8& mimetype(aDownload.ContentMimeType() );
    RDEBUG8_2( "CAMDownloadManager::HandleContentTypeRecEventL: Content type: (%S)", &mimetype );
    }

// ----------------------------------------------------------------------------------------
// CAMDownloadManager::HandleDlCompletedEventL
// ----------------------------------------------------------------------------------------	
void CAMDownloadManager::HandleDlCompletedEventL(CAMDownload& aDownload)
    {
    RDEBUG( "CAMDownloadManager::HandleDlCompletedEventL" );
    aDownload.DownloadCompleted();
    }

// ----------------------------------------------------------------------------------------
// CAMDownloadManager::HandleDlFailedEventL
// ----------------------------------------------------------------------------------------	
void CAMDownloadManager::HandleDlFailedEventL(CAMDownload& aDownload, THttpDownloadState aDownloadState)
    {
    RDEBUG( "CAMDownloadManager::HandleDlFailedEventL" );
    aDownload.DownloadFailedL(aDownloadState);
    }

// ----------------------------------------------------------------------------------------
// CAMDownloadManager::HandleDlDeleteEventL
// ----------------------------------------------------------------------------------------	
void CAMDownloadManager::HandleDlDeleteEventL(CAMDownload* aDownload)
    {
    RDEBUG( "CAMDownloadManager::HandleDlDeleteEventL - start" );
    TInt idx(iDownloads.Find(aDownload) );
        {
        RDEBUG_2( "CAMDownloadManager::HandleDlDeleteEventL found download: (%d) delete it", idx );
        iDownloads.Remove(idx);
        //	delete aDownload;
        }
    RDEBUG( "CAMDownloadManager::HandleDlDeleteEventL - end" );
    }

// ----------------------------------------------------------------------------------------
// CAMDownloadManager::DownloadComplete
// ----------------------------------------------------------------------------------------	
void CAMDownloadManager::DownloadComplete(CDeploymentComponent *aComponent,
        TInt aStatus)
    {
    RDEBUG_2( "CAMDownloadManager::DownloadComplete - start with status (%d)", aStatus );
    TInt err(KErrNone);
    TRAP( err, DownloadCompleteL( aComponent ));
    RDEBUG_2( "CAMDownloadManager::DownloadComplete - end with status (%d)", err);
    }

// ----------------------------------------------------------------------------------------
// CAMDownloadManager::DownloadCompleteL
// ----------------------------------------------------------------------------------------	
void CAMDownloadManager::DownloadCompleteL(CDeploymentComponent *aComponent)
    {
    RDEBUG( "CAMDownloadManager::DownloadComplete - start" );
    __ASSERT_DEBUG( aComponent, User::Invariant() );

    CAMDownload* dowload = FindDownload(aComponent);

    TInt idx(iDownloads.Find(dowload) );
    if (dowload)
        dowload->StopDownload(); // delete RHttpDownload
    if (KErrNotFound != idx)
        {
        // NOTICE: Download is removed from downloadmanager AFTER receiving EHttpDlDeleting message
        //         from RHttpDownload
        // Call server
        iObserver.ComponentDownloadComplete(aComponent, iapid, iServerId);
        }

    RDEBUG( "CAMDownloadManager::DownloadComplete - end" );
    }

// ----------------------------------------------------------------------------------------
// CAMDownloadManager::UidExistsL
// ----------------------------------------------------------------------------------------	
TBool CAMDownloadManager::UidExistsL(const TUid &aUid,
        CDeploymentComponent *& aCompo, CDeploymentComponent *aIgnored)
    {
    TBool uidExist(EFalse);
    uidExist = iObserver.HasUidL(aUid, aCompo, aIgnored);
    return uidExist;
    }

// ----------------------------------------------------------------------------------------
// CAMDownloadManager::DownloadCount
// ----------------------------------------------------------------------------------------	
TInt CAMDownloadManager::DownloadCount() const
    {
    TInt count(0);
    count = iDownloads.Count();
    RDEBUG_2( "CAMDownloadManager::DownloadCount: (%d)", count );
    return count;
    }

void CAMDownloadManager::HandleAlreadyRunningL(TInt32 aDownloadedSize,
        TInt32 aContentSize, CAMDownload& aDownload, THttpProgressState aState)
    {

    aDownload.ProgressDialogL(aDownloadedSize, aContentSize, aState);

    }

//  End of File	
