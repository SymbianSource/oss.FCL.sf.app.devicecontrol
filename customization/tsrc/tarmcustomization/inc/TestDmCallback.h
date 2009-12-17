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



#ifndef __TESTDMCALLBACK_H
#define __TESTDMCALLBACK_H

//  INCLUDES
#include <smldmadapter.h>
#include <badesca.h>


// FORWARD DECLARATIONS
class CStifLogger;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class CTestDmCallback : public CBase, public MSmlDmCallback
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CTestDmCallback* NewL( CStifLogger *aLog );
        
        /**
        * Destructor.
        */
        virtual ~CTestDmCallback();

    public: // New functions

        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );
        
		TPtrC8 GetLuidL( const TDesC8& aURI );
		TInt FindMappingURI( const TDesC8& aURI );
		TInt GetNewResultsRefL( );
		TInt GetNewStatusRefL( );
		CBufBase& GetResultsObjectL( TInt aResultsRef );
		TPtrC8 GetResultsTypeL( TInt aResultsRef );
		MSmlDmAdapter::TError GetStatusL( TInt aStatusRef );
		void FreeResultsRefsL( );
		void FreeStatusRefsL( );

    public: // Functions from base classes

        	public:
		/**
	The function is used to return the data in case of FetchLeafObjectL(),
	FetchLeafObjectSizeL() and ChildURIListL() functions. It should not be
	called where the DM command has failed, i.e. the error code returned in
	SetStatusL is something other than EOk.
	@param aResultsRef	Reference to correct command
	@param aObject		The data which should be returned
	@param aType			MIME type of the object
	@publishedPartner
	@prototype
	*/
	void SetResultsL( TInt aResultsRef, CBufBase& aObject,
							  const TDesC8& aType );
	
	/**
	The function is used to return the data in case of FetchLeafObjectL() and
	ChildURIListL() functions, where the size of the data being returned is
	large enough for the Adapter to stream it. This function should not be
	called when command was failed, i.e. the error code returned in SetStatusL
	is something other than EOk.
	@param aResultsRef	Reference to correct command
	@param aStream		Large data which should be returned, DM engine
							closes stream when it has read all the data
	@param aType			MIME type of the object
	@publishedPartner
	@prototype
	*/
	void SetResultsL( TInt /*aResultsRef*/, RReadStream*& /*aStream*/,
							  const TDesC8& /*aType*/ ) 
	{
		
	}

	/**
	The function returns information about the Add,Update,Delete and Fetch
	commands success to DM engine. The reference to correct command must be
	used when calling the SetStatusL function, the reference is got from the
	argument of the command functions. The SetStatusL function must be called
	separately for every single command.
	@param aStatusRef	Reference to correct command
	@param aErrorCode	Information about the command success
	@publishedPartner
	@prototype
	*/
	void SetStatusL( TInt aStatusRef,
							 MSmlDmAdapter::TError aErrorCode ) ;

	/**
	The function passes map information to DM Module. This function is called
	for a new management object, both for node objects and for leaf objects.
	In addition if ChildURIListL() function has returned new objects a mapping
	information of the new objects must be passed. A mapping is treated as
	inheritable. If the mapping is not set with this function, the mapping
	LUID of the parent object is passed in following commands to the object.
	@param aURI	URI of the object. 
	@param aLUID	LUID of the object. LUID must contain the all information,
					which is needed for retrieve the invidual object from the
					database. Typically it is ID for the database table. In
					more complicated structures it can be combination of IDs,
					which represent path to the object.
	@publishedPartner
	@prototype
	*/
	void SetMappingL( const TDesC8& aURI, const TDesC8& aLUID );

	/**
	The function is used to make a fetch to other adapters. The most common
	use is to make a fetch to the AP adapter, because when managing the access
	points, the data comes as URI. For example, there are ToNAPId field in
	some adapters, and data to it can be something like AP/IAPidx, and then
	the link to AP adapter is needed.
	Using FetchLinkL causes the DM Framework to make a Get request to the
	appropriate DM adapter.  The receiving adapter MUST complete the Get
	request synchronously.
	@param aURI		URI of the object. 
	@param aData		Reference to data, i.e. data is returned here
	@param aStatus	The status of fetch command is returned here
	@publishedPartner
	@prototype
	*/
	void FetchLinkL( const TDesC8& /*aURI*/, CBufBase& /*aData*/,
							 MSmlDmAdapter::TError& /*aStatus*/ ) 
		{
			
		}

	/**
	The function returns the LUID which is mapped to aURI. If LUID is not
	found, the function allocates a null length string, i.e. the function
	allocates memory in every case.
	@param aURI	URI of the object. 
	@publishedPartner
	@prototype
	*/
	HBufC8* GetLuidAllocL( const TDesC8& aURI ) ;
        
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
        CTestDmCallback( CStifLogger *aLog );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // CTestDmCallback( const CTestDmCallback& );
        // Prohibit assigment operator if not deriving from CBase.
        // CTestDmCallback& operator=( const CTestDmCallback& );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    /**
    * Logger.
    */
    CStifLogger*    iLog;

	/* Containers for URI/LUID mapping */

	CDesC8ArrayFlat iMappingURI;
	CDesC8ArrayFlat iMappingLuid;

	/* Containers for results */
	RPointerArray<CBufBase> iResultObjects;
	CDesC8ArrayFlat iResultTypes;

	/* Container for status */
	RArray<TInt> iStatusArray;
//sf-	RArray<MSmlDmAdapter::TError> iStatusArray;

         
        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;
    };

#endif      // __TESTDMCALLBACK_H
            
// End of File
