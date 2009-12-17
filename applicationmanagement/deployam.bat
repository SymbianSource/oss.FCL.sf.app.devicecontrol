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

@echo This script adds the required centrep files and resets required databases
rem @echo It also empties DM databases!!! Press Ctrl-C if not
rem @pause

@xcopy /Q /Y group\1020781c.txt \epoc32\winscw\c\private\10202be9\persists\
@if exist \epoc32\winscw\c\private\10202be9\persists\1020781c.cre call del \epoc32\winscw\c\private\10202be9\persists\1020781c.cre
@echo removing DM databases...
@del /Q \epoc32\winscw\c\private\087295d3\*.*
@echo removing AppMgmt private data
@del /Q \epoc32\winscw\c\private\1020781c\*.*


@echo Deployment done