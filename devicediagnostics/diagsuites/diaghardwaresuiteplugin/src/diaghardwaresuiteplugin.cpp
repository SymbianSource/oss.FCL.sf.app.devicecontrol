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
* Description:  This is the implementation of the Hardware Diagnostics
*                Suite, which is used to contain all tests and suites.
*
*/


// System Include Files
#include <DiagFrameworkDebug.h>             // Debugging Macros
#include <StringLoader.h>                   // StringLoader
#include <gulicon.h>                        // CGulIcon
#include <AknIconUtils.h>                   // AknIconUtils
#include <devdiaghardwaresuitepluginrsc.rsg>   // Resource Definitions
#include <devdiaghardwaresuiteplugin.mbg>      // Icon Indices

// User Include Files
#include "diaghardwaresuiteplugin.h"        // CDiagHardwareSuitePlugin
#include "diaghardwaresuiteplugin.hrh"      // UID definition
#include "diaghardwaresuiteplugin.pan"      // Panic

// Local Constants
_LIT( KDiagHardwareSuitePluginResourceFileName,
      "z:devdiaghardwaresuitepluginrsc.rsc" );
const TUid KDiagHardwareSuitePluginUid = { _UID3 };


// ============================ GLOBAL FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Static two-phase constructor.
// ---------------------------------------------------------------------------
//
MDiagPlugin* CDiagHardwareSuitePlugin::NewL( TAny* aInitParams )
    {
    LOGSTRING2( "CDiagHardwareSuitePlugin::NewL( 0x%x )", aInitParams )

    __ASSERT_ALWAYS( aInitParams,
                     Panic( EDiagHardwareSuitePluginConstruction ) );

    // Construct the plugin.  The base class will take ownership of the
    // initialization parameters.
    CDiagPluginConstructionParam* param =
            static_cast< CDiagPluginConstructionParam* >( aInitParams );

    CleanupStack::PushL( param );
    CDiagHardwareSuitePlugin* self =
        new( ELeave ) CDiagHardwareSuitePlugin( param );
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
CDiagHardwareSuitePlugin::~CDiagHardwareSuitePlugin()
    {
    LOGSTRING( "CDiagHardwareSuitePlugin::~CDiagHardwareSuitePlugin()" )

    }

// ---------------------------------------------------------------------------
// The default constructor.
// ---------------------------------------------------------------------------
//
CDiagHardwareSuitePlugin::CDiagHardwareSuitePlugin(
    CDiagPluginConstructionParam* aParam )
:   CDiagSuitePluginBase( aParam )
    {
    LOGSTRING2(
        "CDiagHardwareSuitePlugin::CDiagHardwareSuitePlugin( 0x%x )",
        aParam )

    }

// ---------------------------------------------------------------------------
// The second phase constructor.
// ---------------------------------------------------------------------------
//
void CDiagHardwareSuitePlugin::ConstructL()
    {
    LOGSTRING( "CDiagHardwareSuitePlugin::ConstructL()" );
    BaseConstructL ( KDiagHardwareSuitePluginResourceFileName );
    }

// ---------------------------------------------------------------------------
// From class MDiagPlugin.
// Returns whether the plugin should be displayed or not.
// ---------------------------------------------------------------------------
TBool CDiagHardwareSuitePlugin::IsVisible() const
    {
    return ETrue;
    }

// ---------------------------------------------------------------------------
// From class MDiagPlugin.
// Returns the plugin name.
// ---------------------------------------------------------------------------
HBufC* CDiagHardwareSuitePlugin::GetPluginNameL(
    TNameLayoutType aLayoutType ) const
    {
    switch ( aLayoutType )
        {
        case ENameLayoutListLargeGraphic:
            return StringLoader::LoadL( R_DIAG_HARDWARE_SUITE_LIST_LARGE_GRAPHIC );

        case ENameLayoutHeadingPane:
            return StringLoader::LoadL( R_DIAG_HARDWARE_SUITE_HEADING_PANE );

        case ENameLayoutPopupInfoPane:
            return StringLoader::LoadL( R_DIAG_HARDWARE_SUITE_POPUP_INFO_PANE );

        case ENameLayoutTitlePane:
            return StringLoader::LoadL( R_DIAG_HARDWARE_SUITE_TITLE_PANE );

        case ENameLayoutListSingleGraphic:
            return StringLoader::LoadL( R_DIAG_HARDWARE_SUITE_LIST_SINGLE_GRAPHIC );

        case ENameLayoutListSingle:
            return StringLoader::LoadL( R_DIAG_HARDWARE_SUITE_LIST_SINGLE );

        default:
            __ASSERT_DEBUG( EFalse,
                            Panic( EDiagHardwareSuitePluginBadArgument ) );
            return StringLoader::LoadL( R_DIAG_HARDWARE_SUITE_LIST_LARGE_GRAPHIC );
        }
    }

// ---------------------------------------------------------------------------
// From class MDiagPlugin.
// Returns the plugin's UID.
// ---------------------------------------------------------------------------
TUid CDiagHardwareSuitePlugin::Uid() const
    {
    return KDiagHardwareSuitePluginUid;
    }

// ---------------------------------------------------------------------------
// From class MDiagPlugin.
// Load the plugin's icon.
// ---------------------------------------------------------------------------
//
CGulIcon* CDiagHardwareSuitePlugin::CreateIconL() const
    {
    LOGSTRING( "CDiagHardwareSuitePlugin::CreateIconL()" )

    _LIT( KDiagHardwareSuitePluginIconFile,
          "\\resource\\apps\\devdiaghardwaresuiteplugin.mif" );

    // Load the icon.
    CFbsBitmap* bitmap;
    CFbsBitmap* mask;
    AknIconUtils::CreateIconL(
        bitmap,
        mask,
        KDiagHardwareSuitePluginIconFile(),
        EMbmDevdiaghardwaresuitepluginQgn_prop_cp_diag_hw,
        EMbmDevdiaghardwaresuitepluginQgn_prop_cp_diag_hw_mask );

    // Create the icon.
    return CGulIcon::NewL( bitmap, mask );
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Active object run handler.
// ---------------------------------------------------------------------------
void CDiagHardwareSuitePlugin::RunL()
    {
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Active object cancel.
// ---------------------------------------------------------------------------
void CDiagHardwareSuitePlugin::DoCancel()
    {
    }

// End of File
