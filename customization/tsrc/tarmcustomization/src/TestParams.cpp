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
* Description: Implementation of customization components
*
*/



// INCLUDE FILES

#include "TestParams.h"

//#include <e32svr.h>

#include <StifLogger.h>
#include <StifItemParser.h>
//#include <Stiftestinterface.h>

//#include "ThemesTest.h"
#include "TestDmCallback.h"
#include "TestDmDDFObject.h"
#include "FileCoderB64.h"

#include "nsmldmuri.h"


// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestParams::CTestParams
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTestParams::CTestParams( CStifLogger *aLog )
	: iLog( aLog )
//	, iStr( 0 )
	, iUri( 0 )
	, iData( 0 )
	, iDataType( 0 )
	, iOutputFile( 0 )
	, iExpectedStatus( -1 )
	, iExpectedData( 0 )
	, iExpectedType( 0 )
	, iStatusRef( -1 )
	, iResultsRef( -1 )
	{
	/**/
	}

// -----------------------------------------------------------------------------
// CTestParams::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTestParams::ConstructL( CStifItemParser& aItem )
	{
	// Parse parameters
	TPtrC str;
    TInt ret = aItem.Remainder( str );

	TBuf8<200> buf;
	buf.Copy( str )    ;

	iLog->Log( _L("CmdParams: '%S'"), &str );


//	TInt i = 0;	
//	while (aItem.GetNextString( str ) == KErrNone )	
//		{
//		iLog->Log( _L("Par[%d]: '%S'"), i, &str );
//		}

//	iLog->Log( _L("Version is")) ;
//	iLog->Log( version->Ptr(0) ) ;
//	
//			iLog->Log( _L("Add Test Adding Policy data failed! %d" ), iStatus );	

	TTestParamVar var( buf );
	while (var.Next())
		{
		TPtrC8 key = var.Name();
		TPtrC8 value = var.Value();
		TBool exist = var.ValueExist();
		iLog->Log(_L8("key: '%S', value: '%S', exist: %d"), &key, &value, exist);

/*
		Syntax:
		-------
		
		URI=

		File=
		Text=

		IT= Input (text)
		IF= Input File (filename)
		OF= Output File

		ET= Expected result (text)
		EF= Expected file (filename)

*/
		_LIT8( KURI,			"URI" ); // = value

		_LIT8( KText,			"Text" ); // = "Text input"
		_LIT8( KFile,			"File" ); // = Filename (and path)
		_LIT8( KType,			"Type" ); // = MimeType (of data)

		_LIT8( KInputText,		"IT" ); // = "Input Text"
		_LIT8( KInputFile,		"IF" ); // = InputFileName (and path)
		_LIT8( KInputFile2,		"IF2" ); // = InputFileName (and path)
		_LIT8( KInputType,		"IM" ); // = Input MimeType (of data)
		_LIT8( KOutputFile,		"OF" ); // = OutputFileName (and path) (where result data is saved)

		_LIT8( KExpectedStatus,	"ES" ); // = <Expected Status Code>
		_LIT8( KExpectedText,	"ET" ); // = "Expected Result Text"
		_LIT8( KExpectedFile,	"EF" ); // = ExpectedResultFile (and path)
		_LIT8( KExpectedFile2,	"EF2" ); // = ExpectedResultFile (and path)
		_LIT8( KExpectedType,	"EM" ); // = Expected MimeType (of return data)

//	, iUri( 0 )
//	, iData( 0 )
//	, iDataType( 0 )

		if ( key.CompareF( KURI ) == 0 )
			{
			if (iUri == 0)
				{
				delete iUri;
				iUri = 0;
				iUri = value.AllocL();
				}
			else
				{
				iLog->Log(_L8("Warning: URI already given, not resetting to: '%S'"), &value );
				}
			}
		else if ( key.CompareF( KInputText ) == 0 || key.CompareF( KText ) == 0 )
			{
			delete iData;
			iData = 0;
			iData = value.AllocL();
			}
		else if ( key.CompareF( KInputFile ) == 0 || key.CompareF( KFile ) == 0 )
			{
			delete iData;
			iData = 0;

			TFileCoderB64 coder;
			User::LeaveIfError( coder.EncodeL( value, iData ) );
			}
		else if ( key.CompareF( KInputFile2) == 0 )
			{
			delete iData;
			iData = 0;

            RFs fs; fs.Connect();
            TFileName fn; fn.Copy(value);
            RFile f; f.Open(fs, fn, EFileShareExclusive|EFileRead);

            TInt size; f.Size( size );
            iData = HBufC8::NewL( size );
            TPtr8 ptr( iData->Des() );
            f.Read( ptr );

			f.Close();
			fs.Close();
			}
		else if ( key.CompareF( KInputType ) == 0 || key.CompareF( KType ) == 0 )
			{
			delete iDataType;
			iDataType = 0;
			iDataType = value.AllocL();
			}
		else if (key.CompareF( KOutputFile ) == 0 )
			{
			delete iOutputFile;
			iOutputFile = 0;
			iOutputFile = value.AllocL();
			}
		else if (key.CompareF( KExpectedStatus ) == 0 )
			{
			TLex8 lex( value );
			User::LeaveIfError( lex.Val( iExpectedStatus ) );
			}
		else if (key.CompareF( KExpectedText ) == 0 )
			{
			delete iExpectedData;
			iExpectedData = 0;
			iExpectedData = value.AllocL();
			}
		else if (key.CompareF( KExpectedFile ) == 0 )
			{
			delete iExpectedData;
			iExpectedData = 0;

			TFileCoderB64 coder;
			User::LeaveIfError( coder.EncodeL( value, iExpectedData ) );
			}
		else if (key.CompareF( KExpectedFile2 ) == 0 )
			{
			delete iExpectedData;
			iExpectedData = 0;

            RFs fs; fs.Connect();
            TFileName fn; fn.Copy(value);
            RFile f; f.Open(fs, fn, EFileShareExclusive|EFileRead);
            
            TInt size; f.Size( size );
            iExpectedData = HBufC8::NewL( size );
            TPtr8 ptr( iExpectedData->Des() );
            f.Read( ptr );
            
			f.Close();
			fs.Close();
			}
		else if (key.CompareF( KExpectedType ) == 0 )
			{
			delete iExpectedType;
			iExpectedType = 0;
			iExpectedType = value.AllocL();
			}
		else
			{
			// Unknown parameter
			iLog->Log(_L8("Warning: Unknown parameter: key: '%S', value: '%S', exist: %d"), &key, &value, exist);
			}
			
		}
	// Cleanup
	}


// -----------------------------------------------------------------------------
// CTestParams::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTestParams* CTestParams::NewL( CStifLogger *aLog, CStifItemParser& aItem )
	{
    CTestParams* self = NewLC( aLog, aItem );
    CleanupStack::Pop( self );

    return self;
	}
CTestParams* CTestParams::NewLC( CStifLogger *aLog, CStifItemParser& aItem )
	{
    CTestParams* self = new (ELeave) CTestParams( aLog );

    CleanupStack::PushL( self );
    self->ConstructL( aItem );

    return self;
	}

    
// Destructor
CTestParams::~CTestParams()
	{
	/**/
	delete iUri;
	delete iData;
	delete iDataType;
	delete iOutputFile;
	delete iExpectedData;
	delete iExpectedType;
	}


// -----------------------------------------------------------------------------
// CTestParams::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*?type CTestParams::?member_function(
    ?arg_type arg,
    ?arg_type arg )
    {
    
    ?code
    
    }
*/





/*
void CTestParams::SetL( CStifItemParser& aItem )
	{
//	iString = aItem.
	TPtrC str;
    TInt ret = aItem.Remainder( str );
	}
*/
const TDesC8& CTestParams::Uri()
	{
	if (iUri != 0)
		{
		return *iUri;
		}
	else
		{
		return KNullDesC8;
		}
	}
const TDesC8& CTestParams::Data()
	{
	if (iData != 0)
		{
		return *iData;
		}
	else
		{
		return KNullDesC8;
		}
	}
const TDesC8& CTestParams::DataType()
	{
	if (iDataType != 0)
		{
		return *iDataType;
		}
	else
		{
		return KNullDesC8;
		}
	}
//sf-void CTestParams::SetCallback( CTestDmCallback* aCallback )
//sf-	{
//sf-	iCallback = aCallback;
//sf-	}
void CTestParams::SetResultsRef( TInt aResultsRef )
	{
	iResultsRef = aResultsRef;
	}
void CTestParams::SetStatusRef( TInt aStatusRef )
	{
	iStatusRef = aStatusRef;
	}


//sf-TInt CTestParams::CheckStatusL( MSmlDmAdapter::TError aStatus )
TInt CTestParams::CheckStatusL( TInt aStatus )
	{
	TInt ret = KErrNone;
	
	if (iExpectedStatus >= 0 && iExpectedStatus != aStatus)
		{
		iLog->Log( _L8("Error: CTestParams::CheckStatusL: expected=%d, status=%d"), iExpectedStatus, aStatus );
		ret = KErrGeneral;
		}

	return ret;
	}

TInt CTestParams::CheckDataL( CBufFlat& aObject )
	{
	TPtrC8 ptr = aObject.Ptr(0);
	TInt ret = CheckDataL( ptr );
	return ret;
/*
	TInt ret = KErrNone;

	if (iExpectedData)
		{
		TPtrC8 ptr = aObject.Ptr(0);
		if (iExpectedData->Compare( ptr ) != 0)
			{
			iLog->Log( _L8("Error: CTestParams::CheckDataL") );
			ret = KErrGeneral;
			}
		}

	return ret;
*/
	}

TInt CTestParams::CheckDataL( const TDesC8& aObject )
	{
	TInt ret = KErrNone;

	if (iExpectedData)
		{
		TInt len1 = iExpectedData->Length();
		TInt len2 = aObject.Length();
		if (iExpectedData->Compare( aObject ) != 0)
			{
			TPtrC8 expt = iExpectedData->Left( 80 );
			TPtrC8 real = aObject.Left( 80 );
//sf-			iLog->Log( _L8("Error: CTestParams::CheckDataL") );
			iLog->Log( _L8("Error: CTestParams::CheckDataL: expected='%S', real='%S'"), &expt, &real );
			ret = KErrGeneral;
			}
		}

	return ret;
	}

TInt CTestParams::CheckTypeL( const TDesC8 &aType )
	{
	TInt ret = KErrNone;

	if (iExpectedType)
		{
		if (iExpectedType->CompareF( aType ) != 0)
			{
			iLog->Log( _L8("Error: CTestParams::CheckTypeL: exp='%S', typ='%S'"), iExpectedType, &aType );
			ret = KErrGeneral;
			}
		}

	return ret;
	}

TInt CTestParams::CheckL( CTestDmCallback* aCallback )
	{
	TInt ret = KErrNone;
//	MSmlDmAdapter::TError status = EOk;
//		const CBufBase& GetResultsObjectL( TInt aResultsRef );
//		const TDesC8& GetResultsTypeL( TInt aResultsRef );

	if (iStatusRef >= 0)
		{
		MSmlDmAdapter::TError status = aCallback->GetStatusL( iStatusRef );
		TInt ret1 = CheckStatusL( status );

		ret = (ret != KErrNone) ? ret : ret1;
		}
	else
		{
//		iLog->"Error: StatusRef not set"
		}

	if (iResultsRef >= 0)
		{
		const CBufBase& tmpObject = aCallback->GetResultsObjectL( iResultsRef );
		TInt ret1 = CheckDataL( *(CBufFlat*) &tmpObject );

		TPtrC8 tmpType = aCallback->GetResultsTypeL( iResultsRef );
		TInt ret2 = CheckTypeL( tmpType );

		// choose first error code
		ret = (ret != KErrNone) ? ret : ret1;
		ret = (ret != KErrNone) ? ret : ret2;
		}
	else
		{
//		iLog->"Note: ResultsRef not set"
		}

	return ret;
	}
	
TInt CTestParams::CheckStatusRefL( TInt aStatusRef )
	{
	TInt ret = KErrNone;

	if (iExpectedStatus)
		{
		
		}

	return ret;
	}
TInt CTestParams::CheckResultsRefL( TInt aResultsRef )
	{
	TInt ret = KErrNone;

	if (iExpectedStatus)
		{
		
		}

	return ret;
	}


//******************************************************************************
//
//
//
//******************************************************************************

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDDFTestParams::CDDFTestParams
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDDFTestParams::CDDFTestParams( CStifLogger *aLog )
	: iLog( aLog )
	, iUri( 0 )

//	, iAccessTypes
	, iDefaultValue( 0 )
	, iDescription( 0 )
//	, iFormat( 0 )
//	, iOccurence( 0 )
//	, iScope( 0 )
	, iTitle( 0 )
	, iMimeType( 0 )
	, iObjectGroup( 0 )
	{
	/**/
	}

// -----------------------------------------------------------------------------
// CDDFTestParams::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDDFTestParams::ConstructL( CStifItemParser& aItem )
	{
	// Parse parameters
	TPtrC str;
    TInt ret = aItem.Remainder( str );

	HBufC8* ptr = HBufC8::NewLC( str.Length() );
	ptr->Des().Copy( str );

//sf-	TBuf8<400> buf;
//sf-	buf.Copy( str )    ;

	iLog->Log( _L("CmdParams: '%S'"), &str );

/*
		Syntax:
		-------
//same as uri		Name=
		AccessTypes=
		DefaultValue=
		Description=
		Format=
		Occurence=
		Scope=
		Title=
		MimeType=
		ObjectGroup=
*/
	/* Keywords */
	_LIT8( KURI,			"URI" ); // = uri
	_LIT8( KAccessTypes,	"AccessTypes" ); // =
	_LIT8( KDefaultValue,	"DefaultValue" ); // =
	_LIT8( KDescription,	"Description" ); // =
	_LIT8( KFormat,			"Format" ); // =
	_LIT8( KOccurence,		"Occurence" ); // =
	_LIT8( KScope,			"Scope" ); // =
	_LIT8( KTitle,			"Title" ); // =
	_LIT8( KMimeType,		"MimeType" ); // = 
	_LIT8( KObjectGroup,	"ObjectGroup" ); // =

	/* AccessTypes */
	const TChar KAccessType_Add     = 'A';
	const TChar KAccessType_Copy    = 'C';
	const TChar KAccessType_Delete	= 'D';
	const TChar KAccessType_Exec	= 'E';
	const TChar KAccessType_Get		= 'G';
	const TChar KAccessType_Replace = 'R';

	/* Format */
	_LIT8( KFormat_B64,		"B64" );
	_LIT8( KFormat_Bool,	"Bool" );
	_LIT8( KFormat_Chr,		"Chr" );
	_LIT8( KFormat_Int,		"Int" );
	_LIT8( KFormat_Node,	"Node" );
	_LIT8( KFormat_Null,	"Null" );
	_LIT8( KFormat_Xml,		"Xml" );
	_LIT8( KFormat_Bin,		"Bin" );

	/* Occurrence */
	_LIT8( KOccurence_One,			"One" );
	_LIT8( KOccurence_ZeroOrOne,	"ZeroOrOne" );
	_LIT8( KOccurence_ZeroOrMore,	"ZeroOrMore" );
	_LIT8( KOccurence_OneOrMore,	"OneOrMore" );
	_LIT8( KOccurence_ZeroOrN,		"ZeroOrN" );
	_LIT8( KOccurence_OneOrN,		"OneOrN" );

	/* Scope */
	_LIT8( KScope_Permanent,	"Permanent" );
	_LIT8( KScope_Dynamic,		"Dynamic" );


	TTestParamVar var( *ptr );
//sf-	TTestParamVar var( buf );
	while (var.Next())
		{
		TPtrC8 key = var.Name();
		TPtrC8 value = var.Value();
		TBool exist = var.ValueExist();
		iLog->Log(_L8("key: '%S', value: '%S', exist: %d"), &key, &value, exist);

		if ( key.CompareF( KURI ) == 0 )
			{
			delete iUri;
			iUri = 0;
			iUri = value.AllocL();
			}
		else if ( key.CompareF( KAccessTypes ) == 0)
			{
//sf-		SetAccessTypes( value );
			for (TInt i = 0 ; i < value.Length() ; i++)
				{
				if ( value[i] == KAccessType_Add )
					{
//sf-				iAccessTypes |= TSmlDmAccessTypes::EAccessType_Add;
					iAccessTypes.SetAdd();
					}
				else if ( value[i] == KAccessType_Copy )
					{
//sf-				iAccessTypes |= TSmlDmAccessTypes::EAccessType_Copy;
					iAccessTypes.SetCopy();
					}
				else if ( value[i] == KAccessType_Delete )
					{
//sf-				iAccessTypes |= TSmlDmAccessTypes::EAccessType_Delete;
					iAccessTypes.SetDelete();
					}
				else if ( value[i] == KAccessType_Exec )
					{
//sf-				iAccessTypes |= TSmlDmAccessTypes::EAccessType_Exec;
					iAccessTypes.SetExec();
					}
				else if ( value[i] == KAccessType_Get )
					{
//sf-				iAccessTypes |= TSmlDmAccessTypes::EAccessType_Get;
					iAccessTypes.SetGet();
					}
				else if ( value[i] == KAccessType_Replace )
					{
//sf-				iAccessTypes |= TSmlDmAccessTypes::EAccessType_Replace;
					iAccessTypes.SetReplace();
					}
				else
					{
					iLog->Log(_L8("Warning: Unknown AccessType: '%S'"), &value );
					}
				}
			}
		else if ( key.CompareF( KDefaultValue ) == 0 )
			{
			delete iDefaultValue;
			iDefaultValue = 0;
			iDefaultValue = value.AllocL();
			}
		else if ( key.CompareF( KDescription ) == 0 )
			{
			delete iDescription;
			iDescription = 0;
			iDescription = value.AllocL();
			}
		else if ( key.CompareF( KFormat ) == 0 )
			{
//sf-		SetFormat( value );
			if (value.CompareF( KFormat_B64 ) == 0 )
				{
				iFormat = MSmlDmDDFObject::EB64;
				}
			else if (value.CompareF( KFormat_Bool ) == 0 )
				{
				iFormat = MSmlDmDDFObject::EBool;
				}
			else if (value.CompareF( KFormat_Chr ) == 0 )
				{
				iFormat = MSmlDmDDFObject::EChr;
				}
			else if (value.CompareF( KFormat_Int ) == 0 )
				{
				iFormat = MSmlDmDDFObject::EInt;
				}
			else if (value.CompareF( KFormat_Node ) == 0 )
				{
				iFormat = MSmlDmDDFObject::ENode;
				}
			else if (value.CompareF( KFormat_Null ) == 0 )
				{
				iFormat = MSmlDmDDFObject::ENull;
				}
			else if (value.CompareF( KFormat_Xml ) == 0 )
				{
				iFormat = MSmlDmDDFObject::EXml;
				}
			else if (value.CompareF( KFormat_Bin ) == 0 )
				{
				iFormat = MSmlDmDDFObject::EBin;
				}
			else
				{
				iLog->Log(_L8("Warning: Unknown Format: '%S'"), &value );
				}
			}
		else if ( key.CompareF( KOccurence ) == 0 )
			{
			if (value.CompareF( KOccurence_One ) == 0 )
				{
				iOccurence = MSmlDmDDFObject::EOne;
				}
			else if (value.CompareF( KOccurence_ZeroOrOne ) == 0 )
				{
				iOccurence = MSmlDmDDFObject::EZeroOrOne;
				}
			else if (value.CompareF( KOccurence_ZeroOrMore ) == 0 )
				{
				iOccurence = MSmlDmDDFObject::EZeroOrMore;
				}
			else if (value.CompareF( KOccurence_OneOrMore ) == 0 )
				{
				iOccurence = MSmlDmDDFObject::EOneOrMore;
				}
			else if (value.CompareF( KOccurence_ZeroOrN ) == 0 )
				{
				iOccurence = MSmlDmDDFObject::EZeroOrN;
				}
			else if (value.CompareF( KOccurence_OneOrN ) == 0 )
				{
				iOccurence = MSmlDmDDFObject::EOneOrN;
				}
			else
				{
				iLog->Log(_L8("Warning: Unknown Occurrence: '%S'"), &value );
				}
			}
		else if ( key.CompareF( KScope ) == 0 )
			{
			if (value.CompareF( KScope_Permanent ) == 0 )
				{
				iScope = MSmlDmDDFObject::EPermanent;
				}
			else if (value.CompareF( KScope_Dynamic ) == 0 )
				{
				iScope = MSmlDmDDFObject::EDynamic;
				}
			else
				{
				iLog->Log(_L8("Warning: Unknown Scope: '%S'"), &value );
				}
			}
		else if ( key.CompareF( KTitle ) == 0 )
			{
			delete iTitle;
			iTitle = 0;
			iTitle = value.AllocL();
			}
		else if ( key.CompareF( KMimeType ) == 0 )
			{
			delete iMimeType;
			iMimeType = 0;
			iMimeType = value.AllocL();
			}
		else if ( key.CompareF( KObjectGroup ) == 0 )
			{
			TLex8 lex( value );
			lex.Val( iObjectGroup );
			}
		else
			{
			// Unknown parameter
			iLog->Log(_L8("Warning: Unknown parameter: key: '%S', value: '%S', exist: %d"), &key, &value, exist);
			}
			
		}
		
	// Cleanup
	CleanupStack::PopAndDestroy( ptr );
	}


// -----------------------------------------------------------------------------
// CDDFTestParams::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDDFTestParams* CDDFTestParams::NewL( CStifLogger *aLog, CStifItemParser& aItem )
	{
    CDDFTestParams* self = NewLC( aLog, aItem );
    CleanupStack::Pop( self );

    return self;
	}
CDDFTestParams* CDDFTestParams::NewLC( CStifLogger *aLog, CStifItemParser& aItem )
	{
    CDDFTestParams* self = new (ELeave) CDDFTestParams( aLog );

    CleanupStack::PushL( self );
    self->ConstructL( aItem );

    return self;
	}


// Destructor
CDDFTestParams::~CDDFTestParams()
	{
	/**/
	delete iUri;
	delete iDefaultValue;
	delete iDescription;
	delete iTitle;
	delete iMimeType;
	}


// -----------------------------------------------------------------------------
// CTestParams::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*?type CTestParams::?member_function(
    ?arg_type arg,
    ?arg_type arg )
    {
    
    ?code
    
    }
*/

const TDesC8& CDDFTestParams::Uri()
	{
	if (iUri != 0)
		{
		return *iUri;
		}
	else
		{
		return KNullDesC8;
		}
	}

TInt CDDFTestParams::CheckL( CTestDmDDFObject* aDdfRoot )
	{
	TInt ret = KErrNone;

	// Find correct node
	CTestDmDDFObject* curNode = aDdfRoot;
	TInt num = NSmlDmURI::NumOfURISegs( *iUri );

	for (TInt i = 0 ; i < num && curNode != 0 ; i++)
		{
		TPtrC8 nodeName = NSmlDmURI::URISeg( *iUri, i );
		if ( nodeName.CompareF( _L8("<X>") ) == 0 )
			{
			nodeName.Set( KNullDesC8 );
			}
		curNode = curNode->FindChildObject( nodeName );
		}

	// Check node data
	iLog->Log(_L8("Checking node: URI='%S'"), iUri );

	if (curNode != 0)
		{
/*
		if ( iUri && iUri->Compare( curNode->Uri() ) != 0 )
			{
			TPtrC8 expt = *iUri;
			TPtrC8 real =  curNode->Uri();
			iLog->Log(_L8("Error: Uri: expected='%S' real='%S'"), &expt, &real );
			ret = KErrGeneral;
			}
*/
		if ( iAccessTypes.GetACL() != curNode->AccessTypes().GetACL() )
			{
			TUint8 expt = iAccessTypes.GetACL();
			TUint8 real =  curNode->AccessTypes().GetACL();
			iLog->Log(_L8("Error: AccessTypes: expected=%d real=%d"), expt, real );
			ret = KErrGeneral;
			}
		if ( iDefaultValue && iDefaultValue->Compare( curNode->DefaultValue() ) != 0 )
			{
			TPtrC8 expt = *iDefaultValue;
			TPtrC8 real =  curNode->DefaultValue();
			iLog->Log(_L8("Error: DefaultValue: expected='%S' real='%S'"), &expt, &real );
			ret = KErrGeneral;
			}
		if ( iDescription && iDescription->Compare( curNode->Description() ) != 0 )
			{
			TPtrC8 expt = *iDescription;
			TPtrC8 real =  curNode->Description();
			iLog->Log(_L8("Error: DefaultValue: expected='%S' real='%S'"), &expt, &real );
			ret = KErrGeneral;
			}
		if ( iFormat != curNode->DFFormat() )
			{
			TInt expt = iFormat;
			TInt real =  curNode->DFFormat();
			iLog->Log(_L8("Error: Format: expected=%d real=%d"), expt, real );
			ret = KErrGeneral;
			}
		if ( iOccurence != curNode->Occurence() )
			{
			TInt expt = iOccurence;
			TInt real =  curNode->Occurence();
			iLog->Log(_L8("Error: Occurence: expected=%d real=%d"), expt, real );
			ret = KErrGeneral;
			}
		if ( iScope != curNode->Scope() )
			{
			TInt expt = iScope;
			TInt real =  curNode->Scope();
			iLog->Log(_L8("Error: Scope: expected=%d real=%d"), expt, real );
			ret = KErrGeneral;
			}
		if ( iTitle && iTitle->Compare( curNode->DFTitle() ) != 0 )
			{
			TPtrC8 expt = *iTitle;
			TPtrC8 real =  curNode->DFTitle();
			iLog->Log(_L8("Error: Title: expected='%S' real='%S'"), &expt, &real );
			ret = KErrGeneral;
			}
		if ( iMimeType && iMimeType->Compare( curNode->DFTypeMimeType() ) != 0 )
			{
			TPtrC8 expt = *iMimeType;
			TPtrC8 real =  curNode->DFTypeMimeType();
			iLog->Log(_L8("Error: MimeType: expected='%S' real='%S'"), &expt, &real );
			ret = KErrGeneral;
			}
		if ( iObjectGroup != curNode->ObjectGroup() )
			{
			TInt expt = iObjectGroup;
			TInt real =  curNode->ObjectGroup();
			iLog->Log(_L8("Error: ObjectGroup: expected=%d real=%d"), expt, real );
			ret = KErrGeneral;
			}
		}
	else
		{
		iLog->Log(_L8("Error: Node not found") );
		ret = KErrGeneral;
		}

	return ret;
	}

//******************************************************************************
//
//
//
//******************************************************************************


TTestParamVar::TTestParamVar( const TDesC8 &aStr )
	: iLog( 0 )
	, iLex( aStr )
//sf-	: iStr( aStr )
//sf-	, iPos( 0 )
	, iName( KNullDesC8 )
	, iValue( KNullDesC8 )
	, iValueExist( EFalse )
	{
	}

TBool TTestParamVar::Next()
	{
	TBool ok = ETrue;
	
	iName.Set( KNullDesC8 );
	iValue.Set( KNullDesC8 );
	iValueExist = EFalse;

	ok = !iLex.Eos();

	if (ok)
		{
		iName.Set( GetWord() );
		if (iName.Length() == 0)
			{
			ok = EFalse;
			}
		}

	TChar chr = GetChar();
	if (chr == '=')
		{
		iValue.Set( GetWord() );
/*
        TInt i, j, k = iValue.Length();
        for( i=j=0; i<k; i++,j++ )
            {
            if( iValue[i] == TChar('\"') && iValue[i+1] == TChar('\"') )
                {
                i++;
                }
            iValue[j] = iValue[i];
            }
        iValue.SetLength(j);
        */
/*            
		while( iValue.Find( _L("\"\"") != KErrNotFound ) 
		    {
		    iValue.Replace( iValue.Find( _L("\"\""), 2, _L("\"") );
		    }
		    */
		
		iValueExist = ETrue;
		}
	else if (!chr.Eos())
		{
		UnGetChar();
		}

	if (ok && iLog)
		{
		iLog->Log( _L8("name: '%S', value: '%S', exist: %d"), &iName, &iValue, iValueExist );
		}

	return ok;
	}

const TDesC8& TTestParamVar::Name()
	{
	return iName;
	}

const TDesC8& TTestParamVar::Value()
	{
	return iValue;
	}

TBool TTestParamVar::ValueExist()
	{
	return iValueExist;
	}

TChar TTestParamVar::GetChar()
	{
	iLex.SkipSpace();
	return iLex.Get();
	}

void TTestParamVar::UnGetChar()
	{
	iLex.UnGet();
	}

TPtrC8 TTestParamVar::GetWord()
	{
	TBool quoted = EFalse;

	iLex.SkipSpace();

	// Check if word is quoted
	if (iLex.Peek() == '\"')
		{
		iLex.Get();
		quoted = ETrue;
		}

	// Mark start position of word
	iLex.Mark();

	// Seek end of word and extract it
	TBool ready = EFalse;
	while (!ready && !iLex.Eos())
		{
		TChar chr = iLex.Get();

//sf-	if (chr == '\"' || (!quoted && (chr.IsSpace() || chr == '=')))
		if (quoted)
			{
			if (chr == '\"')
				{
				if((iLex.Eos()) || (iLex.Peek() == ' '))
				    {
				    iLex.UnGet();
				    ready = ETrue;
				    }
				}
			}
		else if (chr.IsSpace() || chr == '=' || chr == '\"')
			{
			iLex.UnGet();
			ready = ETrue;
			}
		}

	// Extract word
	TPtrC8 word = iLex.MarkedToken();
	if (quoted)
		{
		iLex.Get();
		}

	// return
	return word;
	}


/*
// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ?function_name implements...
// ?implementation_description.
// Returns: ?value_1: ?description
//          ?value_n: ?description
//                    ?description
// -----------------------------------------------------------------------------
//
?type  ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg )  // ?description
    {

    ?code

    }
*/
//  End of File  
