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
* Description:  Application view class.
*
*/


#ifndef DEVDIAGSUITEVIEW_H
#define DEVDIAGSUITEVIEW_H

#include "devdiagengineobserver.h"

#include <aknview.h> 
#include <vwsdef.h> //TVwsViewId

class CAknViewAppUi;
class CDevDiagSuiteContainer;
class CDevDiagEngine;
class CDevDiagPluginView;
class CAknWaitDialog;
class CDevDiagExecResults;

/**
* An instance of the Application View object for the DevDiagApp 
* application
* @since 5.0
**/
class CDevDiagSuiteView : public CAknView,
			              public MDevDiagEngineObserver			   
	
    {
public:

    /**
    * NewL
    *  
    * Create a CDevDiagSuiteView object.
    * @param aEngine Diagnostics engine.
    *  
    * @result a pointer to the created instance of CDevDiagSuiteView
    **/
    static CDevDiagSuiteView* NewL( CDevDiagEngine& aEngine );

    /**
    * NewLC
    * 
    * Create a CDevDiagSuiteView object.
    * @param aEngine Diagnostics engine.
    *
    * @result a pointer to the created instance of CDevDiagSuiteView
    **/
    static CDevDiagSuiteView* NewLC( CDevDiagEngine& aEngine );


    /**
    * Destructor.
    *
    * Destroy the object and release all memory objects
    **/
    ~CDevDiagSuiteView();

    
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


protected:
    /**
    * From CAknView Activates view.
    * @since Series 60 3.1
    * @param aPrevViewId Id of previous view.
    * @param aCustomMessageId Custom message id.
    * @param aCustomMessage Custom message.
    * @return None.
    */
    virtual void DoActivateL ( const TVwsViewId &aPrevViewId, 
                               TUid aCustomMessageId, 
                               const TDesC8 &aCustomMessage );
    /**
    * From CAknView Deactivates view.        
    * @since Series 60 3.1
    * @param None.
    * @return None.
    */
    virtual void DoDeactivate();     
        
public: // engine observer

    /**
    * From MDevDiagEngineObserver.
    **/
    virtual void HandleEngineCommandL( TAppEngineCommand aCommand,
                                       TInt aError,
                                       TAny* aCustom );    
  

private:

    /**
    * Change the title of the application.
    * @param aResourceId Title text ID.
    **/
    void ChangeTitleL(TInt aResourceId);        
    
    /**
    * Perform the second phase construction of a CDevDiagSuiteView object    
    **/
    void ConstructL( );

    /**
    * Perform the first phase of two phase construction 
    *
    * @param aEngine Application model class.
    **/
    CDevDiagSuiteView( CDevDiagEngine& aEngine );
    
    /**
    * Update command buttons.
    * @param aResourceId Id to a new command button resource.
    **/
    void UpdateCbaL(TInt aResourceId);
    
    /**
    * Display a message query dialog.
    *
    * @param aResourceId Message query resource ID.
    **/
    TBool MessageQueryDialogLaunchL( TInt aResourceId );
    
    /**
    * Display confirmation dialog.
    *
    * @param aPassed ETrue if tests were passed, EFalse otherwise.
    **/ 
    void DisplayConfirmationDialogL( TBool aPassed );
    
    /**
    * Returns ETrue if uid is one of the suite UIDs.
    * The suites are hardware,coverage,service, and connectivity.
    *
    * @param aUid Unique identifier.
    * @return ETrue if uid is one of the suite UIDs, EFalse otherwise.
    **/
    TBool SuiteUid( TUid aUid );
    
    /**
    * Search for a test record that crashed when the device diagnostics application
    * was running. If a crashed test record is found, figure out what test crashed and 
    * suggest that the test could be executed again.
    **/
    void CheckCrashedTestL();
    
      /**
    * Display confirmation dialog after successful group execution.
    * succesful execution means that all tests have the following result:
    * - passed
    * - cancelled
    * - skipped    
    **/ 
    void DisplayConfirmationDialogL();
    
    /**
    * Displays message query dialog. The dialog lists all tests cases that failed.
    **/
    void DisplayMessageQueryDialogFailedResultsL();
    
private: // waiting dialogs
    
    /**
     * Display a non-dismissable generic dialog to inform the user that the
     * application is "preparing."          
     */
    void DisplayWaitDialogPreparingL();
    
    /**
     * Dismisses the wait dialog        
     */
    void DismissWaitDialogPreparingL();        
    
    /**
    * Create result texts for failed test cases.
    *
    * @param aExecResults Execution results.
    * @return New heap descriptor. Caller owns the descriptor.
    **/ 
    HBufC* CreateResultsTextsForFailedTestsLC( const CDevDiagExecResults& aExecResults );
    
private:   

    //Main container that displayes suites. Own.
    CDevDiagSuiteContainer* iContainer;   
    
    //Plug-in view that displayes plug-ins. Own.    
    CDevDiagPluginView* iPluginView;
    
    //Model.
    CDevDiagEngine& iEngine;
    
    //Parent suite ID. This is HW,connectivity, service or coverage suite ID.
    TUid iParentPlugin;
        
    // Wait dialog for plugin loading. Own.
    CAknWaitDialog* iWaitDialog;          
    
    // Previous View ID.
    TVwsViewId iPrevViewId;
    };


#endif // DEVDIAGSUITEVIEW_H
