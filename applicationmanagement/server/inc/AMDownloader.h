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

#ifndef __CAMDOWNLOADER_H__
#define __CAMDOWNLOADER_H__

#include <e32base.h>
#include "AppMgmtProgDialog.h"
#include "AppMgmtSrvApp.h"

// FORWARD DECLARATIONS
class RHttpDownload;

namespace NApplicationManagement
    {

    // CLASS DECLARATION

    /**
     *  CAMDownloader lets AM to download content.
     *
     *  @since Series 
     */
    class CAMDownloader : public CBase, public MDLProgressDlgObserver
        {
public:
        // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CAMDownloader* NewL(RHttpDownload& aDownload,
                MAMDownloaderObserver& aObserver);
        /**
         * Destructor.
         */
        virtual ~CAMDownloader();
        void HandleDLProgressDialogExitL(TInt aButtonId);
	void DisplayDownloadFailedNoteL(THttpDownloadMgrError aError, TInt32 aGlobalErr);

private:
        /**
         * C++ default constructor.
         */
        CAMDownloader(RHttpDownload& aDownload,
                MAMDownloaderObserver& aObserver);

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

public:
        // New functions

        void CreateAndUpdateProgressDialogL(TInt32 aDownloadedSize,
                TInt32 aContentSize, THttpProgressState aState);
        /**
         * Starts download of content
         * @since 
         * @leave If staring download fails
         */
        TInt StartDownload();

        /**
         * Stop download of content
         * @since 
         */
        TInt StopDownload();

        /**
         * Continue download of content
         * @since 
         */
        TInt ContinueDownload();

        /**
         * 
         * @since 
         */
        TBool CompareDownloadURI(const TDesC8& aURI);

        /**
         *
         * @leave If staring download fails
         */
        TInt32 DownloadStatusCodeL(THttpDownloadState aDownloadState);

        /**
         * Returns the filename that holds the downloaded
         *		content if download is successful
         * @since 
         * @return	filename
         */
        const TDesC& FileName();

        /**
         * Returns the mimetype of downloaded
         * @since 
         * @return	mimetype
         */
        const TDesC8& MimeType();

        /**
         * Returns the URI for download
         * @since 
         * @return	download URI
         */
        const TDesC8& DownloadURI();

private:
        void FileNameFromDownloadL();
        void ContentTypeFromDownloadL();
        void DownloadURIFromDownloadL();

public:
        // Functions from base classes
        TUint32 iProgStarted;
        TUint32 iSetFinalValue;

        CAppMgmtProgDialog *iProgressNote;

private:
        // Data
        RHttpDownload &iDownload;

        HBufC *iFileName;
        HBufC8 *iContentType;
        HBufC8 *iURI;
        MAMDownloaderObserver& iObserver;
        TBool iProgressCancelled;
        THttpProgressState iCurrentDownloadProgState;
        };

    } // namespace NApplicationManagement

#endif // __CAMDOWNLOADER_H__
// End of File
