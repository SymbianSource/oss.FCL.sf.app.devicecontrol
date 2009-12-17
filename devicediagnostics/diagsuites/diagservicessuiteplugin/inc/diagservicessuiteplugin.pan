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
* Description:  Panic functionality for the Services Suite Plugin.
*
*/


#ifndef DIAGSERVICESSUITEPLUGIN_PAN
#define DIAGSERVICESSUITEPLUGIN_PAN

// Data Types

/** Services Suite panic codes.  These are used to identify specific reasons
 *  why the suite has panicked.
 */
enum TDiagServicesSuitePluginPanics
    {
    EDiagServicesSuitePluginConstruction = 1,
    EDiagServicesSuitePluginBadArgument
    };

inline void Panic( TDiagServicesSuitePluginPanics aReason )
    {
    _LIT( KDiagServicesSuiteName,"Services Suite" );
    User::Panic( KDiagServicesSuiteName, aReason );
    }

#endif // DIAGSERVICESSUITEPLUGIN_PAN

