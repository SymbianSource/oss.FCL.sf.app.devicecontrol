/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DM Idle SoftKey Adapter
*
*/


// s60
#include <FavouritesItemList.h>
#include <msvuids.h>
#include <SenduiMtmUids.h>
#include <javaregistryentrytype.h>
#include <featmgr.h>
#include <aiscutplugindomaincrkeys.h>

#include "UiSettingsUtil.h"
#include "IsAdapterLiterals.h"

#include "debug.h"


// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil* CUiSettingsUtil::NewL( )
// ------------------------------------------------------------------------------------------------
CUiSettingsUtil* CUiSettingsUtil::NewL( )
    {
    RDEBUG( "CUiSettingsUtil::NewL() >" );
    CUiSettingsUtil* self = NewLC( );
    CleanupStack::Pop();
    RDEBUG( "CUiSettingsUtil::NewL() <" );
    return self;
    }

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil* CUiSettingsUtil::NewLC( )
// ------------------------------------------------------------------------------------------------
CUiSettingsUtil* CUiSettingsUtil::NewLC( )
    {
    RDEBUG( "CUiSettingsUtil::NewLC() >" );
    CUiSettingsUtil* self = new (ELeave) CUiSettingsUtil();
    CleanupStack::PushL(self);
    self->ConstructL();
    RDEBUG( "CUiSettingsUtil::NewLC() <" );
    return self;
    }

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil::CUiSettingsUtil()
// ------------------------------------------------------------------------------------------------
CUiSettingsUtil::CUiSettingsUtil()
    {
    RDEBUG( "CUiSettingsUtil::CUiSettingsUtil()" );
    }

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil::ConstructL
// ------------------------------------------------------------------------------------------------
void CUiSettingsUtil::ConstructL()
    {
    RDEBUG( "CUiSettingsUtil::ConstructL() >" );

	FeatureManager::InitializeLibL();
	
/*	ReadAppTargetsL();
	ReadJavaTargetsL();	
	ReadMailBoxesL();
	ReadBookmarksL();
	ReadStaticAppTargetsL();*/
	
    
    RDEBUG( "CIsAdapter::ConstructL() <" );
    }

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil::~CUiSettingsUtil()
// ------------------------------------------------------------------------------------------------
CUiSettingsUtil::~CUiSettingsUtil()
    {
    RDEBUG( "CUiSettingsUtil::~CUiSettingsUtil() >" );
	
	if(iListItems.Count())
		iListItems.ResetAndDestroy();
	
	if(iJaveUids.Count())
		iJaveUids.Reset();
		
	if(iMailBoxes)
	{
		iMailBoxes->Reset();
		delete iMailBoxes;		
	}
	
	if(iStaticAppParam)
	{
		iStaticAppParam->Reset();
		delete iStaticAppParam;	
	}
	
	if(iStaticAppCaption)
	{
		iStaticAppCaption->Reset();
		delete iStaticAppCaption;		
	}
	
	if(iFavList)
	{
		delete iFavList;
	}
    
    FeatureManager::UnInitializeLib();
    
    RDEBUG( "CUiSettingsUtil::~CUiSettingsUtil() <" );
    }
   
// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil::GetShortcutRtType
// ------------------------------------------------------------------------------------------------
  
void CUiSettingsUtil::GetShortcutRtTypeL(TInt aUid, TInt &aRtType)
{	
	RDEBUG( "CUiSettingsUtil::GetShortcutRtTypeL() >" );
	aRtType = 1;	
    TBuf16<256> KeyValue;
    GetKeyValueL(aUid, KeyValue);
  	TInt temp = KeyValue.FindF(KBookmarkText); 
	if(temp==KErrNotFound)
		aRtType = 0;
	RDEBUG( "CUiSettingsUtil::GetShortcutRtTypeL() <" );
}

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil* CUiSettingsUtil::GetSoftkeyCountL( )
// ------------------------------------------------------------------------------------------------

TInt CUiSettingsUtil::GetSoftkeyCountL()
{		
	RDEBUG( "CUiSettingsUtil::GetSoftkeyCountL() >" );
	TInt count;
	CRepository *cenRep = CRepository::NewL(KCRUidShortcutItems);
	RArray<TUint32> defaultThemeKeys;
    CleanupClosePushL(defaultThemeKeys);
    cenRep->FindL(KScutCenRepKeyThemeDefault, KScutCenRepKeyMask, defaultThemeKeys);
    count = defaultThemeKeys.Count();
    //default SoftKeys always return Active Idle softkeys
    //total number of Softkeys = defalult + scroll (left,right,up,down,selection)
    count = count + 5;
    CleanupStack::PopAndDestroy(&defaultThemeKeys);
    delete cenRep;    
    RDEBUG( "CUiSettingsUtil::GetSoftkeyCountL() <" ); 
    return count;       
}

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil* CUiSettingsUtil::GetKeyValueL( )
// ------------------------------------------------------------------------------------------------

void CUiSettingsUtil::GetKeyValueL(TInt aUid, TDes16 &aKeyValue)
{	
	RDEBUG( "CUiSettingsUtil::GetKeyValueL() >" );
		CRepository *cenRep = CRepository::NewL(KCRUidShortcutItems);
    
    RArray<TUint32> defaultThemeKeys;
    CleanupClosePushL(defaultThemeKeys);

    RArray<TUint32> userDefinedKeys;
    CleanupClosePushL(userDefinedKeys);	
    
    cenRep->FindL(KScutCenRepKeyThemeDefault, KScutCenRepKeyMask, defaultThemeKeys);
    cenRep->FindL(KScutCenRepKeyUserDefined, KScutCenRepKeyMask, userDefinedKeys);

	TUint keyUid = aUid;
	keyUid = keyUid | 0x80000000 ;
    TUint defaultThemeKey;
    TUint userDefinedKey;
    TInt countThemeKey;
    TInt countUserKey;
	TBool isUserDefinedKey = EFalse;

    countThemeKey = defaultThemeKeys.Count();
    countUserKey = 	userDefinedKeys.Count();
    TBuf16<256> KeyValue; 
 
//First retrive the UID of the softkey's application    
    for (TInt i = 0; i < countUserKey; i++)
    {
        userDefinedKey = userDefinedKeys[i];
        userDefinedKey = userDefinedKey | 0x80000000;
        if (keyUid == userDefinedKey)
        {
        	isUserDefinedKey = ETrue;
        	cenRep->Get(userDefinedKeys[i],KeyValue);
        	break;
        }
    }
    
    if (!isUserDefinedKey)
    {
    	for (TInt i = 0; i < countThemeKey; i++)
    	{
        	defaultThemeKey = defaultThemeKeys[i];
        	if (keyUid == defaultThemeKey)
        	{
        		cenRep->Get(defaultThemeKey,KeyValue);
        		break;
        	}
    	}	
    }	
    aKeyValue = KeyValue;
    delete cenRep;
    CleanupStack::PopAndDestroy(&userDefinedKeys);
    CleanupStack::PopAndDestroy(&defaultThemeKeys);
    RDEBUG( "CUiSettingsUtil::GetKeyValueL() <" );    
}

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil::GetShortcutTargetIndex()
// ------------------------------------------------------------------------------------------------
  
TInt CUiSettingsUtil::GetShortcutTargetIndexL(TInt aUid, TInt& aIndex) 
{	
	RDEBUG( "CUiSettingsUtil::GetShortcutTargetIndexL() >" );
	TInt index;
    TBuf16<256> KeyValue;
    GetKeyValueL(aUid, KeyValue);
    if(GetStaticAppIndex(KeyValue, index))
    {
    	aIndex=index + iListItems.Count() + iJaveUids.Count() + iMailBoxes->Count();
    	return KErrNone;
    }
    TInt FindBm = KeyValue.FindF(KBookmarkText);
    if (FindBm==KErrNotFound)
    {    	    	
    	TInt FindMB = KeyValue.FindF(KMailBoxText);
   	    if (FindMB==KErrNotFound)
   	    {
   	       	TInt FindApp = KeyValue.FindF(KAppText); 
			KeyValue.Delete(0, KAppText().Length());

			TLex16 lex(KeyValue);
			TInt64 value;
			lex.Val(value, EHex);
		
			TUid ApplicationUid = { value };
			
    		if(GetAppIndexToUID(ApplicationUid, index))	
    		{
    			aIndex = index;
    		}
    		else
			{
				aIndex = -1;
				return KErrArgument;
			}	
   	    }
   	    else 
   	    {
   	    	KeyValue.Delete(0, KMailBoxText().Length());
   	    	if( GetMailBoxTargetIndex(KeyValue, index))
   	    	{
   	    		aIndex = index + iListItems.Count() + iJaveUids.Count();
   	    	}
   	    	else
   	    	{
   	    		aIndex = -1;
   	    		return KErrArgument;
   	    	}	
   	    }
   	}
    else
    {
    	KeyValue.Delete(0, KBookmarkText().Length());

		TLex16 lex(KeyValue);
		TInt64 value;
		lex.Val(value, EHex);
		
		TInt WebUid = value ;
		
		if (GetWebTargetIndex( WebUid, index) == KErrNotFound)
		{
			aIndex = -1;
			return KErrArgument;
		}
		else
			aIndex = index;
    }
    RDEBUG( "CUiSettingsUtil::GetShortcutTargetIndexL() <" );
    return KErrNone;            
}

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil::GetAppTargetCaptionL()
// ------------------------------------------------------------------------------------------------

// Caption and description are the same. They are obtained from
// RApaLsSession API by passing the application UID
// ApplicationUID is obtained from the CentRep

TInt CUiSettingsUtil::GetAppTargetCaptionL(TInt aIndex, TDes& aBufferCaption) 
{		
    RDEBUG( "CUiSettingsUtil::GetShortcutTargetCaptionL() >" );
    TInt NoOfTargetApps = iListItems.Count();
    TInt NoOfJavaApps = iJaveUids.Count();
    TInt NoOfMailBoxes = iMailBoxes->Count();
    TInt NoOfStaticApps = iStaticAppParam->Count();
         
    if(aIndex < NoOfTargetApps)
    {
		TApaAppInfo appInfo;
		GetAppInfoAtIndex(aIndex, appInfo);
	    aBufferCaption.Copy(appInfo.iCaption);        
	}
    else if ((aIndex - NoOfTargetApps) < NoOfJavaApps )
    {
      	CJavaRegistry* registry = CJavaRegistry::NewLC();
		CJavaRegistryEntry* entry = 
				registry->RegistryEntryL( iJaveUids[aIndex - NoOfTargetApps] );
		CleanupStack::PushL(entry);
   		const TDesC& name = entry->Name();
   		aBufferCaption.Copy(name);
   		CleanupStack::PopAndDestroy(entry);
    	CleanupStack::PopAndDestroy(registry);		
    }
    else if((aIndex - NoOfTargetApps - NoOfJavaApps ) < NoOfMailBoxes)
    {
    	TInt index = aIndex - NoOfTargetApps - NoOfJavaApps;
    	aBufferCaption = iMailBoxes->MdcaPoint(index);
    }    
    else if((aIndex - NoOfTargetApps - NoOfJavaApps - NoOfMailBoxes) < NoOfStaticApps)
    {
    	TInt index = aIndex - NoOfTargetApps - NoOfJavaApps - NoOfMailBoxes;
    	aBufferCaption = iStaticAppCaption->MdcaPoint(index);
    }
    else
    {
    	return KErrArgument;
    }
    RDEBUG( "CUiSettingsUtil::GetShortcutTargetCaptionL() <" );
    return KErrNone;    
}

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil::SetShortcutTargetAppIndex()
// ------------------------------------------------------------------------------------------------

TInt CUiSettingsUtil::SetShortcutTargetAppIndexL( TInt aUid, TInt aIndex )
{	
	RDEBUG( "CUiSettingsUtil::SetShortcutTargetAppIndexL() >" );
	TUint softkeyID = aUid;
    
	TBuf16<256> SetKeyValue;
	TBuf16<10> appUid ;
    
    TInt NoOfTargetApps = iListItems.Count();
    TInt NoOfJavaApps = iJaveUids.Count();
    TInt NoOfMailBoxes = iMailBoxes->Count();
    TInt NoOfStaticApps = iStaticAppParam->Count();
            
    CRepository *cenRep = CRepository::NewL(KCRUidShortcutItems);
    TBool found = EFalse;
    if (aIndex < NoOfTargetApps)
    {
    	TApaAppInfo appInfo;
		found = GetAppInfoAtIndex(aIndex, appInfo);
		appUid.Format(KFormat,appInfo.iUid);
		SetKeyValue = KAppText;
		SetKeyValue.Insert (SetKeyValue.Length(),appUid);	
    }
    else if ((aIndex - NoOfTargetApps) < NoOfJavaApps)
    {
    	CJavaRegistry* registry = CJavaRegistry::NewLC();
    	CJavaRegistryEntry* entry = registry->RegistryEntryL( iJaveUids[aIndex - NoOfTargetApps] );
    	CleanupStack::PushL(entry);
    	const TDesC& name = entry->Name();
       	found = ETrue;
    	appUid.Format(KFormat,entry->Uid());
    	CleanupStack::PopAndDestroy(entry);
    	CleanupStack::PopAndDestroy(registry);
    	SetKeyValue = KAppText;
    	SetKeyValue.Insert (SetKeyValue.Length(),appUid);	
    }
    else if((aIndex - NoOfTargetApps - NoOfJavaApps) < NoOfMailBoxes)
    {
    	TInt index = aIndex - NoOfTargetApps - NoOfJavaApps;    
    	SetKeyValue = KMailBoxText;
    	SetKeyValue.Insert (SetKeyValue.Length(),iMailBoxes->MdcaPoint(index)); 
    	found = ETrue;
    }
    else if((aIndex - NoOfTargetApps - NoOfJavaApps - NoOfMailBoxes) < NoOfStaticApps)
    {
    	TInt index = aIndex - NoOfTargetApps - NoOfJavaApps - NoOfMailBoxes;
    	SetKeyValue.Insert (SetKeyValue.Length(),iStaticAppParam->MdcaPoint(index));
    	found = ETrue;
    }
    else
    {
    	delete cenRep;
    	return KErrArgument;
    }
    	
	if (found)	
	{				
		cenRep->Set(softkeyID,SetKeyValue);
	}
	delete cenRep;	
	RDEBUG( "CUiSettingsUtil::SetShortcutTargetAppIndexL() <" );
	return KErrNone;
}

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil* CUiSettingsUtil::GetAppInfoAtIndex( )
// ------------------------------------------------------------------------------------------------

TBool CUiSettingsUtil::GetAppInfoAtIndex(TInt aIndex, TApaAppInfo& aAppInfo )
{	
	RDEBUG( "CUiSettingsUtil::GetAppInfoAtIndex() >" );
	aAppInfo = *iListItems[aIndex];
	if(aIndex < iListItems.Count() )
		return ETrue;
	else
		return EFalse;	
}

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil* CUiSettingsUtil::GetAppIndexToUID( )
// ------------------------------------------------------------------------------------------------

TBool CUiSettingsUtil::GetAppIndexToUID(TUid aUID, TInt& aIndex )
{
	RDEBUG( "CUiSettingsUtil::GetAppIndexToUID() >" );
	TBool found = EFalse;
	for (TInt i=0; i < iListItems.Count() ; i++)
	{
	    if(iListItems[i]->iUid == aUID)
        {	
        	found = ETrue;
        	aIndex = i;
	    	break;	
    	}
    }
    if(!found)
    {
    	for (TInt i=0; i < iJaveUids.Count() ; i++)
		{
	    	if(iJaveUids[i] == aUID)
        	{	
        		found = ETrue;
        		aIndex = i + iListItems.Count();
	    		break;	
    		}
    	}
    }
    RDEBUG( "CUiSettingsUtil::GetAppIndexToUID() <" );
	return found;	
}

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil* CUiSettingsUtil::GetMailBoxTargetIndex( )
// ------------------------------------------------------------------------------------------------

TBool CUiSettingsUtil::GetMailBoxTargetIndex(const TDesC& aName, TInt& aIndex )
{
	RDEBUG( "CUiSettingsUtil::GetMailBoxTargetIndex() >" );
	TBool found = EFalse;
	for (TInt i=0; i < iMailBoxes->Count() ; i++)
	{
	    if(aName.CompareF(iMailBoxes->MdcaPoint(i)) == 0 )
        {	
        	found = ETrue;
        	aIndex = i;
	    	break;	
    	}
    }
    RDEBUG( "CUiSettingsUtil::GetMailBoxTargetIndex() <" );
	return found;
}

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil* CUiSettingsUtil::GetStaticAppIndex( )
// ------------------------------------------------------------------------------------------------

TBool CUiSettingsUtil::GetStaticAppIndex(const TDesC& aKeyValue, TInt& aIndex)
{	
	RDEBUG( "CUiSettingsUtil::GetStaticAppIndex() >" );
	TBool found = EFalse;
	for (TInt i=0; i < iStaticAppParam->Count() ; i++)
	{
	    if(aKeyValue.CompareF(iStaticAppParam->MdcaPoint(i)) == 0 )
        {	
        	found = ETrue;
        	aIndex = i;
	    	break;	
    	}
    }
    RDEBUG( "CUiSettingsUtil::GetStaticAppIndex() <" );
	return found;	
}

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil* CUiSettingsUtil::ReadTargetsL( )
// ------------------------------------------------------------------------------------------------

void CUiSettingsUtil::ReadTargetsL()
{
	RDEBUG( "CUiSettingsUtil::ReadTargetsL() >" );
	ReadAppTargetsL();
	ReadJavaTargetsL();
	ReadMailBoxesL();	
	ReadStaticAppTargetsL();
	RDEBUG( "CUiSettingsUtil::ReadTargetsL() <" );
}

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil* CUiSettingsUtil::ReadAppTargetsL( )
// ------------------------------------------------------------------------------------------------

void CUiSettingsUtil::ReadAppTargetsL()
{
	RDEBUG( "CUiSettingsUtil::ReadAppTargetsL() >" );
	RApaLsSession apaSession;
	if ( apaSession.Handle() == KNullHandle )
    {
    	User::LeaveIfError( apaSession.Connect() );
    }
	       
	TInt count_app =0 ;
	if (apaSession.GetAllApps() == KErrNone)
	{
		TInt err = apaSession.AppCount(count_app);
	}
	iListItems.ReserveL(150);
	for (TInt i=0; i < count_app ; i++)
	{
	    TApaAppInfo* appInfo = new (ELeave) TApaAppInfo;
	    TInt err = apaSession.GetNextApp(*appInfo);
   	    if (err == KErrNone)
    	{
		    TApaAppCapabilityBuf buf;
   		    if (apaSession.GetAppCapability(buf, appInfo->iUid) == KErrNone &&
       	       !buf().iAppIsHidden && !IsNonNativeL(appInfo->iUid) )
        	{
    	   		iListItems.Append(appInfo);	
    	   	}
    	   	else
    	   	{
    	   		delete appInfo;
    	   	}
       	}
       	else
       	{
       		delete appInfo;	
       	}
	}
	apaSession.Close();	
	RDEBUG( "CUiSettingsUtil::ReadAppTargetsL() <" );
}

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil* CUiSettingsUtil::ReadJavaTargetsL( )
// ------------------------------------------------------------------------------------------------

void CUiSettingsUtil::ReadJavaTargetsL()
{
	RDEBUG( "CUiSettingsUtil::ReadJavaTargetsL() >" );
	CJavaRegistry* registry = CJavaRegistry::NewLC();

    // get all uids stored in registry
    RArray<TUid> uids;
    CleanupClosePushL(uids);
    registry->GetRegistryEntryUidsL( uids );
	//TBool duplicate = EFalse ;
	for(TInt i =0; i <uids.Count();i++)
	{
		CJavaRegistryEntry* entry = registry->RegistryEntryL( uids[i] );
		CleanupStack::PushL(entry);
		if(entry->Type() >= EGeneralApplication)
		{
			iJaveUids.Append(uids[i]);
		}
		CleanupStack::PopAndDestroy(entry);
	}
	CleanupStack::PopAndDestroy(&uids);
	CleanupStack::PopAndDestroy(registry);	
	RDEBUG( "CUiSettingsUtil::ReadJavaTargetsL() <" );
}

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil* CUiSettingsUtil::ReadMailBoxesL( )
// ------------------------------------------------------------------------------------------------

void CUiSettingsUtil::ReadMailBoxesL()
{
	RDEBUG( "CUiSettingsUtil::ReadMailBoxesL() >" );
	CMsvSession* msvSession = CMsvSession::OpenAsObserverL(*this);
	iMailBoxes = new (ELeave) CDesCArrayFlat(1);
    
    CMsvEntry* rootEntry = msvSession->GetEntryL(KMsvRootIndexEntryIdValue);
    CleanupStack::PushL(rootEntry);

    for (TInt i = rootEntry->Count() - 1; i >= 0; --i)
    {
        const TMsvEntry& tentry = (*rootEntry)[i];

        if (tentry.iMtm == KSenduiMtmImap4Uid || tentry.iMtm == KSenduiMtmPop3Uid)
        {
            iMailBoxes->AppendL(tentry.iDetails);
        }
    }
	CleanupStack::PopAndDestroy(rootEntry);
	delete msvSession;
	RDEBUG( "CUiSettingsUtil::ReadMailBoxesL() <" );
}

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil* CUiSettingsUtil::ReadStaticAppTargetsL( )
// ------------------------------------------------------------------------------------------------

void CUiSettingsUtil::ReadStaticAppTargetsL()
{
	RDEBUG( "CUiSettingsUtil::ReadStaticAppTargetsL() >" );
	//add the code to read the static applications here
	
	iStaticAppParam = new (ELeave) CDesCArrayFlat(1);
	iStaticAppCaption = new (ELeave) CDesCArrayFlat(1);
	
	iStaticAppParam->AppendL(KNewMsg());
	iStaticAppCaption->AppendL(KNewMsgCaption());
	
	iStaticAppParam->AppendL(KNewEmail());
	iStaticAppCaption->AppendL(KNewEmailCaption());
	
	#ifdef __SYNCML_DS_EMAIL
		iStaticAppParam->AppendL(KNewSyncMLMail());
		iStaticAppCaption->AppendL(KNewSyncMLMailCaption());
	#endif	
	
	if (FeatureManager::FeatureSupported(KFeatureIdMmsPostcard))
    {
        // New Postcard item.
        iStaticAppParam->AppendL(KNewMMS());
		iStaticAppCaption->AppendL(KNewMMSCaption());
    }

    if (FeatureManager::FeatureSupported(KFeatureIdAudioMessaging))
    {
    	// New Audio Messaging
    	iStaticAppParam->AppendL(KNewAudioMsg());
		iStaticAppCaption->AppendL(KNewAudioMsgCaption());
    }	
    
	iStaticAppParam->AppendL(KNewMsgType());
	iStaticAppCaption->AppendL(KNewMsgTypeCaption());
	
	iStaticAppParam->AppendL(KChangeTheme());
	iStaticAppCaption->AppendL(KChangeThemeCaption());		
	RDEBUG( "CUiSettingsUtil::ReadStaticAppTargetsL() <" );
}

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil* CUiSettingsUtil::IsNonNativeL( )
// ------------------------------------------------------------------------------------------------

TBool CUiSettingsUtil::IsNonNativeL(const TUid& aUid) const
{
    RDEBUG( "CUiSettingsUtil::IsNonNativeL() >" );
    TBool ret = EFalse;

    const TUid KMidletAppType = { 0x10210E26 };
    TUid typeuid = KNullUid;
	RApaLsSession apaSession;
	if ( apaSession.Handle() == KNullHandle )
	       {
	       User::LeaveIfError( apaSession.Connect() );
	       }
    if (KErrNone == apaSession.GetAppType(typeuid, aUid))
    {
        if (typeuid == KMidletAppType)
        {
            ret = ETrue;
        }
    }
    apaSession.Close();
    RDEBUG( "CUiSettingsUtil::IsNonNativeL() <" );
    return ret;
}

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil* CUiSettingsUtil::GetNoOfTargetApps( )
// ------------------------------------------------------------------------------------------------

TInt CUiSettingsUtil::GetNoOfTargetApps()
{
	RDEBUG( "CUiSettingsUtil::GetNoOfTargetApps() >" );
	return iListItems.Count() + iJaveUids.Count() + iMailBoxes->Count() + iStaticAppParam->Count();
}

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil* CUiSettingsUtil::ReadBookmarksL( )
// ------------------------------------------------------------------------------------------------

void CUiSettingsUtil::ReadBookmarksL()
{
	RDEBUG( "CUiSettingsUtil::ReadBookmarksL() >" );
	RFavouritesSession                  bmSess;
    RFavouritesDb                       bmDb;
	User::LeaveIfError(bmSess.Connect());
    User::LeaveIfError(bmDb.Open(bmSess, KBrowserBookmarks));
    if(iFavList)
    {
    	delete iFavList;
    	iFavList = NULL;
    }
	iFavList = new (ELeave) CFavouritesItemList();
    TInt err = bmDb.GetAll(*iFavList, KFavouritesNullUid, CFavouritesItem::EItem);
    bmDb.Close();
    bmSess.Close();
    RDEBUG( "CUiSettingsUtil::ReadBookmarksL() <" );
}

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil* CUiSettingsUtil::GetWebTargetsCount( )
// ------------------------------------------------------------------------------------------------

TInt CUiSettingsUtil::GetWebTargetsCount( TInt &aCount )
{
	RDEBUG( "CUiSettingsUtil::GetWebTargetAppUid() >" );
	aCount = iFavList->Count();	
	RDEBUG( "CUiSettingsUtil::GetWebTargetAppUid() <" );
    return KErrNone;    
}

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil* CUiSettingsUtil::GetWebTargetAppUid( )
// ------------------------------------------------------------------------------------------------

TInt CUiSettingsUtil::GetWebTargetAppUid( TInt aIndex, TInt& aUid )
{
    RDEBUG( "CUiSettingsUtil::GetWebTargetAppUid() >" );
    CFavouritesItem* item;
    if(aIndex>=0 && aIndex<iFavList->Count())
	    item = iFavList->At(aIndex);
    else
    	return KErrArgument;
    aUid  = item->Uid();
    RDEBUG( "CUiSettingsUtil::GetWebTargetAppUid() <" );
    return KErrNone;    
}

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil* CUiSettingsUtil::GetWebTargetCaption( )
// ------------------------------------------------------------------------------------------------

TInt CUiSettingsUtil::GetWebTargetCaption( TInt aIndex, TDes& aCaption )
{
    RDEBUG( "CUiSettingsUtil::GetWebTargetCaption() >" );
    CFavouritesItem* item;
    if(aIndex>=0 && aIndex<iFavList->Count())
   	   	item = iFavList->At(aIndex);
    else
    	return KErrArgument;
    aCaption = 	item->Name();
    RDEBUG( "CUiSettingsUtil::GetWebTargetCaption() <" );
    return KErrNone;
}

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil* CUiSettingsUtil::GetWebTargetIndex( )
// ------------------------------------------------------------------------------------------------

TInt CUiSettingsUtil::GetWebTargetIndex( TInt& aUid, TInt& aIndex)
{
	RDEBUG( "CUiSettingsUtil::GetWebTargetIndex() >" );
	TBool found = EFalse;
	TInt i;
    for (i=0; i < iFavList->Count() ; i++)
    {
       	CFavouritesItem* item = iFavList->At(i);
       	if(item->Uid() == aUid)
       	{
       		found = ETrue;
       		break;	
       	}
    }
    if (found)
    {
    	aIndex = i;
    	return KErrNone;
    }    	
    else
    {
        aIndex = -1;
        return KErrNotFound;
    }
}

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil* CUiSettingsUtil::SetShortcutTargetWebIndexL( )
// ------------------------------------------------------------------------------------------------

TInt CUiSettingsUtil::SetShortcutTargetWebIndexL( TInt aUid, TInt aIndex )
{
	RDEBUG( "CUiSettingsUtil::SetShortcutTargetWebIndexL() >" );
	TUint webTargetID = aUid;
    CFavouritesItem* item;         
    CRepository *cenRep = CRepository::NewL(KCRUidShortcutItems);
   	if(aIndex>=0 && aIndex<iFavList->Count())
   	   	item = iFavList->At(aIndex);
   	else
   	{
   		delete cenRep;
   		return KErrArgument;
   	}   		
   	
	TBuf16<256> SetKeyValue;
	TBuf16<10> appUid ;			
	appUid.Format(KFormat,item->Uid());
	SetKeyValue = KBookmarkText;
	SetKeyValue.Insert (SetKeyValue.Length(),appUid);
	cenRep->Set(webTargetID, SetKeyValue);			

	delete cenRep;
	RDEBUG( "CUiSettingsUtil::SetShortcutTargetWebIndexL() <" );
	return KErrNone;
}

// ------------------------------------------------------------------------------------------------
// CUiSettingsUtil* CUiSettingsUtil::HandleSessionEventL( )
// ------------------------------------------------------------------------------------------------

void CUiSettingsUtil::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
{
  	// A null function
  	// since we are not registering for any observer
  	// but required since its a pure virtual function in 
  	// the parent class
}
