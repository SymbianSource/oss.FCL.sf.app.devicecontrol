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



#ifndef THEMESTEST_H
#define THEMESTEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>


// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
// Logging path
_LIT( KThemesTestLogPath, "\\logs\\testframework\\ThemesTest\\" ); 
// Log file
_LIT( KThemesTestLogFile, "ThemesTest.txt" ); 

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CThemesTest;
class CTestDmCallback;
class CTestDmDDFObject;
class CSmlDmAdapter;
class CTestParams;
class CDDFTestParams;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CThemesTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
class CThemesTest : public CScriptBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CThemesTest* NewL( CTestModuleIf& aTestModuleIf );
        
        /**
        * Destructor.
        */
        virtual ~CThemesTest();

    public: // New functions
        
        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );
                
    protected:  // New functions
        
        /**
        * ?member_description.
        * @since ?Series60_version
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
        CThemesTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );
    
        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();
        
        /**
        * Test methods are listed below. 
        */
        
        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt ExampleL( CStifItemParser& aItem );
        
        /**
        * Seconda test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt SecondaL( CStifItemParser& aItem );
        virtual TInt InstallL( CStifItemParser& aItem );

        /**
        * Test methods for DM Adapter.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt LoadL( CStifItemParser& aItem );
		virtual TInt AddDDFCheckL( CStifItemParser& aItem );

		virtual TInt DDFVersionL( CStifItemParser& aItem );
		virtual TInt DDFStructureL( CStifItemParser& aItem );
		
		virtual TInt UpdateLeafObjectL( CStifItemParser& aItem );
		virtual TInt UpdateLeafObjectStreamL( CStifItemParser& aItem );
		virtual TInt DeleteObjectL( CStifItemParser& aItem );
		virtual TInt FetchLeafObjectL( CStifItemParser& aItem );
		virtual TInt FetchLeafObjectSizeL( CStifItemParser& aItem );
		virtual TInt ChildURIListL( CStifItemParser& aItem );
		
		virtual TInt AddNodeObjectL( CStifItemParser& aItem );
		virtual TInt ExecuteCommandL( CStifItemParser& aItem );
		virtual TInt ExecuteCommandStreamL( CStifItemParser& aItem );
		virtual TInt CopyCommandL( CStifItemParser& aItem );
		virtual TInt StartAtomicL( CStifItemParser& aItem );
		virtual TInt CommitAtomicL( CStifItemParser& aItem );
		virtual TInt RollbackAtomicL( CStifItemParser& aItem );
		virtual TInt StreamingSupportL( CStifItemParser& aItem );
		virtual TInt StreamCommittedL( CStifItemParser& aItem );
		virtual TInt CompleteOutstandingCmdsL( CStifItemParser& aItem );
#if 0
		virtual TInt CreateFileL( CStifItemParser& aItem );
		virtual TInt RFSCommandL( CStifItemParser& aItem );
		virtual TInt LoadPolicyL( CStifItemParser& aItem );
		virtual TInt SetCertificateL( CStifItemParser& aItem );
		virtual TInt ConnectToPolicyServerL( CStifItemParser& aItem );
		virtual TInt ConnectToDMUTILL( CStifItemParser& aItem );
		virtual TInt InitDMSessionL( CStifItemParser& aItem );
		virtual TInt ThirdPartyCommandL( CStifItemParser& aItem );
		
    	virtual TInt RRRunL( CStifItemParser& aItem );
#endif
		// Other functions
		void AddParamsL( CTestParams* params );
		void AddDDFParamsL( CDDFTestParams* aDDFParams );
		TInt CheckAllL( );
		TInt CheckDDFL( CTestDmDDFObject *aDDFRoot );
#if 0
        HBufC8* GetThirdPartyValueL( TInt aType );
        void    SetThirdPartyValueL( TInt aType, TDesC8& aData );
#endif
    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
		CTestDmCallback *iCallback;
		CSmlDmAdapter *iAdapter;

		RPointerArray< CTestParams > iParams;
		RPointerArray< CDDFTestParams > iDDFParams;


        // ?one_line_short_description_of_data
        //?data_declaration;
         
        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // THEMESTEST_H
            
// End of File
