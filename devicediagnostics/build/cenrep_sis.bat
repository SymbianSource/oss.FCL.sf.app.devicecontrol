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

:cenrep
makesis cenrep_sis.pkg cenrep_sis.SIS
if '%errorlevel%'=='0' goto signcenrep
echo CenRep SIS creation failed.  Make sure the CenRep files exist.
goto end

:signcenrep
echo.
echo Signing cenrep_sis.SIS...
signsis cenrep_sis.SIS cenrep_sis.sisx s60_rnd\rd_rootca\rd.cer s60_rnd\rd_rootca\rd-key.pem
if '%errorlevel%'=='0' goto cenrepdone
echo CenRep SIS signing failed.  Check the certificates.
goto end

:cenrepdone
echo Created diagbrowserplugin_cenrep.sisx
del /Q cenrep_sis.SIS

:end

