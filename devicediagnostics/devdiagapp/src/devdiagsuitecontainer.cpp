/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Main container that displayes suites.
*
*/


// INCLUDE FILES

#include "devdiagsuitecontainer.h"
#include "devdiagengine.h"   

#include <StringLoader.h>
#include <aknlists.h>
#include <DiagPlugin.h>  
#include <DiagPluginPool.h> 
#include <DiagSuitePlugin.h>   
#include <AknIconArray.h>           // CAknIconArray
#include <AknIconUtils.h>           // AknIconUtils
#include <gulicon.h>                // CGulIcon

#include <eikclbd.h>                // CColumnListBoxData
#include <AknIconArray.h>
#include <AknsUtils.h>
#include <textresolver.h>

///@@@KSR: changes for BAD Warnings - KNSmlTab" was declared but never referenced
//_LIT( KNSmlTab,     "\t" );
_LIT( KListSuiteElementFormat,  "%d\t%S" );

const TInt KIconArrayGranularity(5);

// ========================== MEMBER FUNCTIONS =================================

// -----------------------------------------------------------------------------
// CDevDiagSuiteContainer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDevDiagSuiteContainer* CDevDiagSuiteContainer::NewL( CDevDiagSuiteView* aView,
                                                         CDevDiagEngine& aEngine,
                                                const TRect&    aRect )
    {
    CDevDiagSuiteContainer* self = new( ELeave ) 
                                     CDevDiagSuiteContainer( aView, aEngine );
    
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CDevDiagSuiteContainer::CDevDiagSuiteContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDevDiagSuiteContainer::CDevDiagSuiteContainer( CDevDiagSuiteView* aView,
                                                    CDevDiagEngine& aEngine )
    : iView( aView ), iEngine(aEngine)
    {
    }

// -----------------------------------------------------------------------------
// CDevDiagSuiteContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDevDiagSuiteContainer::ConstructL( const TRect& aRect )
    {
    CreateWindowL();

	iListBox = new ( ELeave ) CAknSingleLargeStyleListBox;
	
	iListBox->SetContainerWindowL( *this );
	iListBox->ConstructL( this, EAknListBoxSelectionList );

	iListBox->CreateScrollBarFrameL( ETrue );
	iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
	                                            CEikScrollBarFrame::EOff,
	                                            CEikScrollBarFrame::EAuto );
       
    CArrayPtr< CGulIcon >* icons = new ( ELeave ) CAknIconArray(
        KIconArrayGranularity );
   
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( icons );
    
	iListBox->UpdateScrollBarsL();
	//iListBox->ScrollBarFrame()->MoveVertThumbTo( 0 );		

	iListBox->SetListBoxObserver( this );
			
    SetRect( aRect );
    ActivateL();    
    }

// -----------------------------------------------------------------------------
// CDevDiagSuiteContainer::~CDevDiagSuiteContainer
// -----------------------------------------------------------------------------
//
CDevDiagSuiteContainer::~CDevDiagSuiteContainer()
    {
    delete iListBox;
        
    //iChildren.ResetAndDestroy();
    iChildren.Close();
    }
    
// -----------------------------------------------------------------------------
// CDevDiagSuiteContainer::SizeChanged
// -----------------------------------------------------------------------------
//
void CDevDiagSuiteContainer::SizeChanged()
    {
    iListBox->SetRect( Rect() );    // Mandatory, otherwise not drawn
    }

// -----------------------------------------------------------------------------
// CDevDiagSuiteContainer::HandleResourceChange
// -----------------------------------------------------------------------------
//
void CDevDiagSuiteContainer::HandleResourceChange( TInt aType )
    {    
    CCoeControl::HandleResourceChange( aType );
    
    //Handle change in layout orientation
    if ( aType == KEikDynamicLayoutVariantSwitch || aType == KAknsMessageSkinChange )
        {
        if ( aType == KAknsMessageSkinChange )
        	{
        	// Skin changed; we'll have to reload the icon array
            CArrayPtr<CGulIcon>* iconArray =
                iListBox->ItemDrawer()->ColumnData()->IconArray();
            if ( iconArray )
                {
    	        iconArray->ResetAndDestroy();
	            delete iconArray;
	            iconArray = NULL;
    	        }
    	    TRAP_IGNORE( AddIconsL() );
        	}

        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
        SetRect( mainPaneRect );
		DrawDeferred();
		}	
    }

// -----------------------------------------------------------------------------
// CDevDiagSuiteContainer::CountComponentControls
// -----------------------------------------------------------------------------
//
TInt CDevDiagSuiteContainer::CountComponentControls() const
    {
    TInt retval( 0 );
    if ( iListBox )
        {
        retval = 1;
        }
    return retval; // return nbr of controls inside this container
    }

// -----------------------------------------------------------------------------
// CDevDiagSuiteContainer::ComponentControl
// -----------------------------------------------------------------------------
//
CCoeControl* CDevDiagSuiteContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            {
            return iListBox;
            }
        default:
            {
            return NULL;
            }
        }
    }

// -----------------------------------------------------------------------------
// CDevDiagSuiteContainer::Draw
// -----------------------------------------------------------------------------
//
void CDevDiagSuiteContainer::Draw( const TRect& /* aRect */ ) const
    {
    }


// -----------------------------------------------------------------------------
// CDevDiagSuiteContainer::HandleControlEventL
// -----------------------------------------------------------------------------
//

void CDevDiagSuiteContainer::HandleControlEventL( CCoeControl* /* aControl */,
                                                TCoeEvent /* aEventType */ )
    {
    }

// -----------------------------------------------------------------------------
// CDevDiagSuiteContainer::OfferKeyEventL
// -----------------------------------------------------------------------------
//

TKeyResponse CDevDiagSuiteContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                             TEventCode aType )
	{

	if ( iListBox && aType == EEventKey )
		{
		switch ( aKeyEvent.iCode )
			{
			case EStdKeyHome:
				{			
			    break;           
				}
				
			case EKeyEscape:
				{
			
				iAvkonAppUi->ProcessCommandL( EEikCmdExit );
				
		    	break;
				}
			case EKeyUpArrow:
			case EKeyDownArrow:
			    {
			    TKeyResponse retVal =
			        iListBox->OfferKeyEventL( aKeyEvent, aType );
                return retVal;
			    }
			case EKeyLeftArrow:
			case EKeyRightArrow:
			    {
			    break;  // AppUi handles the tab changes
			    }
			default:
			    {
			    return iListBox->OfferKeyEventL( aKeyEvent, aType );
			    }
			}
		}
	
	return EKeyWasNotConsumed;
	}

// -----------------------------------------------------------------------------
// CDevDiagSuiteContainer::HandleListBoxEventL
// -----------------------------------------------------------------------------
//
void CDevDiagSuiteContainer::HandleListBoxEventL( CEikListBox* /*aListBox*/, 
                                                 TListBoxEvent aEventType )
	{	
	// if the Select Key has been pressed
	if ((aEventType == MEikListBoxObserver::EEventEnterKeyPressed) ||
	(aEventType == MEikListBoxObserver::EEventItemClicked))
		{ 
		
		}
	}


#ifdef __SERIES60_HELP
// -----------------------------------------------------------------------------
// CDevDiagSuiteContainer::GetHelpContext
// -----------------------------------------------------------------------------
//
void CDevDiagSuiteContainer::GetHelpContext( TCoeHelpContext& /*aContext*/ ) const
	{	
	//aContext.iMajor = KUidSmlSyncApp;
    //aContext.iContext = KDM_HLP_DIAGNOSTICS_APPLICATION;
	}

#else

// -----------------------------------------------------------------------------
// CDevDiagSuiteContainer::GetHelpContext
// -----------------------------------------------------------------------------
//
void CDevDiagSuiteContainer::GetHelpContext( TCoeHelpContext& /*aContext*/ ) const
	{
	}
#endif // __SERIES60_HELP

// -----------------------------------------------------------------------------
// CDevDiagSuiteContainer::FocusChanged
// -----------------------------------------------------------------------------
//
void CDevDiagSuiteContainer::FocusChanged( TDrawNow /*aDrawNow*/ )
	{
	if ( iListBox )
		{
		iListBox->SetFocus( IsFocused() );
		}
	}

// -----------------------------------------------------------------------------
// Display a group of plug-ins that are under the parent suite. 
// -----------------------------------------------------------------------------
//
void CDevDiagSuiteContainer::DisplayChildrenL( TUid aParentUid )
    {
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );

    // Add listbox items.
    CDesCArray* pluginTextArray = static_cast< CDesCArray* >(
        iListBox->Model()->ItemTextArray() );
    pluginTextArray->Reset();

    // Get the root suite.
    MDiagPlugin* plugin;
    HBufC* formattedName = NULL;
    if ( iEngine.PluginPool().FindPlugin( aParentUid, plugin ) == KErrNone )
        {

        iChildren.Reset(); //plug-ins are owned by the engine.
       
        MDiagSuitePlugin* suite = static_cast< MDiagSuitePlugin* >( plugin );
        suite->GetChildrenL( iChildren, MDiagSuitePlugin::ESortByPosition );
        
        AddIconsL(); //Update icon array to match texts
     
        for ( TInt j = 0; j < iChildren.Count(); j++ )
            {            
            HBufC* name = iChildren[j]->GetPluginNameL(
                MDiagPlugin::ENameLayoutListSingle );
            CleanupStack::PushL( name );           
            
            formattedName = HBufC::NewLC( name->Length() + 
                                        KListSuiteElementFormat().Length() );
            formattedName->Des().Format( KListSuiteElementFormat(), j ,name );    
                
            pluginTextArray->AppendL( *formattedName );
            
            CleanupStack::PopAndDestroy( formattedName );
            CleanupStack::PopAndDestroy( name );
            }
                
        iListBox->SetFocus( ETrue );
        iListBox->HandleItemAdditionL();
        iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
        iListBox->DrawDeferred();
        }
    else 
        {
        User::Leave(KErrNotFound);
        }
    }


// -----------------------------------------------------------------------------
// Append aPlugin name onto the listbox.
// -----------------------------------------------------------------------------
//
void CDevDiagSuiteContainer::DisplayPluginL( MDiagPlugin* aPlugin )
    {
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );

    // Add listbox items.
    CDesCArray* pluginTextArray = static_cast< CDesCArray* >(
        iListBox->Model()->ItemTextArray() );
        
    iChildren.Append( aPlugin );
                           
    HBufC* name = aPlugin->GetPluginNameL(
                MDiagPlugin::ENameLayoutListSingle );
    CleanupStack::PushL( name );           
    
    CArrayPtr< CGulIcon >* icons =
        iListBox->ItemDrawer()->ColumnData()->IconArray();
          
    icons->AppendL( aPlugin->CreateIconL() );
            
    HBufC* formattedName = HBufC::NewLC( name->Length() + 
                                  KListSuiteElementFormat().Length() );
    formattedName->Des().Format( KListSuiteElementFormat(), (iChildren.Count() -1), name );    
                
    pluginTextArray->AppendL( *formattedName );
            
    CleanupStack::PopAndDestroy( formattedName );
    CleanupStack::PopAndDestroy( name );
                                   
    iListBox->SetFocus( ETrue );
    iListBox->HandleItemAdditionL();
    iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
    iListBox->DrawDeferred();
    }

// -----------------------------------------------------------------------------
// Returns currently selected plug-in.
// -----------------------------------------------------------------------------
//
void CDevDiagSuiteContainer::SelectedPlugin(MDiagPlugin*& aPlugin) const
    {
    aPlugin = iChildren[ iListBox->CurrentItemIndex() ];
    }

// -----------------------------------------------------------------------------
// CDevDiagSuiteContainer::AddIconsL
// -----------------------------------------------------------------------------
//
void CDevDiagSuiteContainer::AddIconsL()
    {        
    CArrayPtr<CGulIcon>* icons = NULL;
    
    if ( !iListBox->ItemDrawer()->ColumnData()->IconArray() )         
        {
        icons = new (ELeave) CArrayPtrFlat<CGulIcon>(KIconArrayGranularity);
        iListBox->ItemDrawer()->ColumnData()->SetIconArray( icons );
        }
    else 
        {
        icons = iListBox->ItemDrawer()->ColumnData()->IconArray();
        }
    if(icons)
    	{	
    	for ( TInt i = 0; i < iChildren.Count(); ++i )
        {
        icons->AppendL( iChildren[i]->CreateIconL( ));
        }
      }   
    }




// End of File
