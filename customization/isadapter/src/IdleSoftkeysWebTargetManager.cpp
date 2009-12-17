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


#include "IdleSoftkeysWebTargetManager.h"
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

// #define ISADAPTER_USE_CAPTION_IN_WEBTARGET_NAME


CIdleSoftkeysWebTargetManager::CIdleSoftkeysWebTargetManager( MSmlDmCallback *& aCallBack,
        RFavouritesDb   & aFavDb ) : iCallBack( aCallBack ), iFavDb( aFavDb )        
        {
        	iUiSettingsNotInitialized = ETrue;	
        }
        
CIdleSoftkeysWebTargetManager::~CIdleSoftkeysWebTargetManager()
	{
	if(iUiSettingsNotInitialized == EFalse)
		{
		delete iUiSettings;
		}
	}

        
// -------------------------------------------------------------------------------------
// CIdleSoftkeysWebTargetManager::ListWebTargetsL
// -------------------------------------------------------------------------------------
TInt CIdleSoftkeysWebTargetManager::ListWebTargetsL( CBufFlat &aList )
    {
    RDEBUG( "CIdleSoftkeysWebTargetManager::ListWebTargetsL() >" );

    TInt i, num = 6, offset;
   	if(iUiSettingsNotInitialized)
   	{
   		iUiSettings = CUiSettingsUtil::NewL( );
   		iUiSettings->ReadBookmarksL();
   		iUiSettingsNotInitialized = EFalse;
   	}
	iUiSettings->GetWebTargetsCount( num ) ;
    HBufC8* dataH = HBufC8::NewLC( KRepositoryEntryMaxLength );
    RDEBUG_2( "CIdleSoftkeysWebTargetManager::ListWebTargetsL() HBufC8 dataH ALLOC %x", dataH);
    TPtr8 data( dataH->Des() );

    for(i=0, offset=0; i<num; i++)
        {
        TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> numBuf;
        TInt mappingIndex;
        TInt bmUid;

        //
        // List BM's with valid UID (>= 0)
        //
        // General names for the new bookmarks on the fly
        //
        User::LeaveIfError(iUiSettings->GetWebTargetAppUid( i, bmUid ));
        
        
        if( bmUid >= 0 )
            {
            mappingIndex = -1;
            numBuf.Num( bmUid );
            HBufC8* targetName = NULL;

            //
            // Case 1: There is already URI to LUID mapping, continue with that
            //
            if( (KErrNone == FindURIByLUIDL( data, numBuf, mappingIndex )) && (mappingIndex != -1) )
                {
                //
                // LUid was found in the database
                //
                targetName = GenerateNodeNameL( NSmlDmURI::LastURISeg(data) );
                CleanupStack::PushL(targetName);
                }
#ifdef ISADAPTER_USE_CAPTION_IN_WEBTARGET_NAME
            else
            //
            // Case 2: Bookmark caption is not used in mappings, use that webtarget name
            //
            if( NULL != (targetName = GenerateNodeNameL( i )) )
                {
                //
                // 
                //
                CleanupStack::PushL(targetName);
                UpdateLUIDDatabaseL( *targetName, numBuf, mappingIndex);
                }
#endif
            else
                {
                //
                // Generate unique name
                //
                targetName = GenerateNodeNameL( offset, KPrefixWebTargetURI() );
                CleanupStack::PushL(targetName);

                mappingIndex = -1;
                while( (KErrNone == FindLUIDByURIL( *targetName, data, mappingIndex )) &&
                       (mappingIndex != -1) )
                    {
                    offset ++;
                    mappingIndex = -1;

                    CleanupStack::PopAndDestroy( targetName );
                    targetName = GenerateNodeNameL( offset, KPrefixWebTargetURI() );
                    CleanupStack::PushL(targetName);
                    }

                // We have now unique webtarget node name and unique LUid
                //
                numBuf.Num( bmUid );
                mappingIndex = -1;
                UpdateLUIDDatabaseL( *targetName, numBuf, mappingIndex);
                }

            // Add webtarget node name to list
            //
            if( aList.Size() > 0 )
            {
                aList.InsertL( aList.Size(), KNSmlISSeparator() );
            }
            aList.InsertL( aList.Size(), NSmlDmURI::LastURISeg(*targetName) );
            CleanupStack::PopAndDestroy( targetName );
            }
        }

    CleanupStack::PopAndDestroy( dataH );

    RDEBUG( "CIdleSoftkeysWebTargetManager::ListWebTargetsL() <" );
    return KErrNone;
    }

// -------------------------------------------------------------------------------------
// CIdleSoftkeysWebTargetManager::CheckWebTargetsL
// -------------------------------------------------------------------------------------
void CIdleSoftkeysWebTargetManager::CheckWebTargetsL()
    {
    RDEBUG( "CIdleSoftkeysWebTargetManager::CheckWebTargetsL() ><" );
    }

#ifdef ISADAPTER_USE_CAPTION_IN_WEBTARGET_NAME
// -------------------------------------------------------------------------------------
// CIdleSoftkeysWebTargetManager::GenerateNodeNameL
// -------------------------------------------------------------------------------------
HBufC8* CIdleSoftkeysWebTargetManager::GenerateNodeNameL(TInt aIndex)
    {
    RDEBUG( "CIdleSoftkeysWebTargetManager::GenerateNodeNameL() 1 >" );

    //
    // Get caption of the bookmark
    //
    TBuf<64> caption; 
    if(iUiSettingsNotInitialized)
   	{
   		iUiSettings = CUiSettingsUtil::NewL( );
   		iUiSettings->ReadBookmarksL();
   		iUiSettingsNotInitialized = EFalse;
   	}
   	
    User::LeaveIfError( iUiSettings->GetWebTargetCaption( aIndex, caption ) );
    

    
    if( HasInvalidCharacters( caption ) )
        {
        return NULL;
        }

    //
    // Generate URI with caption
    // Reserve space
    //
    TInt length = caption.Length() + KPrefixWebTargetURI().Length();
    HBufC8* targetName = HBufC8::NewLC( length );
    TPtr8 ptr( targetName->Des() );

    //
    // Generate URI with caption
    // Append strings
    //
    ptr.Append( KPrefixWebTargetURI() );
    ptr.Append( caption );

    HBufC8* dataH = HBufC8::NewLC( KRepositoryEntryMaxLength );
    TPtr8 data( dataH->Des() );
    TInt index = -1;
    TBool okToAdd = (KErrNone != FindLUIDByURIL( ptr, data, index ));
    CleanupStack::PopAndDestroy( dataH );

    if( okToAdd )
        {
        RDEBUG8_2( "CIdleSoftkeysWebTargetManager::GenerateNodeNameL() 1 < %S", &targetName );
        CleanupStack::Pop(targetName);
        return targetName;
        }
    else
        {
        RDEBUG( "CIdleSoftkeysWebTargetManager::GenerateNodeNameL() 1 < NULL, WebTarget name was reserved!" );
        CleanupStack::PopAndDestroy(targetName);
        return NULL;
        }
    }
#endif

// -------------------------------------------------------------------------------------
// CIdleSoftkeysWebTargetManager::GenerateNodeNameL
// -------------------------------------------------------------------------------------
HBufC8* CIdleSoftkeysWebTargetManager::GenerateNodeNameL(TInt aIndex, const TDesC8 &aPrefix)
    {
    RDEBUG( "CIdleSoftkeysWebTargetManager::GenerateNodeNameL() 2 >" );

    TInt length = aPrefix.Length()
                    + KPrefixWebTargetNodeName().Length()
                    + MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64;

    RDEBUG_2( "CIdleSoftkeysWebTargetManager::GenerateNodeNameL() 2 About to ALLOC %d bytes.", length );

    HBufC8* targetName = HBufC8::NewL( length );

    RDEBUG_2( "CIdleSoftkeysWebTargetManager::GenerateNodeNameL() 2 ALLOC HBufC8 targetName %x", targetName );

    TPtr8 ptr( targetName->Des() );

    ptr.Format(KFormatWebTargetNodeName, aIndex+1);
    ptr.Insert(0, aPrefix);

    RDEBUG8_2( "CIdleSoftkeysWebTargetManager::GenerateNodeNameL() 2 < %S", &targetName );
    return targetName;
    }

// -------------------------------------------------------------------------------------
// CIdleSoftkeysWebTargetManager::GenerateNodeNameL
// -------------------------------------------------------------------------------------
HBufC8* CIdleSoftkeysWebTargetManager::GenerateNodeNameL(const TDesC8 &aText)
    {
    RDEBUG( "CIdleSoftkeysWebTargetManager::GenerateNodeNameL() 3 >" );

    TInt length = aText.Length();

    RDEBUG_2( "CIdleSoftkeysWebTargetManager::GenerateNodeNameL() 3 About to ALLOC %d bytes.", length );

    HBufC8* targetName = HBufC8::NewL( length );

    RDEBUG_2("CIdleSoftkeysWebTargetManager::GenerateNodeNameL() 3 ALLOC HBufC8 targetName %x", targetName );

    TPtr8 ptr( targetName->Des() );

    ptr.Copy( aText );

    RDEBUG8_2( "CIdleSoftkeysWebTargetManager::GenerateNodeNameL() 3 < %S", &targetName );
    return targetName;
    }

// -------------------------------------------------------------------------------------
// CIdleSoftkeysWebTargetManager::RenameL
// -------------------------------------------------------------------------------------
TInt CIdleSoftkeysWebTargetManager::RenameL( const TDesC8& aURI, const TDesC8& /*aLUID*/,
                                   const TDesC8& aNewURI )
    {
    RDEBUG( "CIdleSoftkeysWebTargetManager::RenameL() >" );

    HBufC8* dataH = HBufC8::NewLC( KRepositoryEntryMaxLength );
    RDEBUG_2( "CIdleSoftkeysWebTargetManager::RenameL() HBufC8 dataH ALLOC %x", dataH);

    TPtr8 data( dataH->Des() );
    TInt index = -1;
    TInt retVal = KErrNone;

// OPTION 1: URI is already new one, find old according to LUID
#ifdef NOT_DEFINED

    // If we find some URI with that LUID ...
    if( ((KErrNone != FindLUIDByURIL( aURI, data, index ))) &&
         (KErrNone == FindURIByLUIDL( data, aLUID, index )) && (index != -1)  )
    
        {
        // ... we replace that URI with new one
        UpdateLUIDDatabaseL( aURI, aLUID, index );
        }
    else
        {
        retVal = KErrNotFound;
        }

// OPTION 2: New (complete) URI is found in the argument
#else

    TInt index2 = -1;
    if ( (KErrNone != FindLUIDByURIL( aNewURI, data, index )) &&
         (KErrNone == FindLUIDByURIL( aURI, data,  index2 )) 
         )
        {
        UpdateLUIDDatabaseL( aNewURI, data, index2 );
        }
    else
        {
        retVal = KErrNotFound;
        }

#endif

    CleanupStack::PopAndDestroy( dataH );

    RDEBUG_2( "CIdleSoftkeysWebTargetManager::RenameL() < %d", retVal );
    return retVal;
    }

// -------------------------------------------------------------------------------------
// CIdleSoftkeysWebTargetManager::GetTargetFromNodeNameL
// -------------------------------------------------------------------------------------
TInt CIdleSoftkeysWebTargetManager::GetTargetFromNodeNameL(const TDesC8& aURI, TInt &aIndex )
    {
    RDEBUG( "CIdleSoftkeysWebTargetManager::GetTargetFromNodeNameL() >" );
	__UHEAP_MARK;
    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;
    TInt index = -1;
    TInt compareLen = KPrefixWebTargetNodeName().Length();
    TPtrC8 nodeName = NSmlDmURI::URISeg(aURI, 3);

    HBufC8* dataH = HBufC8::NewLC( KRepositoryEntryMaxLength );
    RDEBUG_2( "CIdleSoftkeysWebTargetManager::GetTargetFromNodeNameL() HBufC8 dataH ALLOC %x", dataH);
    TPtr8 data( dataH->Des() );

    if( NSmlDmURI::NumOfURISegs( aURI ) >= 4 )
        {
        TInt bmIndex = -1;
        HBufC8* targetName = NULL;
        if( KErrNone == FindLUIDByURIL( NSmlDmURI::URISeg(aURI, 0, 4), data, bmIndex ) )
            {
            targetName = GenerateNodeNameL( NSmlDmURI::LastURISeg(aURI) );
            }
        else
            {
            status = CSmlDmAdapter::EError;
            }

        if( CSmlDmAdapter::EOk == status )
            {
            CleanupStack::PushL( targetName );
            TPtrC8 ptr( NSmlDmURI::LastURISeg(*targetName) );
            if( ptr == nodeName )
                {
                aIndex = index;
                }
            CleanupStack::PopAndDestroy( targetName );
            }
        }
    else
        {
        status = CSmlDmAdapter::EError;
        }

    CleanupStack::PopAndDestroy( dataH );
	__UHEAP_MARKEND;
    RDEBUG_2( "CIdleSoftkeysWebTargetManager::GetTargetFromNodeNameL() < %d", (TInt)status );
    return status;
    }

// -------------------------------------------------------------------------------------
// CIdleSoftkeysWebTargetManager::GetTargetFromNodeNameL
// -------------------------------------------------------------------------------------
TInt CIdleSoftkeysWebTargetManager::GetTargetFromNodeNameL(const TDesC8& aURI, CFavouritesItem &aItem, TInt &aIndex )
    {
    RDEBUG( "CIdleSoftkeysWebTargetManager::GetTargetFromNodeNameL() >" );

    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;
    TInt bmIndex = -1;
    TInt bmUid = -1;
    TInt compareLen = KPrefixWebTargetNodeName().Length();
    TPtrC8 nodeName = NSmlDmURI::URISeg(aURI, 3);

    HBufC8* dataH = HBufC8::NewLC( KRepositoryEntryMaxLength );
    RDEBUG_2( "CIdleSoftkeysWebTargetManager::GetTargetFromNodeNameL() HBufC8 dataH ALLOC %x", dataH);
    TPtr8 data( dataH->Des() );

    if( NSmlDmURI::NumOfURISegs( aURI ) >= 4 )
        {
        TInt repIndex = -1;
        HBufC8* targetName = NULL;
        if( KErrNone == FindLUIDByURIL( NSmlDmURI::URISeg(aURI, 0, 4), data, repIndex ) )
            {
            targetName = GenerateNodeNameL( nodeName );
            CleanupStack::PushL( targetName );
            TPtrC8 ptr( NSmlDmURI::LastURISeg(*targetName) );
            User::LeaveIfError( ptr == nodeName ? KErrNone : KErrGeneral );
            CleanupStack::PopAndDestroy( targetName );

            TLex8 lex;
            lex.Assign(data);
            User::LeaveIfError( lex.Val( bmUid ) );
            TInt ret = KErrNone;
           	if(iUiSettingsNotInitialized)
   			{	
   				iUiSettings = CUiSettingsUtil::NewL( );
   				iUiSettings->ReadBookmarksL();
   				iUiSettingsNotInitialized = EFalse;
   			}
           	User::LeaveIfError(iUiSettings->GetWebTargetIndex(bmUid,bmIndex));
            if( -1 == bmIndex )
                {
                ret = KErrNotFound;
                }
            if( KErrNotFound == ret )
                {
                User::LeaveIfError(iUiSettings->GetWebTargetIndex(bmUid, bmIndex));                
                }
            else
                {
                User::LeaveIfError( ret );
                }
            }
        else
            {
            status = CSmlDmAdapter::EError;
            }

        if( CSmlDmAdapter::EOk == status )
            {
            if( KErrNone == iFavDb.Get( bmUid, aItem ) )
                {
                aIndex = bmIndex;
                }
            else
                {
                status = CSmlDmAdapter::EError;
                }
            }
        }
    else
        {
        status = CSmlDmAdapter::EError;
        }

    CleanupStack::PopAndDestroy( dataH );

    RDEBUG_2( "CIdleSoftkeysWebTargetManager::GetTargetFromNodeNameL() < %d", (TInt)status );
    return status;
    }

// -------------------------------------------------------------------------------------
// CIdleSoftkeysWebTargetManager::PrepareItemL
// -------------------------------------------------------------------------------------
void CIdleSoftkeysWebTargetManager::PrepareItemL( const TDesC8& aURI, CFavouritesItem &aItem, TInt &aIndex )
    {
    RDEBUG( "CIdleSoftkeysWebTargetManager::PrepareItemL() >" );
	CheckWebTargetsL();
    TInt error = GetTargetFromNodeNameL( aURI, aItem, aIndex );
    if( KErrNone != error )
        {
        RDEBUG_2( "CIdleSoftkeysWebTargetManager::PrepareItemL TError %d", error );
        User::Leave( KErrNotFound );
        }

    RDEBUG( "CIdleSoftkeysWebTargetManager::PrepareItemL() <" );
    }

// -------------------------------------------------------------------------------------
// CIdleSoftkeysWebTargetManager::ReleaseItemL
// -------------------------------------------------------------------------------------
void CIdleSoftkeysWebTargetManager::ReleaseItemL( CFavouritesItem &aItem, TBool aUpdate/*=ETrue*/ )
    {
    if( aUpdate )
        {
        User::LeaveIfError( iFavDb.Update( aItem, aItem.Uid(), ETrue ) );
        }
    }

// -------------------------------------------------------------------------------------
// CIdleSoftkeysWebTargetManager::SetCaptionL
// -------------------------------------------------------------------------------------
void CIdleSoftkeysWebTargetManager::SetCaptionL( const TDesC8& aURI, const TDesC8& aCaption )
    {
    RDEBUG( "CIdleSoftkeysWebTargetManager::SetCaptionL() >" );

    TInt index;

    CFavouritesItem *favItem = CFavouritesItem::NewLC();
    RDEBUG_2( "CIdleSoftkeysWebTargetManager::SetCaptionL() CFavouritesItem ALLOC %x", favItem);

    HBufC* newCaption = CTARMCharConv::ConvertFromUtf8LC( aCaption );

    PrepareItemL( aURI, *favItem, index );
    favItem->SetNameL( *newCaption );
    if( favItem->Name().Compare( *newCaption ) != 0 )
        {
        User::Leave( KErrGeneral );
        }
    ReleaseItemL( *favItem );

    CleanupStack::PopAndDestroy( newCaption );
    CleanupStack::PopAndDestroy( favItem );

    RDEBUG( "CIdleSoftkeysWebTargetManager::SetCaptionL() <" );
    }

// -------------------------------------------------------------------------------------
// CIdleSoftkeysWebTargetManager::FindWapApL
// -------------------------------------------------------------------------------------
void CIdleSoftkeysWebTargetManager::FindWapApL( const TDesC8& aURI, TInt &aWapAp )
    {
    TPtrC8  apURI     = NSmlDmURI::RemoveDotSlash( aURI );
    TInt    numOfSegs = NSmlDmURI::NumOfURISegs( apURI );
    TPtrC8I seg1      = NSmlDmURI::URISeg( apURI, 0 );
    TInt    value     = 0;

    if( (numOfSegs == 2) && (seg1.Compare( _L8("AP") ) == 0) )
        {
        HBufC8* buf = iCallBack->GetLuidAllocL( apURI );
        CleanupStack::PushL( buf );
        if( buf->Des().Length() == 0 )
            {
            CleanupStack::PopAndDestroy( buf );
            buf = iCallBack->GetLuidAllocL( aURI );
            CleanupStack::PushL( buf );
            if( buf->Des().Length() == 0 )
                {
                CleanupStack::PopAndDestroy( buf );
                User::Leave( KErrNotFound );
                }
            }

        TLex8 lex;
        lex.Assign( buf->Des() );
        User::LeaveIfError( lex.Val( value ) );
        CleanupStack::PopAndDestroy( buf );
        }
    else
        {
        User::Leave( KErrArgument );
        }

    aWapAp = value;
    }


// -------------------------------------------------------------------------------------
// CIdleSoftkeysWebTargetManager::UpdateLUIDDatabaseL
// -------------------------------------------------------------------------------------
TInt CIdleSoftkeysWebTargetManager::UpdateLUIDDatabaseL(const TDesC8& aURI, const TDesC8& aLUID, TInt &aIndex)
    {
    RDEBUG( "CIdleSoftkeysWebTargetManager::UpdateLUIDDatabaseL() >" );

    TInt ret = KErrGeneral;

    CRepository* rep = CRepository::NewLC( KNSmlDMISAdapterCentRepUid );
    RDEBUG_2( "CIdleSoftkeysWebTargetManager::UpdateLUIDDatabaseL() CRepository ALLOC %x", rep);

    HBufC8* data = HBufC8::NewLC( aURI.Length() + aLUID.Length() + 1 );
    RDEBUG_2( "CIdleSoftkeysWebTargetManager::UpdateLUIDDatabaseL() HBufC8 ALLOC data %x", data);
    TPtr8 dataPtr( data->Des() );

    dataPtr.Copy(aURI);
    dataPtr.Append(TChar(' '));
    dataPtr.Append(aLUID);

    TInt count = 0;

    // Get / create the count
    //
    ret = rep->Get((TUint32)KRepositoryIdBookmarkCount, count);
    if( ret == KErrNotFound )
        {
        ret = rep->Create((TUint32)KRepositoryIdBookmarkCount, count);
        }

    if( KErrNone == ret )
        {
        // Replace old or ...
        //
        if( aIndex != -1 )
            {
            ret = rep->Set(aIndex, dataPtr );
            }
        else
        // ... Create new index
        //
            {
            TUint32 newIndex = (TUint32)KRepositoryIdBookmarkFirst + count;

            ret = rep->Create(newIndex, dataPtr);
            if(KErrAlreadyExists == ret)
                {
                ret = rep->Set(newIndex, dataPtr);
                }
            
            if( KErrNone == ret )
                {
                HBufC8* check = HBufC8::NewLC( dataPtr.Length() );
                RDEBUG_2( "CIdleSoftkeysWebTargetManager::UpdateLUIDDatabaseL() HBufC8 ALLOC check %x", check);
                TPtr8 checkPtr( check->Des() );

                User::LeaveIfError( rep->Get(newIndex, checkPtr ) );
                User::LeaveIfError( (dataPtr.Compare( checkPtr )==0)?KErrNone:KErrGeneral );
                aIndex = newIndex;

                CleanupStack::PopAndDestroy( check );

                rep->Set((TUint32)KRepositoryIdBookmarkCount, count+1 );
                }
            }
        }

    CleanupStack::PopAndDestroy( data );
    CleanupStack::PopAndDestroy( rep );

    RDEBUG_2( "CIdleSoftkeysWebTargetManager::UpdateLUIDDatabaseL() returns %d<", ret );
    if(ret == KErrNone && !iUiSettingsNotInitialized)
   	{
   		iUiSettings->ReadBookmarksL();   		
   	}
    return ret;
    }

// -------------------------------------------------------------------------------------
// CIdleSoftkeysWebTargetManager::CheckLUIDDatabaseL
// -------------------------------------------------------------------------------------
TInt CIdleSoftkeysWebTargetManager::CheckLUIDDatabaseL( TBool aCleanDatabase/*=ETrue*/ )
    {
    RDEBUG( "CIdleSoftkeysWebTargetManager::CheckLUIDDatabaseL() >" );

    TInt i;
    TInt j;
    TInt ret = KErrNone;
    TUint32 repIndex, repIndex2;
    TInt strIndex;
    TLex8 lex;
	
	if(iUiSettingsNotInitialized)
   	{
   		iUiSettings = CUiSettingsUtil::NewL( );
		iUiSettings->ReadBookmarksL();   		
		iUiSettingsNotInitialized = EFalse;
   	}
   	else
   	{
   		iUiSettings->ReadBookmarksL();
   	}
	
    CRepository* rep = CRepository::NewLC( KNSmlDMISAdapterCentRepUid );
    RDEBUG_2( "CIdleSoftkeysWebTargetManager::CheckLUIDDatabaseL() CRepository ALLOC %x", rep);

    HBufC8* repDataH = HBufC8::NewLC( KRepositoryEntryMaxLength );
    RDEBUG_2("CIdleSoftkeysWebTargetManager::CheckLUIDDatabaseL() HBufC8 repDataH ALLOC %x", repDataH);
    TPtr8 repData( repDataH->Des() );

    HBufC8* repData2H = HBufC8::NewLC( KRepositoryEntryMaxLength );
    RDEBUG_2( "CIdleSoftkeysWebTargetManager::CheckLUIDDatabaseL() HBufC8 repData2H ALLOC %x", repData2H);
    TPtr8 repData2( repData2H->Des() );

    TInt count = 0;
    ret = rep->Get((TUint32)KRepositoryIdBookmarkCount, count);

    if( KErrNone == ret )
        {
        // DEBUG TEST S
#ifdef __WINS__
        for( i=0; i<count; i++ )
            {
            repIndex = (TUint32)i + KRepositoryIdBookmarkFirst;
            ret = rep->Get(repIndex, repData);
            if( KErrNone != ret )
                {
                User::Panic(_L("IS_DEBUG"), 1);
                }

            strIndex = repData.Find( _L8(" ") );
            if( strIndex < 1 )
                {
                User::Panic(_L("IS_DEBUG"), 2);
                }

            TInt theLuid, bmIndex = -1;
            lex.Assign( repData );
            lex.Inc( strIndex );

            TChar ch = lex.Get( );
            if( ch != TChar(' ') )
                {
                User::Panic(_L("IS_DEBUG"), 3);
                }
            if( KErrNone != lex.Val( theLuid ) )
                {
                User::Panic(_L("IS_DEBUG"), 4);
                }
            if( theLuid < 0 )
                {
                User::Panic(_L("IS_DEBUG"), 5);
                }
            
            if( (KErrNone != iUiSettings->GetWebTargetIndex(theLuid, bmIndex) /*iUiSrv.GetShortcutTargetIndex( EUiSrvRtTypeBm, theLuid, bmIndex )*/ ) ||
                (bmIndex == -1) )
                {
                RDEBUG( "CIdleSoftkeysWebTargetManager::CheckLUIDDatabaseL(): WARNING Non-existent BM-UID in IsAdapter!" );
                }
            
            if( (NSmlDmURI::NumOfURISegs( repData.Left(strIndex) ) != 4) ||
                (NSmlDmURI::URISeg( repData, 0 ) != KNSmlCustomizationNodeName()) ||
                (NSmlDmURI::URISeg( repData, 1 ) != KNSmlIS_IdleSoftKeyNodeName()) ||
                (NSmlDmURI::URISeg( repData, 2 ) != KNSmlIS_WebTargetsNodeName())
                )
                {
                User::Panic(_L("IS_DEBUG"), 6);
                }
            }

        for( i=0; i<count-1; i++ )
            {
            repIndex = (TUint32)i + KRepositoryIdBookmarkFirst;
            rep->Get(repIndex, repData);
            strIndex = repData.Find( _L8(" ") );
            lex.Assign( repData );
            lex.Inc( strIndex ); lex.Inc( 1 );
            TInt theLuid;
            lex.Val( theLuid );
            for( j=i+1; j<count; j++ )
                {
                TInt strIndex2;
                repIndex2 = (TUint32)j + KRepositoryIdBookmarkFirst;
                rep->Get(repIndex2, repData2);
                strIndex2 = repData2.Find( _L8(" ") );
                lex.Assign( repData2 );
                lex.Inc( strIndex2 ); lex.Inc( 1 );
                TInt theLuid2;
                lex.Val( theLuid2 );

                if( repData.Left( strIndex+1 ).Compare( repData2.Left( strIndex+1 ) ) == 0 )
                    {
                    User::Panic(_L("IS_DEBUG"), 7);
                    }
                if( theLuid == theLuid2 )
                    {
                    User::Panic(_L("IS_DEBUG"), 8);
                    }
                }
            }
#endif
        // DEBUG TEST E

        for( i=0; i<count; i++ )
            {
            repIndex = (TUint32)i + KRepositoryIdBookmarkFirst;

            ret = rep->Get(repIndex, repData);
            if( KErrNone == ret )
                {
                    strIndex = repData.Find( _L8(" ") );
                    if( strIndex > 0 )
                        {
                        TInt theLuid, bmIndex = -1;
                        lex.Assign( repData );
                        lex.Inc( strIndex+1 );
                        if( KErrNone == lex.Val( theLuid ) )
                            {
                            if( theLuid < 0 )
                                {
                                rep->Set( repIndex, KNullDesC() );
                                }
                            
                            if( (KErrNone != iUiSettings->GetWebTargetIndex(theLuid, bmIndex) /*iUiSrv.GetShortcutTargetIndex( EUiSrvRtTypeBm, theLuid, bmIndex )*/ ) ||
                                (bmIndex == -1) )
                                {
                                TBool exists = EFalse;
                                iFavDb.ItemExists( theLuid, exists );
                                if( !exists )
                                    {
                                    if( aCleanDatabase )
                                        {
                                        rep->Set( repIndex, KNullDesC() );
                                        RDEBUG( "CIdleSoftkeysWebTargetManager::CheckLUIDDatabaseL(): WARNING Removing invalid WebTarget LUID mapping" );
                                        }
                                    else
                                        {
                                        RDEBUG( "CIdleSoftkeysWebTargetManager::CheckLUIDDatabaseL(): WARNING Invalid WebTarget LUID" );
                                        }
                                    }
                                }
                                
                            }
                        if( (NSmlDmURI::NumOfURISegs( repData.Left(strIndex) ) != 4) ||
                            (NSmlDmURI::URISeg( repData, 0 ).Compare( KNSmlCustomizationNodeName() ) != 0) ||
                            (NSmlDmURI::URISeg( repData, 1 ).Compare( KNSmlIS_IdleSoftKeyNodeName() ) != 0) ||
                            (NSmlDmURI::URISeg( repData, 2 ).Compare( KNSmlIS_WebTargetsNodeName() ) != 0)
                            )
                            {
                            rep->Set( repIndex, KNullDesC() );
                            RDEBUG( "CIdleSoftkeysWebTargetManager::CheckLUIDDatabaseL(): WARNING Invalid WebTarget URI" );
                            }
                        }
                }
            }
        }

    // Get rid of empty entries and update count
    count = 0;
    ret = rep->Get((TUint32)KRepositoryIdBookmarkCount, count);
    TInt oldcount = count;
    TInt newcount = count;
	RArray<TInt32> remList;
	CleanupClosePushL(remList);
    if( KErrNone == ret )
        {
        for( i=0; i<count; i++ )
            {
            repIndex = (TUint32)i + KRepositoryIdBookmarkFirst;

            ret = rep->Get(repIndex, repData);
            if( KErrNone == ret )
                {
                if( repData.Length() == 0 )
                    {
                    for( j=count; j>i+1; j-- )
                        {
                        repIndex2 = (TUint32)j-1 + KRepositoryIdBookmarkFirst;
                        ret = rep->Get(repIndex2, repData2);
                        if( (KErrNone == ret) && (repData2.Length() > 0) )
                            {
                            repData.Copy(repData2);
                            repData2.Zero();
                            ret = rep->Set(repIndex, repData);
                            ret = rep->Set(repIndex2, repData2);
                            newcount --;
                            remList.Append(j);
                            break;
                            }
                        }
                    if( j == i+1 && remList.Find(j) == KErrNotFound)
                        {
                        // Did not find a replacement, just delete the entry
                        newcount --;
                        }
                    }
                }
            }
        ret = rep->Set((TUint32)KRepositoryIdBookmarkCount, newcount);
        
        for(i=newcount; i<oldcount; i++)
            {
            repIndex = (TUint32)i + KRepositoryIdBookmarkFirst;
            ret = rep->Delete( repIndex );
            }

        for(i=0; i<newcount; i++)
            {
            TUint32 repIndex = (TUint32)i + KRepositoryIdBookmarkFirst;
            ret = rep->Get( repIndex, repData );
            if( (KErrNone != ret) || (repData.Length() == 0) )
                {
                count = i;
                ret = rep->Set((TUint32)KRepositoryIdBookmarkCount, count);
                break;
                }
            }
        }
    CleanupStack::PopAndDestroy(&remList);
    CleanupStack::PopAndDestroy( repData2H );
    CleanupStack::PopAndDestroy( repDataH );
    CleanupStack::PopAndDestroy( rep );

    RDEBUG_2( "CIdleSoftkeysWebTargetManager::CheckLUIDDatabaseL() returns %d<", ret );
    //iUiSettings->ReadBookmarksL();
    return ret;
    }

// -------------------------------------------------------------------------------------
// CIdleSoftkeysWebTargetManager::FindLUIDByURIL
// -------------------------------------------------------------------------------------
TInt CIdleSoftkeysWebTargetManager::FindLUIDByURIL(const TDesC8& aURI, TDes8& aLUID, TInt &aIndex)
    {
    RDEBUG8_2( "CIdleSoftkeysWebTargetManager::FindLUIDByURIL() > URI=%S", &aURI );

    TInt ret = KErrGeneral;

    CRepository* rep = CRepository::NewLC( KNSmlDMISAdapterCentRepUid );
    RDEBUG_2( "CIdleSoftkeysWebTargetManager::FindLUIDByURIL() CRepository ALLOC %x", rep);

    HBufC8* repDataH = HBufC8::NewLC( KRepositoryEntryMaxLength );
    RDEBUG_2("CIdleSoftkeysWebTargetManager::FindLUIDByURIL() HBufC8 repDataH ALLOC %x", repDataH);
    TPtr8 repData( repDataH->Des() );

    TInt count;

    ret = rep->Get((TUint32)KRepositoryIdBookmarkCount, count);
    if( KErrNone == ret )
        {
        RDEBUG("CIdleSoftkeysWebTargetManager::FindLUIDByURIL() rep->Get((TUint32)KRepositoryIdBookmarkCount");
        TInt i;
        for( i=0; i<count; i++ )
            {
            TUint32 repIndex = (TUint32)i + KRepositoryIdBookmarkFirst;

            ret = rep->Get(repIndex, repData);
            if( KErrNone == ret )
                {
                RDEBUG_2( "CIdleSoftkeysWebTargetManager::FindLUIDByURIL() rep->Get(repIndex, repData), repIndex=%d", repIndex);
                RDEBUG8_2( "CIdleSoftkeysWebTargetManager::FindLUIDByURIL() rep->Get(repIndex, repData), repData=%S", &repData);
                if(repData.Length() > aURI.Length())
                    {
                    if( repData.Left( aURI.Length() ).Compare( aURI ) == 0 )
                        {
                        RDEBUG( "CIdleSoftkeysWebTargetManager::FindLUIDByURIL(), lexical analysis starts" );
                        TLex8 lex;
                        lex.Assign( repData );
                        lex.Inc( aURI.Length() );
                        TChar ch = lex.Get();
                        RDEBUG( "CIdleSoftkeysWebTargetManager::FindLUIDByURIL(), lexical analysis more" );
                        if( ch == TChar(' ') )
                            {
                            aLUID.Copy( lex.Remainder() );
                            aIndex = repIndex;
                            ret = KErrNone;
                            RDEBUG( "CIdleSoftkeysWebTargetManager::FindLUIDByURIL(), lexical analysis OK." );
                            break;
                            }
                        }
                    }
                }
            }

        if( i==count )
            {
            ret = KErrNotFound;
            }
        }

    CleanupStack::PopAndDestroy( repDataH );
    CleanupStack::PopAndDestroy( rep );

    RDEBUG_2( "CIdleSoftkeysWebTargetManager::FindLUIDByURIL() < %d", ret );
    return ret;
    }

// -------------------------------------------------------------------------------------
// CIdleSoftkeysWebTargetManager::FindURIByLUIDL
// -------------------------------------------------------------------------------------
TInt CIdleSoftkeysWebTargetManager::FindURIByLUIDL(TDes8& aURI, const TDesC8& aLUID, TInt &aIndex)
    {
    RDEBUG( "CIdleSoftkeysWebTargetManager::FindURIByLUIDL() >" );

    TInt ret = KErrGeneral;

    CRepository* rep = CRepository::NewLC( KNSmlDMISAdapterCentRepUid );
    RDEBUG_2( "CIdleSoftkeysWebTargetManager::FindURIByLUIDL() CRepository ALLOC %x", rep);

    HBufC8* repDataH = HBufC8::NewLC( KRepositoryEntryMaxLength );
    RDEBUG_2( "CIdleSoftkeysWebTargetManager::FindURIByLUIDL() HBufC8 repDataH ALLOC %x", repDataH);
    TPtr8 repData( repDataH->Des() );

    HBufC8* verifyDataH = HBufC8::NewLC( KRepositoryEntryMaxLength );
    RDEBUG_2( "CIdleSoftkeysWebTargetManager::FindURIByLUIDL() HBufC8 verifyDataH ALLOC %x", verifyDataH);
    TPtr8 verifyData( verifyDataH->Des() );

    TInt count;

    ret = rep->Get((TUint32)KRepositoryIdBookmarkCount, count);
    if( KErrNone == ret )
        {
        TInt i;
        for( i=0; i<count; i++ )
            {
            TUint32 repIndex = (TUint32)i + KRepositoryIdBookmarkFirst;

            ret = rep->Get(repIndex, repData);
            if( KErrNone == ret )
                {
                if(repData.Length() > aLUID.Length())
                    {
                    TInt strIndex = repData.Find( aLUID );

                    if( strIndex > 0 )
                        {
                        verifyData.Copy( repData.Left( strIndex ) );
                        verifyData.Append( aLUID );

                        if( verifyData.Compare( repData ) == 0 )
                            {
                            aURI.Copy( repData.Left( strIndex - 1 ) );
                            aIndex = repIndex;
                            ret = KErrNone;
                            break;
                            }
                        }
                    }
                }
            }
        if( i==count )
            {
            ret = KErrNotFound;
            }
        }

    CleanupStack::PopAndDestroy( verifyDataH );
    CleanupStack::PopAndDestroy( repDataH );
    CleanupStack::PopAndDestroy( rep );

    RDEBUG_2( "CIdleSoftkeysWebTargetManager::FindURIByLUIDL() < %d", ret );
    return ret;
    }

// End of file
