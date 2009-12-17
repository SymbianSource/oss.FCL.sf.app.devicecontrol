/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DM Email Settings Adapter
*
*/



#include "EmbeddedLinkAdapter.h"

#include <implementationproxy.h> // For TImplementationProxy definition

//#ifdef __SAP_POLICY_MANAGEMENT
#include <PolicyEngineXACML.h>
//#endif

#include "EmbeddedLinkHandler.h"
#include "nsmldmuri.h"
#include "debug.h"
#include <featmgr.h>
#include <utf.h> 


#define TO_BE_IMPLEMENTED()	{ ret = EError; }

typedef TBuf<10> TEmbeddedLinkAppName;

// ------------------------------------------------------------------------------------------------
// CNSmlEmbeddedLinkAdapter* CNSmlEmbeddedLinkAdapter::NewL( )
// ------------------------------------------------------------------------------------------------
CNSmlEmbeddedLinkAdapter* CNSmlEmbeddedLinkAdapter::NewL( MSmlDmCallback* aDmCallback )
	{
	RDEBUG("CNSmlEmbeddedLinkAdapter::NewL(): begin");
	CNSmlEmbeddedLinkAdapter* self = NewLC( aDmCallback );
	CleanupStack::Pop();
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlEmbeddedLinkAdapter* CNSmlEmbeddedLinkAdapter::NewLC( )
// ------------------------------------------------------------------------------------------------
CNSmlEmbeddedLinkAdapter* CNSmlEmbeddedLinkAdapter::NewLC( MSmlDmCallback* aDmCallback )
	{
	RDEBUG("CNSmlEmbeddedLinkAdapter::NewLC(): begin");
	CNSmlEmbeddedLinkAdapter* self = new ( ELeave ) CNSmlEmbeddedLinkAdapter( aDmCallback );
	CleanupStack::PushL( self );
	self->ConstructL( );
	return self;
	}


// ------------------------------------------------------------------------------------------------
// CNSmlEmbeddedLinkAdapter::CNSmlEmbeddedLinkAdapter()
// ------------------------------------------------------------------------------------------------

CNSmlEmbeddedLinkAdapter::CNSmlEmbeddedLinkAdapter( MSmlDmCallback* aDmCallback ) 
	: CTARMDmAdapter( aDmCallback )
	, iCallBack( aDmCallback )
	{
	RDEBUG("CNSmlEmbeddedLinkAdapter::CNSmlEmbeddedLinkAdapter(): begin");
	}


// ------------------------------------------------------------------------------------------------
// CNSmlEmbeddedLinkAdapter::~CNSmlEmbeddedLinkAdapter()
// ------------------------------------------------------------------------------------------------


CNSmlEmbeddedLinkAdapter::~CNSmlEmbeddedLinkAdapter()
	{
	RDEBUG("CNSmlEmbeddedLinkAdapter::~CNSmlEmbeddedLinkAdapter(): begin");
	FeatureManager::UnInitializeLib();
	delete iLinkHandler;
	}

// ------------------------------------------------------------------------------------------------
//  CNSmlEmbeddedLinkAdapter::DDFVersionL()
// ------------------------------------------------------------------------------------------------
void CNSmlEmbeddedLinkAdapter::DDFVersionL( CBufBase& aDDFVersion )
	{
	RDEBUG("CNSmlEmbeddedLinkAdapter::DDFVersionL(TDes& aDDFVersion): begin");
	aDDFVersion.InsertL( 0,KNSmlEmbeddedLinkAdapterDDFVersion );
	RDEBUG("CNSmlEmbeddedLinkAdapter::DDFVersionL(TDes& aDDFVersion): end");
	}


// ------------------------------------------------------------------------------------------------
//  CNSmlEmbeddedLinkAdapter::DDFStructureL()
//
// ------------------------------------------------------------------------------------------------
void CNSmlEmbeddedLinkAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
	{
	RDEBUG("CNSmlEmbeddedLinkAdapter::DDFStructureL(): begin");	

	TSmlDmAccessTypes accessTypesGet;
	accessTypesGet.SetGet();

	TSmlDmAccessTypes accessTypesAddGet;
	accessTypesAddGet.SetAdd();
	accessTypesAddGet.SetGet();

	TSmlDmAccessTypes accessTypesGetReplace;
	accessTypesGetReplace.SetGet();
	accessTypesGetReplace.SetReplace();

	TSmlDmAccessTypes accessTypesNone;

	/*
	Node: ./Customization
	*/
	MSmlDmDDFObject& Customization = aDDF.AddChildObjectL( KNSmlEmbeddedLinkCustomization );
	FillNodeInfoL( Customization, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
				MSmlDmDDFObject::ENode, KNullDesC8 );	
	
	/*
	Node: ./EmbeddedLinks
	*/
	MSmlDmDDFObject& EmbeddedLinks = Customization.AddChildObjectL( KNSmlEmbeddedLinkEmbeddedLinks );
	FillNodeInfoL( EmbeddedLinks, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
				MSmlDmDDFObject::ENode, KNullDesC8 );


	/*
	Node: ./EmbeddedLinks/<X>
	*/
	MSmlDmDDFObject& groupNode = EmbeddedLinks.AddChildObjectGroupL( );
	FillNodeInfoL( groupNode, accessTypesGet, MSmlDmDDFObject::EZeroOrMore, MSmlDmDDFObject::EDynamic,
					MSmlDmDDFObject::ENode, KNSmlEmbeddedLinkDynamicNode );

	/*
	Node: ./EmbeddedLinks/<X>/Links
	*/
	MSmlDmDDFObject& Links = groupNode.AddChildObjectL( KNSmlEmbeddedLinkLink );
	FillNodeInfoL( Links,accessTypesAddGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
					MSmlDmDDFObject::ENode, KNSmlEmbeddedLinkLink );

	/*
	Node: ./EmbeddedLinks/<X>/ID
	*/
	/*
	MSmlDmDDFObject& idNode = groupNode.AddChildObjectL( KNSmlEmbeddedLinkID );
	FillNodeInfoL( idNode, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
					MSmlDmDDFObject::EChr, KNSmlEmbeddedLinkLink );
	*/
	/*
	Node: ./EmbeddedLinks/<X>/AppName
	*/
	/*
	MSmlDmDDFObject& appNameNode = groupNode.AddChildObjectL( KNSmlEmbeddedLinkAppName );
	FillNodeInfoL( appNameNode, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
					MSmlDmDDFObject::EChr, KNSmlEmbeddedLinkLink );
	*/
	
	/*
	Node: ./EmbeddedLinks/<X>/Links/<X>
	*/
	TSmlDmAccessTypes accessTypesAddDeleteGetReplace;
	accessTypesAddDeleteGetReplace.SetAdd();
	accessTypesAddDeleteGetReplace.SetDelete();
	accessTypesAddDeleteGetReplace.SetGet();
	accessTypesAddDeleteGetReplace.SetReplace();

	MSmlDmDDFObject& group2Node = Links.AddChildObjectGroupL( );
	FillNodeInfoL( group2Node, accessTypesAddDeleteGetReplace, MSmlDmDDFObject::EZeroOrMore,
					MSmlDmDDFObject::EDynamic,			//EDynamic
					MSmlDmDDFObject::ENode, KNSmlEmbeddedLinkLink );


	/*
	Node: ./EmbeddedLinks/<X>/Links/<X>/URL
	*/
	MSmlDmDDFObject& urlNode = group2Node.AddChildObjectL( KNSmlEmbeddedLinkURL );
	FillNodeInfoL( urlNode, accessTypesGetReplace, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
					MSmlDmDDFObject::EChr, KNSmlEmbeddedLinkURL );

	/*
	Node: ./EmbeddedLinks/<X>/Links/<X>/Name
	*/
	MSmlDmDDFObject& nameNode = group2Node.AddChildObjectL( KNSmlEmbeddedLinkName );
	FillNodeInfoL( nameNode, accessTypesGetReplace, MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
					MSmlDmDDFObject::EChr, KNSmlEmbeddedLinkName );

	/*
	Node: ./EmbeddedLinks/<X>/Links/<X>/UserName
	*/
	MSmlDmDDFObject& usernameNode = group2Node.AddChildObjectL( KNSmlEmbeddedLinkUserName );
	FillNodeInfoL( usernameNode, accessTypesGetReplace, MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
					MSmlDmDDFObject::EChr, KNSmlEmbeddedLinkUserName );

	/*
	Node: ./EmbeddedLinks/<X>/Links/<X>/Password
	*/
	MSmlDmDDFObject& passwordNode = group2Node.AddChildObjectL( KNSmlEmbeddedLinkPassword );
	FillNodeInfoL( passwordNode, accessTypesGetReplace, MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
					MSmlDmDDFObject::EChr, KNSmlEmbeddedLinkPassword );

	/*
	Node: ./EmbeddedLinks/<X>/Links/<X>/ConRef
	*/
	MSmlDmDDFObject& confrefNode = group2Node.AddChildObjectL( KNSmlEmbeddedLinkConfRef );
	FillNodeInfoL( confrefNode, accessTypesGetReplace, MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
					MSmlDmDDFObject::EChr, KNSmlEmbeddedLinkConfRef );


	}

// ------------------------------------------------------------------------------------------------
//  CNSmlEmbeddedLinkAdapter::UpdateLeafObjectL()
// ------------------------------------------------------------------------------------------------
void CNSmlEmbeddedLinkAdapter::_UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& /*aLUID*/, const TDesC8& aObject, const TDesC8& /*aType*/, const TInt aStatusRef )	//OK
	{
	RDEBUG( "CNSmlEmbeddedLinkAdapter::UpdateLeafObjectL(): begin" );

	TError ret( EOk ) ;

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TInt cnt( NSmlDmURI::NumOfURISegs( uriPtrc ) );
	TPtrC8 field( NSmlDmURI::LastURISeg( uriPtrc ) ) ;
#else
	TInt cnt( NSmlDmURI::NumOfURISegs( aURI ) );
	TPtrC8 field( NSmlDmURI::LastURISeg( aURI ) ) ;
#endif	
	
	TBuf<1000>obj;
	TInt err = CnvUtfConverter::ConvertToUnicodeFromUtf8(obj,aObject);
	
	if( cnt == 6 )
		{	
		TInt linkUid;
		
		HBufC8* luid = iCallBack->GetLuidAllocL( NSmlDmURI::RemoveLastSeg( aURI ) );	

		TLex8 lexa( luid->Des() );
		TInt err = lexa.Val( linkUid );

		delete luid;

		if (err == KErrNone)
			{
			if ( field == KNSmlEmbeddedLinkURL )
				{
				iLinkHandler->UpdateLinkUrlL( linkUid, obj );
				}
			else if ( field == KNSmlEmbeddedLinkName )
				{
				iLinkHandler->UpdateLinkNameL( linkUid, obj );
				}
			else if ( field == KNSmlEmbeddedLinkUserName )
				{
				iLinkHandler->UpdateUserNameL( linkUid, obj );
				}
			else if ( field == KNSmlEmbeddedLinkPassword )
				{
				iLinkHandler->UpdatePasswordL( linkUid, obj );
				}
			else if ( field == KNSmlEmbeddedLinkConfRef )
				{
				iLinkHandler->UpdateWapApL( linkUid, obj );
				}
			else 
				{
				ret = EInvalidObject;
				}
			}
		else 
			{
			ret = EInvalidObject;
			}
		} 
	else 
		{
		ret = EInvalidObject;
		}

	iCallBack->SetStatusL( aStatusRef, ret );
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CNSmlEmbeddedLinkAdapter::_UpdateLeafObjectL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/, RWriteStream*& /*aStream*/, const TDesC8& /*aType*/, TInt aStatusRef )
	{
	RDEBUG( "CNSmlEmbeddedLinkAdapter::UpdateLeafObjectL(...RWriteStream...): begin" );
	//Not supported
	TError ret = EError;
	iCallBack->SetStatusL( aStatusRef, ret );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlEmbeddedLinkAdapter::DeleteObjectL( const TDesC& aURI, const TDesC& aLUID )
// ------------------------------------------------------------------------------------------------
void CNSmlEmbeddedLinkAdapter::_DeleteObjectL( const TDesC8& aURI, const TDesC8& /*aLUID*/, const TInt aStatusRef )		// OK
	{
	RDEBUG("CNSmlEmbeddedLinkAdapter::DeleteLeafObjectL( ): begin");

	TError ret = EOk ;
	TInt linkUid;
	HBufC8* field = HBufC8::NewLC( 16 );
	
#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TInt cnt( NSmlDmURI::NumOfURISegs( uriPtrc ) );
	field->Des().Copy( NSmlDmURI::LastURISeg( uriPtrc ) );
#else
	TInt cnt( NSmlDmURI::NumOfURISegs( aURI ) );
	field->Des().Copy( NSmlDmURI::LastURISeg( aURI ) );
#endif	

	if ( cnt == 5 )
		{					// remove item from bookmarks
			
		HBufC8* luid = iCallBack->GetLuidAllocL( aURI );
	
		TLex8 lexa( luid->Des() );
		TInt err( lexa.Val( linkUid ) );
		delete luid;
		luid = NULL;
		if ( err == KErrNone )
			{
			iLinkHandler->RemoveLinkL( linkUid );
			} 
		else 
			{
			ret = EInvalidObject;
			}

		} 
	else if ( cnt == 6 ) 
		{
		
		HBufC8* luid=iCallBack->GetLuidAllocL( NSmlDmURI::RemoveLastSeg( aURI ) );		
		TLex8 lexa( luid->Des() );
		
		TInt err( lexa.Val( linkUid ) );
		delete luid;
		luid = NULL;
		TBuf<8> emptyStr;
		emptyStr.Copy( _L("") );
		
		if ( err == KErrNone )
			{
			if( field->Des() == KNSmlEmbeddedLinkURL )
				{
				//iLinkHandler->UpdateLinkUrlL(linkUid, _L(""));
				iLinkHandler->UpdateLinkUrlL( linkUid, emptyStr );
				}
			else if ( field->Des() == KNSmlEmbeddedLinkName )
				{
				iLinkHandler->UpdateLinkNameL( linkUid, emptyStr );
				}
			else if ( field->Des() == KNSmlEmbeddedLinkUserName )
				{
				iLinkHandler->UpdateUserNameL(linkUid, emptyStr);
				}
			else if ( field->Des() == KNSmlEmbeddedLinkPassword )
				{
				iLinkHandler->UpdatePasswordL( linkUid, emptyStr );
				}
			else if ( field->Des() == KNSmlEmbeddedLinkConfRef ) 
				{
				
				}
			else 
				{
				ret = EInvalidObject;
				}
			}
		else 
			{
			ret = EInvalidObject;
			}
		
		}	
	else
		{
		}
	CleanupStack::PopAndDestroy(); //  field
	iCallBack->SetStatusL(aStatusRef, ret);
	}

// ------------------------------------------------------------------------------------------------
//  CNSmlEmbeddedLinkAdapter::FetchLeafObjectL( const TDesC& aURI, const TDesC& aLUID, const TDesC& aType, CBufBase& aObject )
// ------------------------------------------------------------------------------------------------
void CNSmlEmbeddedLinkAdapter::_FetchLeafObjectL( const TDesC8& aURI, const TDesC8& /*aLUID*/, const TDesC8& aType, const TInt aResultsRef, const TInt aStatusRef )
	{
	RDEBUG( "CNSmlEmbeddedLinkAdapter::FetchLeafObjectL(): begin" );

	TError ret( EOk );

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TInt cnt( NSmlDmURI::NumOfURISegs( uriPtrc ) );
	TPtrC8 mapping( NSmlDmURI::LastURISeg( uriPtrc ) );
#else
	TInt cnt( NSmlDmURI::NumOfURISegs( aURI ) );
	TPtrC8 mapping( NSmlDmURI::LastURISeg( aURI ) );
#endif
	
	HBufC *data = HBufC::NewLC( 1000 );			
	CBufBase* object = CBufFlat::NewL( 1000 );
	CleanupStack::PushL( object );

	TBuf<64> appName;
	
#ifdef __TARM_SYMBIAN_CONVERGENCY
	RecognizeAppTypeL( uriPtrc, appName );
#else
	RecognizeAppTypeL( aURI, appName );
#endif	
	
	if ( cnt == 6 )
		{
		
		TPtrC8 linkName( NSmlDmURI::RemoveLastSeg( aURI ) );	
		HBufC8* uidBuf = iCallBack->GetLuidAllocL( linkName );
		
		CleanupStack::PushL( uidBuf );
		TInt uidInt;
		TInt app;

		TLex8 leksa( uidBuf->Des() );
		leksa.Val( uidInt );

		TLex leksa2( appName );
		leksa2.Val( app );

		CEmbeddedLinkApp* linkApp = NULL;
		
		User::LeaveIfError( iLinkHandler->GetLinkAppL( app, linkApp ) );

		CFavouritesItem* favItem = CFavouritesItem::NewL();
		CleanupStack::PushL( favItem );
		linkApp->GetLinkL( uidInt, *favItem );


		// HBufC *uidi = iCallback->GetLuidALloc(linkname);
		// tee uidista TUid ja  hae browserista uidia vastaava linkki
		// kaiva applikaatiofoldertieto urin seasta jos tartteee
		// ota linkistä tarvittava tieto kuten allakin on jo tehhty

		if( mapping == KNSmlEmbeddedLinkURL )
			{
			data->Des().Copy( favItem->Url() );
			} 
		else if ( mapping == KNSmlEmbeddedLinkName )
			{
			data->Des().Copy( favItem->Name() );
			}
		else if ( mapping == KNSmlEmbeddedLinkUserName )
			{
			ret = EError;
			}
		else if( mapping == KNSmlEmbeddedLinkPassword )
			{
			ret = EError;
			}
		else if( mapping == KNSmlEmbeddedLinkConfRef )
			{
			TFavouritesWapAp wapAp = favItem->WapAp();
			if (wapAp.IsDefault())
				{
				data->Des().Copy( KWapApDefault() );
				}
			else if (wapAp.IsNull())
				{
				data->Des().Copy( KWapApNull() );
				}
			else
				{
				TUint32 val = wapAp.ApId();
				data->Des().Num( (TInt64)val );
				}
			}
		else
			{
			ret = EInvalidObject;
			}

		CleanupStack::PopAndDestroy( 2 );	// favItem, //sf-NO:(linkApp), uidBuf
		}
	else if ( cnt == 4 )
		{
		TInt app;
		TLex leksaApp( appName );
		leksaApp.Val( app );

		CEmbeddedLinkApp* linkApp = NULL;
		User::LeaveIfError( iLinkHandler->GetLinkAppL( app, linkApp ) );

		if( mapping == KNSmlEmbeddedLinkID )
			{
			TInt id = linkApp->AppId();
			data->Des().Num( (TInt64)id );
			}
		else if ( mapping == KNSmlEmbeddedLinkAppName )
			{
			TPtrC name = linkApp->AppName();
			data->Des().Copy( name );
			}
		else
			{
			ret = EInvalidObject;
			}
		}
	else 
		{
		ret = EInvalidObject;
		}
	TBuf8<1000> resultBuf;
	 TInt retVal = CnvUtfConverter::ConvertFromUnicodeToUtf8(resultBuf,data->Des());
	object->InsertL( 0, resultBuf );
	iCallBack->SetResultsL( aResultsRef, *object, aType );
	iCallBack->SetStatusL( aStatusRef, ret );
	CleanupStack::PopAndDestroy( 2 ); // object, data
	}


// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CNSmlEmbeddedLinkAdapter::_FetchLeafObjectSizeL( const TDesC8& aURI, const TDesC8& /*aLUID*/, const TDesC8& /*aType*/, TInt /*aResultsRef*/, TInt aStatusRef )
	{
	RDEBUG("CNSmlEmbeddedLinkAdapter::FetchLeafObjectSizeL(): begin");

	TError ret = EOk;

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TNodeId nodeId = NodeId( uriPtrc );
#else
	TNodeId nodeId = NodeId( aURI );
#endif	

	switch( nodeId )
		{
	case ENodeCustomization:
		ret = EError;
		break;

	default:
		ret = EError;
		break;
		}

	iCallBack->SetStatusL( aStatusRef, ret );
	}

// ------------------------------------------------------------------------------------------------
//  CNSmlEmbeddedLinkAdapter::ChildURIListL( const TDesC& aURI, const TDesC& aParentLUID, const CArrayFix<TNSmlDmMappingInfo>& aPreviousURISegmentList, CArrayFix<TPtrC>& aCurrentURISegmentList )
// ------------------------------------------------------------------------------------------------
void CNSmlEmbeddedLinkAdapter::_ChildURIListL( const TDesC8& aURI, 
		const TDesC8& /*aParentLUID*/, 
		const CArrayFix<TSmlDmMappingInfo>& /*aPreviousURISegmentList*/, 
		const TInt aResultsRef, const TInt aStatusRef  )		//OK
	{
	RDEBUG("CNSmlEmbeddedLinkAdapter::ChildURIListL(): begin");
	TError ret( EOk );
	HBufC *data = HBufC::NewLC( 1000 );	
	CBufBase* object = CBufFlat::NewL( 1000 );
	
	CleanupStack::PushL( object );

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TInt cnt( NSmlDmURI::NumOfURISegs( uriPtrc ) );
#else
	TInt cnt( NSmlDmURI::NumOfURISegs( aURI ) );
#endif

	if ( cnt == 1 )
		{
		object->InsertL( 0, KRootLeafs8 );
		}
	else if ( cnt == 2 )
		{							// application list							
		RArray<TEmbeddedLinkAppName> array;
		
		iLinkHandler->SupportingApplicationsL( array );
		
		data->Des().Zero();
		TBuf<8> app;
		for( TInt i=0; i<array.Count(); i++)
			{
			if ( i > 0 ) 
				{
				data->Des().Append( '/' );
				}
			app=array[i];
			data->Des().Append( array[i] );
			}
		array.Close();
		}	
	else if ( cnt == 3 )
		{
		object->InsertL( 0, KAppLeafs8 );
		} 
	else if ( cnt == 4 )
		{				// tietyn applikaation linkkilistaus		/Customization/EmbeddedLinks/4/Links
		TBuf<32> appName;
		TInt appType;
#ifdef __TARM_SYMBIAN_CONVERGENCY		
		User::LeaveIfError( RecognizeAppTypeL( uriPtrc, appName ) );
#else
		User::LeaveIfError( RecognizeAppTypeL( aURI, appName ) );
#endif		
		TLex lexa( appName );
		User::LeaveIfError( lexa.Val( appType ) );
	
		CEmbeddedLinkApp* linkApp = NULL;
		
		User::LeaveIfError( iLinkHandler->GetLinkAppL( appType, linkApp ) );
		
		CFavouritesItemList* itemList = new ( ELeave ) CFavouritesItemList;
		CleanupStack::PushL( itemList );

		linkApp->GetLinksL( *itemList );
		
		HBufC8 *URIname = HBufC8::NewLC( 64 );
		
		TBuf8<32> uidBuf;

		for( TInt i = 0; i < itemList->Count(); i++ )
			{				// Generates the list of bookmarks AND also maps the luids to the bookmarks
			if( i > 0 && object->Size() > 0 ) 
				{
				data->Des().Append( '/' );
				}
			data->Des().Append( itemList->At( i )->Name() );
			{
			TBuf8<1000> resultBuf;
			TInt retVal = CnvUtfConverter::ConvertFromUnicodeToUtf8(resultBuf,data->Des());
			object->InsertL( object->Size(), resultBuf);
			data->Des().Zero();
			}
			// TODO, SymbianConvergencym is this aURI ok in this method? should be I guess...
			URIname->Des().Append( aURI );
			URIname->Des().Append( _L8( "/" ) );
			URIname->Des().Append( itemList->At( i )->Name() );
			
			TInt contekstiId( itemList->At( i )->ContextId() );
			TInt pfolderid( itemList->At( i )->ParentFolder() );	
			TInt linkUid( itemList->At( i )->Uid() );
			uidBuf.AppendNum( (TInt64)linkUid );
			
			HBufC8* ptr = iCallBack->GetLuidAllocL( URIname->Des() );
			
			if ( ptr->Length() == 0 )
				{
				iCallBack->SetMappingL( URIname->Des(), uidBuf );		// Check if the bookmark has already a luid mapping	
				}
			
			delete ptr;
			ptr = NULL;
			
			uidBuf.Zero();
			URIname->Des().Zero();
		
			}
		CleanupStack::PopAndDestroy( 2 );	// URIname, itemList, //sf-NO:linkApp
		}
	else if ( cnt == 5 )
		{
		object->InsertL( 0, KLinkLeafs8 );
		} 
	else 
		{
		ret = EInvalidObject;
		}
	    TBuf8<1000> resultBuf;
	     TInt retVal = CnvUtfConverter::ConvertFromUnicodeToUtf8(resultBuf,data->Des());
	    object->InsertL( 0, resultBuf );
	
	iCallBack->SetResultsL( aResultsRef, *object, KNullDesC8 );
	iCallBack->SetStatusL( aStatusRef, ret );
	CleanupStack::PopAndDestroy( 2 );	//data, object
	}


// ------------------------------------------------------------------------------------------------
// CNSmlEmbeddedLinkAdapter::AddNodeObjectL( const TDesC& aURI, const TDesC& aParentLUID )
// ------------------------------------------------------------------------------------------------
void CNSmlEmbeddedLinkAdapter::_AddNodeObjectL( const TDesC8& aURI, const TDesC8& /*aParentLUID*/, const TInt aStatusRef )
{
	TError ret( EOk );
	RDEBUG( "CNSmlEmbeddedLinkAdapter::AddNodeObjectL(): begin" );

	// tee uus bookmark
	// sitte iCallback->SetMapping (aURI, bookmarkUIDi favouriteItemistä);

	TInt appType;
	TBuf<32> appName;

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	User::LeaveIfError( RecognizeAppTypeL( uriPtrc, appName ) );
#else
	User::LeaveIfError( RecognizeAppTypeL( aURI, appName ) );
#endif	
	
	TLex lexa( appName );

	User::LeaveIfError( lexa.Val( appType ) );
	
	CEmbeddedLinkApp* linkApp = NULL;
	User::LeaveIfError( iLinkHandler->GetLinkAppL( appType, linkApp ) );

	TBuf<8> emptyStr;
	emptyStr.Copy( _L( "Unknown" ) );

	TInt luid( linkApp->AddLinkL( emptyStr, emptyStr ) );
	if( luid < KErrNone )
		{
		ret = EError;
		}
	else
		{
		TBuf8<16> luidBuf;
		luidBuf.AppendNum( (TInt64)luid );
		iCallBack->SetMappingL( aURI, luidBuf );	
		}

	iCallBack->SetStatusL( aStatusRef, ret );
}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CNSmlEmbeddedLinkAdapter::_ExecuteCommandL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/, const TDesC8& /*aArgument*/, const TDesC8& /*aType*/, TInt aStatusRef )
	{
	RDEBUG("CNSmlEmbeddedLinkAdapter::ExecuteCommandL(): begin");
	//Not supported
	TError ret = EError;
	iCallBack->SetStatusL( aStatusRef, ret );
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CNSmlEmbeddedLinkAdapter::_ExecuteCommandL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/, RWriteStream*& /*aStream*/, const TDesC8& /*aType*/, TInt aStatusRef )
	{
	RDEBUG("CNSmlEmbeddedLinkAdapter::ExecuteCommandL(...RWriteStream...): begin");
	//Not supported
	TError ret = EError;
	iCallBack->SetStatusL( aStatusRef, ret );
	}
	
// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CNSmlEmbeddedLinkAdapter::_CopyCommandL( const TDesC8& /*aTargetURI*/, const TDesC8& /*aTargetLUID*/, const TDesC8& /*aSourceURI*/, const TDesC8& /*aSourceLUID*/, const TDesC8& /*aType*/, TInt aStatusRef )
	{
	RDEBUG("CNSmlEmbeddedLinkAdapter::CopyCommandL(): begin");
	//Not supported
	TError ret = EError;
	iCallBack->SetStatusL( aStatusRef, ret );
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CNSmlEmbeddedLinkAdapter::StartAtomicL()
	{
	RDEBUG("CNSmlEmbeddedLinkAdapter::StartAtomicL(): begin");
	//Not supported
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CNSmlEmbeddedLinkAdapter::CommitAtomicL()
	{
	RDEBUG("CNSmlEmbeddedLinkAdapter::CommitAtomicL(): begin");
	//Not supported
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CNSmlEmbeddedLinkAdapter::RollbackAtomicL()
	{
	RDEBUG("CNSmlEmbeddedLinkAdapter::RollbackAtomicL(): begin");
	//Not supported
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
TBool CNSmlEmbeddedLinkAdapter::StreamingSupport( TInt& /*aItemSize*/ )
	{
	RDEBUG("CNSmlEmbeddedLinkAdapter::StreamingSupport(): begin");
	return EFalse;
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
#ifdef __TARM_SYMBIAN_CONVERGENCY	
void CNSmlEmbeddedLinkAdapter::StreamCommittedL( RWriteStream& /*aStream*/ )
#else
void CNSmlEmbeddedLinkAdapter::StreamCommittedL()
#endif	
	{	
	RDEBUG("CNSmlEmbeddedLinkAdapter::StreamCommittedL(): begin");
	RDEBUG("CNSmlEmbeddedLinkAdapter::StreamCommittedL(): end");
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CNSmlEmbeddedLinkAdapter::CompleteOutstandingCmdsL()
	{
	RDEBUG("CNSmlEmbeddedLinkAdapter::CompleteOutstandingCmdsL(): begin");
	}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
//#ifdef __SAP_POLICY_MANAGEMENT

TPtrC8 CNSmlEmbeddedLinkAdapter::PolicyRequestResourceL( const TDesC8& /*aURI*/ )
{
	
	if(!FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement ))
	{
		User::Leave(KErrNotSupported);
		
	}
		return PolicyEngineXACML::KCustomizationManagement();
	
}
	
//#endif

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
CNSmlEmbeddedLinkAdapter::TNodeId CNSmlEmbeddedLinkAdapter::NodeId( const TDesC8& aURI )
	{
	TNodeId id = ENodeUnknown;

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TInt cnt = NSmlDmURI::NumOfURISegs( uriPtrc ) ;
	TPtrC8 app = NSmlDmURI::LastURISeg( uriPtrc ) ;
#else
	TInt cnt = NSmlDmURI::NumOfURISegs( aURI ) ;
	TPtrC8 app = NSmlDmURI::LastURISeg( aURI ) ;
#endif

	if ( cnt == 1 )
		{
		if (app == KNSmlEmbeddedLinkCustomization)
			{
			id = ENodeCustomization;
			}
		}
	else if ( cnt == 2 ) 
		{
		if (app == KNSmlEmbeddedLinkEmbeddedLinks)
			{
			id = ENodeEmbeddedLinks;
			}
		}
	else if ( cnt == 3 ) 
		{
		id = ENodeEmbeddedLinksX1;
		}
	else if ( cnt == 4 ) 
		{
		if (app == KNSmlEmbeddedLinkLink)
			{
			id = ENodeLinks;
			}
		else if (app == KNSmlEmbeddedLinkID)
			{
			id = ENodeID;
			}
		else if (app == KNSmlEmbeddedLinkAppName)
			{
			id = ENodeAppName;
			}
		}
	else if ( cnt == 5 ) 
		{
		id = ENodeLinksX2;
		}
	else if ( cnt == 6 )
		{
		if (app == KNSmlEmbeddedLinkEmbeddedLinks)
			{
			id = ENodeURL;
			}
		else if (app == KNSmlEmbeddedLinkEmbeddedLinks)
			{
			id = ENodeName;
			}
		else if (app == KNSmlEmbeddedLinkEmbeddedLinks)
			{
			id = ENodeUserName;
			}
		else if (app == KNSmlEmbeddedLinkEmbeddedLinks)
			{
			id = ENodePassword;
			}
		else if (app == KNSmlEmbeddedLinkEmbeddedLinks)
			{
			id = ENodeConRef;
			}
		}

	return id;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlEmbeddedLinkAdapter::RecognizeAppTypeL(const TDesC8& aURI, TDes& aAppName)
// ------------------------------------------------------------------------------------------------
TInt CNSmlEmbeddedLinkAdapter::RecognizeAppTypeL( const TDesC8& aURI, TDes& aAppName )
	{
	HBufC8 *appName = HBufC8::NewLC( 64 );
	
#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	appName->Des().Copy( uriPtrc );
#else
	appName->Des().Copy( aURI );
#endif	

	if ( NSmlDmURI::NumOfURISegs( aURI ) > 2 )
		{
		while( NSmlDmURI::NumOfURISegs( appName->Des() ) > 3 )
			{
			
			appName->Des().Copy( NSmlDmURI::RemoveLastSeg( appName->Des() ) );
			}
		appName->Des().Copy( NSmlDmURI::LastURISeg( appName->Des() ) );
		aAppName.Copy( appName->Des() );
		} 
	else 
		{
		CleanupStack::PopAndDestroy();	//appName
		return KErrNotFound;	
		}
	CleanupStack::PopAndDestroy();	//appName
	return KErrNone;
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] = 
{
	{ {KNSmlEmbeddedLinkAdapterImplUid}, (TProxyNewLPtr)CNSmlEmbeddedLinkAdapter::NewL }
};

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CNSmlEmbeddedLinkAdapter::ConstructL( )
{
	TRAPD( errf, FeatureManager::InitializeLibL() );
	if( errf != KErrNone )
	{
		User::Leave(errf);
	}
	if(FeatureManager::FeatureSupported(KFeatureIdSapEmbeddedLinkAdapter))
	{
			iLinkHandler = CEmbeddedLinkHandler::NewL();
	}
	else
	{
		FeatureManager::UnInitializeLib();
   		User::Leave( KErrNotSupported );
	}
}


// -------------------------------------------------------------------------------------
// CNSmlEmbeddedLinkAdapter::FillNodeInfoL()
// Fills the node info in ddf structure
// -------------------------------------------------------------------------------------
void CNSmlEmbeddedLinkAdapter::FillNodeInfoL( MSmlDmDDFObject& aNode,TSmlDmAccessTypes aAccTypes,
										MSmlDmDDFObject::TOccurence aOccurrence, MSmlDmDDFObject::TScope aScope, 
										MSmlDmDDFObject::TDFFormat aFormat,const TDesC8& aDescription )
{
	aNode.SetAccessTypesL(aAccTypes);
	aNode.SetOccurenceL(aOccurrence);
	aNode.SetScopeL(aScope);
	aNode.SetDFFormatL(aFormat);
	if(aFormat!=MSmlDmDDFObject::ENode)
	{
		aNode.AddDFTypeMimeTypeL(KNSmlEmbeddedLinkAdapterTextPlain);
	}
	aNode.SetDescriptionL(aDescription);
}


