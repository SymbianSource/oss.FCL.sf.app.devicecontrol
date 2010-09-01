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
* Description:  Save mail settings base class 
*
*/



#ifndef CWPEMAILSAVEITEM_H
#define CWPEMAILSAVEITEM_H

//  INCLUDES
#include <e32base.h>
#include <msvapi.h>
#include    <cemailaccounts.h>
#include <MuiuMsvUiServiceUtilities.h>

// FORWARD DECLARATIONS
class CWPEmailItem;
class MWPSaveObserver;
class CDesC16Array;
class CMsvSession;
class CCommsDatabase;

#include <ImumDaSettingsData.h>
#include <ImumDomainApi.h>
#include <ImumDaMailboxServices.h>


//Default values of Retrieval Parameters 
const TInt64 KImumMboxDefaultAutoRetrievalStartHour = 0;
const TInt64 KImumMboxDefaultAutoRetrievalEndHour = 0;
const TInt KImumMboxDefaultAutoRetrieval = TImumDaSettings::EValueAutoOff;

//Boundary values of Retrieval Parameters 
const TInt KImumPollMinUpdateLimit = 5;
const TInt KImumPollMaxUpdateLimit = 360;
const TInt KImumMboxInboxMinUpdateLimit = 0;
const TInt KImumMboxInboxMaxUpdateLimit = 999;
const TInt KRetHourLowerBound = 0;
const TInt KRetHourUpperBound = 23;
const TInt KRetMinLowerBound = 0;
const TInt KRetMinUpperBound = 59;
const TInt KRetHourMinLeadLimit = 10;

//Standard Lengths of Hours, Minutes and Days
const TInt KRetHourStandardLength = 2;
const TInt KRetMinStandardLength = 2;
const TInt KRetDaysStandardMaxLength = 14;
const TInt KRetDayStandardLength = 1;

//Delimiters
const TInt KColonChar = ':';
const TInt KGTChar = '>';
const TInt KCommaChar = ',';


//Retrieval Days Constants
const TInt KRetSunday = 0;
const TInt KRetMonday = 1;
const TInt KRetTuesday = 2;
const TInt KRetWednesday = 3;
const TInt KRetThursday = 4;
const TInt KRetFriday = 5;
const TInt KRetSaturday = 6;

//Poll Values constants
const TInt KPoll5minutes = 5;
const TInt KPoll15minutes = 15;
const TInt KPoll30minutes = 30;
const TInt KPoll1Hour = 1;
const TInt KPoll2Hours = 2;
const TInt KPoll4Hours = 4;
const TInt KPoll6Hours = 6;
            
//Literals for CS parameter parsing
_LIT( KRetSecMilliSec, "00.000000" );
_LIT(KComma, ","); 
_LIT( KRetLeadingZero, "0" );


// CLASS DECLARATION

/**
*  Save settings base class.
*
*  @lib WPEmailAdapter.lib
*  @since 2.0
*/
class CWPEmailSaveItem : public CBase
    {
    public:  // Constructors and destructor
        
        /// Setting ids
        struct TSettingId
            {
            TInt32 iSmtpId;
            TInt32 iPopImapId;
            };
        
        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();
        
        /**
        * constructor.
        */
        CWPEmailSaveItem();
        
        /**
        * Destructor.
        */
        virtual ~CWPEmailSaveItem();

    public: // New functions
        
        /**
        * Save settings
        * Save all settings to system.
        * @param aSession  
        * @since 2.0
        */
        virtual void SaveL( CMsvSession* aSession ) = 0;

        /**
        * Validate Setting content
        * Check that mandatory fields exist and 
        * maximum lengths are not exeeded.
        * @since 2.0
        * @return ETrue if settings are valid.
        */
        virtual TBool ValidateL() = 0;

        /**
        * Set this setting as default.
        * @since 2.0
        */
        virtual void SetAsDefaultL();

        /**
        * Setting name
        * @since 2.0
        * @return default setting name.
        */
        virtual const TDesC& SummaryText();
        
        /**
        * Set new default name
        * default name is used when saving settings to the
        * msgServer. 
        * @param aDefaultName new dafault name.
        */
        void SetDefaultNameL( const TDesC& aDefaultName );

        /**
        * Set reference to CommsDataBase
        * This is needed when saving email settings.
        * Must be set before calling SaveL. 
        * @param aCommsDataBase reference to CommsDB
        */
        void SetCommsDataBase( CCommsDatabase& aCommsDataBase );
        
        /**
        * Get data needed for removing these setting from the system.
        * @return setting IDs of saved settings.
        */
        const TDesC8& SaveData();

    protected:  // New functions
        
        /**
        * Saves given settings to message server
        * @param aSmtp SMTP item
        * @param aPopImap POP or IMAP item
        * @param aIsPopNotImap email POP or IMAP setting item
        * @param aSession message server session 
        * Panic if aSession is NULL,
        */
        void SaveMailSettingsToMessageServerL( CWPEmailItem& aSmtp, 
                                               CWPEmailItem& aPopImap,
                                               TBool aIsPopNotImap,
                                               CMsvSession* aSession );

        /**
        * Valid Email address
        * @param aAddress email address.
        * @return ETrue if address format is valid
        */
        TBool IsValidEmailAddressL(const TDesC& aAddress);

        /**
        * Get valid email address
        * @param aAddress
        * @return Valid email address or empty.
        */
        TPtrC GetValidEmailAddressL(const TDesC& aAddress);

        /**
        * Validate Text
        * @param serverAddress
        * @return KErrGeneral for server address containing only white space characters; KErrNone otherwise.
        */
        TInt ValidateText(const TDesC& serverAddress);
        
        /**
        * Validate Server Address
        * @param serverAddress
        * @return KErrNone for a valid server address; KErrGeneral otherwise.
        */
        TInt ValidateServerAddress(const TDesC& serverAddress);
        
        /**
        * Validate Port No
        * @param portVal
        * @return KErrNone for Port No falls between the valid range; KErrArgument otherwise.
        */
        TInt ValidatePortNo(const TUint& portVal);
      
    private: // New functions

        /**
        * Search existing email settings
        */
        TInt GetMailAccountNamesL();

        /**
        * Fill array with existing setting names.
        * @param aArray array to be filled 
        */
        void ExistingSettingNamesL( CDesC16Array& aArray );
        
        /**
        * Search existing setting names.
        * @param aName setting name.
        * @param aReload ETrue if reload is needed.
        * @return ETrue if setting name already exist.
        */
        TBool SettingNameAlreadyExistsL( const TDesC& aName, TBool aReload );
     
        /**
        * Fill TEntry with SMTP specific data
        * @param TEntry TEntry to be filled.
        * @param aIapName name of the used IAP
        * @param aMailboxName name of the remote mailbox
        */
        void FillTEntryWithSmtpData( TMsvEntry& aTEntry,
                                     const TDesC& aIapName, 
                                     const TDesC& aMailboxName );
            
        /**
        * Fill TEntry with POP or IMAP specific data
        * @param aTEnry TEntry to be filled.
        * @param aSmtpEntry sending host data
        * @param aMailboxName name of the remote mailbox
        * @param aIsPop ETrue to create POP or EFalse to create IMAP entry.
        */
        void FillTEntryWithPopImapData( TMsvEntry& aTEntry,
                                        const TMsvEntry& aSmtpEntry,
                                        const TDesC& aMailboxName,
                                        TBool aIsPop );
        /**
        * Convert TDesC16 to TDesC8
        */
        const TDesC8& Des16To8L(const TDesC& aDes16);
        
        /**
        * Get IAP id from WAP id
        * @param aWapId wap accesspoint id.
        * @return IAP id (panic if not found)
        */
        TUint32 IapIdFromWapIdL(TUint32 aWapId) const;
        
        /**
        * Creates a mail box
        * @param aPopImapItem pop/imap item
        * @param aSmtpItem smtp item
        * @return error
        */        
        TInt CreateMailboxEntryL( CWPEmailItem& aPopImapItem,
            CWPEmailItem& aSmtpItem  );
        
        /**
        * Validates settings name
        * @param None
        * @return None
        */        
        void ValidateSettingsNameL();
        /**
        * 
        * @param aText
        * @return None
        */
        static void IncrementNameL(TDes& aText);
        
        
        /**
        * 
        * @param aText
        * @param aMaxLength
        * @return None
        */
        static void IncrementNameL(TDes& aText, TInt aMaxLength);
        
	/**
	 * Set Email settings application this value is used for 
         * PNPMS service
         */        

	void SetEmailAppIdForPnpServiceL(TDesC& appIDValue);

	 /**
     * Sets Email Notification and Retrieval Parameters
     * @param aEmailSettingsData Email settings
     * @param aPopImapItem Pop/Imap Item
     * @param aPopImapSet Pop/Imap Set
     * @return error
     */
    TInt SetEmailNotificationRetrievalParamsL(
            CImumDaSettingsData& aEmailSettingsData,
            CWPEmailItem& aPopImapItem,
            MImumDaSettingsDataCollection& aPopImapSet);

    /**
     * Sets Integer Parameters like EMN, ONLINE, POLL and LMS
     * @param aEmailSettingsData Email settings
     * @param aPopImapItem Pop/Imap Item
     * @param aParamName Parameter Name
     * @return error
     */
    TInt SetIntParam(CImumDaSettingsData& aEmailSettingsData,
            CWPEmailItem& aPopImapItem, const TDesC& aParamName);

    /**
     * Gets the value of the Parameter in Integer form
     * @param aPopImapItem Pop/Imap Item
     * @param aParamID Parameter ID
     * @param aVal Value of the Parameter
     * @return error
     */
    TInt GetParamValue(CWPEmailItem& aPopImapItem, const TInt aParamID,
            TInt& aVal);

    /**
     * Gets the value of the Parameter in Integer form
     * @param aPopImapItem Pop/Imap Item
     * @param aParamName Parameter Name
     * @param aVal Value of the Parameter
     * @return error
     * */
    TInt GetParamValue(CWPEmailItem& aPopImapItem, const TDesC& aParamName,
            TInt& aVal);

    /**
     * Gets the value of the Parameters (Retrieval Hours and Days) 
     * from CS in String form
     * @param aCSParamValue Value of CS Parameter
     * @param aTokenMark Demarcating Character
     * @param aParm Value of the Parameter
     * @return True if Parameter value is properly Marked
     * */
    TBool GetRetParamValuesFromCS(TLex& aCSParamValue, const TInt aTokenMark,
            TPtrC& aParm);

    /**
     * Gets the value of Retrieval Hours parameter from CS in Integer form
     * @param aCSParamValue Value of CS Parameter
     * @param aTokenMark Demarcating Character
     * @param aRetHourMinValue Value of Retrieval Hours
     * @return error
     * */
    TInt GetRetHoursValueL(TLex& aCSParamValue, TInt aTokenMark,
            TInt64& aRetHourMinValue);

    /**
     * Sets the value of Retrieval Days parameter
     * @param aCSParamValue Value of CS Parameter
     * @param aEmailSettingsData Email settings
     * @return error
     * */
    TInt SetRetDaysL(TLex& aCSParamValue,
            CImumDaSettingsData& aEmailSettingsData);
    /**
     * Sets the value of Poll parameter
     * @param aEmailSettingsData Email settings
     * aPollValue Value of Poll parameter
     * @return error
     * */
    TInt SetPollValue(CImumDaSettingsData& aEmailSettingsData,
            TInt aPollValue);
    /**
     * Validates User Name and Password
     * @param aPopImapSet Pop/Imap Set
     * @return error
     */
    TInt ValidateUserNamePassword(MImumDaSettingsDataCollection& aPopImapSet, TBool& aValidUserPass);
    
    /**
     * Changes the Absolute Integer Hour/Min value to a string 
     * @param aRetHourMin Retrieval Hour/Min string got from CS parameter
     * @param aValRetHourMin Retrieval Hour/Min Value
     * @param aPtrRetHourMin Retrieval Hour/Min string got from Integer value of Hour/Min
     * @return void
     */
    void GetRetHourorMinfromIntValue(TPtrC& aRetHourMin,
            TInt64 aValRetHourMin, TPtr& aPtrRetHourMin);
    
    /**
     * Validates Retrieval Hours and Minutes
     * @param aRetHourMin Lexer of Retrieval Hours/Minutes
     * @return error
     */
    TInt ValidateRetHourorMin(TLex& aRetHourMin);
	

    private:    // Data

        /// Ref: Message server session
        CMsvSession* iSession;
        /// Ref: CommsDataBase
        CCommsDatabase* iCommDb;
        /// Own: array of names of existing mail settings.
        CDesC16Array* iExistingSettings;
        /// Own: for converting 16 to 8 bit descriptor
        HBufC8* iBufConvert;
        //Own default name for email settings.
        HBufC* iDefaultName;
        //Own: IAP name
        HBufC*                  iIapName;
        //Created smtp entry
        TMsvEntry               iSmtpTEntry;
        //Created POP/IMAP entry
        TMsvEntry               iPopImapTEntry;
        TPckgBuf<TSettingId>    iUid;
        // Etrue if POP mail
        TBool                   iPopMail;
        
        TMsvId iDefaultMsvId;
        
        CImumDomainApi* iDomainApi;
        MImumDaMailboxServices* iServices;

             
    };

#endif      // CWPEMAILSAVEITEM_H   
            
// End of File
