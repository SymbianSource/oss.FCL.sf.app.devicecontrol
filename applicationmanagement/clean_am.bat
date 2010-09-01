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
rem Description: Implementation of applicationmanagement components
rem

@echo off
echo ######################
echo Cleaning client+server
echo ######################
echo 

pushd group
@if exist abld.bat call abld reallyclean %1 %2
@call bldmake clean

rem @if exist ..\BWINS call rmdir /S /Q ..\BWINS
rem @if exist ..\Bmarm call rmdir /S /Q ..\Bmarm
rem @if exist ..\eabi call rmdir /S /Q ..\eabi

popd

call clean_module amadapter2 %1 %2
rem call clean_module installer %1 %2
call clean_module options %1 %2