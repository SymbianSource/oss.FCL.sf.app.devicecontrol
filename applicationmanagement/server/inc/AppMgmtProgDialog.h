/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:  Implementation of applicationmanagement components
 *
*/


#include <eikprogi.h>
#include <AknWaitDialog.h> 
#include <AknProgressDialog.h>

#include <coemain.h>

class MDLProgressDlgObserver
    {
public:

    /**
     * Called when the dialog is going to be closed.
     *
     * @since S60 v3.1
     * @param aButtonId    Id of the button, which was used to cancel 
     *                       the dialog.
     */
    virtual void HandleDLProgressDialogExitL(TInt aButtonId) = 0;
    };

class CAppMgmtProgDialog : public CBase, public MProgressDialogCallback
    {
public:

    CAppMgmtProgDialog(MDLProgressDlgObserver *iCallback);
    ~CAppMgmtProgDialog();

    void StartProgressNoteL();
    void UpdateProcessL(TInt aProgress);

    void SetFinalValueL(TInt32 aFinalvalue);

    /**
     * Offers key event for progress dialog.
     *
     * @since S60 v3.1
     * @param aKeyEvent             Key event
     * @param aType                 Event type
     * @return None
     */
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

public:
    // from base classes


    /**
     * From CEikDialog, respond to softkey inputs.
     *
     * @since S60 v3.1
     * @para aButtonId, type of pressed Button or Softkey
     * @return TBool, ETrue if exit the dialog, otherwise EFalse.
     */
    TBool OkToExitL(TInt aButtonId);

    void ProgressCompletedL();

public:

protected:
    // other system interface functions
    void DialogDismissedL(TInt aButtonId);

private:

    CAknProgressDialog* iProgressDialog;
    CEikProgressInfo* iProgressInfo;

    MDLProgressDlgObserver* iDlgObserver;

    };
