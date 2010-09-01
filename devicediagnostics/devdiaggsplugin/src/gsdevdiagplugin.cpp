/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  General Settings plug-in that launches an embedded 
*                 application.
*
*/


// INCLUDE FILES
#include "gsdevdiagplugin.h"
#include "gsdevdiagplugin.hrh"

#include <GSDevDiagPlugin.mbg> // Icons
#include <gsdevdiagpluginrsc.rsg>
#include <gsprivatepluginproviderids.h>

#include <StringLoader.h>
#include <bautils.h>
#include <AknNullService.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
//const TInt KGSAutodTimeIndex = 0;

_LIT( KGSDDPluginResourceFileName, "z:GSDevDiagPluginRsc.rsc" );

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ==============================

// ========================= MEMBER FUNCTIONS ================================

// ----------------------------------------------------------------------------
// CGSDevDiagPlugin::CGSDevDiagPlugin()
//
// Constructor
// ----------------------------------------------------------------------------
//
CGSDevDiagPlugin::CGSDevDiagPlugin(): iResourceLoader( *iCoeEnv ), 
    iNullService(NULL)
    {
    }


// ---------------------------------------------------------------------------
// CGSDevDiagPlugin::NewL()
//
// Symbian OS default constructor
// ---------------------------------------------------------------------------
CGSDevDiagPlugin* CGSDevDiagPlugin::NewL( TAny* /*aInitParams*/ )
    {
    CGSDevDiagPlugin* self = new( ELeave ) CGSDevDiagPlugin ();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
// CGSDevDiagPlugin::ConstructL()
//
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
void CGSDevDiagPlugin::ConstructL()
    {
    OpenLocalizedResourceFileL( KGSDDPluginResourceFileName, iResourceLoader);    
    }


// ----------------------------------------------------------------------------
// CGSDevDiagPlugin::~CGSDevDiagPlugin
//
// Destructor
// ----------------------------------------------------------------------------
CGSDevDiagPlugin::~CGSDevDiagPlugin()
    {
    iResourceLoader.Close();
    
    if ( iNullService )
    	{
        delete iNullService;
        }
    }


// ---------------------------------------------------------------------------
// TUid CGSDevDiagPlugin::Id()
//
// Returns view's ID.
// ---------------------------------------------------------------------------
TUid CGSDevDiagPlugin::Id() const
    {
    return KGSDDPluginUid;
    }

// ----------------------------------------------------------------------------
// CGSDevDiagPlugin::GetCaption
//
// Return application/view caption.
// ----------------------------------------------------------------------------
//
void CGSDevDiagPlugin::GetCaptionL( TDes& aCaption ) const
    {
    // the resource file is already opened.
    HBufC* result = StringLoader::LoadL( R_GS_DIAGNOSTICS_VIEW_CAPTION );

    aCaption.Copy( *result );
    delete result;
    }


// ----------------------------------------------------------------------------
// CGSDevDiagPlugin::PluginProviderCategory
//
// A means to identify the location of this plug-in in the framework.
// ----------------------------------------------------------------------------
//
TInt CGSDevDiagPlugin::PluginProviderCategory() const
    {
    //To identify internal plug-ins.
    return KGSPluginProviderInternal;
    }


// -----------------------------------------------------------------------------
// CGSDevDiagPlugin::ItemType (from CGSPluginInterface)
//
// -----------------------------------------------------------------------------
//
TGSListboxItemTypes CGSDevDiagPlugin::ItemType()
    {
    return EGSItemTypeSettingDialog;
    }


// -----------------------------------------------------------------------------
// CGSDevDiagPlugin::GetValue (from CGSPluginInterface)
//
// -----------------------------------------------------------------------------
//
void CGSDevDiagPlugin::GetValue( const TGSPluginValueKeys /*aKey*/,
                                      TDes& /*aValue*/ )
    {
    }
// ---------------------------------------------------------------------------
// CAuthtypePlugin::CreateIconL
// ---------------------------------------------------------------------------
//
CGulIcon* CGSDevDiagPlugin::CreateIconL( const TUid aIconType )
    {

   //EMbm<Mbm_file_name><Bitmap_name>
    CGulIcon* icon;
    TParse* fp = new( ELeave ) TParse();
    CleanupStack::PushL( fp );
    fp->Set( KGSDDPluginIconDirAndName, &KDC_BITMAP_DIR, NULL );

    if ( aIconType == KGSIconTypeLbxItem )
        {
        icon = AknsUtils::CreateGulIconL(
        AknsUtils::SkinInstance(),
        KAknsIIDDefault,
        fp->FullName(),
        EMbmGsdevdiagpluginQgn_prop_set_conn_data, 
        EMbmGsdevdiagpluginQgn_prop_set_conn_data_mask );
        }    
    else
        {
        icon = CGSPluginInterface::CreateIconL( aIconType );
        }
        
    CleanupStack::PopAndDestroy( fp );
    return icon;
	
   }
   
// ----------------------------------------------------------------------------
// CGSDevDiagPlugin::DoActivateL
//
// Do nothing. See HandleSelection.
// ----------------------------------------------------------------------------
//
void CGSDevDiagPlugin::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
                                     TUid /*aCustomMessageId*/,
                                     const TDesC8& /*aCustomMessage*/ )
    {
    }

// ----------------------------------------------------------------------------
// CGSDevDiagPlugin::DoDeactivate
//
// Do nothing. See HandleSelection.
// ----------------------------------------------------------------------------
//
void CGSDevDiagPlugin::DoDeactivate()
    {
    }


// ----------------------------------------------------------------------------
// Opens the resource file, because it is not loaded automatically by the
// GS framework.
// ----------------------------------------------------------------------------
//
void CGSDevDiagPlugin::OpenLocalizedResourceFileL(
    const TDesC& aResourceFileName,
    RConeResourceLoader& aResourceLoader )
    {
    RFs fsSession;
    User::LeaveIfError( fsSession.Connect() );

    // Find the resource file:
    TParse parse;
    parse.Set( aResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL );
    TFileName fileName( parse.FullName() );

    // Get language of resource file:
    BaflUtils::NearestLanguageFile( fsSession, fileName );

    // Open resource file:
    aResourceLoader.OpenL( fileName );

    // If leave occurs before this, close is called automatically when the
    // thread exits.
    fsSession.Close();
    }
    
        
 // -----------------------------------------------------------------------------
 // GS calls this function when plug-in is opened.
 //
 // -----------------------------------------------------------------------------
 //          
void CGSDevDiagPlugin::HandleSelection(
     const TGSSelectionTypes /*aSelectionType*/ )
     {
     TRAP_IGNORE( LaunchDiagnosticsAppL() );
     }
 

 // -----------------------------------------------------------------------------
 // CGSDevDiagPlugin::LaunchDiagnosticsAppL
 //
 // Checks first is the application already running.
 // -----------------------------------------------------------------------------
 //
void CGSDevDiagPlugin::LaunchDiagnosticsAppL()
    {
    // Get the correct application data
    RWsSession ws;
    User::LeaveIfError(ws.Connect());
    CleanupClosePushL(ws);
 
    // Find the task with uid
    TApaTaskList taskList(ws);
    TApaTask task = taskList.FindApp( KGsDiagnosticsAppUid );
 
    if ( task.Exists() )
        {
        task.BringToForeground();
        }
    else
        {
        //Launch Diagnostics application as embedded
        TAppInfo app( KGsDiagnosticsAppUid, KGSDiagnosticsApp );
        EmbedAppL( app );
        }
    CleanupStack::PopAndDestroy(&ws);
    }


 // -----------------------------------------------------------------------------
 // Handle Diagnostics application exit. GS should be closed down also.
 //
 // -----------------------------------------------------------------------------
 //
void CGSDevDiagPlugin::HandleServerAppExit(TInt aReason )
   {
   if ( aReason != EAknSoftkeyBack )
        {
        TRAP_IGNORE(AppUi()->HandleCommandL( EAknSoftkeyExit ));
        }      
   }


 // -----------------------------------------------------------------------------
 // Use null service to launch the embedded application
 //
 // -----------------------------------------------------------------------------
 //
void CGSDevDiagPlugin::EmbedAppL( const TAppInfo& aApp )
    {
    if ( iNullService )
        {
        delete iNullService;
        iNullService = NULL;
        }
        
    iNullService = CAknNullService::NewL( aApp.iUid, this );
    }
 

// End of File
