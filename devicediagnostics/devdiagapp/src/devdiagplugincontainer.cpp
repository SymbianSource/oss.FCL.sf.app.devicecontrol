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
* Description: Implementation of diagnostics components
*
*/


// INCLUDE FILES

#include "devdiagplugincontainer.h"
#include "devdiagengine.h"   
#include "devdiag.pan" 
#include "devdiagutil.h" //Time conversion functions

#include <DiagFrameworkDebug.h>   
#include <devdiagapp.rsg>
#include <DiagPlugin.h>  
#include <DiagPluginPool.h> 
#include <DiagSuitePlugin.h>   
#include <DiagResultsDatabaseItem.h>
#include <eikclbd.h>                // CColumnListBoxData
#include <aknmessagequerydialog.h> 
#include <data_caging_path_literals.hrh>
#include <aknPopup.h>		// popup dialogs
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <textresolver.h>
#include <AknIconArray.h>
#include <AknsUtils.h>
#include <featmgr.h>
#include <AknWaitDialog.h> 
#include <devdiagapp.mbg>


#include <AknsUtils.h>
///@@@KSR: changes for BAD Warnings - #177-D: variable "KDiagSpace" was declared but never referenced
//_LIT( KDiagSpace, " " );
_LIT( KListTestElementFormatDouble,  "%d\t%S\t%S" );
_LIT( KDevDiagAppIconFileName, "\\resource\\apps\\devdiagapp.mif");

const TInt KLastResultTextLength(256);
const TInt KIconArrayGranularity(20);

//INCORRECT!
const MDiagPlugin::TNameLayoutType KLayoutType( MDiagPlugin::ENameLayoutListSingle );


// ========================== MEMBER FUNCTIONS =================================

// -----------------------------------------------------------------------------
// CDevDiagPluginContainer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDevDiagPluginContainer* CDevDiagPluginContainer::NewL( CDevDiagPluginView* aView,
                                                         CDevDiagEngine& aEngine,
                                                const TRect&    aRect )
    {
    CDevDiagPluginContainer* self = new( ELeave ) 
                                     CDevDiagPluginContainer( aView, aEngine );
    
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CDevDiagPluginContainer::CDevDiagPluginContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDevDiagPluginContainer::CDevDiagPluginContainer( CDevDiagPluginView* aView,
                                                    CDevDiagEngine& aEngine )
    : iView( aView ), iEngine(aEngine), iModelUpdated( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CDevDiagPluginContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDevDiagPluginContainer::ConstructL( const TRect& aRect )
    {
    LOGSTRING( "CDevDiagPluginContainer::ConstructL" );
    CreateWindowL();

	iListBox = new ( ELeave ) CAknDoubleLargeStyleListBox;
	
	iListBox->SetContainerWindowL( *this );
	iListBox->ConstructL( this, EAknListBoxSelectionList );

	iListBox->CreateScrollBarFrameL( ETrue );
	iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
	                                            CEikScrollBarFrame::EOff,
	                                            CEikScrollBarFrame::EAuto );
                	
    AddIconsL();

	iListBox->UpdateScrollBarsL();
	//iListBox->ScrollBarFrame()->MoveVertThumbTo( 0 );		

	iListBox->SetListBoxObserver( this );
			
    SetRect( aRect );
    ActivateL();    
    }

// -----------------------------------------------------------------------------
// CDevDiagPluginContainer::~CDevDiagPluginContainer
// -----------------------------------------------------------------------------
//
CDevDiagPluginContainer::~CDevDiagPluginContainer()
    {
    delete iListBox;
        
    //Do not call ResetAndDestroy, because plug-ins are not owned.
    iChildren.Close();
    }
    
// -----------------------------------------------------------------------------
// CDevDiagPluginContainer::SizeChanged
// -----------------------------------------------------------------------------
//
void CDevDiagPluginContainer::SizeChanged()
    {
    iListBox->SetRect( Rect() );    // Mandatory, otherwise not drawn
    }

// -----------------------------------------------------------------------------
// CDevDiagPluginContainer::HandleResourceChange
// -----------------------------------------------------------------------------
//
void CDevDiagPluginContainer::HandleResourceChange( TInt aType )
    {    
    CCoeControl::HandleResourceChange( aType );
    
    //Handle change in layout orientation
    if ( aType == KEikDynamicLayoutVariantSwitch || 
         aType == KAknsMessageSkinChange )
        {
        if ( aType == KAknsMessageSkinChange )
        	{
        	// Skin changed; we'll have to reload the icon array
            CArrayPtr<CGulIcon>* iconArray =
                iListBox->ItemDrawer()->FormattedCellData()->IconArray();
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
// CDevDiagPluginContainer::CountComponentControls
// -----------------------------------------------------------------------------
//
TInt CDevDiagPluginContainer::CountComponentControls() const
    {
    TInt retval( 0 );
    if ( iListBox )
        {
        retval = 1;
        }
    return retval; // return nbr of controls inside this container
    }

// -----------------------------------------------------------------------------
// CDevDiagPluginContainer::ComponentControl
// -----------------------------------------------------------------------------
//
CCoeControl* CDevDiagPluginContainer::ComponentControl( TInt aIndex ) const
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
// CDevDiagPluginContainer::Draw
// -----------------------------------------------------------------------------
//
void CDevDiagPluginContainer::Draw( const TRect& /* aRect */ ) const
    {
    }


// -----------------------------------------------------------------------------
// CDevDiagPluginContainer::HandleControlEventL
// -----------------------------------------------------------------------------
//

void CDevDiagPluginContainer::HandleControlEventL( CCoeControl* /* aControl */,
                                                TCoeEvent /* aEventType */ )
    {
    }

// -----------------------------------------------------------------------------
// CDevDiagPluginContainer::OfferKeyEventL
// -----------------------------------------------------------------------------
//

TKeyResponse CDevDiagPluginContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
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
// CDevDiagPluginContainer::HandleListBoxEventL
// -----------------------------------------------------------------------------
//
void CDevDiagPluginContainer::HandleListBoxEventL( CEikListBox* /*aListBox*/, 
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
// CDevDiagPluginContainer::GetHelpContext
// -----------------------------------------------------------------------------
//
void CDevDiagPluginContainer::GetHelpContext( TCoeHelpContext& /*aContext */ ) const
	{	
	//aContext.iMajor = KUidSmlSyncApp;
    //aContext.iContext = KDM_HLP_DIAGNOSTICS_APPLICATION;
	}

#else

// -----------------------------------------------------------------------------
// CDevDiagPluginContainer::GetHelpContext
// -----------------------------------------------------------------------------
//
void CDevDiagPluginContainer::GetHelpContext( TCoeHelpContext& /*aContext*/ ) const
	{
	}
#endif // __SERIES60_HELP

// -----------------------------------------------------------------------------
// CDevDiagPluginContainer::FocusChanged
// -----------------------------------------------------------------------------
//
void CDevDiagPluginContainer::FocusChanged( TDrawNow /*aDrawNow*/ )
	{
	if ( iListBox )
		{
		iListBox->SetFocus( IsFocused() );
		}
	}


// -----------------------------------------------------------------------------
// Change text of the focused item into "Testing ..."
// -----------------------------------------------------------------------------
//
void CDevDiagPluginContainer::DisplayStartTestingL()
    {
    LOGSTRING( "CDevDiagPluginContainer::DisplayStartTestingL" ); 
    __ASSERT_ALWAYS( iModelUpdated, 
                     Panic( EDevDiagListboxModelNotUpdated ) ); 
    
    // Add listbox items.
    CDesCArray* pluginTextArray = static_cast< CDesCArray* >(
        iListBox->Model()->ItemTextArray() );
    
    __ASSERT_ALWAYS( pluginTextArray->Count() == iChildren.Count(), 
                     Panic( EDevDiagListboxContainerModel ) );
    
    MDiagPlugin* plugin;
    HBufC* formattedName = NULL;
    
    SelectedPlugin ( plugin );
    
    HBufC* name = plugin->GetPluginNameL( KLayoutType );
    CleanupStack::PushL( name );      
     
    HBufC* testingText = StringLoader::LoadL( R_DIAG_LST_TEST_ONGOING );
    CleanupStack::PushL ( testingText );
     
    formattedName = HBufC::NewLC( name->Length() + 
                                  KListTestElementFormatDouble().Length() + 
                                  testingText->Length() );
    formattedName->Des().Format( KListTestElementFormatDouble(), 0, name, testingText );

    TInt index = iListBox->CurrentItemIndex();           
    pluginTextArray->Delete( index );
    
    pluginTextArray->InsertL( index, *formattedName  );    
    
    CleanupStack::PopAndDestroy( formattedName );
    CleanupStack::PopAndDestroy( testingText );         
    CleanupStack::PopAndDestroy( name );
        
    iListBox->SetFocus( ETrue );
    iListBox->HandleItemAdditionL();
    iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
    iListBox->DrawDeferred();
    }
 
// -----------------------------------------------------------------------------
// Move "Testing ..." one step lower. This is needed when a group of plug-ins
// are executed.
// -----------------------------------------------------------------------------
//
 void CDevDiagPluginContainer::MoveStartTestingDownL()
    {
    if ( iListBox->CurrentItemIndex() + 1 < iChildren.Count() )
        {
        iListBox->SetCurrentItemIndex ( iListBox->CurrentItemIndex() +1 );
        DisplayStartTestingL();
        }   
    }


// -----------------------------------------------------------------------------
// Set focus at the beginning of the listbox.
// -----------------------------------------------------------------------------
//
void CDevDiagPluginContainer::ResetL()
    {
    iListBox->SetCurrentItemIndexAndDraw  ( 0 );
    }


// -----------------------------------------------------------------------------
// Create the plug-in model.
// The function needs to know the parent uid and its children are loaded.
// -----------------------------------------------------------------------------
// 
void CDevDiagPluginContainer::UpdateModelL( TUid aParentSuiteUid )    
    {
    LOGSTRING( "CDevDiagPluginContainer::UpdateModelL" );         
    MDiagPlugin* plugin;
    if ( iEngine.PluginPool().FindPlugin( aParentSuiteUid, plugin ) == KErrNone )
        {
        iChildren.Reset(); //plug-ins are owned by the engine.
       
        MDiagSuitePlugin* suite = static_cast< MDiagSuitePlugin* >( plugin );
        suite->GetChildrenL( iChildren, MDiagSuitePlugin::ESortByPosition );
        
        iModelUpdated = ETrue;
        }
    else
        {
        Panic( EDevDiagListboxFindPluginFailure );
        }
    }
    

// -----------------------------------------------------------------------------
// The plug-in execution was not performed. Display the text on the listbox.
// -----------------------------------------------------------------------------
//    
void CDevDiagPluginContainer::DisplayNotPerformedL( CDesCArray& aListboxModel, 
                                                    MDiagPlugin*& aPlugin )    
    {    
    LOGSTRING( "CDevDiagPluginContainer::DisplayNotPerformedL" );                    
    HBufC* name = aPlugin->GetPluginNameL( KLayoutType );
                
    CleanupStack::PushL( name );
    
    HBufC* result = GetPluginResultL( ENotPerformed );    
    CleanupStack::PushL( result );
            
    HBufC* formattedName = HBufC::NewLC( name->Length() + 
                                  result->Length() +
                                  KListTestElementFormatDouble().Length() );
    formattedName->Des().Format( KListTestElementFormatDouble(), 0, name, result);
            
    aListboxModel.AppendL( *formattedName );
            
    CleanupStack::PopAndDestroy( formattedName );
    CleanupStack::PopAndDestroy( result );
    
    CleanupStack::PopAndDestroy( name );         
    }
    

// -----------------------------------------------------------------------------
// Display icon and test result for the plug-in.
// -----------------------------------------------------------------------------
//
void CDevDiagPluginContainer::DisplayLastResultL( CDesCArray& aListboxModel,
                                                  MDiagPlugin*& aPlugin,
                                                  CDiagResultsDatabaseItem& aItem )    
    {   
    LOGSTRING( "CDevDiagPluginContainer::DisplayLastResultL" );                    
    
    HBufC* name = aPlugin->GetPluginNameL( KLayoutType );
    CleanupStack::PushL( name );           
            
    TBuf<KLastResultTextLength> execTime;
                                
    TTime started = aItem.TimeStarted();
            
    if ( TDevDiagUtil::IsToday(started))
        {
        TDevDiagUtil::GetTimeTextL( execTime, started );
        LOGTEXT( execTime );
        }
    else 
        {
        TDevDiagUtil::GetDateTextL( execTime, started );
        LOGTEXT( execTime );
        }
      
    TInt iconIndex = -1;  
    
    HBufC* result = ConvertResultLC( execTime, iconIndex, &aItem );          
                                                                                                    
    HBufC* formattedName = HBufC::NewLC( name->Length() + 
                                  result->Length() +
                                  KListTestElementFormatDouble().Length());
                                 
    // Format icon index, then the name of the plug-in and then time.                                  
    formattedName->Des().Format( KListTestElementFormatDouble(), 
                                 iconIndex, 
                                 name, 
                                 result);    
                
    aListboxModel.AppendL( *formattedName );
                        
    CleanupStack::PopAndDestroy( formattedName );
    CleanupStack::PopAndDestroy( result );
    CleanupStack::PopAndDestroy( name );                
    }
    

// -----------------------------------------------------------------------------
// Refresh the listbox with last results.
// -----------------------------------------------------------------------------
//    
void CDevDiagPluginContainer::DisplayChildrenWithResultsL(
                        RPointerArray<CDiagResultsDatabaseItem>& aArray )
    {
    LOGSTRING( "CDevDiagPluginContainer::DisplayChildrenWithResultsL" );
    
    __ASSERT_ALWAYS( iModelUpdated, 
                     Panic( EDevDiagListboxModelNotUpdated ) ); 
                   
    // arrays must match                     
    __ASSERT_ALWAYS( aArray.Count() == iChildren.Count(), 
                     Panic( EDevDiagContainerArrayMismatch ) );                      
    
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );

    // Add listbox items.
    CDesCArray* pluginTextArray = static_cast< CDesCArray* >(
        iListBox->Model()->ItemTextArray() );
    pluginTextArray->Reset();
         
    // Go through the last result list         
    for (TInt i = 0; i < aArray.Count(); ++i)
        {                             
        if ( !aArray[i] ) //Item has not been performed (last result not found)
            {
            LOGSTRING2("Item number: %d was null", i );                            
            DisplayNotPerformedL( *pluginTextArray, iChildren[i] );                                   
            }
        else  //last result found, display a test result
            {   
            LOGSTRING2("Result found: %d", i );                 
            DisplayLastResultL( *pluginTextArray, iChildren[i], *aArray[i] );                                        
            }
        }   
        
    iListBox->SetFocus( ETrue );
    iListBox->HandleItemAdditionL();
    iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
    iListBox->DrawDeferred();
    
    LOGSTRING( "CDevDiagPluginContainer::DisplayChildrenWithResultsL end" );
    }


// -----------------------------------------------------------------------------
// Get localised test result text.
// -----------------------------------------------------------------------------
// 
HBufC* CDevDiagPluginContainer::GetPluginResultL( TContainerResult aResult ) const
    {
    LOGSTRING( "CDevDiagPluginContainer::GetPluginResultL" );
    
    switch ( aResult  )
        {
        case ESuccess:
            return StringLoader::LoadL( R_DIAG_LST_TEST_PASS  );

        case EFailed:
            return StringLoader::LoadL ( R_DIAG_LST_TEST_FAILED  );

        case ENotPerformed:
            return StringLoader::LoadL( R_DIAG_LIST_NOT_PERFORMED  );

        default:
            Panic( EDevDiagApplicationInvalidResultIndex );            
        }
        
    return NULL;    
    }


// -----------------------------------------------------------------------------
// Get localised test result text and the icon array
// -----------------------------------------------------------------------------
// 
HBufC* CDevDiagPluginContainer::ConvertResultLC( const TDesC& aString, 
                                                TInt& aIconIndex, 
                                                CDiagResultsDatabaseItem* aResult ) const
    {
    LOGSTRING( "CDevDiagPluginContainer::ConvertResultL" );
    if ( !aResult )
        {
        return StringLoader::LoadLC( R_DIAG_LIST_NOT_PERFORMED  );
        }
    
    switch ( aResult->TestResult() )
        {
        case CDiagResultsDatabaseItem::ESuccess:
            aIconIndex = 1;
            return StringLoader::LoadLC( R_DIAG_LST_TEST_PASS, aString );

        case CDiagResultsDatabaseItem::EFailed:
            aIconIndex = 2;
            return StringLoader::LoadLC ( R_DIAG_LST_TEST_FAILED, aString );      
        case CDiagResultsDatabaseItem::ESkipped:
        case CDiagResultsDatabaseItem::ECancelled:
        case CDiagResultsDatabaseItem::EQueuedToRun:
            {
            return StringLoader::LoadLC( R_DIAG_LIST_NOT_PERFORMED  );
            }

        default:
            LOGSTRING2("PANIC: Results item had a result: %d ", aResult->TestResult() );
            Panic( EDevDiagApplicationInvalidResultIndex );            
        }
        
    return NULL;
    }

void CDevDiagPluginContainer::SelectedPlugin(MDiagPlugin*& aPlugin)
    {
    LOGSTRING( "CDevDiagPluginContainer::SelectedPluginL" );
    aPlugin = iChildren[ iListBox->CurrentItemIndex() ];
    }

// -----------------------------------------------------------------------------
// Load icons. 
//
// Icons are:
// Not performed
// Passed
// Failed
// -----------------------------------------------------------------------------
//
void CDevDiagPluginContainer::AddIconsL()
    {   
    LOGSTRING( "CDevDiagPluginContainer::AddIconsL" );        
	CArrayPtr<CGulIcon>* icons = new (ELeave) CArrayPtrFlat<CGulIcon>
	                                                (KIconArrayGranularity);
	                                                
	iListBox->ItemDrawer()->FormattedCellData()->SetIconArray( icons );
    
	CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
        
    LOGSTRING( "CDevDiagPluginContainer::AddIconsL 1" ); 
        
    AknIconUtils::CreateIconL(
        bitmap,
        mask,
        KDevDiagAppIconFileName(),
        EMbmDevdiagappQgn_prop_cp_diag_not,
        EMbmDevdiagappQgn_prop_cp_diag_not_mask );

    icons->AppendL( CGulIcon::NewL( bitmap, mask ));
    
    LOGSTRING( "CDevDiagPluginContainer::AddIconsL 2" ); 
    
    AknIconUtils::CreateIconL(
        bitmap,
        mask,
        KDevDiagAppIconFileName(),
        EMbmDevdiagappQgn_prop_cp_diag_pass,
        EMbmDevdiagappQgn_prop_cp_diag_pass_mask );
    
    LOGSTRING( "CDevDiagPluginContainer::AddIconsL 3" );     

    icons->AppendL( CGulIcon::NewL( bitmap, mask ));
    
    AknIconUtils::CreateIconL(
        bitmap,
        mask,
        KDevDiagAppIconFileName(),
        EMbmDevdiagappQgn_prop_cp_diag_fail,
        EMbmDevdiagappQgn_prop_cp_diag_fail_mask );

	// Create the icon.
    icons->AppendL( CGulIcon::NewL( bitmap, mask ));
    
    LOGSTRING( "CDevDiagPluginContainer::AddIconsL end" );            
    }

// End of File
