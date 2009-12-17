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



#ifndef FILE_CODER_B64_H
#define FILE_CODER_B64_H

//  INCLUDES
#include <e32base.h>


// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class TFileCoderB64 // : public ?base_class_list
    {

    public: // New functions
        
        /**
        * Encode data from given file to memory with B64 coding.
        * @since Series 60 3.1
        * @param aFileName file to be encoded
        * @param aData return encoded data object
        * @return void
        */
        TInt EncodeL( const TDesC8& aFileName, CBufFlat &aData ) const;
        TInt EncodeL( const TDesC8& aFileName, HBufC8* &aData ) const;

        /**
        * Decode B64 data to temporary file.
        * @since Series 60 3.1
        * @param aData Data object to be decoded
        * @param aFileName return created temporary file name
        * @return void
        */
        void DecodeL( const TDesC8& aData, TDes& aFileName ) const;

        void DecodeToL( const TDesC8& aData, const TDesC& aFileName ) const;
        
    public: // static function
    
    	static TBool CheckB64Encode( const TDesC8& aData );
        

    };

#endif      // FILE_CODER_B64_H
            
// End of File
