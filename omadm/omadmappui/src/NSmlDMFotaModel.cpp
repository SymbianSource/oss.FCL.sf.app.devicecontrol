/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class containing the FOTA functionality and communication to
*                other related components
*
*/



// INCLUDE FILES
#include <sysutil.h>
#include <centralrepository.h>
#include <fotaengine.h>
#include <DevManInternalCRKeys.h>
#include "NSmlDMSyncPrivateCRKeys.h"

#include "NSmlDMSyncDocument.h"
#include "NSmlDMFotaModel.h"
#include "NSmlDMSyncUi.hrh"
#include "NSmlDMSyncDebug.h"


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
    FLOG( "[OMADM] CNSmlDMFotaModel::~CNSmlDMFotaModel()" );
    //iFwUpdIdStateList.Close();
    iFotaEngine.Close();
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
    FLOG( "[OMADM] CNSmlDMFotaModel::GetProfileNameL()" );

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

    FTRACE( FPrint( _L(
        "[OMADM] CNSmlDMFotaModel::GetProfileNameL(): aProfileId = %d, ProfileName = \"%S\"" ),
         aProfileId, aProfileName ) );
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::DefaultFotaProfileIdL
// -----------------------------------------------------------------------------
//
TInt CNSmlDMFotaModel::DefaultFotaProfileIdL() const
    {
    FLOG( "[OMADM] CNSmlDMFotaModel::DefaultFotaProfileIdL()" );
    
    TInt profileId( KErrNotFound );
    CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidNSmlDMSyncApp ) );
    FTRACE( FPrint( _L(
        "[OMADM] CNSmlDMFotaModel::DefaultFotaProfileIdL(), opening cenrep returned %d" ),
        err ) );

    User::LeaveIfError( err );
    centrep->Get( KNSmlDMDefaultFotaProfileKey, profileId );
    delete centrep;
    
    FTRACE( FPrint( _L(
        "[OMADM] CNSmlDMFotaModel::DefaultFotaProfileIdL() completed, profileId = %d" ),
        profileId ) );
    return profileId;
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::SetDefaultFotaProfileIdL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaModel::SetDefaultFotaProfileIdL( const TInt aProfileId )
    {
    FLOG( "[OMADM] CNSmlDMFotaModel::SetDefaultFotaProfileIdL()" );
    
    CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidNSmlDMSyncApp ) );
    FTRACE( FPrint( _L(
        "[OMADM] CNSmlDMFotaModel::SetDefaultFotaProfileIdL(), opening cenrep returned %d" ),
        err ) );

    User::LeaveIfError( err );
    centrep->Set( KNSmlDMDefaultFotaProfileKey, aProfileId );
    delete centrep;
    
    FLOG( "[OMADM] CNSmlDMFotaModel::SetDefaultFotaProfileIdL() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::EnableFwUpdRequestL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaModel::EnableFwUpdRequestL( const TInt aProfileId )
    {
    FLOG( "[OMADM] CNSmlDMFotaModel::SetDefaultFotaProfileIdL()" );
    
    CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidDeviceManagementInternalKeys ) );
    FTRACE( FPrint( _L(
        "[OMADM] CNSmlDMFotaModel::EnableFwUpdRequestL(), opening cenrep returned %d" ),
        err ) );
    User::LeaveIfError( err );
    centrep->Set( KDevManClientInitiatedFwUpdateId, aProfileId );
    delete centrep;
    
    FLOG( "[OMADM] CNSmlDMFotaModel::SetDefaultFotaProfileIdL() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::ReadProfileListL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaModel::ReadProfileListL( CDesCArray*          aItems,
                                         CArrayFixFlat<TInt>* aProfileIdList )
    {
    FLOG( "[OMADM] CNSmlDMFotaModel::ReadProfileListL()" );

    TInt index( 0 );

    iDocument->RefreshProfileListL( ETrue ); // Include hidden profile
    CArrayFixFlat<TNSmlDMProfileItem>* profileList = iDocument->ProfileList( index );

    for ( index = 0; index < profileList->Count(); index++ )
        {
        aItems->AppendL( ( *profileList )[index].iProfileName );
        aProfileIdList->AppendL( ( *profileList )[index].iProfileId );
        }
        
    FTRACE( FPrint( _L(
        "[OMADM] CNSmlDMFotaModel::ReadProfileListL() completed, items = %d" ),
        index ) );
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::SelectDefaultProfileFromList
// -----------------------------------------------------------------------------
//
TInt CNSmlDMFotaModel::SelectDefaultProfileFromList(
    const CArrayFixFlat<TInt>* aProfileIdList ) const
    {
    FLOG( "[OMADM] CNSmlDMFotaModel::SelectDefaultProfileFromList()" );

    TInt retval( KErrNotFound );
    TInt defaultId( KErrNotFound );
    
    TRAPD( err, defaultId = iDocument->FotaModel()->DefaultFotaProfileIdL() );
    
    if ( err != KErrNone )
        {
        // We do not care about the possible errors here. It only means
        // that the identifier is not found on the list.
        FTRACE( FPrint( _L(
            "[OMADM] CNSmlDMFotaModel::SelectDefaultProfileFromList() received error %d" ),
            err ) );
        }
        
    for ( TInt index( 0 ); index < aProfileIdList->Count(); index++ )
        {
        if ( ( *aProfileIdList )[index] == defaultId )
            {
            retval = index;
            break;  // Jump out of the loop
            }
        }
    
    FTRACE( FPrint( _L(
        "[OMADM] CNSmlDMFotaModel::SelectDefaultProfileFromList() completed, retval = %d" ),
        retval ) );
    return retval;
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::VerifyProfileL
// -----------------------------------------------------------------------------
//
TBool CNSmlDMFotaModel::VerifyProfileL( const TInt aProfileId ) const
    {
    FTRACE( FPrint( _L(
        "[OMADM] CNSmlDMFotaModel::VerifyProfileL(), aProfileId = %d" ),
        aProfileId ) );

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
        
    FTRACE( FPrint( _L(
        "[OMADM] CNSmlDMFotaModel::VerifyProfileL() completed, return = %d" ),
        retval ) );
    return retval;
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::RetrieveFwUpdPkgIdListL
// -----------------------------------------------------------------------------
//
TInt CNSmlDMFotaModel::RetrieveFwUpdPkgIdListL(TFotaState aState)
    {
    const TInt maxNumOfProfiles = 32;
    
    TBuf16< maxNumOfProfiles > idListDescriptor;
    User::LeaveIfError( iFotaEngine.GetUpdatePackageIds( idListDescriptor ) );
	TInt retval = KErrNotFound;
    RArray< TInt > idList;
    
    TInt count = idListDescriptor.Length();
    for ( TInt i = 0; i < count; i++ )
        {
        idList.Append( idListDescriptor[ i ] );
        }
	TInt count1 = idList.Count();
	for ( TInt i = 0; (i < count1) && (retval == KErrNotFound); i++ )
		{
		RFotaEngineSession::TState pkgState;
		pkgState = iFotaEngine.GetState( idList[ i ] );
		FTRACE( FPrint( _L(
				"[OMADM] CNSmlDMFotaModel::RetrieveFwUpdPkgIdListL(): profileid = %d, pkgstate = %d" ),
				idList[i], (TInt) pkgState ) );
		if ( pkgState == aState )
			{
			retval = idList[ i ];
			}
		}
	FTRACE( FPrint( _L(
	"[OMADM] CNSmlDMFotaModel::RetrieveFwUpdPkgIdListL(): completed, retval = %d" ),
	retval ) );
	idList.Close(); 
	return retval;
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::FindFwUpdPkgWithStateL
// -----------------------------------------------------------------------------
//
TInt CNSmlDMFotaModel::FindFwUpdPkgWithStateL(
    TFotaState aState )
    {
    FTRACE( FPrint( _L(
        "[OMADM] CNSmlDMFotaModel::FindFwUpdPkgWithStateL(): aState = %d" ),
         (TInt) aState ) );
	RFotaEngineSession fotasession;
	fotasession.OpenL();
	CleanupClosePushL(fotasession);
	const TInt x = -1;
	TFotaState res = EIdle;
	res = (TFotaState)fotasession.GetState(x);
	CleanupStack::PopAndDestroy(&fotasession);
	if( res == aState )
		return KErrNone;
	else
		return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::InitiateFwUpdInstall
// -----------------------------------------------------------------------------
//
TInt CNSmlDMFotaModel::InitiateFwUpdInstall( TInt aPkgId, TInt aProfileId )
    {
    FTRACE( FPrint( _L(
        "[OMADM] CNSmlDMFotaModel::InitiateFwUpdInstall(): aPkgId = %d, aProfileId = %d" ),
        aPkgId, aProfileId ) );

    TInt retval = KErrGeneral;
    retval = iFotaEngine.Update( aPkgId, aProfileId, KNullDesC8, KNullDesC8 );

    FTRACE( FPrint( _L(
        "[OMADM] CNSmlDMFotaModel::InitiateFwUpdInstall() completed, retval = %d" ),
        retval ) );
    return retval;
    }
    
// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::ContinueFwUpdInstall
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaModel::ContinueFwUpdInstall( )
    {
    FLOG( "[OMADM] CNSmlDMFotaModel::ContinueFwUpdInstall(): begin" );

    TInt retval = iFotaEngine.TryResumeDownload( );
    
    FTRACE( FPrint( _L(
    		"[OMADM] CNSmlDMFotaModel::ContinueFwUpdInstall() completed, retval = %d") ,retval ));
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::CurrentFwVersionString
// If the software version retrieval fails, the aVersionstring is not modified.
// -----------------------------------------------------------------------------
//
TInt CNSmlDMFotaModel::CurrentFwVersionString( TDes& aVersionString )
    {
    FLOG( "[OMADM] CNSmlDMFotaModel::CurrentFwVersionString()" );
    TBuf< KSysUtilVersionTextLength > buf;

    TInt err = SysUtil::GetSWVersion( buf );
    if ( err == KErrNone )
        {
        _LIT( separator, "\n" );
        TInt location = buf.Find( separator );
        if ( location != KErrNotFound)
            {
            // Delete the separator and the text after it. We are
            // only interested in the first section.
            buf.Delete( location, (buf.Length() - location) );
            }

        aVersionString.Copy( buf.Left( aVersionString.MaxLength() ) );
        }
    FTRACE( FPrint( _L(
        "[OMADM] CNSmlDMFotaModel::CurrentFwVersionString() completed, err = %d, string = \"%S\"" ),
        err, &aVersionString ) );

    return err;
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::LastUpdateTime
// -----------------------------------------------------------------------------
//
TInt CNSmlDMFotaModel::LastUpdateTime( TTime& aTime )
    {
    FLOG( "[OMADM] CNSmlDMFotaModel::LastUpdateTime()" );
    TInt retval = iFotaEngine.LastUpdate( aTime );
    FTRACE( FPrint( _L(
        "[OMADM] CNSmlDMFotaModel::LastUpdateTime() completed, err = %d" ),
        retval ) );
    return retval;
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::MarkFwUpdChangesStartL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaModel::MarkFwUpdChangesStartL()
    {
    FLOG( "[OMADM] CNSmlDMFotaModel::MarkFwUpdChangesStartL()" );
    
    /*iFwUpdIdStateList.Reset();
    RArray< TInt > pkgIdList = RetrieveFwUpdPkgIdListL();

    TInt count = pkgIdList.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        TFwUpdIdState item;
        item.iPkgId = pkgIdList[ i ];
        item.iState = iFotaEngine.GetState( pkgIdList[ i ] );
        iFwUpdIdStateList.Append( item );
        }
    FTRACE( FPrint( _L(
        "[OMADM] CNSmlDMFotaModel::MarkFwUpdChangesStartL() completed, list count = %d" ),
        iFwUpdIdStateList.Count() ) );
    pkgIdList.Close();  */      
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::FwUpdStatesChangedL
// The array is reseted at the end of the execution to save memory, since at
// the moment the information is not needed multiple times.
// -----------------------------------------------------------------------------
//
TBool CNSmlDMFotaModel::FwUpdStatesChangedL()
    {
    FLOG( "[OMADM] CNSmlDMFotaModel::FwUpdStatesChangedL()" );
    
    TBool retval = EFalse;
    /*RArray< TInt > pkgIdList = RetrieveFwUpdPkgIdListL();

    TInt count = pkgIdList.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        FTRACE( FPrint( _L(
            "[OMADM] CNSmlDMFotaModel::FwUpdStatesChangedL() searching pkgid = %d" ),
            pkgIdList[ i ] ) );

        TInt originalCount = iFwUpdIdStateList.Count();
        TBool found = EFalse;
        
        for ( TInt index = 0; (index < originalCount) && (!found);
            index++ )
            {
            if ( iFwUpdIdStateList[ index ].iPkgId == pkgIdList[ i ] )
                {
                FTRACE( FPrint( _L(
                    "[OMADM] CNSmlDMFotaModel::FwUpdStatesChangedL() found pkgid = %d" ),
                    iFwUpdIdStateList[ index ].iPkgId ) );
                FTRACE( FPrint( _L(
                    "[OMADM] CNSmlDMFotaModel::FwUpdStatesChangedL() states are: %d and %d" ),
                    iFotaEngine.GetState( pkgIdList[ i ] ),
                    iFwUpdIdStateList[ index ].iState ) );

                found = ETrue;
                if ( iFwUpdIdStateList[ index ].iState
                    != iFotaEngine.GetState( pkgIdList[ i ] ) )
                    {
                    retval = ETrue;
                    }
                }
            }
        
        if ( !found )
            {
            retval = ETrue;
            }
        }
    iFwUpdIdStateList.Reset();

    FTRACE( FPrint( _L(
        "[OMADM] CNSmlDMFotaModel::FwUpdStatesChangedL() completed, return = %d" ),
        retval ) );
    pkgIdList.Close();*/
    TInt configFlags = EGenericSession;
        //TInt SetGenValue(EGenericSession );
        CRepository* centrep = NULL;
        TRAPD( err, centrep = CRepository::NewL( KCRUidDeviceManagementInternalKeys ) );
        if ( centrep )
            {
            centrep->Get( KDevManSessionType, configFlags );
            
            }
        if ( err != KErrNone ) 
            {
            User::Leave( err );
            }
        else
            {
            centrep->Set( KDevManSessionType, EGenericSession );
            }      
        if(centrep)    
           {
           delete centrep;
           }
        if ( configFlags == EFotaPkgFound )
			{
			retval = ETrue;
			}           
        // configFlags=2 for FOTA Package not downloaded case ,1- successful download
    return retval;
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
    iFotaEngine.OpenL();
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaModel::GetCurrentFwUpdState
// Fetches the state of last or current Fota operation
// -----------------------------------------------------------------------------
//
TFotaState CNSmlDMFotaModel::GetCurrentFwUpdState()
	{
	const TInt x = -1;
	TFotaState res = EIdle;
	res = (TFotaState)iFotaEngine.GetState(x);
	return res;
	}

//  End of File  
