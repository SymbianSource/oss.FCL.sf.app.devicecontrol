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
* Description:  This is the implementation of the Connectivity Diagnostics
*                Suite, which is used to contain all tests and suites.
*
*/


// System Include Files
#include <DiagFrameworkDebug.h>             // Debugging Macros
#include <StringLoader.h>                   // StringLoader
#include <gulicon.h>                        // CGulIcon
#include <AknIconUtils.h>                   // AknIconUtils
#include <devdiagconnectivitysuitepluginrsc.rsg>   // Resource Definitions
#include <devdiagconnectivitysuiteplugin.mbg>      // Icon Indices

// User Include Files
#include "diagconnectivitysuiteplugin.h"        // CDiagConnectivitySuitePlugin
#include "diagconnectivitysuiteplugin.hrh"      // UID definition
#include "diagconnectivitysuiteplugin.pan"      // Panic

// Local Constants
_LIT( KDiagConnectivitySuitePluginResourceFileName,
      "z:devdiagconnectivitysuitepluginrsc.rsc" );
const TUid KDiagConnectivitySuitePluginUid = { _UID3 };


// ============================ GLOBAL FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Static two-phase constructor.
// ---------------------------------------------------------------------------
//
MDiagPlugin* CDiagConnectivitySuitePlugin::NewL( TAny* aInitParams )
    {
    LOGSTRING2( "CDiagConnectivitySuitePlugin::NewL( 0x%x )", aInitParams )

    __ASSERT_ALWAYS( aInitParams,
                     Panic( EDiagConnectivitySuitePluginConstruction ) );

    // Construct the plugin.  The base class will take ownership of the
    // initialization parameters.
    CDiagPluginConstructionParam* param =
            static_cast< CDiagPluginConstructionParam* >( aInitParams );

    CleanupStack::PushL( param );
    CDiagConnectivitySuitePlugin* self =
        new( ELeave ) CDiagConnectivitySuitePlugin( param );
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
CDiagConnectivitySuitePlugin::~CDiagConnectivitySuitePlugin()
    {
    LOGSTRING( "CDiagConnectivitySuitePlugin::~CDiagConnectivitySuitePlugin()" )

    }

// ---------------------------------------------------------------------------
// The default constructor.
// ---------------------------------------------------------------------------
//
CDiagConnectivitySuitePlugin::CDiagConnectivitySuitePlugin(
    CDiagPluginConstructionParam* aParam )
:   CDiagSuitePluginBase( aParam )
    {
    LOGSTRING2(
        "CDiagConnectivitySuitePlugin::CDiagConnectivitySuitePlugin( 0x%x )",
        aParam )

    }

// ---------------------------------------------------------------------------
// The second phase constructor.
// ---------------------------------------------------------------------------
//
void CDiagConnectivitySuitePlugin::ConstructL()
    {
    LOGSTRING( "CDiagConnectivitySuitePlugin::ConstructL()" );
    BaseConstructL ( KDiagConnectivitySuitePluginResourceFileName );
    }

// ---------------------------------------------------------------------------
// From class MDiagPlugin.
// Returns whether the plugin should be displayed or not.
// ---------------------------------------------------------------------------
TBool CDiagConnectivitySuitePlugin::IsVisible() const
    {
    return ETrue;
    }

// ---------------------------------------------------------------------------
// From class MDiagPlugin.
// Returns the plugin name.
// ---------------------------------------------------------------------------
HBufC* CDiagConnectivitySuitePlugin::GetPluginNameL(
    TNameLayoutType aLayoutType ) const
    {
    switch ( aLayoutType )
        {
        case ENameLayoutListLargeGraphic:
            return StringLoader::LoadL( R_DIAG_CONNECTIVITY_SUITE_LIST_LARGE_GRAPHIC );

        case ENameLayoutHeadingPane:
            return StringLoader::LoadL( R_DIAG_CONNECTIVITY_SUITE_HEADING_PANE );

        case ENameLayoutPopupInfoPane:
            return StringLoader::LoadL( R_DIAG_CONNECTIVITY_SUITE_POPUP_INFO_PANE );

        case ENameLayoutTitlePane:
            return StringLoader::LoadL( R_DIAG_CONNECTIVITY_SUITE_TITLE_PANE );

        case ENameLayoutListSingleGraphic:
            return StringLoader::LoadL( R_DIAG_CONNECTIVITY_SUITE_LIST_SINGLE_GRAPHIC );

        case ENameLayoutListSingle:
            return StringLoader::LoadL( R_DIAG_CONNECTIVITY_SUITE_LIST_SINGLE );

        default:
            __ASSERT_DEBUG( EFalse,
                            Panic( EDiagConnectivitySuitePluginBadArgument ) );
            return StringLoader::LoadL( R_DIAG_CONNECTIVITY_SUITE_LIST_LARGE_GRAPHIC );
        }
    }

// ---------------------------------------------------------------------------
// From class MDiagPlugin.
// Returns the plugin's UID.
// ---------------------------------------------------------------------------
TUid CDiagConnectivitySuitePlugin::Uid() const
    {
    return KDiagConnectivitySuitePluginUid;
    }

// ---------------------------------------------------------------------------
// From class MDiagPlugin.
// Load the plugin's icon.
// ---------------------------------------------------------------------------
//
CGulIcon* CDiagConnectivitySuitePlugin::CreateIconL() const
    {
    LOGSTRING( "CDiagConnectivitySuitePlugin::CreateIconL()" )

    _LIT( KDiagConnectivitySuitePluginIconFile,
          "\\resource\\apps\\devdiagconnectivitysuiteplugin.mif" );

    // Load the icon.
    CFbsBitmap* bitmap;
    CFbsBitmap* mask;
    AknIconUtils::CreateIconL(
        bitmap,
        mask,
        KDiagConnectivitySuitePluginIconFile(),
        EMbmDevdiagconnectivitysuitepluginQgn_prop_cp_diag_conn,
        EMbmDevdiagconnectivitysuitepluginQgn_prop_cp_diag_conn_mask );

    // Create the icon.
    return CGulIcon::NewL( bitmap, mask );
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Active object run handler.
// ---------------------------------------------------------------------------
void CDiagConnectivitySuitePlugin::RunL()
    {
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Active object cancel.
// ---------------------------------------------------------------------------
void CDiagConnectivitySuitePlugin::DoCancel()
    {
    }

// End of File
