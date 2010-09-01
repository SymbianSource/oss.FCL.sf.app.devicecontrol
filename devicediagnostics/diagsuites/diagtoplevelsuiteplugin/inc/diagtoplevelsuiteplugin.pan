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
* Description:  Panic functionality for the Top-Level Suite Plugin.
*
*/


#ifndef DIAGTOPLEVELSUITEPLUGIN_PAN
#define DIAGTOPLEVELSUITEPLUGIN_PAN

// Data Types

/** Top-Level Suite panic codes.  These are used to identify specific reasons
 *  why the suite has panicked.
 */
enum TDiagTopLevelSuitePluginPanics
    {
    EDiagTopLevelSuitePluginConstruction = 1,
    EDiagTopLevelSuitePluginBadArgument
    };


/** The panic category for the Top-Level Suite. */
_LIT( KDiagTopLevelSuiteName,"Top-Level Suite" );


/** The inline panic function. */
inline void Panic( TDiagTopLevelSuitePluginPanics aReason )
    {
    User::Panic( KDiagTopLevelSuiteName, aReason );
    }

#endif // DIAGTOPLEVELSUITEPLUGIN_PAN
