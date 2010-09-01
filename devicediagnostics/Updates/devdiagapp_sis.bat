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

@echo off
echo.

:armv5
makesis devdiagapp.pkg devdiagapp.sis
if '%errorlevel%'=='0' goto sign
echo.
echo SIS creation failed.
echo Make sure ARMv5 files and plugin CenRep SIS files were built properly.
goto end

:sign
echo.
echo Signing devdiagapp.sis...
signsis devdiagapp.sis devdiagapp.sisx K:\s60\app\devicecontrol\devicediagnostics\build\s60_rnd\rd_rootca\rd.cer  K:\s60\app\devicecontrol\devicediagnostics\build\s60_rnd\rd_rootca\rd-key.pem
if '%errorlevel%'=='0' goto done
echo SIS signing failed.  Check the certificates.
goto end

:done
echo Created devdiagapp.sisx
#del /Q devdiagapp_Udeb.sis
:end
