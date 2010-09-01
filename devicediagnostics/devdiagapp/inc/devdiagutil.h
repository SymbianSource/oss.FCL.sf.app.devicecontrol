/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Utility classes.
*
*/


#ifndef DEVDIAGUTIL_H
#define DEVDIAGUTIL_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
* TUtil
* 
* This class contains general utility functions. 
*/
NONSHARABLE_CLASS (TDevDiagUtil)
	{

public:
         
    /**
    * Get data as text.
    * @param aText Contains the transformed TTime in text format.
    * @param aDateTime Time to be converted.   
    **/        
    static void GetDateTextL( TDes& aText, TTime aDateTime );
        
    /**
    * Get time as text. 
    * @param aText Contains the transformed TTime in text format.
    * @param aDateTime Time to be converted.    
    */        
    static void GetTimeTextL( TDes& aText, TTime aDateTime );
        
    /**
    * Get data and time as text.
    * @param aText Contains the transformed TTime in text format.
    * @param aDateTime Time to be converted.    
    */        
	static void GetDateTimeTextL( TDes& aText, TTime aDateTime );
        
    /**
    * Check if sync has happened today.
    * @param aTime  Time to be checked.
    * @return Boolean ETrue if time is today, otherwise EFalse.
    */    
    static TBool IsToday( TTime aTime );
        
private:
       
    /**
    * Convert time to hometime.
    * @param aUniversalTime Time to be transformed.
    * @return TTIme Hometime.
    */            
    static TTime ConvertUniversalToHomeTime( const TTime& aUniversalTime );

	};

#endif  // DEVDIAGUTIL_H

// End of File
