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
* Description:  This is the header for the callback interface between the 
*                 plugin class and the engine class
*
*/


#ifndef DIAGDATANETWORKTESTCALLBACKINERFACE_H
#define DIAGDATANETWORKTESTCALLBACKINERFACE_H

#include <DiagResultsDatabaseItem.h> // CDiagResultsDatabaseItem

class MDiagDataNetworkTestCallBackInterface
    {
public:
    /**
    * This function is used by the engine class to report result of the test to the plug-in class
    */  
    virtual void ReportTestResultL(CDiagResultsDatabaseItem::TResult aResult) = 0;    
    
    /**
    * This function is used by the engine class to report progress of the test to the plug-in class
    */          
    virtual void ReportTestProgL(TInt aStep) = 0;   
	virtual TBool DisplayDialog(TInt & aUserResponse) = 0;
    };
    

#endif // DIAGDATANETWORKTESTCALLBACKINERFACE_H