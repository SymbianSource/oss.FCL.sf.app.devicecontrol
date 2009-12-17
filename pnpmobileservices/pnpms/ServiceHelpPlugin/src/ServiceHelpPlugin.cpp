/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ServiceHelpPlugin implementation.
*
*/


// User includes
#include    "ServiceHelpPlugin.h"

#include    <gsparentplugin.h>
#include    <gscommon.hrh>
#include    <PnpProvisioning.rsg>
#include    <ServiceHelpPlugin.mbg>
#include    <gsprivatepluginproviderids.h>
#include    <gsfwviewuids.h>
#include    <gsbaseview.h>

// System includes
#include    <AknWaitDialog.h>
#include    <aknViewAppUi.h>
#include    <bautils.h>
#include    <StringLoader.h>
#include    <badesca.h>
#include    <PnpProvUtil.h>
#include    <featmgr.h>
#include    "ServicePluginLogger.h"



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CServiceHelpPlugin::CServiceHelpPlugin()
// Constructor
// ---------------------------------------------------------------------------
//
CServiceHelpPlugin::CServiceHelpPlugin()
    : iResources( *iCoeEnv )
    {
    }


// ---------------------------------------------------------------------------
// CServiceHelpPlugin::~CServiceHelpPlugin()
// Destructor
// ---------------------------------------------------------------------------
//
CServiceHelpPlugin::~CServiceHelpPlugin()
    {
    iResources.Close();

    
    }


// ---------------------------------------------------------------------------
// CServiceHelpPlugin::ConstructL(const TRect& aRect)
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
void CServiceHelpPlugin::ConstructL()
    {	
       LOGSTRING("ContructL");
     
	OpenLocalizedResourceFileL( KOperatorLogoResourceFileName, iResources );
    
     
    }


// ---------------------------------------------------------------------------
// CServiceHelpPlugin::NewL()
// Static constructor
// ---------------------------------------------------------------------------
//
CServiceHelpPlugin* CServiceHelpPlugin::NewL( TAny* /*aInitParams*/ )
    {
 
    
    LOGSTRING("NewL - CServiceHelpPlugin ");
    CServiceHelpPlugin* self = new( ELeave ) CServiceHelpPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;

    
    }


// -----------------------------------------------------------------------------
// CServiceHelpPlugin::Id
// -----------------------------------------------------------------------------
//
TUid CServiceHelpPlugin::Id() const
    {
    return KServiceHelpPluginUID;
    }


// -----------------------------------------------------------------------------
// CServiceHelpPlugin::DoActivateL
// -----------------------------------------------------------------------------
//
void CServiceHelpPlugin::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
                                  TUid /*aCustomMessageId*/,
                                  const TDesC8& /*aCustomMessage*/ )
    {

    }


// -----------------------------------------------------------------------------
// CServiceHelpPlugin::DoDeactivate
// -----------------------------------------------------------------------------
//
void CServiceHelpPlugin::DoDeactivate()
    {


    }


// ========================= From CGSPluginInterface ==================


// -----------------------------------------------------------------------------
// CServiceHelpPlugin::GetCaptionL
// -----------------------------------------------------------------------------
//
void CServiceHelpPlugin::GetCaptionL( TDes& aCaption ) const
    {    

    LOGSTRING("NewL - GetCaptionL ");
    HBufC* result = StringLoader::LoadL( R_TEXT_PLUGIN_CAPTION );
    aCaption.Copy( *result );
    delete result;

    LOGSTRING("NewL - GetCaptionL - Done");
    
    }


// -----------------------------------------------------------------------------
// CServiceHelpPlugin::PluginProviderCategory
// -----------------------------------------------------------------------------
//
TInt CServiceHelpPlugin::PluginProviderCategory() const
    {
    return KGSPluginProviderInternal;
    }


// -----------------------------------------------------------------------------
// CServiceHelpPlugin::ItemType()
// -----------------------------------------------------------------------------
//
TGSListboxItemTypes CServiceHelpPlugin::ItemType()
    {
    return EGSItemTypeSettingDialog;
    }


// -----------------------------------------------------------------------------
// CServiceHelpPlugin::GetValue()
// -----------------------------------------------------------------------------
//
void CServiceHelpPlugin::GetValue( const TGSPluginValueKeys /*aKey*/,
                                      TDes& /*aValue*/ )
    {
    }


// -----------------------------------------------------------------------------
// CServiceHelpPlugin::HandleSelection()
// -----------------------------------------------------------------------------
//
void CServiceHelpPlugin::HandleSelection(
    const TGSSelectionTypes /*aSelectionType*/ )
    {
    
    TRAP_IGNORE(  LaunchOnlineSupportL() );
   
    }



// ---------------------------------------------------------------------------
// CServiceHelpPlugin::CreateIconL
// ---------------------------------------------------------------------------
//
CGulIcon* CServiceHelpPlugin::CreateIconL( const TUid aIconType )
    {
   
    LOGSTRING("NewL - CreateIconL ");
   //EMbm<Mbm_file_name><Bitmap_name>
    CGulIcon* icon;
    TParse* fp = new( ELeave ) TParse();
    CleanupStack::PushL( fp );
    fp->Set( KServiceHelpPluginIconDirAndName, &KDC_BITMAP_DIR, NULL );

    if ( aIconType == KGSIconTypeLbxItem )
        {
        icon = AknsUtils::CreateGulIconL(
        AknsUtils::SkinInstance(),
        KAknsIIDQgnPropSetAppsWml,
        fp->FullName(),//qgn_prop_set_apps_wml.svg
        EMbmServicehelppluginQgn_prop_set_apps_wml,
        EMbmServicehelppluginQgn_prop_set_apps_wml_mask );
        }    
    else
        {
        icon = CGSPluginInterface::CreateIconL( aIconType );
        }
        
    CleanupStack::PopAndDestroy( fp );

    LOGSTRING("NewL - CreateIconL - Done ");
    return icon;
	
   }


// -----------------------------------------------------------------------------
// CServiceHelpPlugin::OpenLocalizedResourceFileL()
// -----------------------------------------------------------------------------
//
void CServiceHelpPlugin::OpenLocalizedResourceFileL(
    const TDesC& aResourceFileName,
    RConeResourceLoader& aResourceLoader )
    {
    
    LOGSTRING("NewL - OpenLocalizedResourceFileL ");

    RFs fsSession;
    User::LeaveIfError( fsSession.Connect() );

    CleanupClosePushL(fsSession);

    // Find the resource file:
    TParse parse;
    parse.Set( aResourceFileName, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName fileName( parse.FullName() );

    // Get language of resource file:
    BaflUtils::NearestLanguageFile( fsSession, fileName );

    // Open resource file:
    aResourceLoader.OpenL( fileName );

    
    CleanupStack::PopAndDestroy(); //fssession

    LOGSTRING("NewL - OpenLocalizedResourceFileL End ");

    }
    
// -----------------------------------------------------------------------------
// CServiceHelpPlugin::LaunchonlinesupportL()
// -----------------------------------------------------------------------------
//

void CServiceHelpPlugin::LaunchOnlineSupportL()
    {

      LOGSTRING("NewL - LaunchOnlineSupportL ");

      // To Launch online suppport Use PNPMS Provisioning utilities for PNPMS services
      TBufC<1> Nullval(_L(""));
      
      CPnpProvUtil *prov = CPnpProvUtil::NewLC();
      TRAP_IGNORE(prov->LaunchOnlineSupportL(Nullval, EUserTriggered, EStartInteractive,  EFalse, Nullval)); 
      CleanupStack::PopAndDestroy();

       LOGSTRING("NewL - LaunchOnlineSupportL END ");

    }



// End of file
