/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*        Document for the UISettingsSrv Application
*
*/


// INCLUDE FILES
#include    "UISettingsSrvDocument.h"
#include    "UISettingsSrvUi.h"
#include	<apgwgnam.h>

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// CUISettingsSrvDocument::~CUISettingsSrvDocument()
// destructor
//
// ----------------------------------------------------
//
CUISettingsSrvDocument::~CUISettingsSrvDocument()
    {
    }

// ----------------------------------------------------
// CUISettingsSrvDocument::ConstructL()
// Symbian OS default constructor can leave.
//
// ----------------------------------------------------
//
void CUISettingsSrvDocument::ConstructL()
    {
    }

// ----------------------------------------------------
// CUISettingsSrvDocument::NewL()
// Two-phased constructor.
//
// ----------------------------------------------------
//
CUISettingsSrvDocument* CUISettingsSrvDocument::NewL(
        CEikApplication& aApp)
    {
    CUISettingsSrvDocument* self = new(ELeave) CUISettingsSrvDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------
// CUISettingsSrvDocument::UpdateTaskNameL()
// Two-phased constructor.
//
// ----------------------------------------------------
//
void CUISettingsSrvDocument::UpdateTaskNameL(CApaWindowGroupName* aWgName)
	{
	CAknDocument::UpdateTaskNameL( aWgName );
	aWgName->SetHidden( ETrue );
	}

// ----------------------------------------------------
// CUISettingsSrvDocument::CreateAppUiL()
// constructs CGSUi
//
// ----------------------------------------------------
//
CEikAppUi* CUISettingsSrvDocument::CreateAppUiL()
    {
    return new(ELeave) CUISettingsSrvUi;
    }

// End of File
