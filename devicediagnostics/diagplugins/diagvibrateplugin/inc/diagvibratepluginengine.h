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
* Description:  Engine to handle the vibration. Uses MDiagVibratePluginObserver
*                 to inform the vibrate status changes to the client.   
*
*/


#ifndef __DIAGVIBRATEPLUGINENGINE_H__
#define __DIAGVIBRATEPLUGINENGINE_H__

// INCLUDES
#include <hwrmvibra.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class MDiagVibratePluginObserver;

// CLASS DECLARATION
class CDiagVibratePluginEngine : public CBase, 
                                 public MHWRMVibraObserver
    {
    public: // Constructors and destructor
        /**
        * Symbian OS two-phased constructor
        * @return CDiagVibratePluginEngine*
        */
        static CDiagVibratePluginEngine* NewL( MDiagVibratePluginObserver& aObserver );

        /**
        * Symbian OS two-phased constructor
        * @return CDiagVibratePluginEngine*
        */
        static CDiagVibratePluginEngine* NewLC( MDiagVibratePluginObserver& aObserver );

        /**
        * Destructor
        */
        virtual ~CDiagVibratePluginEngine();


    public:

        /**
        * Starts the vibration
        * aVibrationTime Vibration time in seconds
        */
        void StartVibraL( TInt aVibrationTime );

        /**
        * Stops the vibration
        */
        void StopVibra();


    private: // From MHWRMVibraObserver

        /**
        * @see MHWRMVibraObserver::VibraStatusChanged
        */
        virtual void VibraStatusChanged( CHWRMVibra::TVibraStatus aStatus );

        /**
        * @see MHWRMVibraObserver::VibraModeChanged
        */
        virtual void VibraModeChanged( CHWRMVibra::TVibraModeState aStatus );


    private: // Constructors

        /**
        * Private first phase constructor used by NewL and NewLC
        */
        CDiagVibratePluginEngine( MDiagVibratePluginObserver& aObserver );

        /**
        * Second phase constructor.
        */    
        void ConstructL();

    private: //data members
        
        /**
        * Observer for the vibration events
        */
        MDiagVibratePluginObserver& iObserver;

        /**
        * Vibra hw resource manager
        * own.
        */
        CHWRMVibra*                 iVibra;     

    };

#endif // __DIAGVIBRATEPLUGINENGINE_H__

// End of File
