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
* Description:  Plug-in view. Handles test plug-ins.
*
*/


#ifndef DEVDIAGPLUGINVIEW_H
#define DEVDIAGPLUGINVIEW_H

// INCLUDES
#include <aknview.h> 
#include "devdiagengineobserver.h"

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CAknView;
class CDevDiagPluginContainer;
class CDevDiagEngine;
class CDevDiagExecResults;

// CLASS DEFINITION
/**
*  CDevDiagPluginView view class 
*  @since S60 5.0
*
*/
class CDevDiagPluginView : public CAknView, public MDevDiagEngineObserver
    {
    
public: // Constructors and destructor.

    /**
    * Symbian OS two-phased constructor.
    * @return a new view.
    */
    static CDevDiagPluginView* NewL( CDevDiagEngine& aEngine, TUid aParentSuiteUid );

    /**
    * Destructor.
    */
    ~CDevDiagPluginView();
    
    
public: //From CAknView

    /**
    * From CAknView Handles user commands.
    * @since Series 60 3.1
    * @param aCommand The command identifier.
    * @return None.
    */
    void HandleCommandL( TInt aCommand );
    
    /**
    * From CAknView.
    * Returns views id.
    **/
    TUid Id() const;
    
public: // engine observer

    virtual void HandleEngineCommandL( TAppEngineCommand aCommand,
                                       TInt aError,
                                       TAny* aCustom );    

public:

    /**
    * Display confirmation dialog after successful group execution.
    * succesful execution means that all tests have the following result:
    * - passed
    * - cancelled
    * - skipped    
    **/ 
    void DisplayConfirmationDialogL(TInt aResourceId);
    
    /**
    * Displays message query dialog. The dialog lists all tests cases that failed.
    **/
    void DisplayMessageQueryDialogFailedResultsL();

protected:
    /**
    * From CAknView Activates view.
    * @since Series 60 3.1
    * @param aPrevViewId Id of previous view.
    * @param aCustomMessageId Custom message id.
    * @param aCustomMessage Custom message.
    * @return None.
    */
    virtual void DoActivateL (const TVwsViewId &aPrevViewId, 
                                    TUid aCustomMessageId, 
                                    const TDesC8 &aCustomMessage);
    /**
    * From CAknView Deactivates view.        
    * @since Series 60 3.1
    * @param None.
    * @return None.
    */
    virtual void DoDeactivate(); 
    
    /**
    * Change Title.
    *
    * @aResourceId Title text resource.
    **/        
    void ChangeTitleL(TInt aResourceId);
    
    /**
    * Set Context menu.
    **/
    void SetContextMenu();
    
    /**
    * Display a message query dialog.
    *
    * @param aResourceId Message query resource ID.
    **/
    TBool MessageQueryDialogLaunchL( TInt aResourceId );
        
    /**
    * Create result texts for failed test cases.
    *
    * @param aExecResults Execution results.
    * @return New heap descriptor. Caller owns the descriptor.
    **/ 
    HBufC* CreateResultsTextsForFailedTestsLC( const CDevDiagExecResults& aExecResults );
    
private: //Constructors
    
    /**
    * ConstructL.
    **/
    void ConstructL();
    
    /**
    * Normal constructor.
    *
    * @param aEngine Diagnostics Engine.
    * @param aParentSuiteUid UID of the parent suite.
    **/
    CDevDiagPluginView( CDevDiagEngine& aEngine, TUid aParentSuiteUid );    
        
        
private:    
    
    // Container. Own.
    CDevDiagPluginContainer* iContainer;   
    
    // Diagnostics Engine.
    CDevDiagEngine& iEngine;   
    
    // Main view UID.
    TUid iParentSuiteUid;   
    };

#endif //DEVDIAGPLUGINVIEW_H

// End of File
