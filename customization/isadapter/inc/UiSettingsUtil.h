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


#ifndef __UISETTINGSUTIL_H__
#define __UISETTINGSUTIL_H__

#include <favouritesdb.h>           // Favourites Database
#include <msvapi.h>
#include <apgcli.h>
#include <centralrepository.h>

#include "javaregistry.h"
#include "javaregistryentry.h"


using namespace Java;

class CUiSettingsUtil : public CBase, public MMsvSessionObserver
{
public:

    static CUiSettingsUtil* NewL( );
    static CUiSettingsUtil* NewLC( );

    ~CUiSettingsUtil();
    
    // Returns whether the uid is of an application or bookmark
  	void GetShortcutRtTypeL(TInt aUid, TInt &aRtType);
  	
  	// Returns the index of the matching uid Applications and WebTargets
    TInt GetShortcutTargetIndexL(TInt aUid, TInt& aIndex); 
    
    // returns the total number of Softkeys
	
	TInt GetSoftkeyCountL();
    
    // Application Target Functions
    
    // Initializes the Normal, Java, Mailbox and Static Applications
    void ReadTargetsL();    
    
    // Returns the index of the matching index for Normal and Java Applications
    TInt GetAppTargetCaptionL(TInt index, TDes& aBufferCaption);
    
    // Sets the Softkey Target, given the Softkey UID and the App Index
    TInt SetShortcutTargetAppIndexL( TInt aUid, TInt aIndex );
  	
  	// returns total number of applications
	TInt GetNoOfTargetApps();	
	
	// Web Target Functions
	
	// Initializes the WebTarget List
	void ReadBookmarksL();
	
	// returns the number of webtargets
	TInt GetWebTargetsCount( TInt &aCount );
	
	// Returns the index of the matching uid for webtargets
	TInt GetWebTargetAppUid( TInt aIndex, TInt& aUid );
	
	// Returns the caption of the matching index for webtargets
	TInt GetWebTargetCaption( TInt aIndex, TDes& aCaption );
	
	// Get the wetarget index given the UID
	TInt GetWebTargetIndex( TInt& aIndex, TInt& aUid );
		
	// Sets the Softkey Target, given the Softkey UID and the WebTarget Index
	TInt SetShortcutTargetWebIndexL( TInt aUid, TInt aIndex );
  	
  	
  	void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/);
  	
private:	
	
	void ConstructL( );
    CUiSettingsUtil();
    	
	// Following four functions initialize their respective lists
	// Each is called from ReadTargetsL()
	void ReadJavaTargetsL();
	void ReadMailBoxesL();
	void ReadAppTargetsL();
	void ReadStaticAppTargetsL();
	
	// Checks if the java application is NonNative
	TBool IsNonNativeL(const TUid& aUid) const ;
	
	// Gets the key value for a given Softkey node
	void GetKeyValueL(TInt aUid, TDes16 &aValue);
	
	// Gets the Application Info at the given node
	TBool GetAppInfoAtIndex(TInt aIndex, TApaAppInfo& aAppInfo);
	
	// Finds the matching app (normal and Java) in the list to the UID
  	TBool GetAppIndexToUID(TUid aUID, TInt& aIndex );
  	
	// Finds the matching MailBox in the list to the name
  	TBool GetMailBoxTargetIndex(const TDesC& aName, TInt& aIndex );
  	
	// Finds the matching static application in the list to the name
  	TBool GetStaticAppIndex(const TDesC& aKeyValue,TInt& aIndex) ;
  	    
private:

	CFavouritesItemList*				iFavList;    
    RArray<TUid> 						iJaveUids;
    RPointerArray<TApaAppInfo> 			iListItems;
	CDesCArray* 						iMailBoxes;
	CDesCArrayFlat*						iStaticAppParam ;
	CDesCArrayFlat*						iStaticAppCaption ;
};


#endif // __UISETTINGSUTIL_H__