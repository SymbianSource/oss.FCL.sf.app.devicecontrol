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

#ifndef __CAMDOWNLOADMANAGER_H__
#define __CAMDOWNLOADMANAGER_H__

#include <e32base.h>
#include <downloadmgrclient.h>

#include "AMDeploymentComponent.h"
#include "aminstalloptions.h"

namespace NApplicationManagement
    {

    // FORWARD DECLARATIONS
    class CDeploymentComponent;

    // CLASS DECLARATION

    class MDownloadMngrObserver
        {
public:
        virtual void ComponentDownloadComplete(
                class CDeploymentComponent *aComponent, TInt iapid,
                HBufC8* aServerId) = 0;
        virtual TBool HasUidL(const TUid &aUid,
                CDeploymentComponent *& aCompo,
                CDeploymentComponent *aIgnored) = 0;
        };

    /**
     *  CAMDownloadManager lets AM to download content.
     *
     *  @since Series 
     */
    class CAMDownloadManager : public CBase, public MHttpDownloadMgrObserver,
        public MDownloadCallback
        {
private:
        /**
         * C++ default constructor.
         */
        CAMDownloadManager(MDownloadMngrObserver& aObserver);

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

public:
        // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CAMDownloadManager* NewL(MDownloadMngrObserver& aObserver);
        /**
         * Destructor.
         */
        virtual ~CAMDownloadManager();

public:
        // From MHttpDownloadMgrObserver 

        /**
         * Indicates an event.
         * @param aDownload The download with which the event occured.
         * @param aEvent Event type.
         * @return None.
         */
        void HandleDMgrEventL(RHttpDownload& aDownload,
                THttpDownloadEvent aEvent);

public:
        // from MDownloadCallback

        /**
         * 
         * @param aComponent 
         * @param aStatus 
         */
        void DownloadComplete(CDeploymentComponent *aComponent, TInt aStatus);

        /**
         * Looks all deployments components and sis & java registries
         * to find out whether given uid is already used...
         * @param aUid The uid to be looked for
         * @return ETrue if uid is found, EFalse otherwise
         */
        TBool UidExistsL(const TUid &aUid, CDeploymentComponent *& aCompo,
                CDeploymentComponent *aIgnored);
                
        TBool IsOMASCOMOEnabledL();

public:
        // New functions

        /**
         *
         */
        void AddDownloadL(CDeploymentComponent *aComponent);

        /**
         *
         */
        TInt DownloadCount() const;

        /**
         *
         */
        //void CancelDownloads();		

private:
        void ConnectHttpDownloadMgrL();
        RHttpDownload& CreateHttpDownloadL(CDeploymentComponent *aComponent);
        void SetIAPL(CDeploymentComponent *aComponent);
        void SetUserNamePassword(RHttpDownload& aDownload,
                CDeploymentComponent *aComponent);
        void SyncMLCurrentProfileIdL(TSmlProfileId& aOwnerProfile);

        CAMDownload* FindDownload(const TDesC8& aURI);
        CAMDownload* FindDownload(RHttpDownload& aDownload);
        CAMDownload* FindDownload(CDeploymentComponent *aComponent);
        void DownloadURI(RHttpDownload& aDownload, TDes8& aURI);

        void HandlePausedEventL(CAMDownload& aDownload);
        void HandleNonProgEventL(CAMDownload& aDownload);
        void HandleContentTypeRecEventL(CAMDownload& aDownload);
        void HandleDlCompletedEventL(CAMDownload& aDownload);
        void HandleDlFailedEventL(CAMDownload& aDownload, THttpDownloadState aDownloadState);
        void HandleDlDeleteEventL(CAMDownload* aDownload);

        /**
         * Completed download for component.
         * @param aComponent The component which download is completed.
         */
        void DownloadCompleteL(CDeploymentComponent *aComponent);

        /**
         * Cancel downloading of content.
         * @param aDownload The download which is  cancelled.
         */
        void CancelDownloadL(CAMDownload& aDownload);
        void HandleAlreadyRunningL(TInt32 aDownloadedSize,
                TInt32 aContentSize, CAMDownload& aDownload, THttpProgressState aState);

public:
        // Functions from base classes

private:
        // Data
        RHttpDownloadMgr iDownloadMgr;
        TBool iDownloadMgrConnected;
        
        RPointerArray<CAMDownload> iDownloads;
        
        RPointerArray<CAMDownload> iPrevDownloads;
        
        TInt iPrevDownldCount;
        
        MDownloadMngrObserver& iObserver;

        TInt iapid;
        HBufC8 *iServerId;
        TBool iOMASCOMOEnabled;
        TBool iNoteDisabled;
        
        };

    } // namespace NApplicationManagement



#endif // __CAMDOWNLOADMANAGER_H__
// End of File
