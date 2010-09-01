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
#include "AppMgmtProgDialog.h"
#include <avkon.rsg>
#include <applicationmanagement.rsg>
#include <eikenv.h>
#include <aknnotewrappers.h>
#include <avkon.hrh>

CAppMgmtProgDialog::CAppMgmtProgDialog(MDLProgressDlgObserver *iCallback) :
    iDlgObserver(iCallback)
    {

    }

CAppMgmtProgDialog::~CAppMgmtProgDialog()
    {
    if (iProgressDialog)
        {
        delete iProgressDialog;
        }

    }

void CAppMgmtProgDialog::StartProgressNoteL()
    {
    iProgressDialog = new (ELeave) CAknProgressDialog(
            (REINTERPRET_CAST(CEikDialog**, &iProgressDialog)),
            ETrue);
    iProgressDialog->PrepareLC(R_PROGRESS_NOTE);
    iProgressInfo = iProgressDialog->GetProgressInfoL();
    iProgressDialog->SetCallback(this);
    iProgressDialog->RunLD();
    iProgressInfo->SetFinalValue(500);

    }

void CAppMgmtProgDialog::UpdateProcessL(TInt aProgress)
    {
    if (iProgressInfo)
        {
        iProgressInfo->SetAndDraw(aProgress);
        }
    }

void CAppMgmtProgDialog::ProgressCompletedL()
    {
    if (iProgressDialog)
        {
        iProgressDialog->ProcessFinishedL();
        }

    }

void CAppMgmtProgDialog::SetFinalValueL(TInt32 aFinalValue)
    {
    iProgressInfo = iProgressDialog->GetProgressInfoL();
    iProgressInfo-> SetFinalValue(aFinalValue);
    }

void CAppMgmtProgDialog::DialogDismissedL(TInt aButtonId)
    {

    //|| aButtonId == EAknSoftkeyEmpty
    if (aButtonId == EAknSoftkeyCancel)
        {

        iDlgObserver->HandleDLProgressDialogExitL(aButtonId);

        }
    }

