/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of customization components
*
*/


#include <coemain.h>

#include "UISettingsSrvAppView.h"

CUISettingsSrvAppView* CUISettingsSrvAppView::NewL(const TRect& aRect)
    {
    CUISettingsSrvAppView* self = CUISettingsSrvAppView::NewLC(aRect);
    CleanupStack::Pop(self);
    return self;
    }

CUISettingsSrvAppView* CUISettingsSrvAppView::NewLC(const TRect& aRect)
    {
    CUISettingsSrvAppView* self = new (ELeave) CUISettingsSrvAppView;
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
    return self;
    }

CUISettingsSrvAppView::CUISettingsSrvAppView()
    {
	// No implementation required
    }

CUISettingsSrvAppView::~CUISettingsSrvAppView()
    {
	// No implementation required
    }

void CUISettingsSrvAppView::ConstructL(const TRect& aRect)
    {
    // Create a window for this application view
    CreateWindowL();

    // Set the windows size
    SetRect(aRect);

    // Activate the window, which makes it ready to be drawn
    ActivateL();
    }

// Draw this application's view to the screen
void CUISettingsSrvAppView::Draw(const TRect& /*aRect*/) const
    {
    // Get the standard graphics context 
    CWindowGc& gc = SystemGc();
    
    // Gets the control's extent
    TRect rect = Rect();
    
    // Clears the screen
    gc.Clear(rect);
    }


