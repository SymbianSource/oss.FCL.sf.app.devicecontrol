/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  wipe plugins debug file for LAWMO component
*
*/

//#define _DEBUG

#ifndef _LAWMO_DEBUG_H_
#define _LAWMO_DEBUG_H_

        #include <e32std.h>
        #include <f32file.h>
        #include <flogger.h>
        #include <e32svr.h>

        _LIT( KLogFile, "pluginslogs.log" );
        _LIT( KLogDirFullName, "c:\\logs\\" );
        _LIT( KLogDir, "lawmo" );
 
#ifdef _DEBUG
        inline void FWrite (TRefByValue<const TDesC> aFmt,...)
            {
            VA_LIST list;
            VA_START( list, aFmt );
            RFileLogger::WriteFormat( KLogDir,KLogFile,EFileLoggingModeAppend ,TPtrC(aFmt) ,list );
            }

            #ifndef __GNUC__          
                #define FLOG            FWrite
                #define FLOG8(x,y)            RFileLogger::WriteFormat( KLogDir,KLogFile,EFileLoggingModeAppend ,_L8(x) ,y );
            #else   // __GNUC__       
                #define FLOG(arg...)    FWrite(arg);
                #define FLOG8(arg...)    RFileLogger::WriteFormat( KLogDir,KLogFile,EFileLoggingModeAppend ,_L8(aFmt) ,list );
            #endif // __GNUC__        

#else
     #define FLOG            
     #define FLOG8(x,y)  
#endif /* __LAWMO_PLUGINS_ */
#endif /*_LAWMO_DEBUG_H_*/
