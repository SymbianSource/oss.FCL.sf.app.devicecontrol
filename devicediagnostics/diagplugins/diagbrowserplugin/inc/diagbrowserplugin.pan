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
* Description:  Panic functionality for the Browser Test Plugin.
*
*/


#ifndef DIAGBROWSERPLUGIN_PAN
#define DIAGBROWSERPLUGIN_PAN

// Data Types

/** Browser Test Plugin panic codes.  These are used to identify specific
 *  reasons why the plugin has panicked.
 */
enum TDiagBrowserPluginPanics
    {
    EDiagBrowserPluginConstruction = 1,
    EDiagBrowserPluginName,
    EDiagBrowserPluginBadState,
    EDiagBrowserPluginTransaction,
    EDiagBrowserPluginInternal
    };

inline void Panic( TDiagBrowserPluginPanics aReason )
    {
    _LIT( KDiagBrowserPluginName,"Browser Test" );
    User::Panic( KDiagBrowserPluginName, aReason );
    }

#endif // DIAGBROWSERPLUGIN_PAN

