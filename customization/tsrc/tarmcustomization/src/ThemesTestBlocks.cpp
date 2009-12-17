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
* Description: Implementation of customization components
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifItemParser.h>
#include <Stiftestinterface.h>
#include "ThemesTest.h"
#include "TestDmCallback.h"
#include "TestDmDDFObject.h"
#include "FileCoderB64.h"
#include "TestParams.h"
#include "policyengineclient.h"
//#include "TerminalControlClient.h"
//#include "TerminalControl3rdPartyAPI.h"

#include "nsmldmuri.h"
//#include "rfsClient.h"

#include <CentralRepository.h>
//#include "DMUtilClient.h"

#include "nsmlprivateapi.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CThemesTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CThemesTest::Delete() 
    {
    
    }
    
// -----------------------------------------------------------------------------
// CThemesTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CThemesTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {
   
    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", CThemesTest::ExampleL ),
        ENTRY( "Seconda", CThemesTest::SecondaL ),
        ENTRY( "Install", CThemesTest::InstallL ),

        ENTRY( "LoadL",			CThemesTest::LoadL ),
        ENTRY( "AddDDFCheckL",	CThemesTest::AddDDFCheckL ),

        ENTRY( "DDFVersionL",	CThemesTest::DDFVersionL ),
		ENTRY( "DDFStructureL",	CThemesTest::DDFStructureL ),

		ENTRY( "UpdateLeafObjectL",	CThemesTest::UpdateLeafObjectL ),
		ENTRY( "UpdateLeafObjectStreamL",	CThemesTest::UpdateLeafObjectStreamL ),
		ENTRY( "DeleteObjectL",	CThemesTest::DeleteObjectL ),
		ENTRY( "FetchLeafObjectL",	CThemesTest::FetchLeafObjectL ),
		ENTRY( "FetchLeafObjectSizeL",	CThemesTest::FetchLeafObjectSizeL ),
		ENTRY( "ChildURIListL",	CThemesTest::ChildURIListL ),

		ENTRY( "AddNodeObjectL",	CThemesTest::AddNodeObjectL ),
		ENTRY( "ExecuteCommandL",	CThemesTest::ExecuteCommandL ),
		ENTRY( "ExecuteCommandStreamL",	CThemesTest::ExecuteCommandStreamL ),
		ENTRY( "CopyCommandL",	CThemesTest::CopyCommandL ),
		ENTRY( "StartAtomicL",	CThemesTest::StartAtomicL ),
		ENTRY( "CommitAtomicL",	CThemesTest::CommitAtomicL ),
		ENTRY( "RollbackAtomicL",	CThemesTest::RollbackAtomicL ),
		ENTRY( "StreamingSupportL",	CThemesTest::StreamingSupportL ),
		ENTRY( "StreamCommittedL",	CThemesTest::StreamCommittedL ),
		ENTRY( "CompleteOutstandingCmdsL",	CThemesTest::CompleteOutstandingCmdsL ),
      //all are related to TC module..So commented
		/*ENTRY( "RRRun", CThemesTest::RRRunL ),
		ENTRY( "CreateFile",	CThemesTest::CreateFileL ),
		ENTRY( "RFSCommand",	CThemesTest::RFSCommandL ),		
		ENTRY( "LoadPolicy",	CThemesTest::LoadPolicyL ),
		ENTRY( "SetCertificate",	CThemesTest::SetCertificateL ),
		ENTRY( "ConnectToPolicyServer",	CThemesTest::ConnectToPolicyServerL ),
		ENTRY( "ConnectToDMUTIL",	CThemesTest::ConnectToDMUTILL ),
		ENTRY( "ThirdPartyCommand",	CThemesTest::ThirdPartyCommandL), */
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }    

// -----------------------------------------------------------------------------
// CThemesTest::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CThemesTest::ExampleL( CStifItemParser& aItem )
    {
    
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("In Example") );
    // Print to log file
    iLog->Log(_L("In Example"));

    TInt i = 0;
    TPtrC string;
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, _L("ThemesTest"), 
                                _L("Param[%i]: %S"), i, &string );
        i++;
        }

    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// CThemesTest::SecondaL
// Seconda test method function.
// -----------------------------------------------------------------------------
//
TInt CThemesTest::SecondaL( CStifItemParser& aItem )
    {
    
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("In Seconda") );
    // Print to log file
    iLog->Log(_L("In Seconda"));

    TInt i = 0;
    TPtrC string;
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, _L("ThemesTest"), 
                                _L("Param[%i]: %S"), i, &string );
        i++;
        }
        
        
	// Here it starts
	
	const TUint KThemesAdapterImplUid = 0x0FDDEF05;

	TUid iuid = 
		{
		KThemesAdapterImplUid
		};
		
/*	Cpmatest *cb = Cpmatest::NewL( iLog );
	CleanupStack::PushL( cb );	// 1
*/	
	//CSmlDmAdapter *adapter = CSmlDmAdapter::NewL( implInfo->ImplementationUid(), *this );
//	CSmlDmAdapter *adapter = CSmlDmAdapter::NewL( iuid, *this );
	CTestDmCallback *callback = CTestDmCallback::NewL( iLog );
	CleanupStack::PushL( callback );
	
	CSmlDmAdapter *adapter = CSmlDmAdapter::NewL( iuid, *callback );
	CleanupStack::PushL( adapter );
	
	CBufBase* version = CBufFlat::NewL( 16 );
	CleanupStack::PushL( version );

	adapter->DDFVersionL( *version );
	iLog->Log( _L("Version is")) ;
	iLog->Log( version->Ptr(0) ) ;


	CTestDmDDFObject* ddfRoot = CTestDmDDFObject::NewLC( iLog ); //, aNodeName );
	ddfRoot->SetNameL( _L8( "/" ) );

	adapter->DDFStructureL( *ddfRoot );
	
	ddfRoot->DumpL( _L8(""), EFalse );
	
	ddfRoot->DumpL( _L8("") );
	
	CleanupStack::PopAndDestroy( ddfRoot );
	ddfRoot = 0;
	CleanupStack::PopAndDestroy( version );
	version = 0;

	CleanupStack::PopAndDestroy( adapter );
	adapter = 0;
	
	CleanupStack::PopAndDestroy( callback );
	callback = 0;

	
/*
MSmlDmDDFObject& CTestDmDDFObject::AddChildObjectGroupL()
	{
	iLog->Log( _L( "AddChildObjectGroupL, ''" ) );
	CTestDmDDFObject* child = CTestDmDDFObject::NewLC( iLog ); //, KNullDesC );
	child->SetAsObjectGroup();
	iChildren.AppendL( child );
	CleanupStack::Pop( child ); // Don't destroy
	return *child;
	}
*/

/*
	HBufC8 *addnodename = HBufC8::NewLC( nodename.Length() + 1 + KPolicyNode().Length() ) ;
	addnodename->Des().Copy( KPolicyNode );
	addnodename->Des().Append( nodename ) ;
	adapter->AddNodeObjectL( *addnodename, KNullDesC8, 2);
	if ( iStatus == MSmlDmAdapter::EOk )
		{
		
		RFs fs ;
		LEAVE_IF_ERROR( fs.Connect(), _L( "Could not connect fileserver: %d"  ) );
		
		CleanupClosePushL( fs );
		RFile file ;
		LEAVE_IF_ERROR( file.Open(fs,datafile,EFileRead), _L( "Could not open file: %d" ) );
		
		CleanupClosePushL( file );
		TInt dataSize ;
		LEAVE_IF_ERROR( file.Size( dataSize ), _L( "Could not get file size: %d" ) );
		HBufC8 *nodedata = HBufC8::NewLC ( dataSize );
		TPtr8 nodedataptr( nodedata->Des() );
		LEAVE_IF_ERROR( file.Read( nodedataptr ), _L( "Could not read file: %d" ) );
		HBufC8 *datanode = HBufC8::NewLC( addnodename->Des().Length() + KDataLeafEnd().Length() );
		datanode->Des().Copy( *addnodename );
		datanode->Des().Append( KDataLeafEnd() );
		
		adapter->UpdateLeafObjectL(*datanode, KNullDesC8(), nodedataptr, KPolicyMimeType(), 3);
		if ( iStatus == MSmlDmAdapter::EOk )
			{
			
			}
		else
			{
			iLog->Log( _L("Add Test Adding Policy data failed! %d" ), iStatus );	
			ret = KErrGeneral;
			}
		
		CleanupStack::PopAndDestroy( datanode );
		CleanupStack::PopAndDestroy( nodedata );
		CleanupStack::PopAndDestroy( ); // file
		CleanupStack::PopAndDestroy(  ); // fs
		}
	else
		{
		iLog->Log( _L("Add Test Adding Policy node! %d" ), iStatus );	
		ret = KErrGeneral ;
		}
		
	CleanupStack::PopAndDestroy( addnodename );
	CleanupStack::PopAndDestroy( version );
	CleanupStack::PopAndDestroy( adapter );
//	CleanupStack::PopAndDestroy( cb );
*/


	iLog->Log( _L("Seconda test complete with status %d" ), 0 ); //sf-ret );	
	        

    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CThemesTest::InstallL
// Install test method function.
// -----------------------------------------------------------------------------
//
TInt CThemesTest::InstallL( CStifItemParser& aItem )
    {
    
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("In Install") );
    // Print to log file
    iLog->Log(_L("In Install"));

    TInt i = 0;
    TPtrC string;
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, _L("ThemesTest"), 
                                _L("Param[%i]: %S"), i, &string );
        i++;
        }
        


// Command  Leaf DataType Data
// G = Get
// E = Exec
// R = Replace
//          /Customization/Themes
//               T = Text
//               F = File
//                        "Text with spaces included"
//                        FileName.Ext

	// Here it starts

	const TUint KThemesAdapterImplUid = 0x0FDDEF05;

	TUid iuid = 
		{
		KThemesAdapterImplUid
		};
		
/*	Cpmatest *cb = Cpmatest::NewL( iLog );
	CleanupStack::PushL( cb );	// 1
*/	
	//CSmlDmAdapter *adapter = CSmlDmAdapter::NewL( implInfo->ImplementationUid(), *this );
//	CSmlDmAdapter *adapter = CSmlDmAdapter::NewL( iuid, *this );
	CTestDmCallback *callback = CTestDmCallback::NewL( iLog );
	CleanupStack::PushL( callback );

	CSmlDmAdapter *adapter = CSmlDmAdapter::NewL( iuid, *callback );
	CleanupStack::PushL( adapter );
	
	CBufFlat* data = CBufFlat::NewL( 16 );
	CleanupStack::PushL( data );

	adapter->DDFVersionL( *data );
	iLog->Log( _L("Version is")) ;
	iLog->Log( data->Ptr(0) ) ;


	CTestDmDDFObject* ddfRoot = CTestDmDDFObject::NewLC( iLog ); //, aNodeName );
	ddfRoot->SetNameL( _L8( "/" ) );

	adapter->DDFStructureL( *ddfRoot );
	ddfRoot->DumpL( _L8(""), EFalse );
	
	CleanupStack::PopAndDestroy( ddfRoot );
	ddfRoot = 0;
	
// Read file to data buffer and execute DM Adapter command

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

	_LIT8( KThemeFilename, "c:\\jiihaa.sis" );

//sf-	CBufBase* object = CBufFlat::NewL( 1000 );
//sf-	CleanupStack::PushL( object );

	TFileCoderB64 coder;
	User::LeaveIfError( coder.EncodeL( KThemeFilename, *data ) );

	/**
	The function implements execute command. The information about the success
	of the command should be returned by calling SetStatusL function of
	MSmlDmCallback callback interface. This makes it possible to buffer the
	commands.
	However, all the status codes for buffered commands must be returned at
	the latest when the CompleteOutstandingCmdsL() of adapter is called.
	@param aURI			URI of the command
	@param aLUID			LUID of the object (if the adapter have earlier
							returned LUID to the DM Module).   
	@param aArgument		Argument for the command
	@param aType			MIME type of the object 
	@param aStatusRef		Reference to correct command, i.e. this reference
							must be used when calling the SetStatusL of this
							command.
	@publishedPartner
	@prototype
	*/
//	virtual void ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID,
//							      const TDesC8& aArgument, const TDesC8& aType,
//								  TInt aStatusRef ) = 0;

	adapter->ExecuteCommandL( _L8("/Customization/Themes/InstallTheme"),
							  _L8(""), //callback->FindLUID(aURI)
							  data->Ptr(0),
							  _L8("text"),
							  callback->GetNewStatusRefL() );

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx



	CleanupStack::PopAndDestroy( data );
	data = 0;

	CleanupStack::PopAndDestroy( adapter );
	adapter = 0;
	
	CleanupStack::PopAndDestroy( callback );
	callback = 0;

	


	iLog->Log( _L("Install test complete with status %d" ), 0 ); //sf-ret );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CThemesTest::LoadL
// Load adapter to be tested.
// -----------------------------------------------------------------------------
//
TInt CThemesTest::LoadL( CStifItemParser& aItem )
	{
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("LoadL") );
    // Print to log file
    iLog->Log(_L("CThemesTest::LoadL()"));

	TInt ret = KErrNone;
    TUint uid = 0;

	ret = aItem.GetNextInt( uid, EHex );

//sf-	if (ret != KErrNone)
//sf-		{
//sf-//		ret = KErrGeneral ;
//sf-		return ret;
//sf-		}

	// Here it starts
	if (ret == KErrNone)
		{
		TUid iuid = 
			{
			uid
			};

        iLog->Log(_L("CThemesTest::LoadL() next: iCallback = CTestDmCallback::NewL( iLog );"));
		iCallback = CTestDmCallback::NewL( iLog );
		
        iLog->Log(_L("CThemesTest::LoadL() next: iAdapter = CSmlDmAdapter::NewL( iuid, *iCallback );"));
		iAdapter = CSmlDmAdapter::NewL( iuid, *iCallback );

        iLog->Log(_L("CThemesTest::LoadL() Adapter loaded OK."));
		}
	else
	    {
        iLog->Log(_L("CThemesTest::LoadL() Invalid UID in test script, check LoadL parameter."));
	    }

	return ret;
	}

// -----------------------------------------------------------------------------
// CThemesTest::AddDDFCheckL
// Load adapter to be tested.
// -----------------------------------------------------------------------------
//
TInt CThemesTest::AddDDFCheckL( CStifItemParser& aItem )
	{
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("AddDDFCheckL") );
    // Print to log file
    iLog->Log(_L("CThemesTest::AddDDFCheckL()"));

	TInt ret = KErrNone;

	CDDFTestParams *ddfParams = CDDFTestParams::NewL( iLog, aItem );
	AddDDFParamsL( ddfParams );

	return ret;
	}

// -----------------------------------------------------------------------------
// CThemesTest::DDFVersionL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::DDFVersionL( CStifItemParser& aItem )
	{
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("DDFVersionL") );
    // Print to log file
    iLog->Log(_L("CThemesTest::DDFVersionL()"));

//	void DDFVersionL( CBufBase& aVersion );
	TInt ret = KErrNone;

	CTestParams *params = CTestParams::NewLC( iLog, aItem );

/*	
URI=

File=
Text=

IT= Input (text)
IF= Input File (filename)
OF= Output File

ET= Expected result (text)
EF= Expected file (filename)
*/

	CBufFlat *version = CBufFlat::NewL( 16 );
	CleanupStack::PushL( version );
	iAdapter->DDFVersionL( *version );

	ret = params->CheckDataL( *version );
	
	CleanupStack::PopAndDestroy( version );
	CleanupStack::PopAndDestroy( params );

	return ret;
	}

// -----------------------------------------------------------------------------
// CThemesTest::DDFStructureL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::DDFStructureL( CStifItemParser& /*aItem*/ )
	{
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("DDFStructureL") );
    // Print to log file
    iLog->Log(_L("CThemesTest::DDFStructureL()"));

	TInt ret = KErrNone;

//	void DDFStructureL( MSmlDmDDFObject& aDDF );
	CTestDmDDFObject* ddfRoot = CTestDmDDFObject::NewLC( iLog ); //, aNodeName );
	ddfRoot->SetNameL( _L8( "/" ) );

	iAdapter->DDFStructureL( *ddfRoot );
	ddfRoot->DumpL( _L8(""), EFalse );

	ret = CheckDDFL( ddfRoot );

	CleanupStack::PopAndDestroy( ddfRoot );
	ddfRoot = 0;

	return ret;
	}

	
// -----------------------------------------------------------------------------
// CThemesTest::UpdateLeafObjectL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::UpdateLeafObjectL( CStifItemParser& aItem )
	{
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("UpdateLeafObjectL") );
    // Print to log file
    iLog->Log(_L("CThemesTest::UpdateLeafObjectL()"));

	TInt ret = KErrNone;
	CTestParams* params = CTestParams::NewL( iLog, aItem );
	AddParamsL( params );

	TPtrC8 uri( params->Uri() );
	TPtrC8 object( params->Data() );
	TPtrC8 type( params->DataType() );
	HBufC8* luid = iCallback->GetLuidAllocL( uri );
	CleanupStack::PushL( luid );
	TInt statusRef = iCallback->GetNewStatusRefL( );
//	TInt resultsRef = iCallback->GetNewResultsRefL( );

	params->SetStatusRef( statusRef);
//	params->SetResultsRef( resultsRef);

	iAdapter->UpdateLeafObjectL( uri, *luid, object, type, statusRef );

	ret = params->CheckL( iCallback );

	// Cleanup
	CleanupStack::PopAndDestroy( luid );
	
	return ret;
	}

// -----------------------------------------------------------------------------
// CThemesTest::UpdateLeafObjectStreamL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::UpdateLeafObjectStreamL( CStifItemParser& /*aItem*/ )
	{
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("UpdateLeafObjectStreamL") );
    // Print to log file
    iLog->Log(_L("CThemesTest::UpdateLeafObjectStreamL()"));

//	void UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, RWriteStream*& aStream, const TDesC8& aType, TInt aStatusRef );

	TInt ret = KErrNotSupported;
	return ret;
	}

// -----------------------------------------------------------------------------
// CThemesTest::DeleteObjectL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::DeleteObjectL( CStifItemParser& aItem )
	{
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("DeleteObjectL") );
    // Print to log file
    iLog->Log(_L("CThemesTest::DeleteObjectL()"));

	TInt ret = KErrNone;
	CTestParams* params = CTestParams::NewL( iLog, aItem );
	AddParamsL( params );

	TPtrC8 uri( params->Uri() );
	HBufC8* luid = iCallback->GetLuidAllocL( uri );
	CleanupStack::PushL( luid );
	TInt statusRef = iCallback->GetNewStatusRefL( );

//	void DeleteObjectL( const TDesC8& aURI, const TDesC8& aLUID, TInt aStatusRef );
	iAdapter->DeleteObjectL( uri, *luid, statusRef );

	params->SetStatusRef( statusRef);
	ret = params->CheckL( iCallback );

	// Cleanup
	CleanupStack::PopAndDestroy( luid );
	return ret;
	}

// -----------------------------------------------------------------------------
// CThemesTest::FetchLeafObjectL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::FetchLeafObjectL( CStifItemParser& aItem )
	{
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("FetchLeafObjectL") );
    // Print to log file
    iLog->Log(_L("CThemesTest::FetchLeafObjectL()"));

	TInt ret = KErrNone;
	CTestParams* params = CTestParams::NewL( iLog, aItem );
	AddParamsL( params );

	TPtrC8 uri( params->Uri() );
	TPtrC8 type( params->DataType() );
	HBufC8* luid = iCallback->GetLuidAllocL( uri );
	CleanupStack::PushL( luid );
	TInt statusRef = iCallback->GetNewStatusRefL( );
	TInt resultsRef = iCallback->GetNewResultsRefL( );

	params->SetStatusRef( statusRef);
	params->SetResultsRef( resultsRef);

//	void FetchLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aType, TInt aResultsRef, TInt aStatusRef );
	iAdapter->FetchLeafObjectL( uri, *luid, type, resultsRef, statusRef );

	ret = params->CheckL( iCallback );

	// Cleanup
	CleanupStack::PopAndDestroy( luid );
	return ret;
	}

// -----------------------------------------------------------------------------
// CThemesTest::FetchLeafObjectSizeL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::FetchLeafObjectSizeL( CStifItemParser& aItem )
	{
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("FetchLeafObjectSizeL") );
    // Print to log file
    iLog->Log(_L("CThemesTest::FetchLeafObjectSizeL()"));

	TInt ret = KErrNone;
	CTestParams* params = CTestParams::NewL( iLog, aItem );
	AddParamsL( params );

	TPtrC8 uri( params->Uri() );
//	TPtrC8 object( params->Data() );
	TPtrC8 type( params->DataType() );
	HBufC8* luid = iCallback->GetLuidAllocL( uri );
	CleanupStack::PushL( luid );
	TInt statusRef = iCallback->GetNewStatusRefL( );
	TInt resultsRef = iCallback->GetNewResultsRefL( );

	params->SetStatusRef( statusRef);
	params->SetResultsRef( resultsRef);

//	void FetchLeafObjectSizeL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aType, TInt aResultsRef, TInt aStatusRef );
	iAdapter->FetchLeafObjectSizeL( uri, *luid, type, resultsRef, statusRef );

	ret = params->CheckL( iCallback );

	// Cleanup
	CleanupStack::PopAndDestroy( luid );
	return ret;
	}

// -----------------------------------------------------------------------------
// CThemesTest::ChildURIListL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::ChildURIListL( CStifItemParser& aItem )
	{
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("ChildURIListL") );
    // Print to log file
    iLog->Log(_L("CThemesTest::ChildURIListL()"));

	TInt ret = KErrNone;
	CTestParams* params = CTestParams::NewL( iLog, aItem );
	AddParamsL( params );

	TPtrC8 uri( params->Uri() );
//sf-	TPtrC8 object( params->Data() );
	TPtrC8 type( params->DataType() );
	HBufC8* luid = iCallback->GetLuidAllocL( uri );
	CleanupStack::PushL( luid );
	TInt statusRef = iCallback->GetNewStatusRefL( );
	TInt resultsRef = iCallback->GetNewResultsRefL( );

	params->SetStatusRef( statusRef);
	params->SetResultsRef( resultsRef);

//	CreatePreviousURISegmentList( );
	CArrayFixFlat<TSmlDmMappingInfo>* uriSegmentList = new (ELeave) CArrayFixFlat<TSmlDmMappingInfo>( 10 );
	CleanupStack::PushL( uriSegmentList );

	TPtrC8 oldUriSegs = params->Data();
	TInt i;
	TInt count = 0;
	if (oldUriSegs.Length() > 0)
		{
		count = NSmlDmURI::NumOfURISegs( oldUriSegs );;
		}

	for (i = 0 ; i < count ; i++)
		{
		TPtrC8 tmpSeg = NSmlDmURI::URISeg( oldUriSegs, i + 1 );
//		TBuf8< uri.Length() + 1 + KSmlMaxURISegLen > tmpUri;
		TBuf8< 256 > tmpUri;
		tmpUri = uri;
		tmpUri.Append( _L8("/") );
		tmpUri.Append( tmpSeg );
		TPtrC8 tmpLuid = iCallback->GetLuidL( tmpUri );

		TSmlDmMappingInfo t; // = { seg, tmpLuid };
		t.iURISeg = tmpSeg;
		t.iURISegLUID.Set( tmpLuid );

		uriSegmentList->AppendL( t );
		}

//	void ChildURIListL( const TDesC8& aURI, const TDesC8& aLUID, const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, TInt aResultsRef, TInt aStatusRef );
	iAdapter->ChildURIListL( uri, *luid, *uriSegmentList, resultsRef, statusRef );

	ret = params->CheckL( iCallback );

	// Cleanup
	CleanupStack::PopAndDestroy( uriSegmentList );
	CleanupStack::PopAndDestroy( luid );
	return ret;
	}

	
// -----------------------------------------------------------------------------
// CThemesTest::AddNodeObjectL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::AddNodeObjectL( CStifItemParser& aItem )
	{
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("AddNodeObjectL") );
    // Print to log file
    iLog->Log(_L("CThemesTest::AddNodeObjectL()"));

	TInt ret = KErrNone;
	CTestParams* params = CTestParams::NewL( iLog, aItem );
	AddParamsL( params );

	TPtrC8 uri( params->Uri() );
	TPtrC8 parentUri = NSmlDmURI::ParentURI( uri );
	TPtrC8 parentLuid = iCallback->GetLuidL( parentUri );

	TInt statusRef = iCallback->GetNewStatusRefL( );

	params->SetStatusRef( statusRef);

//	void AddNodeObjectL( const TDesC8& aURI, const TDesC8& aParentLUID, TInt aStatusRef );
	iAdapter->AddNodeObjectL( uri, parentLuid, statusRef );

	ret = params->CheckL( iCallback );

	// Cleanup

	return ret;
	}

// -----------------------------------------------------------------------------
// CThemesTest::ExecuteCommandL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::ExecuteCommandL( CStifItemParser& aItem )
	{
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("ExecuteCommandL") );
    // Print to log file
    iLog->Log(_L("CThemesTest::ExecuteCommandL()"));

	TInt ret = KErrNone;
	CTestParams* params = CTestParams::NewL( iLog, aItem );
	AddParamsL( params );

	TPtrC8 uri( params->Uri() );
	TPtrC8 argument( params->Data() );
	TPtrC8 type( params->DataType() );
	HBufC8* luid = iCallback->GetLuidAllocL( uri );
	CleanupStack::PushL( luid );
	TInt statusRef = iCallback->GetNewStatusRefL( );

	params->SetStatusRef( statusRef);

//	void ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aArgument, const TDesC8& aType, TInt aStatusRef );
	iAdapter->ExecuteCommandL( uri, *luid, argument, type, statusRef );

	ret = params->CheckL( iCallback );

	// Cleanup
	CleanupStack::PopAndDestroy( luid );
	return ret;
	}

// -----------------------------------------------------------------------------
// CThemesTest::ExecuteCommandStreamL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::ExecuteCommandStreamL( CStifItemParser& /*aItem*/ )
	{
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("ExecuteCommandStreamL") );
    // Print to log file
    iLog->Log(_L("CThemesTest::ExecuteCommandStreamL()"));

//	void ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, RWriteStream*& aStream, const TDesC8& aType, TInt aStatusRef );

	TInt ret = KErrNotSupported;
	return ret;
	}

// -----------------------------------------------------------------------------
// CThemesTest::CopyCommandL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::CopyCommandL( CStifItemParser& aItem )
	{
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("CopyCommandL") );
    // Print to log file
    iLog->Log(_L("CThemesTest::CopyCommandL()"));

	TInt ret = KErrNone;
	CTestParams* params = CTestParams::NewL( iLog, aItem );
	AddParamsL( params );

	TPtrC8 uri( params->Uri() );
	TPtrC8 target( params->Data() );
	TPtrC8 type( params->DataType() );
	HBufC8* luid = iCallback->GetLuidAllocL( uri );
	CleanupStack::PushL( luid );
	TInt statusRef = iCallback->GetNewStatusRefL( );

	params->SetStatusRef( statusRef );

	TInt pos = target.Locate( ':' );
	if (pos < 0)
		{
		pos = target.Length();
		}
	TPtrC8 targetUri = target.Left( pos );
	
	if (pos < target.Length())
		{
		pos++;
		}
	TPtrC8 targetLuid = target.Right( target.Length() - pos );

//	void CopyCommandL( const TDesC8& aTargetURI, const TDesC8& aTargetLUID, const TDesC8& aSourceURI, const TDesC8& aSourceLUID, const TDesC8& aType, TInt aStatusRef );
	iAdapter->CopyCommandL( targetUri, targetLuid, uri, *luid, type, statusRef );

	ret = params->CheckL( iCallback );

	// Cleanup
	CleanupStack::PopAndDestroy( luid );
	return ret;
	}

// -----------------------------------------------------------------------------
// CThemesTest::StartAtomicL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::StartAtomicL( CStifItemParser& /*aItem*/ )
	{
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("StartAtomicL") );
    // Print to log file
    iLog->Log(_L("CThemesTest::StartAtomicL()"));

	TInt ret = KErrNone;
//sf-	CTestParams* params = CTestParams::NewL( iLog, aItem );
//sf-	AddParamsL( params );

//	void StartAtomicL();
	iAdapter->StartAtomicL();

	// Cleanup
	return ret;
	}

// -----------------------------------------------------------------------------
// CThemesTest::CommitAtomicL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::CommitAtomicL( CStifItemParser& /*aItem*/ )
	{
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("CommitAtomicL") );
    // Print to log file
    iLog->Log(_L("CThemesTest::CommitAtomicL()"));

	TInt ret = KErrNone;
//sf-	CTestParams* params = CTestParams::NewL( iLog, aItem );
//sf-	AddParamsL( params );

//	void CommitAtomicL();
	iAdapter->CommitAtomicL();

	// Cleanup
	return ret;
	}

// -----------------------------------------------------------------------------
// CThemesTest::RollbackAtomicL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::RollbackAtomicL( CStifItemParser& /*aItem*/ )
	{
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("RollbackAtomicL") );
    // Print to log file
    iLog->Log(_L("CThemesTest::RollbackAtomicL()"));

	TInt ret = KErrNone;
//sf-	CTestParams* params = CTestParams::NewL( iLog, aItem );
//sf-	AddParamsL( params );

//	void RollbackAtomicL();
	iAdapter->RollbackAtomicL();

	// Cleanup
	return ret;
	}

// -----------------------------------------------------------------------------
// CThemesTest::StreamingSupportL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::StreamingSupportL( CStifItemParser& aItem )
	{
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("StreamingSupportL") );
    // Print to log file
    iLog->Log(_L("CThemesTest::StreamingSupportL()"));

	TInt ret = KErrNone;
	CTestParams* params = CTestParams::NewL( iLog, aItem );
	AddParamsL( params );

	TInt itemSize = 0;
	TBool status = iAdapter->StreamingSupport( itemSize );

	TBuf8<10> statusStr;
	TBuf8<20> itemSizeStr;
	
	statusStr.Format( _L8("%d"), status );
	itemSizeStr.Format( _L8("%d"), itemSize );
	ret = params->CheckStatusL( status );
	ret = params->CheckDataL( itemSizeStr );

	// Cleanup
	return ret;
	}

// -----------------------------------------------------------------------------
// CThemesTest::StreamCommittedL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::StreamCommittedL( CStifItemParser& /*aItem*/ )
	{
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("StreamCommittedL") );
    // Print to log file
    iLog->Log(_L("CThemesTest::StreamCommittedL()"));

	TInt ret = KErrNone;
//sf-	CTestParams* params = CTestParams::NewL( iLog, aItem );
//sf-	AddParamsL( params );

//	void StreamCommittedL();
	iAdapter->StreamCommittedL();

	// Cleanup
	return ret;
	}

// -----------------------------------------------------------------------------
// CThemesTest::CompleteOutstandingCmdsL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::CompleteOutstandingCmdsL( CStifItemParser& /*aItem*/ )
	{
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("CompleteOutstandingCmdsL") );
    // Print to log file
    iLog->Log(_L("CThemesTest::CompleteOutstandingCmdsL()"));

	TInt ret = KErrNone;
//sf-	CTestParams* params = CTestParams::NewL( iLog, aItem );
//sf-	AddParamsL( params );

//	void CompleteOutstandingCmdsL();
	iAdapter->CompleteOutstandingCmdsL();

	ret = CheckAllL( );

	// Cleanup
	return ret;
	}
#if 0
// -----------------------------------------------------------------------------
// CThemesTest::CreateFileL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::CreateFileL( CStifItemParser& aItem )
    {
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("CreateFileL") );
    // Print to log file
    iLog->Log(_L("CThemesTest::CreateFileL()"));

	TInt ret = KErrNone;
	 
	CTestParams* params = CTestParams::NewL( iLog, aItem );
	AddParamsL( params );
	
	TPtrC8 filename( params->Data() );
	
	RFs fs; fs.Connect();
	
	TFileName fname; fname.Copy( filename );
	RFile f; f.Replace(fs, fname, EFileShareExclusive|EFileWrite);
	TPtrC8 dataType( params->DataType() );
	if(dataType.Length() == 0)
	    {
	    f.Write(_L8("File created by ThemesTest"));
	    }
	else
	    {
	    f.Write( dataType );
	    }
	f.Close();
	
	fs.Close();

    ret = params->CheckL( iCallback );
	//ret = CheckAllL( );

    //CleanupStack::PopAndDestroy( params );

	// Cleanup
	return ret;
    }
    
// -----------------------------------------------------------------------------
// CThemesTest::RFSCommandL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::RFSCommandL( CStifItemParser& aItem )
    {
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("RFSCommandL") );

    // Print to log file
    iLog->Log(_L("CThemesTest::RFSCommandL()"));

	TInt status = KErrNone;
	 
	CTestParams* params = CTestParams::NewL( iLog, aItem );
	AddParamsL( params );
	
	TPtrC8 command( params->Data() );
	
    RRfsClient client;
    
    status = client.Connect();
    
    if( KErrNone == status )
        {        
        CleanupClosePushL( client );

        HBufC* cmd = HBufC::NewLC( command.Length() );
        if(cmd != 0)
            {            
            TPtr ptr( cmd->Des() );
            ptr.Copy( command );
            status = client.RunScriptInDescriptor(ptr);
            if(status >= 0) status = 0;
            }
        CleanupStack::PopAndDestroy( cmd );

        CleanupStack::PopAndDestroy( &client );
        }

    params->CheckL( iCallback );

	return status;    
    }

// -----------------------------------------------------------------------------
// CThemesTest::LoadPolicyL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::LoadPolicyL( CStifItemParser& aItem )
    {
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("LoadPolicyL") );
    // Print to log file
    iLog->Log(_L("CThemesTest::LoadPolicyL()"));
	TInt status = KErrNone;
	 
	CTestParams* params = CTestParams::NewL( iLog, aItem );
	AddParamsL( params );

	TPtrC8 data( params->Data() );

	RPolicyEngine eng;
	status = eng.Connect();
	
	if( KErrNone == status )
	    {
        iLog->Log(_L("CThemesTest::LoadPolicyL() Connected to policy engine"));
	    RPolicyManagement man;
	    status = man.Open( eng );
	    if( KErrNone == status )
	        {
            iLog->Log(_L("CThemesTest::LoadPolicyL() Policy management session open"));
	        TParserResponse resp;
	        status = man.ExecuteOperation(data, resp);
            if( KErrNone == status )
                {
                iLog->Log(_L("CThemesTest::LoadPolicyL() Policy added"));                
                }
            man.Close();
	        }
	    eng.Close();
	    }

    params->CheckL( iCallback );

	return status;
    }

// -----------------------------------------------------------------------------
// CThemesTest::SetCertificateL
// 
// -----------------------------------------------------------------------------
//
/*
Test-Case for this API
[Test]
title SS_GetAll
create ThemesTest ttest
ttest LoadL 1020782E

ttest SetCertificate <CentralRepositoryUid>
[EndTest]
*/

TInt CThemesTest::SetCertificateL( CStifItemParser& aItem )
    {
    TInt errx = KErrNone;

    RDMUtil dm;
    if( KErrNone == dm.Connect() )
        {
        dm.Close();
        }

    TCertInfo info;
    info.iSerialNo    = _L8("L‰ssynl‰‰");
    info.iFingerprint = _L8("Joopajoo");;

	// Changed due to SF.
	TInt ret = KErrNone;
    TUint CentralRepositoryUid = 0;

	ret = aItem.GetNextInt( CentralRepositoryUid, EHex );
	
	const TInt KCertKey = 0x01;
	CRepository *re = NULL;
	TRAP( errx, re = CRepository::NewL ( CentralRepositoryUid ) );
	if (errx == KErrNone )
		{
		TPckg<TCertInfo> pcert( info );
		errx = re->Create( KCertKey, pcert ) ;
		if ( errx == KErrNone )
			{
			}
		else
			{
			if ( errx == KErrAlreadyExists )
				{
				errx = re->Set( KCertKey, pcert ) ;
				if ( errx != KErrNone )
					{
					// DBG_ARGS8(_S8("ERROR Failed to add reposiritry key %d"), errx );
					}
				}
			else
				{
				// DBG_ARGS8(_S8("ERROR Failed to create reposiritry key %d"), errx );	
				}
			
			}	
		delete re ;
		}
	else
		{
		// DBG_ARGS8(_S8("ERROR Failed to open reposiritry %d"), erx );	
		}

    return errx;
    }

// -----------------------------------------------------------------------------
// CThemesTest::ConnectToPolicyServerL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::ConnectToPolicyServerL( CStifItemParser& /*aItem*/ )
    {
    TInt status = KErrNone;

	RPolicyEngine eng;
	status = eng.Connect();
	
	if( KErrNone == status )
	    {
        iLog->Log(_L("CThemesTest::LoadPolicyL() Connected to policy engine"));
	    RPolicyManagement man;
	    status = man.Open( eng );
	    if( KErrNone == status )
	        {
            iLog->Log(_L("CThemesTest::LoadPolicyL() Policy management session open"));
            man.Close();
	        }
	    eng.Close();
	    }
    
    return status;
    }

// -----------------------------------------------------------------------------
// CThemesTest::ConnectToDMUTILL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::ConnectToDMUTILL( CStifItemParser& /*aItem*/ )
    {
    TInt status = KErrNone;

    RDMUtil dm;
    status = dm.Connect();
    if( KErrNone == status )
        {
        dm.Close();
        }

    return status;
    }

// -----------------------------------------------------------------------------
// CThemesTest::GetThirdPartyValueL
// 
// -----------------------------------------------------------------------------
//
HBufC8* CThemesTest::GetThirdPartyValueL( TInt aType )
    {
    RTerminalControl tc;
    User::LeaveIfError( tc.Connect() );

    RTerminalControl3rdPartySession ts;
    User::LeaveIfError( ts.Open( tc ) );

    TInt size = ts.GetDeviceLockParameterSize( aType );
    if( size < 0 )
        {
        User::Leave( size );
        }

    HBufC8* buffer = HBufC8::NewLC( size );
    TPtr8 ptr8( buffer->Des() );

    User::LeaveIfError( ts.GetDeviceLockParameter( aType, ptr8 ) );

    ts.Close();
    tc.Close();

    CleanupStack::Pop( buffer );
    return buffer;
    }

// -----------------------------------------------------------------------------
// CThemesTest::SetThirdPartyValueL
// 
// -----------------------------------------------------------------------------
//
void CThemesTest::SetThirdPartyValueL( TInt aType, TDesC8& aData )
    {
    RTerminalControl tc;
    User::LeaveIfError( tc.Connect() );

    RTerminalControl3rdPartySession ts;
    User::LeaveIfError( ts.Open( tc ) );

    User::LeaveIfError( ts.SetDeviceLockParameter( aType, aData ) );

    ts.Close();
    tc.Close();
    }

// -----------------------------------------------------------------------------
// CThemesTest::ThirdPartyCommandL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::ThirdPartyCommandL( CStifItemParser& aItem )
    {
    CTestDmCallback *cb = CTestDmCallback::NewL( iLog );
    CleanupStack::PushL( cb );
    
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("ThirdP_AutolockPeriodL") );
    // Print to log file
    iLog->Log(_L("CThemesTest::ThirdP_AutolockPeriodL()"));
	TInt status = KErrNone;

	CTestParams* params = CTestParams::NewL( iLog, aItem );
	AddParamsL( params );

    TPtrC8 command( params->Uri() );
    TPtrC8 direction( params->DataType() );
    TPtrC8 value( params->Data() );

/*
    	{
        	ETimeout = 1000,		// Autolock timeout. Values in minutes. 
        	EMaxTimeout,	// Maximum autolock timeout value. When != 0 the lock is enabled, When = 0 the lock is off
        	EPasscode,		// Passcode. Return value not supported
        	EPasscodePolicy, // 0 = passcode free. When != 0 the passcode can not be changed
        	ELock,			// Lock the terminal immediately. Return value not supported
        	EDeviceWipe, 	// When != 0 the Device is wiped
        	ELast = 2000
    	};
*/

    if( direction.Compare( _L8("read") ) == 0)
        {
        HBufC8* data = 0;
        if( command.Compare( _L8("ETimeout") ) == 0 )
            {
            data = GetThirdPartyValueL( RTerminalControl3rdPartySession::ETimeout );
            }
        else if( command.Compare( _L8("EMaxTimeout") ) == 0 )
            {
            data = GetThirdPartyValueL( RTerminalControl3rdPartySession::EMaxTimeout );
            }
        else if( command.Compare( _L8("EPasscode") ) == 0 )
            {
            data = GetThirdPartyValueL( RTerminalControl3rdPartySession::EPasscode );
            }
        else if( command.Compare( _L8("EPasscodePolicy") ) == 0 )
            {
            data = GetThirdPartyValueL( RTerminalControl3rdPartySession::EPasscodePolicy );
            }
        else if( command.Compare( _L8("ELock") ) == 0 )
            {
            data = GetThirdPartyValueL( RTerminalControl3rdPartySession::ELock );
            }
        else if( command.Compare( _L8("EDeviceWipe") ) == 0 )
            {
            data = GetThirdPartyValueL( RTerminalControl3rdPartySession::EDeviceWipe );
            }

        if( 0 == data )
            {
            User::Leave( KErrGeneral );
            }
        CleanupStack::PushL( data );

    	TInt statusRef = cb->GetNewStatusRefL( );
        TInt resultsRef = cb->GetNewResultsRefL( );

    	params->SetStatusRef( statusRef);
        params->SetResultsRef( resultsRef);

        CBufFlat* buf = CBufFlat::NewL(128);
        CleanupStack::PushL( buf );

        buf->InsertL( 0, *data );
        cb->SetResultsL( resultsRef, *buf, KNullDesC8() );
        cb->SetStatusL( statusRef, (MSmlDmAdapter::TError)0 );

        CleanupStack::PopAndDestroy( buf );
        CleanupStack::PopAndDestroy( data );
        }
    else if( direction.Compare( _L8("write") ) == 0)
        {
        if( command.Compare( _L8("ETimeout") ) == 0 )
            {
            SetThirdPartyValueL( RTerminalControl3rdPartySession::ETimeout, value );
            }
        else if( command.Compare( _L8("EMaxTimeout") ) == 0 )
            {
            SetThirdPartyValueL( RTerminalControl3rdPartySession::EMaxTimeout, value );
            }
        else if( command.Compare( _L8("EPasscode") ) == 0 )
            {
            SetThirdPartyValueL( RTerminalControl3rdPartySession::EPasscode, value );
            }
        else if( command.Compare( _L8("EPasscodePolicy") ) == 0 )
            {
            SetThirdPartyValueL( RTerminalControl3rdPartySession::EPasscodePolicy, value );
            }
        else if( command.Compare( _L8("ELock") ) == 0 )
            {
            SetThirdPartyValueL( RTerminalControl3rdPartySession::ELock, value );
            }
        else if( command.Compare( _L8("EDeviceWipe") ) == 0 )
            {
            SetThirdPartyValueL( RTerminalControl3rdPartySession::EDeviceWipe, value );
            }
        }
    else
        {
        User::Leave( KErrArgument );
        }

    params->CheckL( cb );

    CleanupStack::PopAndDestroy( cb );

	return status;
    }

// -----------------------------------------------------------------------------
// CThemesTest::InitDMSessionL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::InitDMSessionL( CStifItemParser& /*aItem*/ )
    {
    TInt status = KErrNone;

	RNSmlPrivateAPI privateApi;
				
	privateApi.OpenL();

	TBuf8<100> buf;
	
	buf.Append( 0x3F);
	buf.Append( 0x4E);
	buf.Append( 0x87);
	buf.Append( 0xBE);
	buf.Append( 0x50);
	buf.Append( 0xCC);
	buf.Append( 0x28);
	buf.Append( 0x5E);
	buf.Append( 0x96);
	buf.Append( 0x87);
	buf.Append( 0x3D);
	buf.Append( 0x29);
	buf.Append( 0x39);
	buf.Append( 0xEB);
	buf.Append( 0x78);
	buf.Append( 0x0B);
	buf.Append( 0x02);
	buf.Append( 0xF8);
	buf.Append( 0x00);
	buf.Append( 0x00);
	buf.Append( 0x00);
	buf.Append( 0x00);
	buf.Append( 0x17);
	buf.Append( 0x06);
	buf.Append( 0x53);
	buf.Append( 0x79);
	buf.Append( 0x6E);
	buf.Append( 0x63);
	buf.Append( 0x34);
	buf.Append( 0x6A);

	privateApi.SendL( buf, ESmlDevMan, ESmlVersion1_1_2 );
		
	privateApi.Close();

    return status;
    }


// -----------------------------------------------------------------------------
// CThemesTest::RRRunL
// 
// -----------------------------------------------------------------------------
//
TInt CThemesTest::RRRunL( CStifItemParser& aItem )
	{
    // Print to UI
    TestModuleIf().Printf( 0, _L("ThemesTest"), _L("RRRunL") );
    // Print to log file
    iLog->Log(_L("CThemesTest::RRRunL()"));

//	void DDFVersionL( CBufBase& aVersion );
	TInt ret = KErrNone;

	CTestParams *params = CTestParams::NewLC( iLog, aItem );

/*	
URI=

File=
Text=

IT= Input (text)
IF= Input File (filename)
OF= Output File

ET= Expected result (text)
EF= Expected file (filename)
*/

	CleanupStack::PopAndDestroy( params );

	return ret;
	}



// -----------------------------------------------------------------------------
// CThemesTest::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CThemesTest::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

#endif
void CThemesTest::AddParamsL( CTestParams* aParams )
	{
	CleanupStack::PushL( aParams );
	User::LeaveIfError( iParams.Append( aParams ) );
	CleanupStack::Pop( aParams );
	}

void CThemesTest::AddDDFParamsL( CDDFTestParams* aDDFParams )
	{
	CleanupStack::PushL( aDDFParams );
	User::LeaveIfError( iDDFParams.Append( aDDFParams ) );
	CleanupStack::Pop( aDDFParams );
	}

TInt CThemesTest::CheckAllL( )
	{
	TInt ret = KErrNone;
	
	for (TInt i = 0 ; i < iParams.Count() ; i++)
		{
		TInt result = iParams[i]->CheckL( iCallback );
		if (result != KErrNone && ret == KErrNone)
			{
			ret = result;
			}
		}

	return ret;
	}

TInt CThemesTest::CheckDDFL( CTestDmDDFObject* aDDFRoot )
	{
	TInt ret = KErrNone;
	TInt result = 0;

	if ( iDDFParams.Count() > 0 )
		{
		}

	TInt i;
	for ( i = 0 ; i < iDDFParams.Count() ; i++ )
		{
		result = iDDFParams[i]->CheckL( aDDFRoot );
		if ( result != KErrNone && ret == KErrNone )
			{
			ret = result;
			}
		}

	if ( i > 0 )
		{
		TInt count = aDDFRoot->SubObjectsCount();
		if (count != i)
			{
			iLog->Log( _L8("Error: CheckDDFL(): DDF count mismatch: expected=%d real=%d"), iDDFParams.Count(), count );
			if (ret == KErrNone)
				{
				ret = KErrGeneral;
				}
			}
		}

	return ret;
	}

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
