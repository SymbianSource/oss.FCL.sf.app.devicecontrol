/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class containing the no FOTA functionality 
*
*/



// INCLUDE FILES
#include <sysutil.h>
#include <centralrepository.h>
#include <devmaninternalcrkeys.h>

#include "nsmldmsyncprivatecrkeys.h"
#include "nsmldmsyncdocument.h"
#include "nsmldmfotamodel.h"
//#include "nsmldmsyncdebug.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNSmlDMFotaModel* CNSmlDMFotaModel::NewL( CNSmlDMSyncDocument* aDocument )
    {
    CNSmlDMFotaModel* self = new( ELeave ) CNSmlDMFotaModel( aDocument );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CNSmlDMFotaModel::~CNSmlDMFotaModel()
    {
    }


// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::GetProfileName
// Changes aProfileName to KNullDesC if the profile identifier is
// not found.
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaModel::GetProfileNameL( const TInt   aProfileId,
                                              HBufC* aProfileName ) const
    {

    TInt index( 0 );
    aProfileName->Des().Copy( KNullDesC );
    
    iDocument->RefreshProfileListL( ETrue ); // Include hidden profile
    CArrayFixFlat<TNSmlDMProfileItem>* profileList = iDocument->ProfileList( index );
    
    for ( index = 0; index < profileList->Count(); index++ )
        {
        if ( ( *profileList )[index].iProfileId == aProfileId )
            {
            aProfileName->Des().Copy( ( *profileList )[index].iProfileName );
            }
        }

    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::DefaultFotaProfileIdL
// -----------------------------------------------------------------------------
//
TInt CNSmlDMFotaModel::DefaultFotaProfileIdL() const
    {
    
    TInt profileId( KErrNotFound );
    CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidNSmlDMSyncApp ) );

    User::LeaveIfError( err );
    centrep->Get( KNSmlDMDefaultFotaProfileKey, profileId );
    delete centrep;
    
    return profileId;
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::SetDefaultFotaProfileIdL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaModel::SetDefaultFotaProfileIdL( const TInt aProfileId )
    {
    
    CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidNSmlDMSyncApp ) );

    User::LeaveIfError( err );
    centrep->Set( KNSmlDMDefaultFotaProfileKey, aProfileId );
    delete centrep;
    
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::EnableFwUpdRequestL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaModel::EnableFwUpdRequestL( const TInt aProfileId )
    {
    /*
    CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidDeviceManagementInternalKeys ) );
    FTRACE( FPrint( _L(
        "[OMADM] CNSmlDMFotaModel::EnableFwUpdRequestL(), opening cenrep returned %d" ),
        err ) );
    User::LeaveIfError( err );
    centrep->Set( KDevManClientInitiatedFwUpdateId, aProfileId );
    delete centrep;
    */
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::ReadProfileListL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaModel::ReadProfileListL( CDesCArray*          aItems,
                                         CArrayFixFlat<TInt>* aProfileIdList )
    {

    TInt index( 0 );

    iDocument->RefreshProfileListL( ETrue ); // Include hidden profile
    CArrayFixFlat<TNSmlDMProfileItem>* profileList = iDocument->ProfileList( index );

    for ( index = 0; index < profileList->Count(); index++ )
        {
        aItems->AppendL( ( *profileList )[index].iProfileName );
        aProfileIdList->AppendL( ( *profileList )[index].iProfileId );
        }
        
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::SelectDefaultProfileFromList
// -----------------------------------------------------------------------------
//
TInt CNSmlDMFotaModel::SelectDefaultProfileFromList(
    const CArrayFixFlat<TInt>* aProfileIdList ) const
    {

    TInt retval( KErrNotFound );
    TInt defaultId( KErrNotFound );
    
    TRAPD( err, defaultId = iDocument->FotaModel()->DefaultFotaProfileIdL() );
    
    if ( err != KErrNone )
        {
        // We do not care about the possible errors here. It only means
        // that the identifier is not found on the list.
        }
        
    for ( TInt index( 0 ); index < aProfileIdList->Count(); index++ )
        {
        if ( ( *aProfileIdList )[index] == defaultId )
            {
            retval = index;
            break;  // Jump out of the loop
            }
        }
    
    return retval;
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::VerifyProfileL
// -----------------------------------------------------------------------------
//
TBool CNSmlDMFotaModel::VerifyProfileL( const TInt aProfileId ) const
    {

    TInt index( 0 );
    TBool retval( EFalse );

    iDocument->RefreshProfileListL( ETrue ); // Include hidden profile
    CArrayFixFlat<TNSmlDMProfileItem>* profileList = iDocument->ProfileList( index );

    for ( index = 0; index < profileList->Count(); index++ )
        {
        if ( ( *profileList )[index].iProfileId == aProfileId )
            {
            retval = ETrue;
            break;  // Jump out of the loop
            }
        }
        
    return retval;
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::RetrieveFwUpdPkgIdListL
// -----------------------------------------------------------------------------
//
TInt CNSmlDMFotaModel::RetrieveFwUpdPkgIdListL(TFotaState aState)
    {
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::FindFwUpdPkgWithStateL
// -----------------------------------------------------------------------------
//
TInt CNSmlDMFotaModel::FindFwUpdPkgWithStateL(
    TFotaState /*aState*/ )
    {
   
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::InitiateFwUpdInstall
// -----------------------------------------------------------------------------
//
TInt CNSmlDMFotaModel::InitiateFwUpdInstall( TInt /*aPkgId*/, TInt /*aProfileId*/ )
    {
    return KErrGeneral;    
    }
    
// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::ContinueFwUpdInstall
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaModel::ContinueFwUpdInstall( )
	{
	return;
	}

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::CurrentFwVersionString
// If the software version retrieval fails, the aVersionstring is not modified.
// -----------------------------------------------------------------------------
//
TInt CNSmlDMFotaModel::CurrentFwVersionString( TDes& aVersionString )
    {
    TBuf< KSysUtilVersionTextLength > buf;

    TInt err = SysUtil::GetSWVersion( buf );
    if ( err == KErrNone )
        {
        //_LIT( separator, "\n" );
        TInt location = buf.Find( KSmlEOL );
        if ( location != KErrNotFound)
            {
            // Delete the separator and the text after it. We are
            // only interested in the first section.
            buf.Delete( location, (buf.Length() - location) );
            }

        aVersionString.Copy( buf.Left( aVersionString.MaxLength() ) );
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::LastUpdateTime
// -----------------------------------------------------------------------------
//
TInt CNSmlDMFotaModel::LastUpdateTime( TTime& /*aTime*/ )
    {
    return KErrGeneral;
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::MarkFwUpdChangesStartL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaModel::MarkFwUpdChangesStartL()
    {
    return;    
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::FwUpdStatesChangedL
// The array is reseted at the end of the execution to save memory, since at
// the moment the information is not needed multiple times.
// -----------------------------------------------------------------------------
//
TBool CNSmlDMFotaModel::FwUpdStatesChangedL()
    {
    return EFalse;    
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::CNSmlDMFotaModel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNSmlDMFotaModel::CNSmlDMFotaModel( CNSmlDMSyncDocument* aDocument ) :
    iDocument( aDocument )
    {
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaModel::ConstructL()
    {    
    }

TFotaState CNSmlDMFotaModel::GetCurrentFwUpdState()
{
	return EIdle;
}

//Dummy implementation just to avoid compile time errors

RFotaEngineSession::RFotaEngineSession()
{
}

void RFotaEngineSession::ExtensionInterface(TUid /*aInterfaceId*/
                                              ,TAny*& /*aImplementation*/)
    {
    /*RProcess pr; TFullName fn = pr.FullName(); 
    FLOG(_L("RFotaEngineSession::ExtensionInterface called by %S"), &fn);*/
    }
TUid RFotaEngineSession::ServiceUid() const
	{
   /* RProcess pr; TFullName fn = pr.FullName(); 
    FLOG(_L( "RFotaEngineSession::ServiceUid() >> called by: %S" ), &fn );
    FLOG(_L( "RFotaEngineSession::ServiceUid() << ret: 0x%X" ),
                                            KFotaServiceUid );
                                            */
	return TUid::Uid( 0x102072c4 );
	}  	
//End of File  	 
