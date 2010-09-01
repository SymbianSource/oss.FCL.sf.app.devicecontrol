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
* Description:  Device updates Ecom plugin Implementation.
*
*/


//System Includes
#include <e32std.h>
#include <implementationproxy.h>
#include <StringLoader.h>
#include <bautils.h>
#include <AknsUtils.h>
#include <gulicon.h>
#include <akntitle.h>
#include <e32property.h>
// Required only for embedded app
#include <AknNullService.h>
#include <apgtask.h>
#include <flogger.h>
#include <nsmldmsync.mbg>
#include <omadmcppluginresource.rsg>
// User includes
#include "OmaDMCPPlugin.h"
#include "NSmlDMSyncApp.h"
#include "nsmldmsyncinternalpskeys.h"
#include <gsprivatepluginproviderids.h>

_LIT(KOmaDmIconFileName, "Z:\\resource\\apps\\nsmldmsync.mif");
_LIT(KOmaDmResFileName,  "Z:\\resource\\OmaDmCPPluginResource.rsc" );

// ---------------------------------------------------------------------------------------------
// OmaDmCPPlugin::NewLC
// returns an object of OmaDmCPPlugin
// ---------------------------------------------------------------------------------------------
//
CGSPluginInterface* COmaDmCPPlugin::NewL()
	{
	
	COmaDmCPPlugin* self = new(ELeave) COmaDmCPPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
	CleanupStack::Pop(self);
	return (CGSPluginInterface*) self;
	}

// ---------------------------------------------------------------------------------------------
// OmaDmCPPlugin::Id()
// returns the view id
// ---------------------------------------------------------------------------------------------
//
TUid COmaDmCPPlugin::Id() const
	{
      return TUid::Uid( KUidOmaDmCPPluginDll );	
	}

// ---------------------------------------------------------------------------------------------
// OmaDmCPPlugin::ConstructL
// ---------------------------------------------------------------------------------------------
//	
void COmaDmCPPlugin::ConstructL()
	{
	BaseConstructL();
    env = CEikonEnv::Static();

	TFileName fileName(KOmaDmResFileName);
	
	BaflUtils::NearestLanguageFile(env->FsSession(), fileName);

	iResId = env->AddResourceFileL(fileName);
	}


// ---------------------------------------------------------------------------------------------
// OmaDmCPPlugin::OmaDmCPPlugin
// ---------------------------------------------------------------------------------------------
//    
COmaDmCPPlugin::COmaDmCPPlugin()
	{
    // Nothing
	}

// ---------------------------------------------------------------------------------------------
// COmaDmCPPlugin::~COmaDmCPPlugin
// ---------------------------------------------------------------------------------------------
//
COmaDmCPPlugin::~COmaDmCPPlugin()
	{
		TInt value =0;
		TInt r1=RProperty::Get(KPSUidNSmlDMSyncApp,KNSmlDMSyncUiLaunchKey,value);   
		if(value ==2 && r1 == KErrNone)
		{
		 CloseDmUi();
		} 
    if (iResId)
	   {
        env->DeleteResourceFile(iResId);
	   }	
	if ( iNullService )
        {
            delete iNullService;
            iNullService = NULL;
        }
	}

// ---------------------------------------------------------------------------------------------
// COmaDmCPPlugin::GetCaptionL
// returns the caption to be shown in GS view
// ---------------------------------------------------------------------------------------------
//
void COmaDmCPPlugin::GetCaptionL( TDes& aCaption ) const
	{	
	StringLoader::Load( aCaption, R_OMADM_CP_TITLE);
	}

// ---------------------------------------------------------------------------------------------
// COmaDmCPPlugin::CreateIconL
// Returns the icon to be shown in GS view
// ---------------------------------------------------------------------------------------------
CGulIcon* COmaDmCPPlugin::CreateIconL( const TUid /*aIconType*/ )
	{
	CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;                                               
 
    MAknsSkinInstance* instance = AknsUtils::SkinInstance();    
    AknsUtils::CreateIconL(instance, KAknsIIDQgnPropCpDevUpdate, bitmap, mask,
    					KOmaDmIconFileName,
    					EMbmNsmldmsyncQgn_prop_cp_dev_update,
    				EMbmNsmldmsyncQgn_prop_cp_dev_update_mask);
    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    icon->SetBitmapsOwnedExternally( EFalse );
    bitmap = NULL;
    mask = NULL;      
    return icon;    
	}


// -----------------------------------------------------------------------------
// COmaDmCPPlugin::PluginProviderCategory
// -----------------------------------------------------------------------------
//
TInt COmaDmCPPlugin::PluginProviderCategory() const
	{
    return KGSPluginProviderInternal;
	}

// -----------------------------------------------------------------------------
// COmaDmCPPlugin::ItemType()
// -----------------------------------------------------------------------------
//
TGSListboxItemTypes COmaDmCPPlugin::ItemType()
	{
    return EGSItemTypeSettingDialog;
	}


// -----------------------------------------------------------------------------
// COmaDmCPPlugin::HandleSelection()
// -----------------------------------------------------------------------------
//
void COmaDmCPPlugin::HandleSelection(
    const TGSSelectionTypes /*aSelectionType*/ )
	{    
	TRAP_IGNORE( OpenDmAppL() );    
	}

// -----------------------------------------------------------------------------
// COmaDmCPPlugin::OpenDmAppL()
// -----------------------------------------------------------------------------
//
void COmaDmCPPlugin::OpenDmAppL()
	{   
	static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
   	static _LIT_SECURITY_POLICY_C1(KAllowWriteDeviceDataPolicy, ECapabilityWriteDeviceData);
    TInt r=RProperty::Define(KPSUidNSmlDMSyncApp,KNSmlDMSyncUiLaunchKey,RProperty::EInt,KAllowAllPolicy,KAllowWriteDeviceDataPolicy);
    TInt value=2;
	if ( r!=KErrNone && r!=KErrAlreadyExists )
       {
       User::LeaveIfError(r);
       }    
       
    RWsSession ws;
    User::LeaveIfError( ws.Connect() );
    CleanupClosePushL( ws );
    // Find the task with uid
    TApaTaskList taskList(ws);
    TApaTask task = taskList.FindApp( KUidSmlSyncApp );
    
    if ( task.Exists() )
        {    	    
        task.BringToForeground();        
        }
    else
        {
        // Launch DM application as embedded.        
        TInt r1=RProperty::Set(KPSUidNSmlDMSyncApp,KNSmlDMSyncUiLaunchKey,value);         
        iNullService = CAknNullService::NewL( KUidSmlSyncApp, this );        
        }
    CleanupStack::PopAndDestroy();  // ws
    
	}

// -----------------------------------------------------------------------------
// COmaDmCPPlugin::DoActivateL
// -----------------------------------------------------------------------------
//
void COmaDmCPPlugin::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
                                  TUid /*aCustomMessageId*/,
                                  const TDesC8& /*aCustomMessage*/ )
	{
    // Nothing
	}

// -----------------------------------------------------------------------------
// COmaDmCPPlugin::DoDeactivate
// -----------------------------------------------------------------------------
//
void COmaDmCPPlugin::DoDeactivate()
	{
    // Closing embedded app
    if ( iNullService )
        {
            delete iNullService;
            iNullService = NULL;
        }
    
	}


// Constants
const TImplementationProxy KImplementationTable[] = 
	{
    IMPLEMENTATION_PROXY_ENTRY( KUidOmaDmCPPluginImpl, COmaDmCPPlugin::NewL ) // used by GS plugin
	};

// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// Gate/factory function
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(  TInt& aTableCount )
	{
	aTableCount = sizeof( KImplementationTable ) 
                / sizeof( TImplementationProxy );
	return KImplementationTable;
	}
// ---------------------------------------------------------------------------------------------
// OmaDmCPPlugin::CloseDmUi
// closes DM Ui
// ---------------------------------------------------------------------------------------------	
void COmaDmCPPlugin:: CloseDmUi()
{
	  
    RWsSession ws  = CEikonEnv::Static()->WsSession();
    TApaTaskList taskList( ws );
    TApaTask task = taskList.FindApp( KUidSmlSyncApp  );
      if (task.Exists())
      {
    	task.EndTask();
      }
	
}	
// End of File

