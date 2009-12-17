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


#ifndef __UISettingsSrvAppView_H__
#define __UISettingsSrvAppView_H__


#include <coecntrl.h>

/*! 
  @class CUISettingsSrvAppView
  
  @discussion An instance of CUISettingsSrvAppView is the Application View object
  for the HelloWorldBasic example application
  */
class CUISettingsSrvAppView : public CCoeControl
    {
public:

/*!
  @function NewL
   
  @discussion Create a CUISettingsSrvAppView object, which will draw itself to aRect
  @param aRect The rectangle this view will be drawn to
  @result A pointer to the created instance of CUISettingsSrvAppView
  */
    static CUISettingsSrvAppView* NewL(const TRect& aRect);

/*!
  @function NewLC
   
  @discussion Create a CUISettingsSrvAppView object, which will draw itself to aRect
  @param aRect The rectangle this view will be drawn to
  @result A pointer to the created instance of CUISettingsSrvAppView
  */
    static CUISettingsSrvAppView* NewLC(const TRect& aRect);


/*!
  @function ~CUISettingsSrvAppView
  
  @discussion Destroy the object
  */
     ~CUISettingsSrvAppView();


public:  // from CCoeControl
/*!
  @function Draw
  
  @discussion Draw this CUISettingsSrvAppView to the screen
  @param aRect The rectangle of this view that needs updating
  */
    void Draw(const TRect& aRect) const;
  

private:

/*!
  @function ConstructL
  
  @discussion  Perform the second phase construction of a CUISettingsSrvAppView object
  @param aRect The rectangle this view will be drawn to
  */
    void ConstructL(const TRect& aRect);

/*!
  @function CUISettingsSrvAppView
  
  @discussion Perform the first phase of two phase construction 
  */
    CUISettingsSrvAppView();
    };


#endif // __UISettingsSrvAppView_H__
