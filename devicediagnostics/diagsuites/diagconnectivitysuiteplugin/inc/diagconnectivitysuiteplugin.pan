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


#ifndef DIAGCONNECTIVITYSUITEPLUGIN_PAN
#define DIAGCONNECTIVITYSUITEPLUGIN_PAN

// Data Types

/** Hardware Suite panic codes.  These are used to identify specific reasons
 *  why the suite has panicked.
 */
enum TDiagConnectivitySuitePluginPanics
    {
    EDiagConnectivitySuitePluginConstruction = 1,
    EDiagConnectivitySuitePluginBadArgument
    };

inline void Panic( TDiagConnectivitySuitePluginPanics aReason )
    {
    _LIT( KDiagConnectivitySuiteName,"Connectivity Suite" );
    User::Panic( KDiagConnectivitySuiteName, aReason );
    }

#endif // DIAGCONNECTIVITYSUITEPLUGIN_PAN
