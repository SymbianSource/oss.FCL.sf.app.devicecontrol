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
* Description:  Implements the vibration using CHWRMVibra
*
*/


// INCLUDE FILES
#include "diagvibratepluginengine.h"
#include "diagvibratepluginobserver.h"

#include <DiagFrameworkDebug.h> 
// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
static const TInt KMilliseconds = 1000;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ==============================

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CDiagVibratePluginEngine::CDiagVibratePluginEngine()
//
// Constructor
// ----------------------------------------------------------------------------
//
CDiagVibratePluginEngine::CDiagVibratePluginEngine( MDiagVibratePluginObserver& aObserver )
    : iObserver( aObserver )
    {
    }

// ---------------------------------------------------------------------------
// CDiagVibratePluginEngine::NewL()
//
// Symbian OS default constructor
// ---------------------------------------------------------------------------
//
CDiagVibratePluginEngine* CDiagVibratePluginEngine::NewL( MDiagVibratePluginObserver& aObserver )
    {
    CDiagVibratePluginEngine* self = CDiagVibratePluginEngine::NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }
    

// ---------------------------------------------------------------------------
// CDiagVibratePluginEngine::NewLC()
//
// Symbian OS default constructor
// ---------------------------------------------------------------------------
//
CDiagVibratePluginEngine* CDiagVibratePluginEngine::NewLC( MDiagVibratePluginObserver& aObserver )
    {
    CDiagVibratePluginEngine* self = new ( ELeave ) CDiagVibratePluginEngine( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    
    return self;
    }
    

// ---------------------------------------------------------------------------
// CDiagVibratePluginEngine::ConstructL()
//
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
void CDiagVibratePluginEngine::ConstructL()
    {
    iVibra = CHWRMVibra::NewL(this);
    }
    

// ----------------------------------------------------------------------------
// CDiagVibratePluginEngine::~CDiagVibratePluginEngine
//
// Destructor
// ----------------------------------------------------------------------------
//    
CDiagVibratePluginEngine::~CDiagVibratePluginEngine()
    {
    if (iVibra)
        {    
        iVibra->ReleaseVibra();
        delete iVibra;
        }    
    }    


// ---------------------------------------------------------------------------
// From MHWRMVibraObserver
// CDiagVibratePluginEngine::VibraStatusChanged()
// ---------------------------------------------------------------------------
//    
void CDiagVibratePluginEngine::VibraStatusChanged(CHWRMVibra::TVibraStatus aStatus)
    {
    TVibrateStatus vibrateStatus = EVibrateFailed;
    
    switch(aStatus)
        {
        case CHWRMVibra::EVibraStatusOn:
            vibrateStatus = EVibrateStarted;
            break;

        case CHWRMVibra::EVibraStatusStopped:
            vibrateStatus = EVibrateEnded;
            break;

        case CHWRMVibra::EVibraStatusNotAllowed:
            vibrateStatus = EVibrateBlocked;
            break;
        
        case CHWRMVibra::EVibraStatusUnknown:
        default:
            break;
        }
        
    iObserver.VibraStatusChanged( vibrateStatus );
    }

// ---------------------------------------------------------------------------
// From MHWRMVibraObserver
// CDiagVibratePluginEngine::VibraModeChanged()
// ---------------------------------------------------------------------------
//    
void CDiagVibratePluginEngine::VibraModeChanged( CHWRMVibra::TVibraModeState aStatus )
    {
    switch (aStatus)
        {
        case CHWRMVibra::EVibraModeON:
            break; //no need to do anything
            
        case CHWRMVibra::EVibraModeOFF:
        case CHWRMVibra::EVibraModeUnknown: 
        default: 
            iObserver.VibraStatusChanged(EVibrateFailed);    
            break;
        }
    }

// ---------------------------------------------------------------------------
// Method to start the vibra
// CDiagVibratePluginEngine::StartVibraL()
// ---------------------------------------------------------------------------
//    
void CDiagVibratePluginEngine::StartVibraL( TInt aVibrationTime )
    {
    iVibra->ReserveVibraL();
    iVibra->StartVibraL( aVibrationTime * KMilliseconds, KHWRMVibraMaxIntensity );
    }


// ---------------------------------------------------------------------------
// Method to stop the vibra
// CDiagVibratePluginEngine::StopVibra()
// ---------------------------------------------------------------------------
//    
void CDiagVibratePluginEngine::StopVibra()
    {
    iVibra->ReleaseVibra();
    }    

// End of File
