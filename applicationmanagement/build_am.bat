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

call build_module options %1 %2
rem call build_module installer %1 %2


echo ######################
echo Building client+server
echo ######################


pushd group
call bldmake bldfiles
@call abld export
@call abld makefile %1
@call abld resource %1 %2
@call abld target %1 %2
@call abld freeze %1 
@call abld makefile %1 
@call abld library %1 
@call abld final %1 %2
popd

call build_module amadapter2 %1 %2

echo ##########################
echo Build AM finished
echo ##########################