rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:
rem

@rd /s /q output



@echo DevDiagApp
@echo off
set CENREP_DIR=%CD%
\epoc32\tools\cenrep\cenrep.pl -o "%CENREP_DIR%\output" -l "%CENREP_DIR%\keys_devdiag.xls" -p 3.2
copy "%CENREP_DIR%\output\inifiles\2000E542.txt" \epoc32\RELEASE\winscw\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\2000E542.txt" \epoc32\RELEASE\winscw\urel\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\2000E542.txt" \epoc32\RELEASE\armv5\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\2000E542.txt" \epoc32\RELEASE\armv5\urel\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\2000E542.txt" \epoc32\data\Z\private\10202be9\
echo.


@echo DiagBrowserPlugin
@echo off
set CENREP_DIR=%CD%
\epoc32\tools\cenrep\cenrep.pl -o "%CENREP_DIR%\output" -l "%CENREP_DIR%\keys_diagbrowserplugin.xls" -p 5.0
copy "%CENREP_DIR%\output\inifiles\2000E59A.txt" \epoc32\RELEASE\winscw\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\2000E59A.txt" \epoc32\RELEASE\winscw\urel\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\2000E59A.txt" \epoc32\RELEASE\armv5\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\2000E59A.txt" \epoc32\RELEASE\armv5\urel\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\2000E59A.txt" \epoc32\data\Z\private\10202be9\
echo.


@echo DiagDataNetworkPlugin
@set XLS_FILE=keys_DiagDataNetworkTestPlugin.xls
@set INI_FILE=2000E598.txt
@set RELEASE=3.2

@set CENREP_DIR=%CD%
@cd \epoc32\tools\cenrep

cenrep.pl -o "%CENREP_DIR%\output" -l "%CENREP_DIR%\%XLS_FILE%" -p %RELEASE%
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\winscw\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\winscw\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\winscw\urel\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\armv5\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\armv5\urel\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\data\Z\private\10202be9\
@cd "%CENREP_DIR%"


@echo DiagEarPiecePlugin
@set XLS_FILE=keys_DiagAudioRecorderPlugin.xls
@set INI_FILE=2000E58C.txt
@set RELEASE=3.2

@set CENREP_DIR=%CD%
@cd \epoc32\tools\cenrep

cenrep.pl -o "%CENREP_DIR%\output" -l "%CENREP_DIR%\%XLS_FILE%" -p %RELEASE%
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\winscw\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\winscw\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\winscw\urel\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\armv5\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\armv5\urel\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\data\Z\private\10202be9\
@cd "%CENREP_DIR%"


@echo DiagLoudspeakerPlugin
@set XLS_FILE=keys_DiagSpeakerTestPlugin.xls
@set INI_FILE=2000E58E.txt
@set RELEASE=3.2

@set CENREP_DIR=%CD%
@cd \epoc32\tools\cenrep

cenrep.pl -o "%CENREP_DIR%\output" -l "%CENREP_DIR%\%XLS_FILE%" -p %RELEASE%
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\winscw\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\winscw\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\winscw\urel\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\armv5\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\armv5\urel\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\data\Z\private\10202be9\
@cd "%CENREP_DIR%"


@echo DiagNetCoveragePlugin
@set XLS_FILE=keys_pdnetworkcoveragetest.xls
@set INI_FILE=2000E596.txt
@set RELEASE=3.2

@set CENREP_DIR=%CD%
@cd \epoc32\tools\cenrep

cenrep.pl -o "%CENREP_DIR%\output" -l "%CENREP_DIR%\%XLS_FILE%" -p %RELEASE%
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\winscw\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\winscw\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\winscw\urel\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\armv5\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\armv5\urel\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\data\Z\private\10202be9\
@cd "%CENREP_DIR%"



@echo DiagVibratePlugin
@set XLS_FILE=keys_devdiagvibratest.xls
@set INI_FILE=2000E586.txt
@set RELEASE=3.2

@set CENREP_DIR=%CD%
@cd \epoc32\tools\cenrep

cenrep.pl -o "%CENREP_DIR%\output" -l "%CENREP_DIR%\%XLS_FILE%" -p %RELEASE%
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\winscw\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\winscw\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\winscw\urel\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\armv5\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\armv5\urel\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\data\Z\private\10202be9\
@cd "%CENREP_DIR%"


@echo DiagFramework
@set XLS_FILE=keys_diagnosticsfw.xls
@set INI_FILE=2000B16B.txt
@set RELEASE=3.2

@set CENREP_DIR=%CD%
@cd \epoc32\tools\cenrep

cenrep.pl -o "%CENREP_DIR%\output" -l "%CENREP_DIR%\%XLS_FILE%" -p %RELEASE%
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\winscw\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\winscw\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\winscw\urel\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\armv5\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\armv5\urel\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\data\Z\private\10202be9\
@cd "%CENREP_DIR%"


@echo ResultsDatabaseServer
@set XLS_FILE=keys_diagresultsdb.xls
@set INI_FILE=2000B45D.txt
@set RELEASE=3.2

@set CENREP_DIR=%CD%
@cd \epoc32\tools\cenrep

cenrep.pl -o "%CENREP_DIR%\output" -l "%CENREP_DIR%\%XLS_FILE%" -p %RELEASE%
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\winscw\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\winscw\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\winscw\urel\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\armv5\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\armv5\urel\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\data\Z\private\10202be9\
@cd "%CENREP_DIR%"

@echo ================= done. Hit any key to continue ==================
@pause