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
rem Description: Implementation of customization components
rem

call clean_adapter DMUtils %1 %2
call clean_adapter UISettingsSrv %1 %2
call clean_adapter EmbeddedLinkAdapter %1 %2
call clean_adapter isadapter %1 %2
call clean_adapter OperatorLogoAdapter %1 %2
call clean_adapter ScreenSaverAdapter %1 %2
call clean_adapter supadapter %1 %2
call clean_adapter ThemesAdapter %1 %2
call clean_adapter WallpaperAdapter %1 %2
