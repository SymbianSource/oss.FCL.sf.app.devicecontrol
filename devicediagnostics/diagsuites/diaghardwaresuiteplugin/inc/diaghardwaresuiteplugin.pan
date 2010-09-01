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
* Description:  Panic functionality for the Hardware Suite Plugin.
*
*/


#ifndef DIAGHARDWARESUITEPLUGIN_PAN
#define DIAGHARDWARESUITEPLUGIN_PAN

// Data Types

/** Hardware Suite panic codes.  These are used to identify specific reasons
 *  why the suite has panicked.
 */
enum TDiagHardwareSuitePluginPanics
    {
    EDiagHardwareSuitePluginConstruction = 1,
    EDiagHardwareSuitePluginBadArgument
    };

inline void Panic( TDiagHardwareSuitePluginPanics aReason )
    {
    _LIT( KDiagHardwareSuiteName,"Hardware Suite" );
    User::Panic( KDiagHardwareSuiteName, aReason );
    }

#endif // DIAGHARDWARESUITEPLUGIN_PAN
