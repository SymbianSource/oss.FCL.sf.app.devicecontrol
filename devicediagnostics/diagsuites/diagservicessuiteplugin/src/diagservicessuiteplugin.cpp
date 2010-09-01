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
* Description:  This is the implementation of the Services Diagnostics
*                Suite, which is used to contain all tests and suites.
*
*/


// System Include Files
#include <DiagFrameworkDebug.h>             // Debugging Macros
#include <StringLoader.h>                   // StringLoader
#include <gulicon.h>                        // CGulIcon
#include <AknIconUtils.h>                   // AknIconUtils
#include <devdiagservicessuitepluginrsc.rsg>   // Resource Definitions
#include <devdiagservicessuiteplugin.mbg>      // Icon Indices

// User Include Files
#include "diagservicessuiteplugin.h"        // CDiagServicesSuitePlugin
#include "diagservicessuiteplugin.hrh"      // UID definition
#include "diagservicessuiteplugin.pan"      // Panic

// Local Constants
_LIT( KDiagServicesSuitePluginResourceFileName,
      "z:devdiagservicessuitepluginrsc.rsc" );
const TUid KDiagServicesSuitePluginUid = { _UID3 };


// ============================ GLOBAL FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Static two-phase constructor.
// ---------------------------------------------------------------------------
//
MDiagPlugin* CDiagServicesSuitePlugin::NewL( TAny* aInitParams )
    {
    LOGSTRING2( "CDiagServicesSuitePlugin::NewL( 0x%x )", aInitParams )

    __ASSERT_ALWAYS( aInitParams,
                     Panic( EDiagServicesSuitePluginConstruction ) );

    // Construct the plugin.  The base class will take ownership of the
    // initialization parameters.
    CDiagPluginConstructionParam* param =
            static_cast< CDiagPluginConstructionParam* >( aInitParams );

    CleanupStack::PushL( param );
    CDiagServicesSuitePlugin* self =
        new( ELeave ) CDiagServicesSuitePlugin( param );
    CleanupStack::Pop( param );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CDiagServicesSuitePlugin::~CDiagServicesSuitePlugin()
    {
    LOGSTRING( "CDiagServicesSuitePlugin::~CDiagServicesSuitePlugin()" )

    }

// ---------------------------------------------------------------------------
// The default constructor.
// ---------------------------------------------------------------------------
//
CDiagServicesSuitePlugin::CDiagServicesSuitePlugin(
    CDiagPluginConstructionParam* aParam )
:   CDiagSuitePluginBase( aParam )
    {
    LOGSTRING2(
        "CDiagServicesSuitePlugin::CDiagServicesSuitePlugin( 0x%x )",
        aParam )

    }

// ---------------------------------------------------------------------------
// The second phase constructor.
// ---------------------------------------------------------------------------
//
void CDiagServicesSuitePlugin::ConstructL()
    {
    LOGSTRING( "CDiagServicesSuitePlugin::ConstructL()" );
    BaseConstructL ( KDiagServicesSuitePluginResourceFileName );
    }

// ---------------------------------------------------------------------------
// From class MDiagPlugin.
// Returns whether the plugin should be displayed or not.
// ---------------------------------------------------------------------------
TBool CDiagServicesSuitePlugin::IsVisible() const
    {
    return ETrue;
    }

// ---------------------------------------------------------------------------
// From class MDiagPlugin.
// Returns the plugin name.
// ---------------------------------------------------------------------------
HBufC* CDiagServicesSuitePlugin::GetPluginNameL(
    TNameLayoutType aLayoutType ) const
    {
    switch ( aLayoutType )
        {
        case ENameLayoutListLargeGraphic:
            return StringLoader::LoadL( R_DIAG_SERVICES_SUITE_LIST_LARGE_GRAPHIC );

        case ENameLayoutHeadingPane:
            return StringLoader::LoadL( R_DIAG_SERVICES_SUITE_HEADING_PANE );

        case ENameLayoutPopupInfoPane:
            return StringLoader::LoadL( R_DIAG_SERVICES_SUITE_POPUP_INFO_PANE );

        case ENameLayoutTitlePane:
            return StringLoader::LoadL( R_DIAG_SERVICES_SUITE_TITLE_PANE );

        case ENameLayoutListSingleGraphic:
            return StringLoader::LoadL( R_DIAG_SERVICES_SUITE_LIST_SINGLE_GRAPHIC );

        case ENameLayoutListSingle:
            return StringLoader::LoadL( R_DIAG_SERVICES_SUITE_LIST_SINGLE );

        default:
            __ASSERT_DEBUG( EFalse,
                            Panic( EDiagServicesSuitePluginBadArgument ) );
            return StringLoader::LoadL( R_DIAG_SERVICES_SUITE_LIST_LARGE_GRAPHIC );
        }
    }

// ---------------------------------------------------------------------------
// From class MDiagPlugin.
// Returns the plugin's UID.
// ---------------------------------------------------------------------------
TUid CDiagServicesSuitePlugin::Uid() const
    {
    return KDiagServicesSuitePluginUid;
    }

// ---------------------------------------------------------------------------
// From class MDiagPlugin.
// Load the plugin's icon.
// ---------------------------------------------------------------------------
//
CGulIcon* CDiagServicesSuitePlugin::CreateIconL() const
    {
    LOGSTRING( "CDiagServicesSuitePlugin::CreateIconL()" )

    _LIT( KDiagServicesSuitePluginIconFile,
          "\\resource\\apps\\devdiagservicessuiteplugin.mif" );

    // Load the icon.
    CFbsBitmap* bitmap;
    CFbsBitmap* mask;
    AknIconUtils::CreateIconL(
        bitmap,
        mask,
        KDiagServicesSuitePluginIconFile(),
        EMbmDevdiagservicessuitepluginQgn_prop_cp_diag_service,
        EMbmDevdiagservicessuitepluginQgn_prop_cp_diag_service_mask );

    // Create the icon.
    return CGulIcon::NewL( bitmap, mask );
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Active object run handler.
// ---------------------------------------------------------------------------
void CDiagServicesSuitePlugin::RunL()
    {
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Active object cancel.
// ---------------------------------------------------------------------------
void CDiagServicesSuitePlugin::DoCancel()
    {
    }

// End of File

