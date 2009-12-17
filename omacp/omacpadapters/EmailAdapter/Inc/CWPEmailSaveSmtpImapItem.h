/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Save SMTP/IMAP settings
*
*/



#ifndef CSAVESMTPIMAPITEM_H
#define CSAVESMTPIMAPITEM_H

//  INCLUDES
#include "CWPEmailSaveItem.h"

// FORWARD DECLARATIONS
class CWPEmailItem;

// CLASS DECLARATION

/**
*  CSaveSmtpImapItem class.
*
*  @lib WPEmailAdapter.lib
*  @since 2.0
*/
class CSaveSmtpImapItem : public CWPEmailSaveItem
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aSmtpItem SMTP setting item. Ownership not taken, Panic if NULL
        * @param aImapItem IMAP setting item. Ownership not taken, Panic if NULL
        * @return self
        */
        static CSaveSmtpImapItem* NewLC(CWPEmailItem* aSmtpItem, CWPEmailItem* aImapItem);
        
        /**
        * Constructor. See params from NewLC
        */
        CSaveSmtpImapItem(CWPEmailItem* aSmtpItem, CWPEmailItem* aImapItem);
        
        /**
        * Destructor.
        */
        virtual ~CSaveSmtpImapItem();

    public: // From CWPEmailSaveItem
        /**
        * From CWPEmailSaveItem
        */        
        void SaveL(CMsvSession* aSession);

        /**
        * From CWPEmailSaveItem
        */        
        TBool ValidateL();

        /**
        * From CWPEmailSaveItem
        */        
        const TDesC& SummaryText();

    private: // New functions
        /**
        * Validate smtp data
        */
        TBool ValidateSmtpItemL();
        /**
        * Validate imap data
        */
        TBool ValidateImapItemL();
        
    private: // Constructor

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: //Data
        /**
        * ref: SMTP setting item.
        */
        CWPEmailItem* iSmtpItem;
        /**
        * ref: POP setting item.
        */
        CWPEmailItem* iImapItem;
    };

#endif      // CSAVESMTPIMAPITEM_H   
            
// End of File
