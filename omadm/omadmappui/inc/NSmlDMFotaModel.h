/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of DM UI component
* 	This is part of omadmappui.
*
*/



#ifndef CNSMLDMFOTAMODEL_H
#define CNSMLDMFOTAMODEL_H

//  INCLUDES
#include <fotaengine.h>

#include "NSmlDMFotaView.h"
// FORWARD DECLARATIONS
class CNSmlDMSyncDocument;
class RFotaEngineSession;
// CLASS DECLARATION

/**
*  Functionality required by the fota user interface.
*
*  @lib NSmlDMSyncApp
*  @since Series 60 3.1
*/
class CNSmlDMFotaModel : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CNSmlDMFotaModel* NewL( CNSmlDMSyncDocument* aDocument );
        
        /**
        * Destructor.
        */
        virtual ~CNSmlDMFotaModel();

    public: // New functions
        
        /**
        * Retrieves the human readable name of the profile.
        * @since Series 60 3.1
        * @param aProfileId Profile identifier
        * @param aProfileId Human readable name of the profile, or KNullDesC if not found.
        * @return None
        */
        void GetProfileNameL( const TInt aProfileId,
                                    HBufC* aProfileName ) const;

        /**
        * Retrieves the default fota profile identifier from
        * the central repository.
        * @since Series 60 3.1
        * @param None
        * @return Profile identifier
        */
        TInt DefaultFotaProfileIdL() const;

        /**
        * Saves the default fota profile identifier to the
        * central repository.
        * @since Series 60 3.1
        * @param aProfileId Profile identifier.
        * @return None
        */
        void SetDefaultFotaProfileIdL( const TInt aProfileId );

        /**
        * Sets the central repository key to indicate that the
        * defined profile should be used to request firmware
        * update.
        * @since Series 60 3.1
        * @param aProfileId Profile identifier.
        * @return None
        */
        void EnableFwUpdRequestL( const TInt aProfileId );
        
        /**
        * Retrieves the device management profile names into a list.
        * @since Series 60 3.1
        * @param aItems Array into which the profile names are appended.
        * @return None
        */
        void ReadProfileListL( CDesCArray* aItems,
                               CArrayFixFlat<TInt>* aProfileIdList );

        /**
        * Returns the index of the default fota profile in the
        * given list of identifiers.
        * @since Series 60 3.1
        * @param aProfileIdList List of profile identifiers.
        * @return Index of the default fota profile in the list.
        */
        TInt SelectDefaultProfileFromList(
            const CArrayFixFlat<TInt>* aProfileIdList ) const;

        /**
        * Checks that the profile identifier is included in the
        * devie management profiles list.
        * @since Series 60 3.1
        * @param aProfileIdList List of profile identifiers.
        * @return Index of the default fota profile in the list.
        */
        TBool VerifyProfileL( const TInt aProfileId ) const;
        
        /**
        * Retrieves the list of firmware update packages known to
        * the FOTA server component.
        * @since Series 60 3.1
        * @param None.
        * @return Array that includes the package identifiers.
        */
        TInt RetrieveFwUpdPkgIdListL(TFotaState aState);
        
        /**
        * Checks if the FOTA server component has knowledge of firmware update
        * packages that are in the given state. 
        * @since Series 60 3.1
        * @param aState Package state to be searched for.
        * @return Identifier of the package that has the given state, or
        *         KErrNotFound if none are found.
        */
        TInt FindFwUpdPkgWithStateL( TFotaState aState );

        /**
        * Initiates the instalklation of a firmware update package.
        * @since Series 60 3.1
        * @param aPkgId Identifier of the update package.
        * @return error code.
        */
        TInt InitiateFwUpdInstall( TInt aPkgId, TInt aProfileId );
		
		/**
        * Continues the installation of a firmware update package.
        * @since Series 60 3.2
        * @param None.
        * @return None.
        */
        void ContinueFwUpdInstall( );
        
        /**
        * Returns the version string of the current firmware.
        * @since Series 60 3.1
        * @param aVersionString The version string of the current firmware.
        * @return Error code.
        */
        TInt CurrentFwVersionString( TDes& aVersionString );

        /**
        * Returns the time of the last successful firmware update.
        * @since Series 60 3.1
        * @param aVersionString The time of the last firmware update.
        * @return Error code.
        */
        TInt LastUpdateTime( TTime& aTime );

        /**
        * Saves the current state of update packages.
        * @param None.
        * @return None.
        */
        void MarkFwUpdChangesStartL();

        /**
        * Checks if the state of update packages has changed since the saved
        * situation.
        * @param None.
        * @return True if there are differences in pcakage identifiers or states.
        */
        TBool FwUpdStatesChangedL();
        
       TFotaState GetCurrentFwUpdState();

        
    public: // Functions from base classes

        /**
        * From ?base_class ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );
        
    protected:  // New functions
        
        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes
        
        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CNSmlDMFotaModel( CNSmlDMSyncDocument* aDocument );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
    
        // Pointer to the application document instance.
        CNSmlDMSyncDocument*    iDocument;
        
        // Is the data package download in progress.
        TBool                   iDpDlInProgress;
    
        RFotaEngineSession      iFotaEngine;     
    };

#endif      // CNSMLDMFOTAMODEL_H   
            
// End of File
