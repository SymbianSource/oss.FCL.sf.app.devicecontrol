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
* Description:  This is the header for the Diagnostics Data Network Test plugin 
*                 class
*
*/


#ifndef DIAGDATANETWORKTESTPLUGIN_H 
#define DIAGDATANETWORKTESTPLUGIN_H

// INCLUDES
#include <DiagResultsDatabaseItem.h>
#include <DiagTestPluginBase.h>     // CDiagTestPluginBase
#include "diagdatanetworktestcallbackinterface.h" // MDiagDataNetworkTestCallBackInterface

//CONSTANTS

// MACROS

// DATA TYPES
    
// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CDiagDataNetworkTestPDPEngine;
class CDiagResultsDbItemBuilder;

// CLASS DEFINITION
class CDiagDataNetworkTestPlugin : public CDiagTestPluginBase, 
                                   public MDiagDataNetworkTestCallBackInterface
    {
    public: // Constructors and destructor

        /**
        * Symbian OS two-phased constructor
        * @return DataNetwork Test Plugin
        */
        static MDiagPlugin* NewL( TAny* aInitParams );

        /**
        * Destructor
        */
        ~CDiagDataNetworkTestPlugin();
        
        /**
        *  The Data Network Test PDP Engine calls this function to provide the result of the test
        */
        void ReportTestResultL(CDiagResultsDatabaseItem::TResult aResult);
        
        /**
        *  The Data Network Test PDP Engine calls this function to provide the progress of the test
        */        
        void ReportTestProgL(TInt aStep);          
		TBool DisplayDialog(TInt &);              

    public: // From CDiagTestPluginBase
    
   

        /**
        * @see CDiagTestPluginBase::Visible()
        */
        TBool IsVisible() const;

        /**
        * @see CDiagTestPluginBase::RunMode()
        */
        TRunMode RunMode() const;

        /**
        * @see CDiagTestPluginBase::TotalSteps()
        */
        TUint TotalSteps() const;

        /**
        * @see CDiagTestPluginBase::GetPluginNameL
        */
        HBufC* GetPluginNameL( TNameLayoutType aLayoutType ) const;

        /**
        * @see CDiagTestPluginBase::Uid
        */
        TUid Uid() const;
        
        
        TBool ShowMessageQueryL( TInt aResourceId, TInt &aButtonId  );

        

    protected: // from CDiagTestPluginBase

        /**
        * C++ default constructor.
        */
        CDiagDataNetworkTestPlugin( CDiagPluginConstructionParam* aInitParams  );

        /**
        * Symbian OS default constructor.
        *
        */
        void ConstructL();

        /**
        * Handle plug-in execution.
        */
        void DoRunTestL();

        /**
        * Handle stop and clean up
        */
        void DoStopAndCleanupL();

                        
    protected: // from CActive
    
        /**
        * Handle active object completion event
        */
        void RunL();
        
            
        /**
        * Handle cancel
        */
        void DoCancel();
        
    private:    // data

        // pointer to Data Network test PDP Engine
        CDiagDataNetworkTestPDPEngine *iPDPEngine;        
    };

#endif //DIAGDATANETWORKTESTPLUGIN_H

// End of File
