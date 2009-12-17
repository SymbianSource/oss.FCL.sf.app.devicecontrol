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



#ifndef __TESTPARAMS_H
#define __TESTPARAMS_H

//  INCLUDES
#include <e32base.h>
#include <smldmadapter.h>

// FORWARD DECLARATIONS
class CStifLogger;
class CStifItemParser;
class CTestDmCallback;
class CTestDmDDFObject;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class CTestParamsBase : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Destructor.
        */
		virtual ~CTestParamsBase();

    public: // New functions
        
		TInt CheckStatusL( TInt aStatus );
		TInt CheckDataL( CBufBase& aObject );
		TInt CheckTypeL( const TDesC8 &aType );

		TInt CheckL( CTestDmCallback* aCallback );
		TInt CheckAllL( CTestDmCallback* aCallback );

		TInt CheckResultsRefL( TInt resultsRef );
		TInt CheckStatusRefL( TInt statusRef );

    public: // Functions from base classes

		virtual TInt SetParam( TInt xxx ) = 0;	
		
    protected:  // New functions

        /**
        * Symbian 2nd phase constructor
        */
		void ConstructL( CStifItemParser& aItem );

    private:

        /**
        * C++ default constructor.
        */
		CTestParamsBase( CStifLogger *aLog );

    protected:  // Data

		CStifLogger* iLog;
    };

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class CTestParams : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
		static CTestParams* NewL( CStifLogger *aLog, CStifItemParser& aItem );
		static CTestParams* NewLC( CStifLogger *aLog, CStifItemParser& aItem );
        
        /**
        * Destructor.
        */
		virtual ~CTestParams();

    public: // New functions
        
		const TDesC8& Uri();
		const TDesC8& Data();
		const TDesC8& DataType();
		
		void SetResultsRef( TInt resultsRef );
		void SetStatusRef( TInt statusRef );

		TInt CheckStatusL( TInt aStatus );
		TInt CheckDataL( CBufFlat& aObject );
		TInt CheckDataL( const TDesC8& aObject );
		TInt CheckTypeL( const TDesC8 &aType );

		TInt CheckL( CTestDmCallback* aCallback );
		TInt CheckAllL( CTestDmCallback* aCallback );

		TInt CheckResultsRefL( TInt resultsRef );
		TInt CheckStatusRefL( TInt statusRef );

    protected:  // Functions from base classes
        
        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
		CTestParams( CStifLogger *aLog );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
		void ConstructL( CStifItemParser& aItem );

    protected:  // Data

		CStifLogger* iLog;

    private:    // Data

		TPtrC iStr;

		HBufC8* iUri;
		HBufC8* iData;
		HBufC8* iDataType;

		HBufC8* iOutputFile;

		TInt iExpectedStatus;
		HBufC8* iExpectedData;
		HBufC8* iExpectedType;

		TInt iStatusRef;
		TInt iResultsRef;

    };

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class CDDFTestParams : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
		static CDDFTestParams* NewL( CStifLogger *aLog, CStifItemParser& aItem );
		static CDDFTestParams* NewLC( CStifLogger *aLog, CStifItemParser& aItem );
        
        /**
        * Destructor.
        */
		virtual ~CDDFTestParams();

    public: // New functions

		TInt GetAccessTypesL( TSmlDmAccessTypes& aAccessTypes );
		TInt GetDefaultValueL( TPtrC8& aDefaultValue );
		TInt GetDescriptionL( TPtrC8& aDescription );
		TInt GetDFFormatL( MSmlDmDDFObject::TDFFormat& aFormat );
		TInt GetOccurenceL( MSmlDmDDFObject::TOccurence& aOccurence );
		TInt GetScopeL( MSmlDmDDFObject::TScope& aScope );
		TInt GetDFTitleL( TPtrC8& aTitle );
		TInt GetDFTypeMimeTypeL( TPtrC8& aMimeType );

		TInt NodeCount();
		TAny* Node( TInt aIndex );

		const TDesC8& Uri();
		const TDesC8& Data();
		const TDesC8& DataType();

		void SetResultsRef( TInt resultsRef );
		void SetStatusRef( TInt statusRef );

		TInt CheckL( CTestDmDDFObject* aDdfRoot );

    private:

        /**
        * C++ default constructor.
        */
		CDDFTestParams( CStifLogger *aLog );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
		void ConstructL( CStifItemParser& aItem );

    protected:  // Data

		CStifLogger* iLog;

    private:    // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
		HBufC8* iUri;
		TSmlDmAccessTypes iAccessTypes;
		HBufC8* iDefaultValue;
		HBufC8* iDescription;
		MSmlDmDDFObject::TDFFormat iFormat;
		MSmlDmDDFObject::TOccurence iOccurence;
		MSmlDmDDFObject::TScope iScope;
		HBufC8* iTitle;
		HBufC8* iMimeType;
		TInt iObjectGroup;
    };




/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class TTestParamVar
    {
    public:  // Constructors and destructor

        /**
        * Constructors.
        */
		TTestParamVar( const TDesC8 &aStr );

        /**
        * Two-phased constructor.
        */
//		static CTestParams* NewL( CStifLogger *aLog, CStifItemParser& aItem );
//		static CTestParams* NewLC( CStifLogger *aLog, CStifItemParser& aItem );
        
        /**
        * Destructor.
        */
//		virtual ~TTestParamVar();

    public: // New functions
        
        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );
		TBool Next();
		const TDesC8& Name();
		const TDesC8& Value();
		TBool ValueExist();

    public: // Functions from base classes

    protected:  // New functions
        
        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );
		TChar GetChar();
		void UnGetChar();
		TPtrC8 GetWord();

    protected:  // Functions from base classes
        
        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
		TTestParamVar();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
//		void ConstructL( CStifItemParser& aItem );

        // Prohibit copy constructor if not deriving from CBase.
        // CTestParams( const CTestParams& );
        // Prohibit assigment operator if not deriving from CBase.
        // CTestParams& operator=( const CTestParams& );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
		CStifLogger* iLog;
    
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
		TLex8 iLex;
//sf-		TPtrC8 iStr;
//sf-		TInt iPos;
		TPtrC8 iName;
		TPtrC8 iValue;
		TBool iValueExist;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;
    };

#endif      // __TESTPARAMS_H
            
// End of File
