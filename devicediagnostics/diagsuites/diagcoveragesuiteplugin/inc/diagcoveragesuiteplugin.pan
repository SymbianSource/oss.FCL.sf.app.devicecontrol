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
* Description:  Panic functionality for the Coverage Suite Plugin.
*
*/


#ifndef DIAGCOVERAGESUITEPLUGIN_PAN
#define DIAGCOVERAGESUITEPLUGIN_PAN

// Data Types

/** Coverage Suite panic codes.  These are used to identify specific reasons
 *  why the suite has panicked.
 */
enum TDiagCoverageSuitePluginPanics
    {
    EDiagCoverageSuitePluginConstruction = 1,
    EDiagCoverageSuitePluginBadArgument
    };

inline void Panic( TDiagCoverageSuitePluginPanics aReason )
    {
    _LIT( KDiagCoverageSuiteName,"Coverage Suite" );
    User::Panic( KDiagCoverageSuiteName, aReason );
    }

#endif // DIAGCOVERAGESUITEPLUGIN_PAN

