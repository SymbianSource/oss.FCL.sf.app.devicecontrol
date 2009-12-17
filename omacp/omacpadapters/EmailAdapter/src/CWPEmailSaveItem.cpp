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
* Description:  Saves CWPEmailItems
*
*/



// INCLUDE FILES
#include    "CWPEmailSaveItem.h"
#include    "CWPEmailItem.h"
#include    "EmailAdapter.pan"
#include    "WPEmail.hrh"

#include    <wpemailadapterresource.rsg>
#include    <WPAdapterUtil.h>   // Adapter utils

#include    <ApUtils.h>
#include    <ApDataHandler.h>

#include	<CWPParameter.h>
#include    <txtrich.h>
#include    <iapprefs.h>
#include    <MuiuMsvUiServiceUtilities.h>

#include    <pop3set.h>

#include    <miutpars.h>        // TImMessageField
#include    <cemailaccounts.h>
#include    <iapprefs.h>

#include    <ImumDaMailboxUtilities.h>
#include    <SendUiConsts.h>

#include <ImumDaSettingsKeys.h>
#include <ImumDaSettingsDataCollection.h>

#include    "../../../group/omacplogger.h"
#include    <PnpProvUtil.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::CWPEmailSaveItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPEmailSaveItem::CWPEmailSaveItem()
    {
    }

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPEmailSaveItem::ConstructL()
    { 
    iExistingSettings = new (ELeave) CDesC16ArrayFlat(2); //1 or 2 settings
        
    TFileName fileName;
    Dll::FileName( fileName );
    iDefaultName = WPAdapterUtil::ReadHBufCL( fileName,
                                              KAdapterName, 
                                              R_QTN_SM_DEFAULT_EMAIL_NAME );    
    iDefaultMsvId =  0 ;
    iDomainApi = CreateDomainApiL(); 
    iServices = &iDomainApi->MailboxServicesL(); 
    }
    
// -----------------------------------------------------------------------------    
// Destructor
// -----------------------------------------------------------------------------
CWPEmailSaveItem::~CWPEmailSaveItem()
    {
    delete iBufConvert;
    delete iExistingSettings;
    delete iDefaultName;
    delete iIapName;
    delete iDomainApi;
    }

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::SetAsDefaultL
// -----------------------------------------------------------------------------
//
void CWPEmailSaveItem::SetAsDefaultL()
    {
     iServices->SetDefaultMailboxL( iDefaultMsvId );
    }
    
// -----------------------------------------------------------------------------
// CWPEmailSaveItem::SummaryText
// -----------------------------------------------------------------------------
//
const TDesC& CWPEmailSaveItem::SummaryText()
    {    
    return *iDefaultName;
    }

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::SetDefaultNameL
// -----------------------------------------------------------------------------
//
void CWPEmailSaveItem::SetDefaultNameL( const TDesC& aDefaultName )
    {
    HBufC* temp = aDefaultName.AllocL();
    delete iDefaultName;
    iDefaultName = temp;
    }
    
// -----------------------------------------------------------------------------
// CWPEmailSaveItem::SetCommsDataBase
// -----------------------------------------------------------------------------
//
void CWPEmailSaveItem::SetCommsDataBase( CCommsDatabase& aCommsDataBase )
    {
    iCommDb = &aCommsDataBase;
    }

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::SaveData
// -----------------------------------------------------------------------------
//
const TDesC8& CWPEmailSaveItem::SaveData()
    {
    iUid().iSmtpId = iSmtpTEntry.Id();
    iUid().iPopImapId = iPopImapTEntry.Id();
    return iUid;
    }

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::GetMailAccountNamesL
// -----------------------------------------------------------------------------
//
TInt CWPEmailSaveItem::GetMailAccountNamesL()
    {
    CMsvEntry* centry = iSession->GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL(centry);
    TInt count = centry->Count();

    for ( TInt j = 0; j < count; j++ )
        {
        if( (*centry)[j].iType == KUidMsvServiceEntry &&
            ((*centry)[j].iMtm == KUidMsgTypePOP3  ||
            (*centry)[j].iMtm == KUidMsgTypeIMAP4))
            {
            iExistingSettings->AppendL( (*centry)[j].iDetails );
            }
        }
    CleanupStack::PopAndDestroy( centry );
    return iExistingSettings->Count();
    }

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::SettingNameAlreadyExistsL
// -----------------------------------------------------------------------------
//
TBool CWPEmailSaveItem::SettingNameAlreadyExistsL( const TDesC& aName, 
                                                   TBool aReload )
    {
    if ( aReload )
        {
        GetMailAccountNamesL();
        }
    TInt count = iExistingSettings->Count();

    for ( TInt n(0); n < count; n++ )
        {
        if (aName.CompareF( (*iExistingSettings)[n]) == 0 )
            {
            // found match
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::SaveMailSettingsToMessageServerL
// -----------------------------------------------------------------------------
//
void CWPEmailSaveItem::SaveMailSettingsToMessageServerL( CWPEmailItem& aSmtp,
                                                         CWPEmailItem& aPopImap,
                                                         TBool aIsPopNotImap,
                                                         CMsvSession* aSession )
    {
    __ASSERT_ALWAYS( aSession, Panic( EMailAdapterNullPointer ) );
    
    iSession = aSession;
    iPopMail = aIsPopNotImap;
    
    ValidateSettingsNameL();

    delete iIapName;
    iIapName = NULL;
    iIapName = aSmtp.AssociatedIapNameL();
    if ( !iIapName )
        {
        iIapName = iDefaultName->Des().AllocL();
        }

    CreateMailboxEntryL( aPopImap,aSmtp ); 
    }

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::IsValidEmailAddressL
// -----------------------------------------------------------------------------
//
TBool CWPEmailSaveItem::IsValidEmailAddressL( const TDesC& aAddress )
    {
    TPtrC string = GetValidEmailAddressL( aAddress );
    TImMessageField address;
    return address.ValidInternetEmailAddress( string );    
    }

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::GetValidEmailAddressL
// -----------------------------------------------------------------------------
//
TPtrC CWPEmailSaveItem::GetValidEmailAddressL( const TDesC& aAddress )
    {
    TImMessageField address;
    return address.GetValidInternetEmailAddressFromString( aAddress );    
    }

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::FillTEntryWithSmtpData
// -----------------------------------------------------------------------------
//
void CWPEmailSaveItem::FillTEntryWithSmtpData( TMsvEntry& aTEntry,
                                               const TDesC& aIapName, 
                                               const TDesC& aMailboxName )
    {
    aTEntry.iMtm = KUidMsgTypeSMTP;
    aTEntry.iType= KUidMsvServiceEntry;
    aTEntry.iDate.HomeTime();
    aTEntry.iDescription.Set( aIapName );
    aTEntry.iDetails.Set( aMailboxName );
    aTEntry.SetVisible( EFalse );
    }

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::FillTEntryWithPopImapData
// -----------------------------------------------------------------------------
//
void CWPEmailSaveItem::FillTEntryWithPopImapData( TMsvEntry& aTEntry,
                                                  const TMsvEntry& aSmtpEntry, 
                                                  const TDesC& aMailboxName, 
                                                  TBool aIsPop )
    {
    aTEntry = aSmtpEntry;
    aTEntry.iRelatedId = aSmtpEntry.Id();
    aTEntry.SetVisible( ETrue );
    aTEntry.iType = KUidMsvServiceEntry;
    aTEntry.iDate.HomeTime();
    aTEntry.iDetails.Set( aMailboxName );
    if ( aIsPop )
        {
        aTEntry.iMtm = KUidMsgTypePOP3;
        }
    else
        {
        aTEntry.iMtm = KUidMsgTypeIMAP4;
        }
    }

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::Des16To8L
// -----------------------------------------------------------------------------
//
const TDesC8& CWPEmailSaveItem::Des16To8L( const TDesC& aDes16 )
    {
    HBufC8* text = HBufC8::NewL(aDes16.Length()); 
    delete iBufConvert;
    iBufConvert = text;
    iBufConvert->Des().Copy(aDes16);
    return *iBufConvert;
    }

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::IapIdFromWapIdL
// -----------------------------------------------------------------------------
//
TUint32 CWPEmailSaveItem::IapIdFromWapIdL( TUint32 aWapId ) const
    {
    CApUtils* apUtils = CApUtils::NewLC( *iCommDb );
    TUint32 iapId = NULL;
    iapId = apUtils->IapIdFromWapIdL( aWapId );
    __ASSERT_ALWAYS( iapId, Panic( EMailAdapterInvalidId ) );
    CleanupStack::PopAndDestroy( apUtils );
    return iapId;
    }

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::ValidateSettingsNameL
// -----------------------------------------------------------------------------
//
void CWPEmailSaveItem::ValidateSettingsNameL( )
    {
    LOGSTRING( "CWPEmailSaveItem::ValidateSettingsNameL : Enter" );
    if ( SettingNameAlreadyExistsL( *iDefaultName, ETrue ) )
       {
        
        //before appending (XX) to name if >26, make sure length is not equal
        //to KSettingsNameMaxLength
        
        
        if(iDefaultName->Length() > 26)	
        {
        	TBuf<30> temp_Name (*iDefaultName);
    		
    		TInt count = iExistingSettings->Count();

    		for ( TInt n(0); n < count; n++ )
        	{
        		if (temp_Name.CompareF( (*iExistingSettings)[n]) == 0 )
            		{
            		// found match
            		User::Leave(KErrAlreadyExists);
            		}
        	}
    		
        }
        
        HBufC* newName = HBufC::NewLC( KSettingsNameMaxLength );
        *newName = *iDefaultName;
        
        TPtr namePtr = newName->Des();
        IncrementNameL( namePtr );
        
        while ( SettingNameAlreadyExistsL( namePtr, EFalse ) )
            {
           IncrementNameL( namePtr );
            }
        
        delete iDefaultName;
        iDefaultName = NULL;
        iDefaultName = namePtr.AllocL();
        CleanupStack::PopAndDestroy( newName );
        }
     LOGSTRING( "CWPEmailSaveItem::ValidateSettingsNameL : Exit" );
    }

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::CreateMailboxEntryL
// -----------------------------------------------------------------------------
//
TInt CWPEmailSaveItem::CreateMailboxEntryL( CWPEmailItem& aPopImapItem,
    CWPEmailItem& aSmtpItem )
    {
        LOGSTRING( "CWPEmailSaveItem::CreateMailboxEntryL : Entry" );

    TInt err = KErrNone;
	TBuf<10> appIdValue;
    
	CImumDaSettingsData* eMailSettingsData(NULL);
    eMailSettingsData = iServices->CreateSettingsDataL(KNullUid);
    CleanupStack::PushL(eMailSettingsData); //1

    CImIAPPreferences* apSet = CImIAPPreferences::NewLC(); //2
    CImIAPPreferences* popImapIapSet = CImIAPPreferences::NewLC(); //3

    TPtrC emailAddress = GetValidEmailAddressL(aSmtpItem.ParameterValue(
            EWPParameterFrom));
    TImumDaSettings::TTextEmailAddress emailAddressText;
    emailAddressText.Append(emailAddress);

    err = eMailSettingsData->SetAttr(TImumDaSettings::EKeyEmailAddress,
            emailAddressText);
    User::LeaveIfError(err);

    TImumDaSettings::TTextMailboxName mailBoxName(*iDefaultName);
    err = eMailSettingsData->SetAttr(TImumDaSettings::EKeyMailboxName,
            mailBoxName);

    // set IMAP/POP specific settings, create the account and set as default     
    if ( iPopMail ) // POP3
        {
        // Add Pop set
        MImumDaSettingsDataCollection& pop3Set = eMailSettingsData->AddSetL(
                KSenduiMtmPop3Uid);


        // set access point preferences
        TImIAPChoice popIapChoice;
        popIapChoice.iDialogPref = ECommDbDialogPrefDoNotPrompt;
        popIapChoice.iIAP = 0;
        // We get Wap AP Uid from AP adapter, so it needs to be converted here. 
        // If correct IAP is not found try to use default IAP from CommsDB
        TRAPD( error, popIapChoice.iIAP = IapIdFromWapIdL( aPopImapItem.AssociatedIapId() ) );
        if ( error == KErrNone )
            {        
            popImapIapSet->AddIAPL(popIapChoice);
            }
        else if ( error == KErrNotFound )
            {
            popIapChoice.iIAP = 0;
            popIapChoice.iDialogPref = ECommDbDialogPrefPrompt;
            popImapIapSet->AddIAPL( popIapChoice );
            }
        else 
            {
            User::Leave( error );
            }

        //Set Access Point
        err = pop3Set.SetAttr(TImumDaSettings::EKeyAccessPoint, TInt(
                popImapIapSet->IAPPreference(0).iIAP));
        User::LeaveIfError(err);

        TImumDaSettings::TTextServerAddress serverAddress;
        serverAddress.Append(aPopImapItem.ParameterValue(EWPParameterAddr));

        err = pop3Set.SetAttr(TImumDaSettings::EKeyServer, serverAddress);
        User::LeaveIfError(err);

        TImumDaSettings::TTextUserName userName(Des16To8L(
                aPopImapItem.ParameterValue(EWPParameterAAuthName)));
        TImumDaSettings::TTextPassword passWord(Des16To8L(
                aPopImapItem.ParameterValue(EWPParameterAAuthSecret)));

        //set application ID
        const TDesC& appID = aPopImapItem.ParameterValue(EWPParameterAppID);
        appIdValue.Copy(appID);

        if (userName.Length() > 0)
            {
            err = pop3Set.SetAttr(TImumDaSettings::EKeyUsername, userName);
            }
        if (passWord.Length() > 0)
            {
            err = pop3Set.SetAttr(TImumDaSettings::EKeyPassword, passWord);
            }

        //Set Port Value
        TInt portVal;
        if (GetParamValue(aPopImapItem, EWPParameterPortNbr, portVal)
                == KErrNone)
            {
            err = pop3Set.SetAttr(TImumDaSettings::EKeyPort, portVal);
            }

        // Set Service parameter values [StartTLS, SSL/TLS, Normal]
        const TDesC& popSecure = aPopImapItem.ParameterValue(
                EWPParameterService);

        if (popSecure.Compare(KSecureString) == 0)
            {
            err = pop3Set.SetAttr(TImumDaSettings::EKeySecurity,
                    TImumDaSettings::EValueSecurityTls);
            }
        else if (popSecure.Compare(KEmailSettingSecurePortPOP) == 0)
            {
            err = pop3Set.SetAttr(TImumDaSettings::EKeySecurity,
                    TImumDaSettings::EValueSecuritySsl);
            if (!err)
                {
                err = pop3Set.SetAttr(TImumDaSettings::EKeyPort,
                        KEmailSslWrappedPop3Port);
                }
            }
        else
            {
            err = pop3Set.SetAttr(TImumDaSettings::EKeySecurity,
                    TImumDaSettings::EValueSecurityOff);
            }
        
        err = SetEmailNotificationRetrievalParamsL(*eMailSettingsData,
                aPopImapItem, pop3Set);

        }
    else // IMAP4
        {
        // Add Imap set
        MImumDaSettingsDataCollection& imap4Set = eMailSettingsData->AddSetL(
                KSenduiMtmImap4Uid);
        // set access point preferences
        TImIAPChoice imapIapChoice;
        imapIapChoice.iDialogPref = ECommDbDialogPrefDoNotPrompt;
        imapIapChoice.iIAP = 0;
        // We get Wap AP Uid from AP adapter, so it needs to be converted here. 
        // If correct IAP is not found try to use default IAP from CommsDB
        TRAPD( error, imapIapChoice.iIAP = IapIdFromWapIdL( aPopImapItem.AssociatedIapId() ) );
        if ( error == KErrNone )
            {        
            popImapIapSet->AddIAPL(imapIapChoice);
            }
        else if ( error == KErrNotFound )
            {
            imapIapChoice.iIAP = 0;
            imapIapChoice.iDialogPref = ECommDbDialogPrefPrompt;
            popImapIapSet->AddIAPL( imapIapChoice );
            }
        else 
            {
            User::Leave( error );
            }

        //Set Access Point
        err = imap4Set.SetAttr(TImumDaSettings::EKeyAccessPoint, TInt(
                popImapIapSet->IAPPreference(0).iIAP));
        User::LeaveIfError(err);

        // Set incoming mail server
        TImumDaSettings::TTextServerAddress serverAddress;
        serverAddress.Append(aPopImapItem.ParameterValue(EWPParameterAddr));
        err = imap4Set.SetAttr(TImumDaSettings::EKeyServer, serverAddress);
        User::LeaveIfError(err);

        TImumDaSettings::TTextUserName userName(Des16To8L(
                aPopImapItem.ParameterValue(EWPParameterAAuthName)));
        TImumDaSettings::TTextPassword passWord(Des16To8L(
                aPopImapItem.ParameterValue(EWPParameterAAuthSecret)));
        
		//set application ID
        const TDesC& appID = aPopImapItem.ParameterValue( EWPParameterAppID );
        appIdValue.Copy(appID); 
       
        if (userName.Length() > 0)
            {
            err = imap4Set.SetAttr(TImumDaSettings::EKeyUsername, userName);
            }
        if (passWord.Length() > 0)
            {
            err = imap4Set.SetAttr(TImumDaSettings::EKeyPassword, passWord);
            }

        //Set Port Value
        TInt portVal;
        if (GetParamValue(aPopImapItem, EWPParameterPortNbr, portVal)
                == KErrNone)
            {
            err = imap4Set.SetAttr(TImumDaSettings::EKeyPort, portVal);
            }

        // Set Service parameter values [StartTLS, SSL/TLS, Normal]
        const TDesC& imapSecure = aPopImapItem.ParameterValue(
                EWPParameterService);
        if (imapSecure.Compare(KSecureString) == 0)
            {
            err = imap4Set.SetAttr(TImumDaSettings::EKeySecurity,
                    TImumDaSettings::EValueSecurityTls);
            }
        else if (imapSecure.Compare(KEmailSettingSecurePortIMAP) == 0)
            {
            err = imap4Set.SetAttr(TImumDaSettings::EKeySecurity,
                    TImumDaSettings::EValueSecuritySsl);
            if (!err)
                {
                err = imap4Set.SetAttr(TImumDaSettings::EKeyPort,
                        KEmailSslWrappedImap4Port);
                }
            }
        else
            {
            err = imap4Set.SetAttr(TImumDaSettings::EKeySecurity,
                    TImumDaSettings::EValueSecurityOff);
            }

        err = SetEmailNotificationRetrievalParamsL(*eMailSettingsData,
                aPopImapItem, imap4Set);

        }

    // Add smtp set
    MImumDaSettingsDataCollection& smtpSet = eMailSettingsData->AddSetL(
            KSenduiMtmSmtpUid);

    // set access point preferences
    TImIAPChoice smtpIapChoice;
    smtpIapChoice.iDialogPref = ECommDbDialogPrefDoNotPrompt;
    smtpIapChoice.iIAP = 0;
        // We get Wap AP Uid from AP adapter, so it needs to be converted here. 
        // If correct IAP is not found try to use default IAP from CommsDB
        LOGSTRING( "CWPEmailSaveItem::IapIdFromWapIdL : Entry" );
    TRAPD( error, smtpIapChoice.iIAP = IapIdFromWapIdL( aSmtpItem.AssociatedIapId() ) );
    if (error == KErrNone)
        {
        apSet->AddIAPL(smtpIapChoice);
        }
    else if (error == KErrNotFound)
        {
        smtpIapChoice.iIAP = 0;
        smtpIapChoice.iDialogPref = ECommDbDialogPrefPrompt;
        apSet->AddIAPL(smtpIapChoice);
        }
    else
        {
        User::Leave(error);
        }
        LOGSTRING( "CWPEmailSaveItem::IapIdFromWapIdL : Exit" );

    //Set Access Point
    err = smtpSet.SetAttr(TImumDaSettings::EKeyAccessPoint, TInt(
            apSet->IAPPreference(0).iIAP));
    User::LeaveIfError(err);

    // Set outgoing server address
    TImumDaSettings::TTextServerAddress serverAddress;
    serverAddress.Append(aSmtpItem.ParameterValue(EWPParameterAddr));
    err = smtpSet.SetAttr(TImumDaSettings::EKeyServer, serverAddress);
    User::LeaveIfError(err);

    TInt val;
    // set port number
    if (GetParamValue(aSmtpItem, EWPParameterPortNbr, val) == KErrNone)
        {
        err = smtpSet.SetAttr(TImumDaSettings::EKeyPort, val);
        }

    // Set Service parameter values [StartTLS, SSL/TLS, Normal]
    const TDesC& secure = aSmtpItem.ParameterValue(EWPParameterService);

    if (secure.Compare(KSecureString) == 0)
        {
        err = smtpSet.SetAttr(TImumDaSettings::EKeySecurity,
                TImumDaSettings::EValueSecurityTls);
        }
    else if (secure.Compare(KEmailSettingSecurePortSMTP) == 0)
        {
        err = smtpSet.SetAttr(TImumDaSettings::EKeySecurity,
                TImumDaSettings::EValueSecuritySsl);
        if (!err)
            {
            smtpSet.SetAttr(TImumDaSettings::EKeyPort,
                    KEmailSslWrappedSmtpPort);
            }
        }
    else
        {
        err = smtpSet.SetAttr(TImumDaSettings::EKeySecurity,
                TImumDaSettings::EValueSecurityOff);
        }

        LOGSTRING( "Des16To8L : Entry" );
    TImumDaSettings::TTextUserName smtpUserName(Des16To8L(
            aSmtpItem.ParameterValue(EWPParameterAAuthName)));
        LOGSTRING( "Des16To8L : Exit" );
        LOGSTRING( "Des16To8L : Entry" );
    TImumDaSettings::TTextPassword smtpPassWord(Des16To8L(
            aSmtpItem.ParameterValue(EWPParameterAAuthSecret)));
        LOGSTRING( "Des16To8L : Exit" );

    if (smtpUserName.Length() > 0)
        {
        err = smtpSet.SetAttr(TImumDaSettings::EKeyUsername, smtpUserName);
        }
    if (smtpPassWord.Length() > 0)
        {
        err = smtpSet.SetAttr(TImumDaSettings::EKeyPassword, smtpPassWord);
        }

    //Set User Preferences
    err = smtpSet.SetAttr(TImumDaSettings::EKeyReplyToAddress,
            emailAddressText);

        LOGSTRING( "Emailservices::CreateMailboxL : Entry" );
    iDefaultMsvId = iServices->CreateMailboxL(*eMailSettingsData);
        LOGSTRING( "Emailservices::CreateMailboxL : Exit" );

    SetEmailAppIdForPnpServiceL(appIdValue);
    
    
    CleanupStack::PopAndDestroy(3);
        LOGSTRING( "CWPEmailSaveItem::CreateMailboxEntryL : Exit" );

    return err;
    }
void CWPEmailSaveItem::SetEmailAppIdForPnpServiceL(TDesC& appIDValue)
{
	
	

	//if PnPMS service launched by Email client for email settings download
	//then set application ID of mail box into Cenrep.
	CPnpProvUtil* prov = CPnpProvUtil::NewLC();
	// use contant value
	const TUint32 uidVal = 0x100058C5;
	TUint32 uidValCenrep = prov->GetApplicationUidL();
	
	if(uidVal == uidValCenrep)
	prov->SetProvAdapterAppIdL(appIDValue); 
	
	CleanupStack::PopAndDestroy();

	
}


void CWPEmailSaveItem::IncrementNameL(TDes& aName)
    {
    LOGSTRING( "CWPEmailSaveItem::IncrementNameL: Enter" );
    
    TInt length = aName.Length();

    // If this fails it means that at least default name should have been assigned
    // to item.
    //__ASSERT_ALWAYS(length > 0, Panic(EMEUNameEmpty));
    
	    	
    TInt newOrdinal = 0;
    TInt index = length-1;
    TInt charValue = aName[index];

    // Check if name ends with ')'
    if (charValue == ')')
        {
        TBool cutBetweenIndexes = EFalse;
        index--;
        TInt multi = 1;
        while ( index >= 0)
            {
            charValue = aName[index];
            TChar ch(charValue);

            if (!ch.IsDigit())
                {
                // There was non-digits inside the brackets.
                if ( charValue == '(')
                    {
                    // We found the start
                    cutBetweenIndexes = ETrue;
                    index--;
                    break;
                    }
                else
                    {
                    // There was a non-digit inside the '(' and ')'
                    // We just append after the original name.
                    break;
                    }
                } // if
            else
                {
                TInt newNum = ch.GetNumericValue();
                if (multi <= 100000000)
                    {
                    // If there's a pattern (007) then this makes it to be (007)(01)
                    if (newNum == 0 && newOrdinal != 0 && multi > 10)
                        {
                        break;
                        }
                    newOrdinal += newNum * multi;
                    multi*=10;
                    }//0010
                else
                    {
                    newOrdinal = 0;
                    break;
                    }
                }
            index--;
            } // while

        // There was correct pattern of (0000) so now we delete it.
        if (cutBetweenIndexes)
            {
            aName.Delete(index+1, length-index);
            }
        else
            {
            // This case is for example 12345) so we get 12345)(01)
            newOrdinal = 0;
            }
        } // if

    // Add one to the ordinal read from the old name
    newOrdinal++;

    // Check that there's enough space to add the ordinal
    TInt maxLength = aName.MaxLength();
    TInt ordinalSpace(4); // Ordinal space comes from (00) so it's 4
    length = aName.Length();
    if (length + ordinalSpace > maxLength)
        {
        aName.Delete(maxLength-ordinalSpace, ordinalSpace);
        }

    // Append the ordinal at the end of the name
    HBufC* old = aName.AllocLC();

    _LIT(KFormatSpecLessTen, "%S(0%d)");
    _LIT(KFormatSpecOverTen, "%S(%d)");
    if (newOrdinal < 10)
        {
        aName.Format(KFormatSpecLessTen, old, newOrdinal);
        }
    else
        {
        aName.Format(KFormatSpecOverTen, old, newOrdinal);
        }
    CleanupStack::PopAndDestroy(old); // old
    
    LOGSTRING( "CWPEmailSaveItem::IncrementNameL: Exit" );
    }


// -----------------------------------------------------------------------------
// CWPEmailSaveItem::IncrementNameL
// -----------------------------------------------------------------------------
//
void CWPEmailSaveItem::IncrementNameL(TDes& aName, TInt aMaxLength)
    {
    LOGSTRING( "CWPEmailSaveItem::IncrementNameL: Enter" );
    
    TInt length = aName.Length();

    // If this fails it means that at least default name should have been assigned
    // to item.
    //__ASSERT_ALWAYS(length > 0, Panic(EMEUNameEmpty));
    
    TInt newOrdinal = 0;
    TInt index = length-1;
    TInt charValue = aName[index];

    // Check if name ends with ')'
    if (charValue == ')')
        {
        TBool cutBetweenIndexes = EFalse;
        index--;
        TInt multi = 1;
        while ( index >= 0)
            {
            charValue = aName[index];
            TChar ch(charValue);

            if (!ch.IsDigit())
                {
                // There was non-digits inside the brackets.
                if ( charValue == '(')
                    {
                    // We found the start
                    cutBetweenIndexes = ETrue;
                    index--;
                    break;
                    }
                else
                    {
                    // There was a non-digit inside the '(' and ')'
                    // We just append after the original name.
                    break;
                    }
                } // if
            else
                {
                TInt newNum = ch.GetNumericValue();
                if (multi <= 100000000)
                    {
                    // If there's a pattern (007) then this makes it to be (007)(01)
                    if (newNum == 0 && newOrdinal != 0 && multi > 10)
                        {
                        break;
                        }
                    newOrdinal += newNum * multi;
                    multi*=10;
                    }//0010
                else
                    {
                    newOrdinal = 0;
                    break;
                    }
                }
            index--;
            } // while

        // There was correct pattern of (0000) so now we delete it.
        if (cutBetweenIndexes)
            {
            aName.Delete(index+1, length-index);
            }
        else
            {
            // This case is for example 12345) so we get 12345)(01)
            newOrdinal = 0;
            }
        } // if

    // Add one to the ordinal read from the old name
    newOrdinal++;

    // Check that there's enough space to add the ordinal
    TInt maxLength = aMaxLength;
    TInt ordinalSpace(4); // Ordinal space comes from (00) so it's 4
    length = aName.Length();
    if (length + ordinalSpace > maxLength)
        {
        aName.Delete(maxLength-ordinalSpace, ordinalSpace);
        }

    // Append the ordinal at the end of the name
    HBufC* old = aName.AllocLC();

    _LIT(KFormatSpecLessTen, "%S(0%d)");
    _LIT(KFormatSpecOverTen, "%S(%d)");
    if (newOrdinal < 10)
        {
        aName.Format(KFormatSpecLessTen, old, newOrdinal);
        }
    else
        {
        aName.Format(KFormatSpecOverTen, old, newOrdinal);
        }
    CleanupStack::PopAndDestroy(old); // old
    
    LOGSTRING( "CWPEmailSaveItem::IncrementNameL: Exit" );
    }

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::ValidateText
// -----------------------------------------------------------------------------
//
TInt CWPEmailSaveItem::ValidateText(const TDesC& serverAddress)
{
	
	// Check if the name of the mailbox contains only 
    // white space characters.
    TInt length = serverAddress.Length();
    	
		
    for( TInt i = 0 ; i < length ; i++ )
    	{
    		TChar tempChar(serverAddress[i]);
    		if( !tempChar.IsSpace() )
    			{
    			// If the character isn't a white space character 
    			// then KErrNone is returned.
    			return KErrNone;
    			}
    	}
    return KErrGeneral;
    
}


// -----------------------------------------------------------------------------
// CWPEmailSaveItem::ValidateServerAddress
// -----------------------------------------------------------------------------
//
TInt CWPEmailSaveItem::ValidateServerAddress(const TDesC& serverAddress)
{
	
	TInt result = ValidateText(serverAddress);

    // Check for valid server address
    if ( result == KErrNone )
        {
        TBool ok = EFalse;
        TRAPD( error, ok = MsvUiServiceUtilities::IsValidDomainL(serverAddress) );

        result = ( error == KErrNone ) && ok ? KErrNone : KErrGeneral;
        }

    return result;
    
}

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::ValidatePortNo
// -----------------------------------------------------------------------------
//
TInt CWPEmailSaveItem::ValidatePortNo(const TUint& portVal)
{
	TInt result = (portVal >= KEmailPortMinValue) && (portVal <= KEmailPortMaxValue) ? KErrNone : KErrArgument;
	return result;	
}

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::SetEmailNotificationRetrievalParamsL
// -----------------------------------------------------------------------------
//
TInt CWPEmailSaveItem::SetEmailNotificationRetrievalParamsL(
        CImumDaSettingsData& aEmailSettingsData, CWPEmailItem& aPopImapItem,
        MImumDaSettingsDataCollection& aPopImapSet)
    {
        LOGSTRING( "CWPEmailSaveItem::SetEmailNotificationRetrievalParamsL : Entry" );
    TInt err = KErrGeneral;

    //Set EMN
    /*
     Characteristic/name: APPLICATION/EMN
     Status: Optional.
     Occurs: 0/1.
     Default value: 0.
     Used values: 0, 1, 2.
     OFF = 0.
     ON = 1.
     VALID = 2.
     Interpretation:
     Specifies the usage of OMA EMN. If the value is OFF (0), OMA EMN
     is disabled. If the value is ON (1) OMA EMN is enabled. If the value
     is VALID (2) OMA EMN is enabled with proxy whitelist validation.
     This means that OMA EMN messages are accepted only from
     trusted push proxy gateways. Whitelist validation is made
     based on Push Validation document available in Forum Nokia.
     */

    const TDesC& emnParamName(KParamEMN);

    TBool validUserPass = EFalse;
    // Check whether Username and Password are there in the incoming settings

    err = ValidateUserNamePassword(aPopImapSet, validUserPass);
    if (!err && validUserPass)
        {
        err = SetIntParam(aEmailSettingsData, aPopImapItem, emnParamName);
        }

    TInt emnValSet;
    TInt err1;
    err1 = aEmailSettingsData.GetAttr(TImumDaSettings::EKeyAutoNotifications,
            emnValSet);

    if (((!err && !err1 && !emnValSet) || err) && validUserPass)
        {
        const TDesC& onlineParamName(KParamONLINE);
        // Check whether Username and Password are there in the incoming settings

        TInt onlineValRec;
        err = GetParamValue(aPopImapItem, onlineParamName, onlineValRec);
        if (emnValSet && !err && onlineValRec)
            {
            //Set EMN to Off before setting Online
            err = aEmailSettingsData.SetAttr(
                    TImumDaSettings::EKeyAutoNotifications,
                    TImumDaSettings::EValueNotificationsOff);
            }
        //Set Online
        /*
         Characteristic/name: APPLICATION/ONLINE
         Status: Optional.
         Occurs: 0/1. 
         Default value: 0.
         Used values: 0 or 1.
         OFF = 0.
         ON = 1.
         Interpretation: 
         Specifies the online mode. Online mode works with APPLICATION/POLL and
         APPLICATION/CS parameters in following way:
         Connection shedule parameter specifies time windows when always online mode
         is used. Polling can be used to shorter polling interval of IMAP IDLE. This
         is useful option when network/NAT drops connection after short inactivity
         */
        if (!err)
            {
            err = SetIntParam(aEmailSettingsData, aPopImapItem,
                    onlineParamName);
            }

        }

    const TDesC& csParamName(KParamCS);
    const TDesC& csParamVal(aPopImapItem.ParameterValue(csParamName));

    if (csParamVal.Length())
        {
            LOGSTRING2( "[Provisioning] CWPEmailSaveItem::SetEmailNotificationRetrievalParamsL: csParamVal value %i", csParamVal );

        TLex csParamValue(csParamVal);
        TInt64 retStartHourMinValue = KImumMboxDefaultAutoRetrievalStartHour;
        TInt64 retEndHourMinValue = KImumMboxDefaultAutoRetrievalEndHour;

        //Get the Retrieval Start and End Hours, first
        err = GetRetHoursValueL(csParamValue, '-', retStartHourMinValue);

        if (!err)
            {
            err = GetRetHoursValueL(csParamValue, '<', retEndHourMinValue);
            }

        //Set Retrieval Days
        /* 
         Characteristic/name: APPLICATION/CS
         Status: Optional.
         Occurs: 0/1.
         Default value: empty.
         Used values: empty or time<days>; 00:00-23:59<0,1,2,3,4,5,6>
         Sunday = 0.
         Monday = 1.
         Tuesday = 2.
         Wednesday = 3.
         Thursday = 4.
         Friday = 5.
         Saturday = 6.
         Interpretation:
         Specifies the connection schedule when always online or polling is
         active. When value is empty, connection schedule is disabled. If
         connection shedule is inactivated, polling is still enabled.
         */
        //Setting Retrieval Days, first, parse the CS parm, till the end
        if (!err)
            {
            err = SetRetDaysL(csParamValue, aEmailSettingsData);
            }

        // If there no error in parsing the CS parameter, then set the Retrieval Start and End Hours
        if (!err && retStartHourMinValue
                > KImumMboxDefaultAutoRetrievalStartHour)
            {
            //Set Retrieval Start Hour
            err = aEmailSettingsData.SetAttr(
                    TImumDaSettings::EKeyAutoRetrievalStartHour,
                    retStartHourMinValue);
            }

        if (!err && retEndHourMinValue > KImumMboxDefaultAutoRetrievalEndHour)
            {
            //Set Retrieval End Hour
            err = aEmailSettingsData.SetAttr(
                    TImumDaSettings::EKeyAutoRetrievalEndHour,
                    retEndHourMinValue);
            }

        }

    const TDesC& pollParamName(KParamPOLL);
    //Set Poll
    /*
     Characteristic/name: APPLICATION/POLL
     Status: Optional.
     Occurs: 0/1.
     Default value: 0.
     Used values: 0 (off), or 16-bit unsigned integer.
     Interpretation:
     Specifies the polling period in minutes. Zero indicates that
     polling is disabled.
     */
    err = SetIntParam(aEmailSettingsData, aPopImapItem, pollParamName);

    const TDesC& lmsParamName(KParamLMS);
    TInt onlineParamVal;
    err = aEmailSettingsData.GetAttr(TImumDaSettings::EKeyAutoRetrieval,
            onlineParamVal);

    if ((!err && onlineParamVal == KImumMboxDefaultAutoRetrieval) || err)
        {
        //Automatic Retrieval is off, then set Mailbox Update Limit to the value received
        /*
         Characteristic/name: APPLICATION/LMS
         Status: Optional.
         Occurs: 0/1.
         Default value: 0.
         Used values: 0 (all mails) or 32-bit unsigned integer.
         Interpretation:
         Specifies the local mailbox size. This defines the amount of mails kept in
         local mailbox. If this value has been defined to zero (all), then all mails
         are fetched to the mobile (local mailbox). If this value is define for
         example 10, then only 10 newest mails are kept in local mailbox.
         */
        err = SetIntParam(aEmailSettingsData, aPopImapItem, lmsParamName);
        }

        LOGSTRING( "CWPEmailSaveItem::SetEmailNotificationRetrievalParamsL : Exit" );

    return err;

    }

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::SetIntParam
// -----------------------------------------------------------------------------
//
TInt CWPEmailSaveItem::SetIntParam(CImumDaSettingsData& aEmailSettingsData,
        CWPEmailItem& aPopImapItem, const TDesC& aParamName)
    {
    TInt err = KErrGeneral;
    TInt paramMinValue;
    TInt paramMaxValue;
    TInt paramKey = 0x0000000;

    if (aParamName.CompareF(KParamEMN) == 0)
        {
        paramMinValue = TImumDaSettings::EValueNotificationsOff;
        paramMaxValue = TImumDaSettings::EValueNotificationsHome;
        paramKey = TImumDaSettings::EKeyAutoNotifications;
        }
    else if (aParamName.CompareF(KParamONLINE) == 0)
        {
        paramMinValue = TImumDaSettings::EValueAutoOff;
        paramMaxValue = TImumDaSettings::EValueAutoHomeNetwork;
        paramKey = TImumDaSettings::EKeyAutoRetrieval;
        }
    else if (aParamName.CompareF(KParamPOLL) == 0)
        {
        paramMinValue = KImumPollMinUpdateLimit;
        paramMaxValue = KImumPollMaxUpdateLimit;
        }

    else if (aParamName.CompareF(KParamLMS) == 0)
        {
        paramMinValue = KImumMboxInboxMinUpdateLimit;
        paramMaxValue = KImumMboxInboxMaxUpdateLimit;
        paramKey = TImumDaSettings::EKeyInboxUpdateLimit;
        }

    else
        {
        return err;
        }

    TInt val;
    err = GetParamValue(aPopImapItem, aParamName, val);
    if (!err)
        {
        if (val >= paramMinValue && val <= paramMaxValue)
            {
            if (aParamName.CompareF(KParamPOLL) != 0)
                {
                //Set the value for the Parm
                err = aEmailSettingsData.SetAttr(paramKey, val);
                }
            else
                {
                //Set the value for Poll
                err = SetPollValue(aEmailSettingsData, val);
                }
            }
        else
            {
            return KErrOverflow;
            }
        }
    return err;
    }
// -----------------------------------------------------------------------------
// CWPEmailSaveItem::GetParamValue
// -----------------------------------------------------------------------------
//
TInt CWPEmailSaveItem::GetParamValue(CWPEmailItem& aEmailItem,
        const TInt aParamID, TInt& aVal)
    {
    TLex paramValue(aEmailItem.ParameterValue(aParamID));
    return (paramValue.Val(aVal));
    }

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::GetParamValue
// -----------------------------------------------------------------------------
//
TInt CWPEmailSaveItem::GetParamValue(CWPEmailItem& aEmailItem,
        const TDesC& aParamName, TInt& aVal)
    {
    TLex paramValue(aEmailItem.ParameterValue(aParamName));
    return (paramValue.Val(aVal));
    }

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::GetRetParamValuesFromCS
// -----------------------------------------------------------------------------
//
TBool CWPEmailSaveItem::GetRetParamValuesFromCS(TLex& aCSParamValue,
        const TInt aTokenMark, TPtrC& aParm)
    {
    aCSParamValue.Mark();
    TInt next = aCSParamValue.Peek();
    TChar cnext = aCSParamValue.Peek();
    TInt count = 0;

    while (!cnext.Eos())
        {
        if (next == aTokenMark)
            {
            aParm.Set(aCSParamValue.MarkedToken());
            aCSParamValue.Inc();
            return ETrue;
            }
        aCSParamValue.Inc();
        next = aCSParamValue.Peek();
        cnext = aCSParamValue.Peek();
        count++;
        }

    while (count--)
        {
        aCSParamValue.UnGet();
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::GetRetHoursValueL
// -----------------------------------------------------------------------------
//
TInt CWPEmailSaveItem::GetRetHoursValueL(TLex& aCSParamValue,
        TInt aTokenMark, TInt64& aRetHourMinValue)
    {
    TInt err = KErrGeneral;
    TPtrC retHour(KNullDesC);
    TPtrC retMin(KNullDesC);

    if (GetRetParamValuesFromCS(aCSParamValue, KColonChar, retHour)
            && GetRetParamValuesFromCS(aCSParamValue, aTokenMark, retMin))
        {
        if (retHour.Length() != KRetHourStandardLength || retMin.Length()
                != KRetMinStandardLength)
            {
            return KErrOverflow;
            }
        TLex lexRetHour(retHour);
        TLex lexRetMin(retMin);
        TInt64 valRetHour;
        TInt64 valRetMin;

        err = ValidateRetHourorMin(lexRetHour);

        if (!err)
            {
            err = ValidateRetHourorMin(lexRetMin);
            }

        if (!err)
            {
            err = lexRetHour.Val(valRetHour);
            }

        if (!err)
            {
            err = lexRetMin.Val(valRetMin);
            }

        if (!err)
            {
            //Check for the boundary values
            if (valRetHour >= KRetHourLowerBound && valRetHour
                    <= KRetHourUpperBound && valRetMin >= KRetMinLowerBound
                    && valRetMin <= KRetMinUpperBound)
                {
                const TDesC& retSecMilliSec(KRetSecMilliSec);
                TInt retHourMinMaxSize = retHour.Size() + retMin.Size()
                        + retSecMilliSec.Size();
                TInt retHourMaxSize = retHour.Size();
                TInt retMinMaxSize = retMin.Size();
                HBufC* bufRetHour = HBufC::NewL(retHourMaxSize);
                HBufC* bufRetMin = HBufC::NewL(retMinMaxSize);
                HBufC* bufRetHourMin = HBufC::NewL(retHourMinMaxSize);

            TPtr ptrRetHour(bufRetHour->Des());
            //Change the Absolute Integer Hour value to a string 
            GetRetHourorMinfromIntValue(retHour, valRetHour, ptrRetHour);

            TPtr ptrRetMin(bufRetMin->Des());
            //Change the Absolute Integer Min value to a string 
            GetRetHourorMinfromIntValue(retMin, valRetMin, ptrRetMin);

            TPtr retHourMin(bufRetHourMin->Des());
            retHourMin.Append(ptrRetHour);
            retHourMin.Append(ptrRetMin);
            //Make the string to be in proper Time format
            retHourMin.Append(retSecMilliSec);

                    LOGSTRING2( "[Provisioning] CWPEmailSaveItem::GetRetHoursValueL: retHourMin value %i", retHourMin );

            TTime timeretHour(retHourMin);
            aRetHourMinValue = timeretHour.Int64();
            delete bufRetHourMin;
            delete bufRetMin;
            delete bufRetHour;

                }
            else
                {
                return KErrOverflow;
                }
            }

        }

    return err;

    }

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::SetRetDaysL
// -----------------------------------------------------------------------------
//
TInt CWPEmailSaveItem::SetRetDaysL(TLex& aCSParamValue,
        CImumDaSettingsData& aEmailSettingsData)
    {
    TInt err = KErrGeneral;
    TPtrC retDays(KNullDesC);

    if (GetRetParamValuesFromCS(aCSParamValue, KGTChar, retDays))
        {
            LOGSTRING2( "[Provisioning] CWPEmailSaveItem::SetRetDaysL: retDays value, %i", retDays );

        const TDesC& comma(KComma);
        const TInt retDaysMaxSize = retDays.Size() + comma.Size();
        HBufC* bufRetDays = HBufC::NewL(retDaysMaxSize);
        *bufRetDays = retDays;

        TPtr ptrRetDays(bufRetDays->Des());
        //Append comma to demarcate the last day
        ptrRetDays.Append(comma);

        if (ptrRetDays.Length() > KRetDaysStandardMaxLength)
            {
            delete bufRetDays;
            return KErrOverflow;
            }

        TLex lexRetDays(ptrRetDays);
        TChar next = lexRetDays.Peek();
        TInt retDaysFlag = 0x0000000;

        while (!next.Eos())
            {
            TPtrC retDay(KNullDesC);

            if (GetRetParamValuesFromCS(lexRetDays, KCommaChar, retDay))
                {
                if (retDay.Length() != KRetDayStandardLength)
                    {
                    delete bufRetDays;
                    return KErrOverflow;
                    }
                TLex lexRetDay(retDay);
                TInt retDayVal;
                    LOGSTRING2( "[Provisioning] CWPEmailSaveItem::SetRetDaysL: retDay value, %i", retDay );
                
                err = lexRetDay.Val(retDayVal);
                if (!err)
                    {
                    switch (retDayVal)
                        {
                        case KRetSunday:
                            {
                            retDaysFlag |= TImumDaSettings::EFlagSunday;
                            break;
                            }
                        case KRetMonday:
                            {
                            retDaysFlag |= TImumDaSettings::EFlagMonday;
                            break;
                            }
                        case KRetTuesday:
                            {
                            retDaysFlag |= TImumDaSettings::EFlagTuesday;
                            break;
                            }
                        case KRetWednesday:
                            {
                            retDaysFlag |= TImumDaSettings::EFlagWednesday;
                            break;
                            }
                        case KRetThursday:
                            {
                            retDaysFlag |= TImumDaSettings::EFlagThursday;
                            break;
                            }
                        case KRetFriday:
                            {
                            retDaysFlag |= TImumDaSettings::EFlagFriday;
                            break;
                            }
                        case KRetSaturday:
                            {
                            retDaysFlag |= TImumDaSettings::EFlagSaturday;
                            break;
                            }
                        default:
                            {
                            delete bufRetDays;
                            return KErrOverflow;
                            }
                        }
                    }
                else
                    {
                    break;
                    }
                    
                }

            next = lexRetDays.Peek();
            }

        if (!err && retDaysFlag)
            {
            //If the flag is set, set the value to Retrieval Days
            err = aEmailSettingsData.SetAttr(
                    TImumDaSettings::EKeyAutoRetrievalDays, retDaysFlag);
            }

        delete bufRetDays;
        }

    return err;

    }

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::SetPollValue
// -----------------------------------------------------------------------------
//
TInt CWPEmailSaveItem::SetPollValue(CImumDaSettingsData& aEmailSettingsData,
        TInt aPollValue)
    {
    TInt err = KErrGeneral;
    TInt pollhour = aPollValue / 60;
    TInt pollmin = aPollValue % 60;
    TInt pollval = pollhour;

    if (!pollhour)
        {
        pollval = pollmin;
        }

    //Consider only the absolute minutes and absolute hours 
    if ((pollhour && !pollmin) || (!pollhour && pollmin))
        {
        TInt val = 0;
        switch (pollval)
            {
            case KPoll5minutes:
                {
                val = TImumDaSettings::EValue5Minutes;
                break;
                }
            case KPoll15minutes:
                {
                val = TImumDaSettings::EValue15Minutes;
                break;
                }
            case KPoll30minutes:
                {
                val = TImumDaSettings::EValue30Minutes;
                break;
                }
            case KPoll1Hour:
                {
                val = TImumDaSettings::EValue1Hour;
                break;
                }
            case KPoll2Hours:
                {
                val = TImumDaSettings::EValue2Hours;
                break;
                }
            case KPoll4Hours:
                {
                val = TImumDaSettings::EValue4Hours;
                break;
                }
            case KPoll6Hours:
                {
                val = TImumDaSettings::EValue6Hours;
                break;
                }
            }
        if (val)
            {
            //If val is set, set it to Poll
            err = aEmailSettingsData.SetAttr(
                    TImumDaSettings::EKeyAutoRetrievalInterval, val);
            }
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::ValidateUserNamePassword
// -----------------------------------------------------------------------------
//
TInt CWPEmailSaveItem::ValidateUserNamePassword(
        MImumDaSettingsDataCollection& aPopImapSet, TBool& aValidUserPass )
    {
    TInt err = KErrGeneral;
    TImumDaSettings::TTextUserName userName(KNullDesC8);
    TImumDaSettings::TTextPassword passWord(KNullDesC8);

    err = aPopImapSet.GetAttr(TImumDaSettings::EKeyUsername, userName);

    if (!err)
        {
        err = aPopImapSet.GetAttr(TImumDaSettings::EKeyPassword, passWord);
        }

    if (!err && userName.Length() > 0 && passWord.Length() > 0)
        {
        aValidUserPass = ETrue;
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::GetRetHourorMinfromIntValue
// -----------------------------------------------------------------------------
//
void CWPEmailSaveItem::GetRetHourorMinfromIntValue(TPtrC& aRetHourMin,
        TInt64 aValRetHourMin, TPtr& aPtrRetHourMin)
    {
    const TDesC& retLeadingZero(KRetLeadingZero);
    TInt retHourMaxSize = aRetHourMin.Size();
    HBufC* bufRetHourMin = HBufC::NewL(retHourMaxSize);
    HBufC* TempBufRetHourMin = HBufC::NewL(retHourMaxSize);

    TPtr ptrTempRetHourMin(TempBufRetHourMin->Des());
    ptrTempRetHourMin.Num(aValRetHourMin);
        LOGSTRING2( "[Provisioning] CWPEmailSaveItem::GetRetHoursValueL: ptrTempRetHourMin value, %i ", ptrTempRetHourMin );

    if (aValRetHourMin < KRetHourMinLeadLimit)
        {
        aPtrRetHourMin.Append(retLeadingZero);
        }

    aPtrRetHourMin.Append(ptrTempRetHourMin);
        LOGSTRING2( "[Provisioning] CWPEmailSaveItem::GetRetHoursValueL: aPtrRetHourMin value %i", aPtrRetHourMin );
    
    delete TempBufRetHourMin;
    delete bufRetHourMin;
    }

// -----------------------------------------------------------------------------
// CWPEmailSaveItem::ValidateRetHourorMin
// -----------------------------------------------------------------------------
//
TInt CWPEmailSaveItem::ValidateRetHourorMin(TLex& aRetHourMin)
    {
    TChar cnext = aRetHourMin.Peek();
    TInt count = 0;

    while (!cnext.Eos())
        {
        if (!(cnext.IsDigit()))
            {
            return KErrGeneral;
            }
        aRetHourMin.Inc();
        cnext = aRetHourMin.Peek();
        count++;
        }

    while (count--)
        {
        aRetHourMin.UnGet();
        }

    return KErrNone;
    }
//  End of File  
