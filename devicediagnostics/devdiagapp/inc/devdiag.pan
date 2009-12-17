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
* Description:  Panic functionality for the Device Diagnostics Application.
*
*/


#ifndef DD_PAN
#define DD_PAN

// System Include Files
#include <e32std.h>         // User

// Data Types

/** Application panic codes.  These are used to identify specific reasons why
 *  the application has panicked.
 */
enum TDevDiagApplicationPanics
    {
    EDevDiagApplicationGeneral = 1,
    EDevDiagApplicationUnhandledCommand,
    EDevDiagApplicationRuntimeReqs,
    EDevDiagApplicationInvalidStopMode,
    EDevDiagApplicationInvalidEngineState,
    EDevDiagApplicationInvalidResultIndex,
    EDevDiagApplicationBadType,
    EDevDiagApplicationNotInitialized,
    EDevDiagListboxContainerModel,
    EDevDiagListboxFindPluginFailure,
    EDevDiagListboxModelNotUpdated,
    EDevDiagApplicationNoExecutionResults,
    EDevDiagContainerArrayMismatch,
    EDevDiagCheckCrashedTest,
    EDDApplicationUnknownSuite,
    };

// -----------------------------------------------------------------------------
// This convenience function will cause the application to panic with the
// specified error code.  Extended panic notes must be enabled to see this
// information in the UI.  This may be done by creating the "c:\resource\ErrRD"
// directory (a signed sis file is available on the S60 wiki to do this).
// -----------------------------------------------------------------------------
//
inline void Panic( TDevDiagApplicationPanics aReason )
    {
    _LIT( applicationName,"DevDiag" );                 //lint !e1534
    User::Panic( applicationName, aReason );
    }

#endif // DD_PAN
