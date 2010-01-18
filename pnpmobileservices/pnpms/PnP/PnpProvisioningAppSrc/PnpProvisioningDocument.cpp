/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of PnPMS components
*
*/


// INCLUDE FILES
#include <AknGlobalNote.h>              // for CAknGlobalNote
#include <AknGlobalConfirmationQuery.h> // for CAknGlobalConfirmationQuery
#include <textresolver.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>
#include <StringLoader.h>
#include <PnpProvisioning.rsg>
#include <apgcli.h>
#include <e32uid.h>
#include <CWPEngine.h>      // Using WAP provisioning engine
#include <e32math.h>

#include "PnpProvisioningDocument.h"
#include "PnpProvisioningAppUi.h"
#include "NHeadWrapperParser.h"
#include "PnpLogger.h"
#include "PnpProvisioningSaver.h"





// ================= MEMBER FUNCTIONS =======================

// constructor
CPnpProvisioningDocument::CPnpProvisioningDocument(CEikApplication& aApp)
: CAknDocument(aApp)    
    {
    LOGSTRING( "CPnpProvisioningDocument()" );
    }

// destructor
CPnpProvisioningDocument::~CPnpProvisioningDocument()
    {
    LOGSTRING( "~CPnpProvisioningDocument()" );
    delete iProvisioningData;
    delete iNHwrParser;
    //delete iPeriodic;
    LOGSTRING( "~CPnpProvisioningDocument() done" );
    }

// EPOC default constructor can leave.
void CPnpProvisioningDocument::ConstructL()
    {
    LOGSTRING( "CPnpProvisioningDocument::ConstructL" );
    }

// Two-phased constructor.
CPnpProvisioningDocument* CPnpProvisioningDocument::NewL(
        CEikApplication& aApp)     // CPnpProvisioningApp reference
    {
    LOGSTRING( "CPnpProvisioningDocument::NewL" );
    CPnpProvisioningDocument* self = new (ELeave) CPnpProvisioningDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
    
// ----------------------------------------------------
// CPnpProvisioningDocument::CreateAppUiL()
// constructs CPnpProvisioningAppUi
// ----------------------------------------------------
//
CEikAppUi* CPnpProvisioningDocument::CreateAppUiL()
    {
    LOGSTRING( "CreateAppUiL" );
    iAppUi = new (ELeave) CPnpProvisioningAppUi;
    return iAppUi;
    }


void CPnpProvisioningDocument::OpenFileL( CFileStore*& /*aFileStore*/, RFile& aFile )
    {
    LOGSTRING( "CPnpProvisioningDocument::OpenFileL (CFileStore*& /*aFileStore*/, RFile& aFile)" );
    TFileName fileName;
    aFile.Name( fileName );
    LOGTEXT( fileName );

    TRAPD( err, DoHandleFileL( aFile ) );
    if( err != KErrNone )
        {
        LOGSTRING2( "error in DoHandleFileL: %i", err );
        User::Leave( err );
        }
    iReady = ETrue;
    }


// ----------------------------------------------------
// CPnpProvisioningDocument::OpenFileL
// Overrides CAknDocument::OpenFileL to support document file
// ----------------------------------------------------
//
CFileStore* CPnpProvisioningDocument::OpenFileL(TBool aDoOpen,const TDesC& aFilename,RFs& aFs)
    {
    LOGSTRING( "CPnpProvisioningDocument::OpenFileL (TBool aDoOpen,const TDesC& aFilename,RFs& aFs)" );

    CFileStore* store = NULL;
    store = CAknDocument::OpenFileL( aDoOpen, aFilename, aFs );
    LOGSTRING2( "store:%i", store );
    LOGSTRING2( "aDoOpen:%i", aDoOpen );
    
    iReady = ETrue;

    return store;
    }

// ----------------------------------------------------
// CPnpProvisioningDocument::RestoreL()
// restore data from persistent store
// ----------------------------------------------------
//      
void CPnpProvisioningDocument::RestoreL(const CStreamStore& /*aStore*/, const CStreamDictionary& /*aStreamDic*/)
    {
    }

// ----------------------------------------------------
// CPnpProvisioningDocument::StoreL()
// store data to persistent store
// ----------------------------------------------------
//          
void CPnpProvisioningDocument::StoreL(CStreamStore& /*aStore*/, CStreamDictionary& /*aStreamDic*/) const
    {
    }

// ----------------------------------------------------
// CPnpProvisioningDocument::HandleFileL
// ----------------------------------------------------
void CPnpProvisioningDocument::HandleFileL(TBool aDoOpen,const TDesC& aFilename,RFs& aFs)
    {
    LOGSTRING( "CPnpProvisioningDocument::HandleFileL" );
    LOGSTRING( "aFilename:" );
    LOGTEXT( aFilename );

    if(aDoOpen)
        {
        LOGSTRING( "aDoOpen" );

        RFile file;
        if(file.Open(aFs, aFilename,EFileRead)==KErrNone)
            {
            LOGSTRING( "File opened" );
            CleanupClosePushL( file );
            DoHandleFileL( file );
            CleanupStack::PopAndDestroy(); // file.Close();
            }
        }
    else
        {
        LOGSTRING( "aDoOpen was false" );
        User::Leave( KErrGeneral );
        }
    LOGSTRING( "Exiting HandleFileL" );
    }

// ----------------------------------------------------
// CPnpProvisioningDocument::DoHandleFileL
// ----------------------------------------------------
void CPnpProvisioningDocument::DoHandleFileL( RFile& aFile )
    {
    LOGSTRING( "CPnpProvisioningDocument::DoHandleFileL" );
    TInt filesize = 0;
    aFile.Size( filesize );
    delete iProvisioningData;
    iProvisioningData = 0;
    iProvisioningData = HBufC8::NewL( filesize );
    TPtr8 ptr = iProvisioningData->Des();
    User::LeaveIfError( aFile.Read( ptr, filesize ) );
    LOGSTRING( "Exiting DoHandleFileL" );
    }

// ----------------------------------------------------
// CPnpProvisioningDocument::SaveSettingsL
// ----------------------------------------------------

THttpProvStates::TProvisioningStatus CPnpProvisioningDocument::SaveSettingsL()
    {
    LOGSTRING( "CPnpProvisioningDocument::SaveSettingsL" ); 
    
    THttpProvStates::TProvisioningStatus status = THttpProvStates::EStatusOk;

    if( !iProvisioningData )
        {
        User::Leave( THttpProvStates::EStatusWrapperParsingFailed );
        }

    iNHwrParser = CNHeadWrapperParser::NewL( *iProvisioningData );
    TInt err = iNHwrParser->Parse();
    if( err != THttpProvStates::EStatusOk )
        {
        LOGSTRING2( "err from parse:%i", err ); 
        User::Leave( err );
        }

    TPtrC8 content = iNHwrParser->GetContentL();

    // Create WPEngine
    CWPEngine* wpengine = CWPEngine::NewL();
    CleanupStack::PushL( wpengine );

    LOGSTRING( "wpengine->ImportDocumentL" );
    TRAPD( result, wpengine->ImportDocumentL( content ) );
    User::LeaveIfError( result );
    LOGSTRING( "wpengine->ImportDocumentL ok" );

    LOGSTRING( "wpengine->PopulateL" );
    wpengine->PopulateL();
    LOGSTRING( "wpengine->PopulateL success" );

    // Empty messages are not supported
    if( wpengine->ItemCount() == 0 )
        {
        LOGSTRING("No items in provisioning message!");
        User::Leave( THttpProvStates::EStatusWrapperParsingFailed );
        }
        

    TInt numSaved( 0 );
    CPnpProvisioningSaver* saver = new(ELeave) CPnpProvisioningSaver( *wpengine, EFalse, EFalse );
    TInt retValue( KErrNone );
    LOGSTRING("saver created");
    // dismiss waitdialog, saver will show own waitdialog
    //iAppUi->WaitDialogDismissedL(EAknSoftkeyEmpty);
  
    TRAPD( saverErr, retValue = saver->ExecuteLD( numSaved ) );
    
    if( retValue == KErrCancel )
        {
        User::Leave( THttpProvStates::EStatusRejected ); // User cancelled
        }
    else if( saverErr != KErrNone || retValue != KErrNone )
        {
        User::Leave( THttpProvStates::EStatusStorageFailed );
        }    

    // check if service activation is enabled if yes then display notes specific to application
    // settings
	TBool activate(EFalse);
     
    // If activation enable follow different UI Flow
    // UI Flow requires changes if any other application requires
    // service UI flow to be changed as of now applicable only for
    // Email service activation
    activate =  iAppUi -> ServiceActivationEnabledL();


    LOGSTRING( "saver executed" );
    if( numSaved >= 0 )
        {
        LOGSTRING( "numSaved >= 0" );
        // Check if any setting should be made default
        TBool setAsDefault( EFalse );
        TInt itemCount( wpengine->ItemCount() );
        for( TInt i( 0 ); i < itemCount && !setAsDefault; i++ )
            {
            // 1. Set as default if there was no "Provisioning Activation" header
            // in the head wrapper or if the header asks to make the settings default
            // 2. Also check if the settings _can_ be set as default at all
            if( wpengine->CanSetAsDefault( i ) )
                {
                LOGSTRING( "set as default" );
                setAsDefault = ETrue;
                }
            }

        // The default value is "ask from user" (if there is no Provisioning-Activation
        // header in the head wrapper, it should be asked from the user)
        TProvisioningActivation activation = iNHwrParser->GetProvisioningActivation();

        if( activation == EProvisioningActivationOff )
            {
            setAsDefault = EFalse;
            }
        else if( activation == EProvisioningActivationUser )
            {
            LOGSTRING( "Ask confirmation for saving as default" );
	     		
	     		if(!activate)
 	     		{
            // Ask confirmation from the user before saving as default settings
            HBufC* msgDefault = CEikonEnv::Static()->AllocReadResourceLC(
                R_TEXT_SET_DEFAULT ); 
                
            CAknGlobalConfirmationQuery* query2 = CAknGlobalConfirmationQuery::NewL();
            CleanupStack::PushL( query2 );
            TRequestStatus status2;
            query2->ShowConfirmationQueryL(
                status2, 
                *msgDefault, 
                R_AVKON_SOFTKEYS_YES_NO__YES,
                R_QGN_NOTE_QUERY_ANIM);
                
            User::WaitForRequest( status2 );
            CleanupStack::PopAndDestroy( 2 );
            
            if( status2 != EAknSoftkeyYes )
                {
                setAsDefault = EFalse; // User rejected
                }
	      	}
	      	else // service activation enabled
	      	{
			   	setAsDefault = ETrue;	
	      	}
            }

        if( setAsDefault )
            {
            LOGSTRING( "create setter" );
            CPnpProvisioningSaver* setter = NULL;
            TInt numSet( 0 );
            if(!activate)
            {
            setter = new(ELeave) CPnpProvisioningSaver( *wpengine, ETrue, ETrue );          
            iAppUi->WaitDialogDismissedL(EAknSoftkeyEmpty);
            }
            else
            {
            setter = new(ELeave) CPnpProvisioningSaver( *wpengine, ETrue, EFalse );          
            }
            LOGSTRING( "setter->ExecuteLD" );
            setter->ExecuteLD( numSet );
            }

		// Display note to user that settings are saved successfully
		// This note is generic not incase service activation is disabled
		if(!activate)
		{
        // All settings saved
        HBufC* msgTextSaved = CEikonEnv::Static()->
            AllocReadResourceLC( R_TEXT_SAVED ); 

        CAknGlobalNote* globalNote = CAknGlobalNote::NewL();
        CleanupStack::PushL( globalNote );
        globalNote->ShowNoteL( EAknGlobalInformationNote , *msgTextSaved );
        CleanupStack::PopAndDestroy( 2 );
        }
         
    }
    else
        {
        User::Leave( THttpProvStates::EStatusStorageFailed );
        }

    LOGSTRING( "exiting SaveSettingsL" );
    CleanupStack::PopAndDestroy(wpengine);
    LOGSTRING( "PopAndDestroy(wpengine) done" );

    return status;
    }

const TDesC8& CPnpProvisioningDocument::GetReportUrlL()
    {
    if( !iNHwrParser )
        {
        User::Leave( KErrNotFound );
        }
    return( iNHwrParser->GetReportUrlL() );
    }

// End of File  
