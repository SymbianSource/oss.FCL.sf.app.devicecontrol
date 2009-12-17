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
* Description:  Callback IF to inform vibrate status changes
*
*/


#ifndef __DIAGVIBRATEPLUGINOBSERVER_H__
#define __DIAGVIBRATEPLUGINOBSERVER_H__

// INCLUDES

// CONSTANTS

// MACROS

// DATA TYPES
enum TVibrateStatus 
    {
    EVibrateStarted,
    EVibrateEnded,
    EVibrateFailed,
    EVibrateBlocked
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class MDiagVibratePluginObserver 
    {
    public: 
    
        /**
        * Inform vibrate status changes
        *
        * @param aStatus Status of the vibrate engine
        */
        virtual void VibraStatusChanged( TVibrateStatus aStatus ) = 0;
    };

#endif // __DIAGVIBRATEPLUGINOBSERVER_H__

//End of File
