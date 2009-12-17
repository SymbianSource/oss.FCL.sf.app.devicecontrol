/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of PnPMS components
*
*/



#ifndef PNPPROVISIONINGDOCUMENT_H
#define PNPPROVISIONINGDOCUMENT_H

// INCLUDES
#include <AknDoc.h>
#include "HttpProvStates.h"

// CONSTANTS

// FORWARD DECLARATIONS
class  CEikAppUi;
class  CPnpProvisioningAppUi;
class  CNHeadWrapperParser;
// CLASS DECLARATION

/**
*  CPnpProvisioningDocument application class.
*/
class CPnpProvisioningDocument : public CAknDocument
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CPnpProvisioningDocument* NewL(CEikApplication& aApp);

        /**
        * Destructor.
        */
        virtual ~CPnpProvisioningDocument();

    public: // New functions

        /**
        * Handles provisioning file
        */
        void HandleFileL(TBool aDoOpen,const TDesC& aFilename,RFs& aFs);
        void DoHandleFileL( RFile& aFile );

        /**
        * Saves settings
        */
        THttpProvStates::TProvisioningStatus SaveSettingsL();

        inline TBool Ready() { return(iReady); }

        /**
        * Get report url
        */
        const TDesC8& GetReportUrlL(); 

    public: // Functions from base classes
        
        /**
        * From CApaDocument: This is called by framework to restore saved data
        * from persistent store to document
        */
        void RestoreL(const CStreamStore& aStore, const CStreamDictionary& aStreamDic);

        /**
        * From CApaDocument: This is called by framework to store data
        * to persistent store
        */
        void StoreL(CStreamStore& aStore, CStreamDictionary& aStreamDic) const;

        // from CEikDocument
        void OpenFileL( CFileStore*& aFileStore, RFile& aFile );

        // from CEikDocument
        CFileStore* OpenFileL(TBool aDoOpen,const TDesC& aFilename,RFs& aFs);

    private:

        /**
        * EPOC default constructor.
        */
        CPnpProvisioningDocument(CEikApplication& aApp);
        void ConstructL();

    private:

        /**
        * From CEikDocument, create CPnpProvisioningAppUi "App UI" object.
        */
        CEikAppUi* CreateAppUiL();
        CPnpProvisioningAppUi* iAppUi;
        HBufC8* iProvisioningData; 
        CNHeadWrapperParser* iNHwrParser;
        THttpProvStates::TProvisioningStatus iProvisioningStatus; 
        TBool iReady;
        //CPeriodic* iPeriodic;    
    };

#endif

// End of File

