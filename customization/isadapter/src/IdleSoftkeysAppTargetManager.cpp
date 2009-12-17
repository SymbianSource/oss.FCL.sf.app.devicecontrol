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
* Description:  DM Idle SoftKey  Adapter
*
*/


#include "IdleSoftkeysAppTargetManager.h"
#include "UiSettingsUtil.h"
// symbian
// s60
#include <centralrepository.h>
// tarm
#include "IsAdapterLiterals.h"
#include "IsAdapterDefines.h"
#include "nsmldmuri.h"
#include "TARMCharConv.h"
#include "TPtrC8I.h"
#include "debug.h"


CIdleSoftkeysAppTargetManager::CIdleSoftkeysAppTargetManager(MSmlDmCallback *& aCallBack)
        : iCallBack( aCallBack )
	{
       	iUiSettingsNotInitialized = ETrue;
	}

CIdleSoftkeysAppTargetManager::~CIdleSoftkeysAppTargetManager()
	{
	if(iUiSettingsNotInitialized == EFalse)
		{
		delete iUiSettings;
		}
	}

// -------------------------------------------------------------------------------------
// CIdleSoftkeysAppTargetManager::ListTargetsL
// This method will list all application targets in the shortcut engine
// -------------------------------------------------------------------------------------
TInt CIdleSoftkeysAppTargetManager::ListTargetsL( CBufFlat &aList )
    {
    RDEBUG( "CIdleSoftkeysAppTargetManager::ListTargetsL() >" );

    TInt i, num ;

   	if(iUiSettingsNotInitialized)
   	{
   		iUiSettings = CUiSettingsUtil::NewL( );
   		iUiSettings->ReadTargetsL();
   		iUiSettingsNotInitialized = EFalse;
   	}   
    
	num = iUiSettings->GetNoOfTargetApps();
		
    for(i=0; i<num; i++)
        {
        if(i>0)
            {
            aList.InsertL( aList.Size(), KNSmlISSeparator() );
            }

        HBufC8* targetName = GenerateNodeNameL( i );
        CleanupStack::PushL( targetName );

        aList.InsertL( aList.Size(), *targetName ); 

        CleanupStack::PopAndDestroy( targetName );
        }

    RDEBUG( "CIdleSoftkeysAppTargetManager::ListTargetsL() <" );
    return KErrNone;
    }


// -------------------------------------------------------------------------------------
// CIdleSoftkeysAppTargetManager::GenerateNodeNameL
// This method generates name for a target according to a given index
// -------------------------------------------------------------------------------------
HBufC8* CIdleSoftkeysAppTargetManager::GenerateNodeNameL(TInt aIndex)
    {
    RDEBUG( "CIdleSoftkeysAppTargetManager::GenerateNodeNameL() >" );

    HBufC8* targetName = HBufC8::NewL( KPrefixTargetNodeName().Length()
                                + MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64 );

    TPtr8 ptr( targetName->Des() );

    ptr.Format(KFormatTargetNodeName, aIndex+1);

    RDEBUG8_2( "CIdleSoftkeysAppTargetManager::GenerateNodeNameL() < %S", &targetName );
    return targetName;
    }


// -------------------------------------------------------------------------------------
// CIdleSoftkeysAppTargetManager::GetTargetFromNodeNameL
// This method parses a node name and returns matching application index and caption
// -------------------------------------------------------------------------------------
void CIdleSoftkeysAppTargetManager::GetTargetFromNodeNameL(const TDesC8& aNodeName, TInt &aIndex, TDes8 &aTargetCaption)
    {
    RDEBUG( "CIdleSoftkeysAppTargetManager::GetTargetFromNodeNameL() >" );

    TInt index = -1;
    TInt compareLenApp = KPrefixTargetNodeName().Length();

    if(aNodeName.Left(compareLenApp) == KFormatTargetNodeName().Left(compareLenApp))
        {
        if(iUiSettingsNotInitialized)
   		{	
   			iUiSettings = CUiSettingsUtil::NewL( );
   			iUiSettings->ReadTargetsL() ;
   			iUiSettingsNotInitialized = EFalse;
   		}
        
        TInt num;
        num = iUiSettings->GetNoOfTargetApps();
		
        TLex8 lex;
        lex.Assign( aNodeName );

        lex.Inc( compareLenApp );
        User::LeaveIfError( lex.Val(index) );
        index--;
        User::LeaveIfError( index>=0 && index<num ? KErrNone : KErrGeneral );

        HBufC8* targetName = GenerateNodeNameL( index );
        CleanupStack::PushL( targetName );

        TPtr8 ptr( targetName->Des() );
        User::LeaveIfError( ptr == aNodeName ? KErrNone : KErrGeneral );

        CleanupStack::PopAndDestroy( targetName );

        HBufC* buffer = HBufC::NewLC( aTargetCaption.MaxLength() );
        RDEBUG_2( "CIdleSoftkeysAppTargetManager::GetTargetFromNodeNameL() HBufC buffer ALLOC %x", buffer);
        TPtr bufferPtr( buffer->Des() );
        User::LeaveIfError(iUiSettings->GetAppTargetCaptionL( index, bufferPtr )) ;
        aTargetCaption.Copy( bufferPtr );
        CleanupStack::PopAndDestroy( buffer );
        aIndex = index;
    }
    else
    {
#ifdef __WINS_DEBUG_TRACE__
       User::Panic( _L("IsAdapter"), 1 );
#else
       User::Leave( KErrNotFound );
#endif
    }

    RDEBUG( "CIdleSoftkeysAppTargetManager::GetTargetFromNodeNameL() <" );
    }


// -------------------------------------------------------------------------------------
// CIdleSoftkeysAppTargetManager::SetCaptionL
// This method sets a new caption to a target (shortcut)
// -------------------------------------------------------------------------------------
void CIdleSoftkeysAppTargetManager::SetCaptionL( const TDesC8& aURI, const TDesC8& aCaption )
    {
    RDEBUG( "CIdleSoftkeysAppTargetManager::SetCaptionL() >" );

    TBuf8<ISADAPTER_SHORTCUT_CAPTION_MAXLENGTH> caption;
    TInt       index;
    GetTargetFromNodeNameL( NSmlDmURI::URISeg( aURI, 3 ), index, caption );

    HBufC* newCaption = CTARMCharConv::ConvertFromUtf8LC( aCaption );
    CleanupStack::PopAndDestroy( newCaption );

    RDEBUG( "CIdleSoftkeysAppTargetManager::SetCaptionL() <" );
    }

//End of file
