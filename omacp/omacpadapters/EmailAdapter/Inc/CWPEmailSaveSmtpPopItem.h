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
* Description:  Save SMTP-POP settings
*
*/



#ifndef CSAVESMTPPOPITEM_H
#define CSAVESMTPPOPITEM_H

//  INCLUDES
#include "CWPEmailSaveItem.h"

// FORWARD DECLARATIONS
class CWPEmailItem;

// CLASS DECLARATION

/**
*  CSaveSmtpPopItem class.
*
*  @lib WPEmailAdapter.lib
*  @since 2.0
*/
class CSaveSmtpPopItem : public CWPEmailSaveItem
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aSmtpItem SMTP setting item, Panic if NULL
        * @param aPopItem POP setting item, Panic if NULL
        * @return self
        */
        static CSaveSmtpPopItem* NewLC(CWPEmailItem* aSmtpItem, 
            CWPEmailItem* aPopItem);
              
        /**
        * Destructor.
        */
        ~CSaveSmtpPopItem();

    public: // from CWPEmailSaveItem
        
        void SaveL(CMsvSession* aSession);
        const TDesC& SummaryText();
        TBool ValidateL();
           
    private: // New functions

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * constructor.
        * @param see NewLC
        */
        CSaveSmtpPopItem(CWPEmailItem* aSmtpItem, 
            CWPEmailItem* aPopItem);

        /**
        * Validate SMTP item data
        * @return ETrue if valid.
        */
        TBool ValidateSmtpItemL();
        /**
        * Validate Pop data
        * @return ETrue if valid
        */
        TBool ValidatePopItemL();


    private:    // Data

        /**
        * Ref. to SMTP item data.
        */
        CWPEmailItem* iSmtpItem;

        /**
        * Ref. to POP item data.
        */
        CWPEmailItem* iPopItem;


    };

#endif      // CSAVESMTPPOPITEM_H   
            
// End of File
