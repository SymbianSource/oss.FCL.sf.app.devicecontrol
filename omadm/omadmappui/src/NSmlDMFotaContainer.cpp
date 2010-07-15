/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Methods for FOTA view control container
*
*/



// INCLUDE FILES
#include <StringLoader.h>
#include <featmgr.h>
#include "NSmlDMSyncApp.h"
#include "NSmlDMFotaContainer.h"
#include "NSmlDMFotaView.h"
#include "NSmlDMFotaModel.h"
#include "NSmlDMUIDefines.h"
#include "nsmldmsyncinternalpskeys.h"
#include "NSmlDMSyncPrivateCRKeys.h"
#include <etel.h>
#include <etelmm.h>
//multi rofs
#include <sysutil.h>
#include <sysversioninfo.h>
#include "NSmlDMdef.h"
#include <centralrepository.h>
//multi rofs
#include <NSmlDMSync.rsg>
#include <e32property.h>
#include <swi/sisregistrypackage.h>
#include <swi/sisregistryentry.h>
#include <swi/sisregistrysession.h>

#include <csxhelp/dm.hlp.hrh>
#include <eiklbx.h>

#include <apgtask.h>

#include "NSmlDMSyncDebug.h"

// ============================ MEMBER FUNCTIONS ===============================
    
// -----------------------------------------------------------------------------
// CNSmlDMFotaContainer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNSmlDMFotaContainer* CNSmlDMFotaContainer::NewL( CAknView* aView,
                                                  const TRect& aRect,
                                                  CNSmlDMFotaModel* aFotaModel )
    {
    CNSmlDMFotaContainer* self =
        new( ELeave ) CNSmlDMFotaContainer( aView, aFotaModel );
    
    CleanupStack::PushL( self );
    self->SetMopParent( (MObjectProvider*) aView );
    self->ConstructL( aRect );
    CleanupStack::Pop();

    return self;
    }

// Destructor
CNSmlDMFotaContainer::~CNSmlDMFotaContainer()
    {
    delete iFotaListBox;
     FeatureManager::UnInitializeLib();
    }


// -----------------------------------------------------------------------------
// CNSmlDMFotaContainer::SizeChanged
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaContainer::SizeChanged()
    {
    iFotaListBox->SetRect( Rect() );    // Mandatory, otherwise not drawn
    }

// ---------------------------------------------------------
// CNSmlDMFotaContainer::HandleResourceChange
// ---------------------------------------------------------
//
void CNSmlDMFotaContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );
    
    //Handle change in layout orientation
    if ( aType == KEikDynamicLayoutVariantSwitch || aType == KAknsMessageSkinChange )
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
        SetRect( mainPaneRect );
		DrawDeferred();
		}
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaContainer::CountComponentControls
// -----------------------------------------------------------------------------
//
TInt CNSmlDMFotaContainer::CountComponentControls() const
    {
    TInt retval( 0 );
    if ( iFotaListBox )
        {
        retval = 1;
        }
    return retval; // return nbr of controls inside this container
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaContainer::ComponentControl
// -----------------------------------------------------------------------------
//
CCoeControl* CNSmlDMFotaContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            return iFotaListBox;
        default:
            return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaContainer::Draw
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaContainer::Draw( const TRect& /* aRect */ ) const
    {
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaContainer::OfferKeyEventL
// Redirect keypresses to the listbox
// -----------------------------------------------------------------------------
//
TKeyResponse CNSmlDMFotaContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                         TEventCode aType )                                                                      
    {
	FTRACE( FPrint(
	_L("[OMADM]\t CNSmlDMFotaContainer::OfferKeyEventL>> Begin code = 0x%x"),aKeyEvent.iCode ) );
	if( FeatureManager::FeatureSupported( KFeatureIdSyncMlDmFota ) ) //NFUI
		{
		FLOG( "[OMADM]\t CNSmlDMFotaContainer::OfferKeyEventL FOta supported ." );
		if (aKeyEvent.iScanCode == EStdKeyDevice0 
				|| aKeyEvent.iScanCode == EStdKeyDevice3 
				|| aKeyEvent.iScanCode ==	EStdKeyHash )
			{
			FLOG( "[OMADM]\t entered in to first if loop of keys" );
			TBool value (EFalse);
			TInt err = RProperty::Get( KFotaServerAppUid, KFotaDLStatus, value );
			if (!err && value == 1)
				{
				FLOG( "[OMADM]\t Ongoing Fota operation identified." );
				TApaTaskList taskList(CEikonEnv::Static()->WsSession());
				TApaTask task1(taskList.FindApp( TUid::Uid(KOmaDMAppUid)));
				FLOG("Bring DM UI into FG"); 
				task1.BringToForeground(); //DM UI
				TApaTask task=taskList.FindApp(TUid::Uid(KFotaServerUid));
			if(task.Exists())
				{
				FLOG( "[OMADM]\t  Switching to FS Download" );
				task.BringToForeground();
				}
				else
				FLOG( "[OMADM]\t Inconsistency problem!! need to debug" );
			return EKeyWasConsumed;
			}
		else
			{
				FLOG("[OMADM]\t No ongoing Fota operation identified!");
			}
		}
}
    if ( aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter) 
        {
        ShowContextMenuL( R_SMLSYNC_CONTXT_MENUBAR_FOTA_VIEW );
        }
    
    if( ( iFotaListBox )
          && ( aKeyEvent.iCode != EKeyLeftArrow )
          && ( aKeyEvent.iCode != EKeyRightArrow ) )
        {
        TKeyResponse res = iFotaListBox->OfferKeyEventL( aKeyEvent, aType );
        // Store current item index for options list usage later
        return res;
        }
     
	FLOG( "[OMADM]\t CNSmlDMFotaContainer::OfferKeyEventL << End");
    return EKeyWasNotConsumed;
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaContainer::HandleListBoxEventL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaContainer::HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                                TListBoxEvent /*aEventType*/ )
    {
    FLOG( "[OMADM]\t CNSmlDMFotaContainer::HandleListBoxEventL()" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaContainer::HandleControlEventL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaContainer::HandleControlEventL( CCoeControl* /* aControl */,
                                                TCoeEvent /* aEventType */ )
    {
    }


// -----------------------------------------------------------------------------
// CNSmlDMFotaContainer::GetHelpContext
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
	FLOG( "[OMADM] CNSmlDMFotaContainer::GetHelpContext" );
	
	aContext.iMajor = KUidSmlSyncApp;
    aContext.iContext = KDM_HLP_MAIN_VIEW;
    }



// -----------------------------------------------------------------------------
// CNSmlDMFotaContainer::FormatListbox
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaContainer::FormatListboxL( CDesCArray* aItemsArray,
										TBool aPostponeDisplay )
    {
	FLOG( "[OMADM] CNSmlDMFotaContainer::FormatListboxL()" );

    TBuf< KNSmlMaxTextLength64 > stringHolder;
    HBufC* itemText = NULL;
    aItemsArray->Reset();

		TBuf< KNSmlMaxTextLength64 > prodrelease;
		prodrelease.Zero();
		TBuf<KSysUtilVersionTextLength> ProdRelease;
		if ( SysUtil::GetPRInformation( ProdRelease ) == KErrNone )
		{
			prodrelease.Copy(ProdRelease);
		}
		
		if( prodrelease.Length()>0 )
  	{
			HBufC* itemTextRelease = StringLoader::LoadLC( R_QTN_FOTA_LIST_RELEASE,
                                            prodrelease );		
   		aItemsArray->AppendL( itemTextRelease->Des() );
			CleanupStack::PopAndDestroy( itemTextRelease );
  	}    
													
   if( FeatureManager::FeatureSupported( KFeatureIdSyncMlDmFota ) ) //NFUI
	       { 
		if ( aPostponeDisplay && iFotaModel->FindFwUpdPkgWithStateL(
				EDownloadProgressingWithResume ) != KErrNotFound )	       
			{
			TInt val (EFalse);
			RProperty::Get(KPSUidNSmlDMSyncApp,KFotaDLRefresh,val);
			if(!val)
				{
				HBufC* swItem = StringLoader::LoadLC( R_ITEM_FOTA_DOWNLOAD_POSTPONED );
				aItemsArray->AppendL( swItem->Des() );
				FLOG( "[OMADM] postponed item added" );
				CleanupStack::PopAndDestroy( swItem );	        
				}
        }
   else if ( iFotaModel->FindFwUpdPkgWithStateL( EStartingUpdate )
                                                               != KErrNotFound )
   		{
		HBufC* swItem = StringLoader::LoadLC( R_ITEM_FOTA_NEW_SW_AVAILABLE );
		aItemsArray->AppendL( swItem->Des() );
		CleanupStack::PopAndDestroy( swItem );		
		}			
	}
    TBuf< KNSmlMaxTextLength64 > swversion;
    TBuf< KNSmlMaxTextLength64 >  swversiondate;
    TBuf< KNSmlMaxTextLength64 >  typedesignator;
    TBuf< KNSmlMaxTextLength64 >  langversion;
    TBuf< KNSmlMaxTextLength64 >  customsw;
    TBuf< KNSmlMaxTextLength64 >  customswdate;
    TBuf<KSysUtilVersionTextLength> version;
    version.Zero();
    swversion.Zero();
    swversiondate.Zero();
    typedesignator.Zero();
    langversion.Zero();
    customsw.Zero();
    customswdate.Zero();
     if ( SysUtil::GetSWVersion( version ) == KErrNone )
         {         
         TInt len= version.Length();
         TInt pos1 = version.Find(KSmlEOL);
         if( pos1 != KErrNotFound && len > pos1 )
            {
         TBuf<KSysUtilVersionTextLength> version1;
         version1.Zero();
         swversion.Append( version.Left(pos1));
         version1.Append( version.Right( len-pos1-1 ));
         len= version1.Length();
         pos1 = version1.Find(KSmlEOL);
            if( pos1 != KErrNotFound  && len > pos1 )
             {
         swversiondate.Append(version1.Left( pos1 ));
         version.Zero();
         version.Append( version1.Right( len-pos1-1 ));
            len= version.Length();
            pos1 = version.Find(KSmlEOL);
            if( pos1 != KErrNotFound  && len > pos1 )
             {
             typedesignator.Append(version.Left(pos1));         
             }
             }
           }
         }
    TBuf<KSysUtilVersionTextLength> Langversion;    
    if( SysUtil::GetLangVersion(Langversion ) == KErrNone )
      {
      langversion.Copy(Langversion);
      }
    SysVersionInfo::TVersionInfoType x = SysVersionInfo::EOPVersion;
    version.Zero();
    if( SysVersionInfo::GetVersionInfo(x,version) == KErrNone )
      {
         TInt len= version.Length();
         TInt pos1 = version.Find(KSmlEOL);
         if( pos1 != KErrNotFound  && len > pos1 )
          {        
          customsw.Append( version.Left(pos1));
          customswdate.Append( version.Right( len-pos1-1 ));         
          }
      }  
    //multi rofs   
    HBufC* itemText1 = StringLoader::LoadLC( R_QTN_FOTA_LIST_SOFTWARE_VERSION,
                                            swversion );
    HBufC* itemText2 = StringLoader::LoadLC( R_QTN_FOTA_LIST_SOFTWARE_VERSION_DATE,
                                            swversiondate );   
    HBufC* itemText3 = StringLoader::LoadLC( R_QTN_FOTA_LIST_CUSTOMSW_VERSION,
                                            customsw );              
    HBufC* itemText4 = StringLoader::LoadLC( R_QTN_FOTA_LIST_CUSTOMSWDATE_VERSION,
                                            customswdate ); 
    HBufC* itemText5 = StringLoader::LoadLC( R_QTN_FOTA_LIST_LANGUAGE_VERSION,
                                            langversion );                                                                                                                                                                                                            
    if( swversion.Length()>0 )
    {
    aItemsArray->AppendL( itemText1->Des() );
    }
    if( swversiondate.Length()>0 )
    {
    aItemsArray->AppendL( itemText2->Des() );
    }     
    if( customsw.Length()>0 )
    {
    aItemsArray->AppendL( itemText3->Des() );
    }
    if( customswdate.Length()>0 )
    {
    aItemsArray->AppendL( itemText4->Des() );
    }
    if( langversion.Length()>0 )
    {
    aItemsArray->AppendL( itemText5->Des() );
    }    
    CleanupStack::PopAndDestroy( 5 );// itemtext1 to itemtext5    
    itemText1 = NULL;
    itemText2 = NULL;
    itemText3 = NULL;
    itemText4 = NULL;
    itemText5 = NULL; 
   TInt runtimesupport(0);
   CRepository* cenrep = NULL;
    TRAPD( error, cenrep = CRepository::NewL( KCRUidNSmlDMSyncApp ) );  
    if(error)
      {
      	
      	FLOG( "[OMADM] CNSmlDMFotaContainer::FormatListboxL() cenrep problem" );
      }    
    if ( cenrep )
        {
        cenrep->Get( KNsmlDmRuntimeVerSupport, runtimesupport );
        delete cenrep; cenrep = NULL;
        }
  if(runtimesupport)
  {   
  	TBuf<KSysUtilVersionTextLength> productcode;
  	productcode.Zero();
  	TInt error = SysVersionInfo::GetVersionInfo(SysVersionInfo::EProductCode, productcode);
  	if(error ==KErrNone )
   	{   		 
    	if( productcode.Length()>0 )
    	{                                       
    		HBufC* itemText6 = StringLoader::LoadLC( R_QTN_PRODUCT_CODE, productcode );   
    		aItemsArray->AppendL( itemText6->Des() ); 
    		CleanupStack::PopAndDestroy( );  
    		itemText6 = NULL;
  		}                    
   	}
  }     
  RTelServer telServer;
  User::LeaveIfError( telServer.Connect() );
  RTelServer::TPhoneInfo teleinfo;
  User::LeaveIfError( telServer.GetPhoneInfo( 0, teleinfo ) );
  RMobilePhone phone;
	User::LeaveIfError( phone.Open( telServer, teleinfo.iName ) );
	User::LeaveIfError(phone.Initialise());	
	TUint32 teleidentityCaps;
	phone.GetIdentityCaps( teleidentityCaps );
	RMobilePhone::TMobilePhoneIdentityV1 telid;
	TRequestStatus status;
	phone.GetPhoneId( status, telid );
	User::WaitForRequest( status );
	if (status==KErrNone)
	{
		TBuf<100> phoneName;
		phoneName.Copy( telid.iModel );
	  HBufC* phoneModl = StringLoader::LoadLC( R_ITEM_FOTA_MODEL, phoneName );
	  if( phoneName.Length()>0 )
    {                      
    	aItemsArray->AppendL(phoneModl ->Des() );
    }
    CleanupStack::PopAndDestroy(phoneModl);
	}
	else
	{
		HBufC* noModel = StringLoader::LoadLC( R_ITEM_DM_PHONE_NO_MODEL );
		aItemsArray->AppendL(noModel ->Des() );
		CleanupStack::PopAndDestroy(noModel);
	} 
  phone.Close();
  telServer.Close();
  itemText1 = StringLoader::LoadLC( R_QTN_FOTA_LIST_TYPE_DESIGNATOR, typedesignator );
  if( typedesignator.Length()>0 )
  {
    aItemsArray->AppendL( itemText1->Des() );
  }
  CleanupStack::PopAndDestroy( itemText1 );
  itemText1 = NULL;  
  	
  if( FeatureManager::FeatureSupported( KFeatureIdSyncMlDmFota ) ) //NFUI
	{
    //Device updated independent of UI Evol
		TTime time;
    TInt err = iFotaModel->LastUpdateTime( time );
    
    if ( err == KErrNone )
    {
    	TTime currentTime;
      currentTime.HomeTime();
      TDateTime currentDateTime = currentTime.DateTime();
      TDateTime dateTime = time.DateTime();
        
      if ( currentDateTime.Year() == dateTime.Year() &&
             currentDateTime.Month() == dateTime.Month() &&
             currentDateTime.Day() == dateTime.Day() )
      {
            HBufC* timeFormat = iEikonEnv->AllocReadResourceLC( R_QTN_TIME_USUAL ); 

            // Format the time to user readable format. The format is locale dependent	
            time.FormatL( stringHolder, *timeFormat );
            CleanupStack::PopAndDestroy(); // timeFormat
      }
      else
      {
            HBufC* dateFormat = iEikonEnv->AllocReadResourceLC( R_QTN_DATE_USUAL );
            
            // Format the date to user readable format. The format is locale dependent
            time.FormatL( stringHolder, *dateFormat );
            CleanupStack::PopAndDestroy(); // dateFormat
      }
      AknTextUtils::DisplayTextLanguageSpecificNumberConversion( stringHolder );
      itemText = StringLoader::LoadLC( R_ITEM_FOTA_LATEST_UPDATE, stringHolder );
      aItemsArray->AppendL( itemText->Des() );
      CleanupStack::PopAndDestroy( itemText );
    }
    else
    {
    	HBufC* resStringHolder = iCoeEnv->AllocReadResourceLC( R_QTN_FOTA_NOT_UPDATED );
      AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ( TDes& ) *resStringHolder );
      itemText = StringLoader::LoadLC( R_ITEM_FOTA_LATEST_UPDATE, *resStringHolder );
      aItemsArray->AppendL( itemText->Des() );
      CleanupStack::PopAndDestroy( 2 ); // itemText, resStringHolder
     }
   }
  
   
   Swi::RSisRegistrySession sisses ;
   TInt r( sisses.Connect() );
   CleanupClosePushL( sisses );
   if (r== KErrNone && runtimesupport)
   {           
   	Swi::RSisRegistryEntry sientry;
    // #define  browseruid = 0x10008d39; 
  
    TInt oerr = sientry.Open(sisses, browseruid);
    TVersion bversion;
    TBuf <255> browserversion;
    if(oerr == KErrNone)
    {
    	TRAPD(err2,bversion= sientry.VersionL());     
      if (err2 == KErrNone)
      {
      	browserversion.AppendNum(bversion.iMajor);
        browserversion.Append(_L("."));
        browserversion.AppendNum(bversion.iMinor);
        if( browserversion.Length()>0 )
    		{                                
        	HBufC* itemText7 = StringLoader::LoadLC( R_QTN_BROWSER_VERSION, browserversion );
        	aItemsArray->AppendL( itemText7->Des() ); 
        	CleanupStack::PopAndDestroy( );
        	itemText7 =NULL;
        }
       }
     }
  }  
      
  if (r== KErrNone&& runtimesupport)
  {           
  	Swi::RSisRegistryEntry sientry;
    TInt oerr = sientry.Open(sisses, flashuid);
    TVersion fversion;
    TBuf <255> flashversion;
    if(oerr == KErrNone)
    {
    	TRAPD(err2,fversion= sientry.VersionL());         
      if (err2 == KErrNone)
      {
      	flashversion.AppendNum(fversion.iMajor);
        flashversion.Append(_L("."));
        flashversion.AppendNum(fversion.iMinor); 
        if( flashversion.Length()>0 )
    		{                               
        	HBufC*	itemText9 = StringLoader::LoadLC( R_QTN_FLASH_VERSION, flashversion );
        	aItemsArray->AppendL( itemText9->Des() ); 
        	CleanupStack::PopAndDestroy( );   
        	itemText9 =NULL;  
        }                                   
       }      
     }
   }
         
   if (r== KErrNone&& runtimesupport)
   {            
   	Swi::RSisRegistryEntry sientry;
    TInt oerr = sientry.Open(sisses, javauid);
    TVersion jversion;
    TBuf <255> javaversion;
    if(oerr == KErrNone)
    {    	
      TRAPD(err2,jversion= sientry.VersionL());     
      if (err2 == KErrNone)
      {
      	javaversion.AppendNum(jversion.iMajor);
        javaversion.Append(_L("."));
        javaversion.AppendNum(jversion.iMinor);  
        if( javaversion.Length()>0 )
    		{ 
    			HBufC*	itemText8 = StringLoader::LoadLC( R_QTN_JAVA_VERSION, javaversion );                          
        	aItemsArray->AppendL( itemText8->Des() );
        	CleanupStack::PopAndDestroy( ); 
        	itemText8 =NULL;  
        } 
       }            
     }            
   }       
      
  CleanupStack::PopAndDestroy(1);
          //sisses.Close();  
	iFotaListBox->HandleItemAdditionL(); 
	FLOG( "[OMADM] CNSmlDMFotaContainer::FormatListboxL() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaContainer::ShowContextMenuL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaContainer::ShowContextMenuL( TInt aResource )
    {
	FLOG( "[OMADM] CNSmlDMFotaContainer::ShowContextMenuL()" );
	
	// Switch to Context specific options menu,
	// Show it and switch back to main options menu.
	CEikMenuBar* menuBar = iView->MenuBar();
	menuBar->SetMenuTitleResourceId( aResource );

	// TRAP displaying of menu bar.
	// If it fails, the correct resource is set back before leave.
	menuBar->SetMenuType( CEikMenuBar::EMenuContext );
	TRAPD( err, menuBar->TryDisplayMenuBarL() );
	menuBar->SetMenuTitleResourceId( R_SMLSYNC_MENUBAR_FOTA_VIEW );
	menuBar->SetMenuType( CEikMenuBar::EMenuOptions );
	User::LeaveIfError( err );

	FLOG( "[OMADM] CNSmlDMFotaContainer::ShowContextMenuL() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaContainer::CNSmlDMFotaContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNSmlDMFotaContainer::CNSmlDMFotaContainer( CAknView* aView,
                                            CNSmlDMFotaModel* aFotaModel )
    : iView( aView ), iFotaModel( aFotaModel )
    {
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaContainer::ConstructL( const TRect& aRect )
    {
	FLOG( "[OMADM] CNSmlDMFotaContainer::ConstructL()" );

    CreateWindowL();
    	FeatureManager::InitializeLibL();
    // Check if listbox is empty
    //__ASSERT_DEBUG( iFotaListBox == 0, User::Panic( KErrAlreadyExists ));

    // Create the itemlist
    iFotaListBox = new (ELeave) CAknDoubleStyleListBox;
    iFotaListBox->SetContainerWindowL( *this );
    iFotaListBox->ConstructL( this, EAknListBoxViewerFlags | 
    	CEikListBox::EDisableItemSpecificMenu | CEikListBox::EViewerFlag );
    // Set up/down arrows at bottom of the screen (scrollbar)
    iFotaListBox->CreateScrollBarFrameL( ETrue );
    iFotaListBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOn,
                                                             CEikScrollBarFrame::EAuto );

    iFotaListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );   // Does delete items array
    CDesCArray* itemsArray = (CDesCArray*) iFotaListBox->Model()->ItemTextArray();
    FormatListboxL( itemsArray, ETrue );
    iFotaListBox->HandleItemAdditionL();

    // Set up the observer (events listener)
    iFotaListBox->SetListBoxObserver( this );

    SetRect( aRect );
    ActivateL();
    
    FLOG( "[OMADM] CNSmlDMFotaContainer::ConstructL() completed" );
    }


// -----------------------------------------------------------------------------
// CNSmlDMFotaContainer::RefreshL 
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaContainer::RefreshL(TBool aPostponeDisplay)
	{
	CDesCArray* itemsArray = (CDesCArray*) iFotaListBox->Model()->ItemTextArray();
	FormatListboxL(itemsArray, aPostponeDisplay);
	}


//  End of File  
