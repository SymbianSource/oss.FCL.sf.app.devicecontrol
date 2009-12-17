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
* Description:  Methods for Server Profiles View
*
*/


// INCLUDE FILES
#include "NSmlDMSyncAppUi.h"
#include "NSmlDMProfilesView.h"
#include "NSmlDMProfilesContainer.h"
#include <NSmlDMSync.rsg>
#include <featmgr.h>
#include "NSmlDMSyncDebug.h"

#include <centralrepository.h>
#include "NSmlDMSyncPrivateCRKeys.h"

#include <e32property.h> 
#include "nsmldmsyncinternalpskeys.h"
#include "NSmlDMFotaView.h"
// ========================== MEMBER FUNCTIONS =================================

// -----------------------------------------------------------------------------
// CNSmlDMProfilesView::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNSmlDMProfilesView::ConstructL()
    {
    FLOG( "[OMADM]\t CNSmlDMProfilesView::ConstructL()" );

    BaseConstructL( R_SMLSYNC_PROFILES_VIEW );

    FLOG( "[OMADM]\t CNSmlDMProfilesView::ConstructL() completed" );
    }


// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CNSmlDMProfilesView::~CNSmlDMProfilesView()
    {
    FLOG( "[OMADM] CNSmlDMProfilesView::~CNSmlDMProfilesView" );
    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        }
    delete iContainer;
    }

// -----------------------------------------------------------------------------
// CNSmlDMProfilesView::RefreshL
// -----------------------------------------------------------------------------
//
void CNSmlDMProfilesView::RefreshL()
    {
    FLOG( "[OMADM]\t CNSmlDMProfilesView::RefreshListBox()" );

    if ( iContainer )
        {
        iContainer->RefreshL();
        }
    FLOG( "[OMADM]\t CNSmlDMProfilesView::RefreshListBox() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMProfilesView::Id
// -----------------------------------------------------------------------------
//
TUid CNSmlDMProfilesView::Id() const
    {
    return KNSmlDMProfilesViewId;
    }

// -----------------------------------------------------------------------------
// CNSmlDMProfilesView::HandleCommandL
// -----------------------------------------------------------------------------
//
void CNSmlDMProfilesView::HandleCommandL( TInt aCommand )
    {
    FLOG( "[OMADM]\t CNSmlDMProfilesView::HandleCommandL()" );

    switch ( aCommand )
        {
 
         case EAknSoftkeyBack:
            {
            TInt value = EFalse;    //IAD: Making DM Idle
            TInt r1 = RProperty::Set(KPSUidNSmlDMSyncApp,KDMIdle,value); 
            AppUi()->ActivateLocalViewL( KNSmlDMFotaViewId  );
            break;
            }    
         
        case EAknCmdExit: 
        case EEikCmdExit:
            {            
            STATIC_CAST( CNSmlDMSyncAppUi*, iEikonEnv->EikAppUi() )->CloseGs(); 
            break;
            }
        
        default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CNSmlDMProfilesView::DoActivateL
// -----------------------------------------------------------------------------
//
void CNSmlDMProfilesView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
                                             TUid /*aCustomMessageId*/,
                                       const TDesC8& /*aCustomMessage*/)
    {
    FLOG( "[OMADM]\t CNSmlDMProfilesView::DoActivateL()" );
    
    	MenuBar()->SetContextMenuTitleResourceId( 
    						 R_SMLSYNC_CONTXT_MENUBAR_PROFILES_VIEW );

    iContainer = CNSmlDMProfilesContainer::NewL( this, ClientRect() );
    AppUi()->AddToStackL( *this, iContainer );
    
    CAknTitlePane* titlePane = 
	    (CAknTitlePane*)(AppUi()->StatusPane())->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) );
	HBufC* titleText = iEikonEnv->AllocReadResourceLC( R_SML_PROFILES_TITLE );
	titlePane->SetTextL( titleText->Des() );
	CleanupStack::PopAndDestroy(titleText);
	

    FLOG( "[OMADM]\t CNSmlDMProfilesView::DoActivateL() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMProfilesView::DoDeactivate
// -----------------------------------------------------------------------------
//
void CNSmlDMProfilesView::DoDeactivate()
    {
    FLOG( "[OMADM]\t CNSmlDMProfilesView::DoDeactivate()" );

    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        }

    delete iContainer;
    iContainer = NULL;

    FLOG( "[OMADM]\t CNSmlDMProfilesView::DoDeactivate() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMProfilesView::DynInitMenuPaneL
// -----------------------------------------------------------------------------
//
void CNSmlDMProfilesView::DynInitMenuPaneL( TInt aResourceID,
                                            CEikMenuPane* aMenuPane)
	{
	FLOG( "[OMADM] CNSmlDMProfilesView::DynInitMenuPaneL" );
	TInt configFlags( 0 );
	TInt retval( -1 );
    CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidNSmlDMSyncApp ) );
    if ( centrep )
        {
        retval = centrep->Get( KNSmlDMEnableConnectMenuKey, configFlags );
        delete centrep;
        }	
    if ( err == KErrNone )  	
        {
        if ( retval != KErrNone )
            User::Leave( retval );
        }
    else 
        {
        User::Leave( err );
        }	
    if ( aResourceID == R_SMLSYNC_MENU_COMMON && 
            ! FeatureManager::FeatureSupported( KFeatureIdHelp ))
        {
        aMenuPane->SetItemDimmed( EAknCmdHelp , ETrue );      
        }	          
    if ( aResourceID == R_SMLSYNC_MENU_PROFILES_VIEW
            || aResourceID == R_SMLSYNC_CONTXT_MENU_PROFILES_VIEW )
        {
        CNSmlDMSyncDocument* doc = STATIC_CAST( CNSmlDMSyncDocument*, 
                                            AppUi()->Document() );	    	    
        if ( iContainer->iProfilesListBox->Model()->NumberOfItems() == 0 )
            {
            aMenuPane->DeleteMenuItem( ENSmlMenuCmdStartSync );
            aMenuPane->DeleteMenuItem( ENSmlMenuCmdOpenLog );
            aMenuPane->DeleteMenuItem( ENSmlMenuCmdOpenSettings );

            if ( aResourceID == R_SMLSYNC_MENU_PROFILES_VIEW )
                {
                aMenuPane->DeleteMenuItem( ENSmlMenuCmdEraseProfile );
                }
            }
        else
            {
            doc->SetCurrentIndex(
                    iContainer->iProfilesListBox->CurrentItemIndex());
            if((doc->ProfileItem()->iProfileLocked))	
                {
                aMenuPane->SetItemDimmed( ENSmlMenuCmdOpenSettings , ETrue);
                if(aResourceID == R_SMLSYNC_MENU_PROFILES_VIEW)	
                   aMenuPane->SetItemDimmed( ENSmlMenuCmdEraseProfile , ETrue);
                }  
            if(!(doc->ProfileItem()->iDeleteAllowed)&& 
                            aResourceID == R_SMLSYNC_MENU_PROFILES_VIEW)	
                {
                aMenuPane->SetItemDimmed( ENSmlMenuCmdEraseProfile , ETrue);
                }
            if ( !(doc->ProfileItem()->iSynced) )
                {
                aMenuPane->SetItemDimmed( ENSmlMenuCmdOpenLog, ETrue );
                if ( configFlags != 1)
                    aMenuPane->SetItemDimmed( ENSmlMenuCmdStartSync, ETrue );
                }
            if ( configFlags != 1)
                {
                aMenuPane->SetItemDimmed( ENSmlMenuCmdStartSync, ETrue );	
                }
            }       
        }
    }

// -----------------------------------------------------------------------------
// CNSmlDMProfilesView::HandleForegroundEventL
// -----------------------------------------------------------------------------
//
void CNSmlDMProfilesView::HandleForegroundEventL(TBool aForeground)
	{
	FLOG( "CNSmlDMProfilesView::HandleForegroundEventL():Begin" );
	if( !aForeground )
		{
		FLOG("CNSmlDMProfilesView::HandleForegroundEventL() not i n FG :end");
	    return;	
		}	
	else
		{
		STATIC_CAST( CNSmlDMSyncAppUi*, iEikonEnv->EikAppUi())->CheckFotaDlL();
		}		
	

	// For Application Management View switching

	if(aForeground)
	{
		FLOG("CNSmlDMProfilesView::HandleForegroundEventL() For AM View Start");
		STATIC_CAST( CNSmlDMSyncAppUi*, iEikonEnv->EikAppUi())->CheckAMDlL();
    FLOG("CNSmlDMProfilesView::HandleForegroundEventL() For AM View End");
        }
	CAknView::HandleForegroundEventL(aForeground);
	FLOG( "[OMADM]\t CNSmlDMProfilesView::HandleForegroundEventL():completed" );
	}    

// End of File
