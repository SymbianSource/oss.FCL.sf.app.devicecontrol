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

#ifndef __CAMDOWNLOAD_H__
#define __CAMDOWNLOAD_H__

#include <e32base.h>
#include <downloadmgrclient.h>


// FORWARD DECLARATIONS
class RHttpDownload;

namespace NApplicationManagement
    {

    // FORWARD DECLARATIONS
    class CDeploymentComponent;
    class CAMDownloader;

    /**
     * Mix in interface that will receive status updates
     *	as the download progresses.
     */
    class MAMDownloadStateObserver
        {
public:
        /**
         * StatusUpdateL Sets the current download status. This is 
         *	currently HTTP status code 
         *	(200 is success, 400+ failure, 100-199 in progress)
         * @param	aNewStatus	The new download status 
         */
        virtual void StatusUpdateL(TInt aNewStatus) = 0;
        virtual void SuccessStatusUpdateL(const TDesC &aDlFileName,
                const TDesC8& aDlMimeType) = 0;
        };

    class MAMDownloaderObserver
        {
public:
        /* Whenever User cancels the download AM Download observes the event. If User cancels
         * then calls download complete of AM Server*/
        virtual void SetStatus(TInt aStatus) = 0;
        };

    /**
     *  CAMDownload contains context info about specific download.
     *
     *  @since Series 
     */
    class CAMDownload : public CBase, public MAMDownloaderObserver
        {
private:
        /**
         * C++ default constructor.
         */
        CAMDownload(CDeploymentComponent *aComponent);

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL(RHttpDownload& aDownload);

public:
        // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CAMDownload* NewL(RHttpDownload& aDownload,
                CDeploymentComponent *aComponent);

        /**
         * Destructor.
         */
        virtual ~CAMDownload();

public:

        TBool DownloadURIMatch(const TDesC8& aURI);

        const TDesC8& DownloadURI();
        const TDesC8& ContentMimeType();

        TInt StartDownload();
        TInt StopDownload();
        TInt ContinueDownload();
        void DownloadCompleted();
        void DownloadFailedL(THttpDownloadState aDownloadState);
        TInt32 DownloadErrorCodeL();
        void ProgressDialogL(TInt32 aDownloadedSize, TInt32 aContentSize, THttpProgressState aState);
        void SetStatus(TInt aStatus);

private:
        CAMDownloader* iDownloader; // owned				
        CDeploymentComponent* iComponent; // NOT owned
        };

    } //namespace NApplicationManagement

#endif // __CAMDOWNLOAD_H__
// End of File
