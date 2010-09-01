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
* Description:  This is the implementation of the Coverage Diagnostics
*                Suite, which is used to contain all tests and suites.
*
*/


// System Include Files
#include <DiagFrameworkDebug.h>             // Debugging Macros
#include <StringLoader.h>                   // StringLoader
#include <gulicon.h>                        // CGulIcon
#include <AknIconUtils.h>                   // AknIconUtils
#include <devdiagcoveragesuitepluginrsc.rsg>   // Resource Definitions
#include <devdiagcoveragesuiteplugin.mbg>      // Icon Indices

// User Include Files
#include "diagcoveragesuiteplugin.h"        // CDiagCoverageSuitePlugin
#include "diagcoveragesuiteplugin.hrh"      // UID definition
#include "diagcoveragesuiteplugin.pan"      // Panic

// Local Constants
_LIT( KDiagCoverageSuitePluginResourceFileName,
      "z:devdiagcoveragesuitepluginrsc.rsc" );
const TUid KDiagCoverageSuitePluginUid = { _UID3 };


// ============================ GLOBAL FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Static two-phase constructor.
// ---------------------------------------------------------------------------
//
MDiagPlugin* CDiagCoverageSuitePlugin::NewL( TAny* aInitParams )
    {
    LOGSTRING2( "CDiagCoverageSuitePlugin::NewL( 0x%x )", aInitParams )

    __ASSERT_ALWAYS( aInitParams,
                     Panic( EDiagCoverageSuitePluginConstruction ) );

    // Construct the plugin.  The base class will take ownership of the
    // initialization parameters.
    CDiagPluginConstructionParam* param =
            static_cast< CDiagPluginConstructionParam* >( aInitParams );

    CleanupStack::PushL( param );
    CDiagCoverageSuitePlugin* self =
        new( ELeave ) CDiagCoverageSuitePlugin( param );
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
CDiagCoverageSuitePlugin::~CDiagCoverageSuitePlugin()
    {
    LOGSTRING( "CDiagCoverageSuitePlugin::~CDiagCoverageSuitePlugin()" )

    }

// ---------------------------------------------------------------------------
// The default constructor.
// ---------------------------------------------------------------------------
//
CDiagCoverageSuitePlugin::CDiagCoverageSuitePlugin(
    CDiagPluginConstructionParam* aParam )
:   CDiagSuitePluginBase( aParam )
    {
    LOGSTRING2(
        "CDiagCoverageSuitePlugin::CDiagCoverageSuitePlugin( 0x%x )",
        aParam )

    }

// ---------------------------------------------------------------------------
// The second phase constructor.
// ---------------------------------------------------------------------------
//
void CDiagCoverageSuitePlugin::ConstructL()
    {
    LOGSTRING( "CDiagCoverageSuitePlugin::ConstructL()" );
    BaseConstructL ( KDiagCoverageSuitePluginResourceFileName );
    }

// ---------------------------------------------------------------------------
// From class MDiagPlugin.
// Returns whether the plugin should be displayed or not.
// ---------------------------------------------------------------------------
TBool CDiagCoverageSuitePlugin::IsVisible() const
    {
    return ETrue;
    }

// ---------------------------------------------------------------------------
// From class MDiagPlugin.
// Returns the plugin name.
// ---------------------------------------------------------------------------
HBufC* CDiagCoverageSuitePlugin::GetPluginNameL(
    TNameLayoutType aLayoutType ) const
    {
    switch ( aLayoutType )
        {
        case ENameLayoutListLargeGraphic:
            return StringLoader::LoadL( R_DIAG_COVERAGE_SUITE_LIST_LARGE_GRAPHIC );

        case ENameLayoutHeadingPane:
            return StringLoader::LoadL( R_DIAG_COVERAGE_SUITE_HEADING_PANE );

        case ENameLayoutPopupInfoPane:
            return StringLoader::LoadL( R_DIAG_COVERAGE_SUITE_POPUP_INFO_PANE );

        case ENameLayoutTitlePane:
            return StringLoader::LoadL( R_DIAG_COVERAGE_SUITE_TITLE_PANE );

        case ENameLayoutListSingleGraphic:
            return StringLoader::LoadL( R_DIAG_COVERAGE_SUITE_LIST_SINGLE_GRAPHIC );

        case ENameLayoutListSingle:
            return StringLoader::LoadL( R_DIAG_COVERAGE_SUITE_LIST_SINGLE );

        default:
            __ASSERT_DEBUG( EFalse,
                            Panic( EDiagCoverageSuitePluginBadArgument ) );
            return StringLoader::LoadL( R_DIAG_COVERAGE_SUITE_LIST_LARGE_GRAPHIC );
        }
    }

// ---------------------------------------------------------------------------
// From class MDiagPlugin.
// Returns the plugin's UID.
// ---------------------------------------------------------------------------
TUid CDiagCoverageSuitePlugin::Uid() const
    {
    return KDiagCoverageSuitePluginUid;
    }

// ---------------------------------------------------------------------------
// From class MDiagPlugin.
// Load the plugin's icon.
// ---------------------------------------------------------------------------
//
CGulIcon* CDiagCoverageSuitePlugin::CreateIconL() const
    {
    LOGSTRING( "CDiagCoverageSuitePlugin::CreateIconL()" )

    _LIT( KDiagCoverageSuitePluginIconFile,
          "\\resource\\apps\\devdiagcoveragesuiteplugin.mif" );

    // Load the icon.
    CFbsBitmap* bitmap;
    CFbsBitmap* mask;
    AknIconUtils::CreateIconL(
        bitmap,
        mask,
        KDiagCoverageSuitePluginIconFile(),
        EMbmDevdiagcoveragesuitepluginQgn_prop_cp_diag_coverage,
        EMbmDevdiagcoveragesuitepluginQgn_prop_cp_diag_coverage_mask );

    // Create the icon.
    return CGulIcon::NewL( bitmap, mask );
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Active object run handler.
// ---------------------------------------------------------------------------
void CDiagCoverageSuitePlugin::RunL()
    {
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Active object cancel.
// ---------------------------------------------------------------------------
void CDiagCoverageSuitePlugin::DoCancel()
    {
    }

// End of File
