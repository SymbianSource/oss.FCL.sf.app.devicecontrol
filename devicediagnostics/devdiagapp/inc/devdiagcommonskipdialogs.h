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
* Description:  These are the declarations for dialogs shared between the
*                Device Diagnostics Application and plugins.
*
*/


#ifndef DEVDIAGCOMMONSKIPDIALOGS_H
#define DEVDIAGCOMMONSKIPDIALOGS_H

// System Include Files
#include <aknlistquerydialog.h>           // CAknQueryDialog

// Forward Declarations
class CDevDiagEngine;

/**
 *  Cancel Execution Dialog Class.
 *  This dialog asks the user if they would like to cancel all test
 *  execution.  It suspends test execution while it is active, and resumes
 *  or cancels test execution when it is dismissed.
 *
 *  @lib avkon.lib
 */
class CDevDiagCommonSkipDialogs : public CAknListQueryDialog
    {

public: // New Functions

    /**
     * Two-phased constructor.
     */
    static CDevDiagCommonSkipDialogs* NewLC( CDevDiagEngine& aEngine,
                                            TBool aSuspendTestsOnExecute );


public: // From base class CAknQueryDialog

    /**
     * From CAknQueryDialog.
     * Runs the dialog, and returns the ID of the button used to dismiss
     * it.
     */
    virtual TInt RunLD();


private: // New Functions

    /**
     * The default constructor.
     */
    CDevDiagCommonSkipDialogs( CDevDiagEngine& aEngine,
                              TBool aSuspendTestsOnExecute );

    /**
     * Two-phased constructor.
     */
    void ConstructL();


private: // From base class CAknQueryDialog

    /**
     * From CAknQueryDialog.
     * This function is called for a button press on the dialog.  It is used
     * to determine if the dialog may be exited.
     *
     * @param aButtonId The id of the button that was pressed.
     * @return ETrue, if the dialog may exit.  EFalse, otherwise.
     */
    virtual TBool OkToExitL( TInt aButtonId );


private: // Data

    /**
     * The application engine, which the dialog uses to suspend and resume
     * test execution.
     */
    CDevDiagEngine& iEngine;

    /**
     * Indicates if the dialog should call suspend when it is executed.  If
     * EFalse, then only the watchdog timer is stopped.
     */
    TBool iSuspendTestsOnExecute;
    
    TInt iIndex;

    };

#endif // DEVDIAGCOMMONSKIPDIALOGS_H
