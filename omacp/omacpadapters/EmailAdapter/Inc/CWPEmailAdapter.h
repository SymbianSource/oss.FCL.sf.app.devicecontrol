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
* Description:  Email setting adapter.
*
*/


#ifndef _CWPEMAILADAPTER_H
#define _CWPEMAILADAPTER_H

// INCLUDES
#include "CWPAdapter.h"
#include <MWPContextExtension.h>

#include <msvapi.h>

// FORWARD DECLARATIONS
class CDesC16Array;
class CWPEmailItem;
class CWPEmailSaveItem;
class CCommsDatabase;


// CLASS DECLARATION

/**
 * CWPEmailAdapter handles provisioning settings.
 *  @lib WPEmail.dll
 *  @since 2.0
 */ 
class CWPEmailAdapter : public CWPAdapter, 
                        private MMsvSessionObserver, 
                        private MWPContextExtension
	{
	public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param None
        * @return self
        */
        static CWPEmailAdapter* NewL();
 		
        /// Destructor
        virtual ~CWPEmailAdapter();

    public: // From CWPAdapter
        /**
        * From CWPAdapter
        * @param 
        * @return 
        */     
        TInt ItemCount() const;
        /**
        * From CWPAdapter
        * @param 
        * @return 
        */             
		const TDesC16& SummaryTitle(TInt aIndex) const;

        /**
        * From CWPAdapter
        * @param 
        * @return 
        */     		
		const TDesC16& SummaryText(TInt aIndex) const;
		
        /**
        * From CWPAdapter
        * @param 
        * @return 
        */     		
        void SaveL(TInt aItem);

        /**
        * From CWPAdapter
        * @param 
        * @return 
        */             
        TBool CanSetAsDefault( TInt aItem ) const;

        /**
        * From CWPAdapter
        * @param 
        * @return 
        */             
        void SetAsDefaultL( TInt aItem );
        
        /**
        * From CWPAdapter
        * @param 
        * @return 
        */             
		TInt DetailsL( TInt aItem, MWPPairVisitor& aVisitor );

        /**
        * From CWPAdapter
        * @param 
        * @return 
        */     		
        TInt ContextExtension( MWPContextExtension*& );

	private: // From MWPVisitor
        /**
        * From MWPVisitor
        * @param 
        * @return 
        */       
		void VisitL(CWPCharacteristic& aElement);
		
		/**
        * From MWPVisitor
        * @param 
        * @return 
        */       
		void VisitL(CWPParameter& aElement);
		
		/**
        * From MWPVisitor
        * @param 
        * @return 
        */       
		void VisitLinkL(CWPCharacteristic& aCharacteristic );
    
    private: // From MWPContextExtension
		
		/**
        * From MWPContextExtension
        * @param 
        * @return 
        */    
		const TDesC8& SaveDataL( TInt aIndex ) const;
		
		/**
        * From MWPContextExtension
        * @param 
        * @return 
        */    		
		void DeleteL( const TDesC8& aSaveData );
		
		/**
        * From MWPContextExtension
        * @param 
        * @return 
        */    		
        TUint32 Uid() const;

	private:  // Constructor
        /**
        * C++ default constructor.
        */
		CWPEmailAdapter();

        /**
        * Symbian 2nd phase constructor.
        */
		void ConstructL();

    private:  // New functions

        /**
        * Check are we dealing with email settings
        * @return ETrue if settings accepted
        */
        TBool AcceptedSettingGroup();

        /**
        * Add SMTP IMAP setting pair to iEmailSettingPair array
        * @param aSmtpItem SMTP setting item.
        * @param aImapItem IMAP setting item.
        */
        void AddSmtpImapSettingsL( CWPEmailItem* aSmtpItem, 
                                   CWPEmailItem* aImapItem );

        /**
        * Add SMTP POP setting pair to iEmailSettingPair array
        * @param aSmtpItem SMTP setting item.
        * @param aPopItem POP setting item.
        */
        void AddSmtpPopSettingsL( CWPEmailItem* aSmtpItem, 
                                  CWPEmailItem* aPopItem );

        /**
        * Search from iEmailSettings array a pair for new setting item.
        * If match found creates new setting pair. 
		* @param aItem new setting item
        */
        void CreateSettingsL( CWPEmailItem* aItem );
    
    protected:  // Functions from base classes
        
        /**
        * From MMsvSessionObserver
        */
        void HandleSessionEventL( TMsvSessionEvent aEvent, 
                                  TAny* aArg1, 
                                  TAny* aArg2, 
                                  TAny* aArg3 );

		
	private:  // Data
        
        // Own: Message server session
        CMsvSession* iSession;
        
        /// Own: CommsDataBase
        CCommsDatabase* iCommsDb;
        /**
        * Own: Stores all individual settings
        */
        CArrayPtr<CWPEmailItem>* iEmailSettings;
        
        /** 
        * Own: Stores complete settings pairs
        */
        CArrayPtr<CWPEmailSaveItem>* iEmailSettingPair;
        
        /** 
        * Ref: Current setting item. 
        * Ownership is transferred to Setting array
        **/
        CWPEmailItem* iCurrentItem;

        /**
        * Own: Title text for email settings
        */
        HBufC* iTitle;
	
    private: // For testing usage.
		friend class T_CWPEmailAdapter;
        
	};

#endif	_CWPEMAILADAPTER_H
            
// End of File
