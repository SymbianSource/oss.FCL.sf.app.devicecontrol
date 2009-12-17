/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
*        General Settings User Interface
*
*/


// INCLUDE FILES
//sf-#include    <bldvariant.hrh>
#include    <eikapp.h>
#include    <avkon.hrh>
#include    <akncontext.h>
#include    <aknnavi.h>
#include    <aknnavide.h>
#include    <akntabgrp.h>
#include    <akntitle.h>
#include    <aknnotedialog.h>
#include    <AknWaitDialog.h>
#include    <aknslider.h>
#include    <AknQueryDialog.h>
#include    <aknsettingpage.h>
#include    <aknradiobuttonsettingpage.h>
#include    <aknslidersettingpage.h>
#include    <akntextsettingpage.h>
#include    <AknDateFormatUtils.h>
#include    <AknUtils.h>
#include    <AknsConstants.h>    //for determining skin change
#include    <barsread.h>
#include    <hal.h>
#include    <featmgr.h>


    #include    <e32property.h>
    #include    <PSVariables.h>


#include    <aknnotewrappers.h>    // for reboot
#include    <PtiEngine.h>

#include    <AknFep.rsg>  
#include    <avkon.rsg>
#include    <e32std.h>             // The USER class
#include    <AknFepGlobalEnums.h>  // EPinyin

#include	<apgwgnam.h>

#include    "UISettingsSrvUi.h"
#include    "UISettingsSrvAppView.h"
#include    "UISettingsSrvDocument.h"

#include    <UISettingsSrv.rsg>
#include    "UISettingsSrv.hrh"

#include    "UISettingsSrv.h"

//CONSTANTS
// _LIT( KGSAppUiClassName, "CUISettingsSrvUi" );



// ================= MEMBER FUNCTIONS =======================


// ----------------------------------------------------
// CUISettingsSrvUi::CUISettingsSrvUi()
// 
// Default constructor.
// ----------------------------------------------------
CUISettingsSrvUi::CUISettingsSrvUi()
//sf-	: 
    {
    }

// ----------------------------------------------------
// CUISettingsSrvUi::ConstructL()
// 
// Symbian OS two-phased constructor
// ----------------------------------------------------
void CUISettingsSrvUi::ConstructL()
    {
    FeatureManager::InitializeLibL();

    BaseConstructL( 0 ); //sf- EAknEnableSkin );

/*
*/
    iAppView = CUISettingsSrvAppView::NewL(ClientRect());
    AddToStackL(iAppView);
/*
*/

	// Hide application from task list
	RWsSession& ws = iEikonEnv->WsSession();
    RWindowGroup& rootWin = iEikonEnv->RootWin();
    TInt wgId = rootWin.Identifier();

	CApaWindowGroupName *windowGroupName =
		CApaWindowGroupName::NewL(ws, wgId);

	windowGroupName->SetHidden( ETrue );
	windowGroupName->SetWindowGroupName( rootWin );

	delete windowGroupName;
    }

// ----------------------------------------------------
// CUISettingsSrvUi::~CUISettingsSrvUi()
// destructor
// frees reserved resources
// ----------------------------------------------------
CUISettingsSrvUi::~CUISettingsSrvUi()
    {
//sf-    delete iDecoratedTabGroup;
//sf-    delete iDatimTitleText;
//sf-    delete iWaitDialog;

    FeatureManager::UnInitializeLib();

    if (iAppView)
        {
        iEikonEnv->RemoveFromStack(iAppView);
        delete iAppView;
        iAppView = NULL;
        }

    //
//sf-    delete iShortcutUi;

    //Embedding - removing created document
/*sf--
    if ( iEmbedded )
	    {
	    iEikonEnv->Process()->DestroyDocument( iEmbedded );
	    iEmbedded = NULL;
	    }
--fs*/
    }

// ------------------------------------------------------------------------------
// CUISettingsSrvUi::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CUISettingsSrvUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }

// ----------------------------------------------------
// CUISettingsSrvUi::HandleCommandL(TInt aCommand)
// 
// Handles commands directed to this class.
// ----------------------------------------------------
void CUISettingsSrvUi::HandleCommandL(TInt aCommand)
    {
    switch (aCommand)
        {
        case EEikCmdExit:   //both exit commands should do the same thing
            iEikonEnv->InfoMsg(_L("ex1"));
        case EAknCmdExit:
            iEikonEnv->InfoMsg(_L("exit2"));
            Exit();
            break;
        /*
        case EUISSCmdTest:
            iEikonEnv->InfoMsg(_L("...test..."));
            break;
        case EUISSCmdTest2:           
            _LIT(message,"Halloo");
            CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
            informationNote->ExecuteLD(message);
            break;
        */
        case EUISSCmdStartSrv:
        	CUISettingsSrvServer::NewL();
            break;
        default:
            CAknAppUi::HandleCommandL(aCommand);
//sf-            CAknViewAppUi::HandleCommandL(aCommand);
            break;
        }
    }



// ----------------------------------------------------
// CUISettingsSrvUi::HandleKeyEventL()
// 
// Called when a key is pressed and no other class has catched the event.
// Handles only key events that are meant to control the tab group. 
// ----------------------------------------------------
TKeyResponse CUISettingsSrvUi::HandleKeyEventL(const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
/*sf--
    // Check if the event should be handled
    if (aType != EEventKey ||  iNaviContainer->Top() != iDecoratedTabGroup)
        {
        return EKeyWasNotConsumed;
        }
    return iTabGroup->OfferKeyEventL( aKeyEvent, aType );
--fs*/
    }



//End of File
