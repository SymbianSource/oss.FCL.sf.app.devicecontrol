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
 * Description: Implementation of applicationmanagement components
 *
 */

// INCLUDE FILES
#include "AMDownload.h"
#include "AMDownloader.h"
#include "AMDeploymentComponent.h"
#include "debug.h"

using namespace NApplicationManagement;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMDownload::CAMDownload
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAMDownload::CAMDownload(CDeploymentComponent *aComponent) :
    iComponent(aComponent)
    {
    }

// -----------------------------------------------------------------------------
// CAMDownload::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAMDownload::ConstructL(RHttpDownload& aDownload)
    {
    iDownloader = CAMDownloader::NewL(aDownload, *this);
    }

// -----------------------------------------------------------------------------
// CAMDownload::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAMDownload* CAMDownload::NewL(RHttpDownload& aDownload,
        CDeploymentComponent *aComponent)
    {
    CAMDownload* self = new( ELeave ) CAMDownload( aComponent );

    CleanupStack::PushL(self);
    self->ConstructL(aDownload);
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CAMDownload::~CAMDownload()
    {
    RDEBUG_3( "CAMDownload::~CAMDownload 0x%X - 0x%X", reinterpret_cast<TUint>( this ),
            reinterpret_cast<TUint>(this)+sizeof( CAMDownload ) );

    delete iDownloader;
    }

// ---------------------------------------------------------------------------
// DownloadCompleted
// ---------------------------------------------------------------------------
void CAMDownload::DownloadCompleted()
    {
    RDEBUG( "CAMDownload::DownloadCompleted: start");

    TInt err(KErrNone);
    TRAP( err, iComponent->SuccessStatusUpdateL( iDownloader->FileName(), iDownloader->MimeType() ) );
    if (err != KErrNone)
        {
        RDEBUG_2(" ->iComponent::StatusUpdateL: %d", err );
        }
    RDEBUG_2( "CAMDownload::DownloadCompleted: end Download delete ERROR (%d)", err );
    }

// ---------------------------------------------------------------------------
// DownloadFailed
//
// EDlAttrStatusCode = 500,// Status code from response header (TInt32 - read-only)
// EDlAttrErrorId,         // See THttpDownloadMgrError (TInt32 - read-only)
// EDlAttrGlobalErrorId,   // Global error id (TInt32 - read-only)
// EDlAttrContentType,     // Content type from response header
//   (String8<KMaxContentTypeLength> - read/write. Write is accepted only if EDlAttrContinue is ETrue)
// ---------------------------------------------------------------------------
void CAMDownload::DownloadFailedL(THttpDownloadState aDownloadState)
    {
    RDEBUG( "CAMDownload::DownloadFailed: start");
    TInt err(KErrNone);
    TInt32 errorCode = iDownloader->DownloadStatusCodeL(aDownloadState);
    TRAP( err, iComponent->StatusUpdateL( errorCode ) );

    if (err != KErrNone)
        {
        RDEBUG_2(" ->iComponent::StatusUpdateL: %d", err );
        }
    RDEBUG_2( "CAMDownload::DownloadFailed: end Download delete ERROR (%d)", err );
    }

void CAMDownload::SetStatus(TInt aStatus)
    {
    TRAPD( err, iComponent->StatusUpdateL( aStatus ) )
    ;
    if (err != KErrNone)
        {
        RDEBUG_2(" ->iComponent::StatusUpdateL: %d", err );
        }
    RDEBUG_2( "CAMDownload::DownloadFailed: end Download delete ERROR (%d)", err );
    }

// ---------------------------------------------------------------------------
// DownloadErrorCodeL
//
// EDlAttrStatusCode = 500,// Status code from response header (TInt32 - read-only)
// EDlAttrErrorId,         // See THttpDownloadMgrError (TInt32 - read-only)
// EDlAttrGlobalErrorId,   // Global error id (TInt32 - read-only)
// EDlAttrContentType,     // Content type from response header
//   (String8<KMaxContentTypeLength> - read/write. Write is accepted only if EDlAttrContinue is ETrue)
// ---------------------------------------------------------------------------
/*TInt32 CAMDownload::DownloadErrorCodeL()
    {
    return iDownloader->DownloadStatusCodeL();
    }*/

// ---------------------------------------------------------------------------
// ContinueDownload
// ---------------------------------------------------------------------------
TInt CAMDownload::ContinueDownload()
    {
    RDEBUG("CAMDownload::ContinueDownload");
    TInt result(KErrNone);
    result = iDownloader->ContinueDownload();
    return result;
    }

// ---------------------------------------------------------------------------
// StartDownload
// ---------------------------------------------------------------------------
TInt CAMDownload::StartDownload()
    {
    RDEBUG("CAMDownload::StartDownload");
    TInt result(KErrNone);
    result = iDownloader->StartDownload();
    return result;
    }

// ---------------------------------------------------------------------------
// StopDownload
// ---------------------------------------------------------------------------
TInt CAMDownload::StopDownload()
    {
    RDEBUG("CAMDownload::StopDownload");
    TInt result(KErrNone);
    result = iDownloader->StopDownload();
    return result;
    }

void CAMDownload::ProgressDialogL(TInt32 aDownloadedSize, TInt32 aContentSize, THttpProgressState aState)
    {

    iDownloader->CreateAndUpdateProgressDialogL(aDownloadedSize, aContentSize, aState);

    }
// ---------------------------------------------------------------------------
// DownloadURIMatch
// ---------------------------------------------------------------------------
TBool CAMDownload::DownloadURIMatch(const TDesC8& aURI)
    {
    return iDownloader->CompareDownloadURI(aURI);
    }

// ---------------------------------------------------------------------------
// DownloadURI
// ---------------------------------------------------------------------------
const TDesC8& CAMDownload::DownloadURI()
    {
    return iDownloader->DownloadURI();
    }

// ---------------------------------------------------------------------------
// ContentMimeType
// ---------------------------------------------------------------------------
const TDesC8& CAMDownload::ContentMimeType()
    {
    return iDownloader->MimeType();
    }

//  End of File
