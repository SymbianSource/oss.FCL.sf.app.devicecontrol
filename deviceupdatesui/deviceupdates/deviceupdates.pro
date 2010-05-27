#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: Device updates application project file
#
symbian: {
SYMBIAN_PLATFORMS = WINSCW \
    ARMV5

:BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>"
    :BLD_INF_RULES.prj_exports += "./rom/deviceupdates.iby CORE_APP_LAYER_IBY_EXPORT_PATH(deviceupdates.iby)"
    :BLD_INF_RULES.prj_exports += "./rom/deviceupdates_resources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(deviceupdates_resources.iby)"
    :BLD_INF_RULES.prj_exports += "./conf/101F6DE5.txt /epoc32/data/z/private/10202be9/101F6DE5.txt"    
    :BLD_INF_RULES.prj_exports += "./conf/101F6DE5.txt /epoc32/winscw/c/private/10202be9/101F6DE5.txt"  
TARGET.UID2 = 0x100039CE
TARGET.UID3 = 0x101F6DE5
TARGET.CAPABILITY = CAP_APPLICATION 
 LIBS +=  -lCommonEngine\
	-lcentralrepository\ 
	-lfeatmgr\ 
	-lsyncmlclientapi\ 
	-lnsmltransporthandler\
	-lnsmltransport\
-lnsmlhistorylog\
-lpolicyengine\
-lsysutil \
        -letel \
        -letelmm \
        -lsysversioninfo\
        -letel3rdparty\
		-letelpckt\
		-lcmmanager\
		-lsisregistryclient
	}
TEMPLATE = app
TARGET = deviceupdates
DEPENDPATH += ./inc
INCLUDEPATH += .
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
# Input
HEADERS += ./inc/dminforetrieval.h \
./inc/nsmldmdef.h\
./inc/nsmldmsyncappengine.h \
./inc/nsmldmsyncdebug.h \
./inc/nsmldmsynchandler.h\
./inc/nsmldmsyncinternalpskeys.h \
./inc/nsmldmsyncprivatecrkeys.h \
./inc/nsmldmsyncprofile.h \
./inc/nsmldmsyncprofileitem.h \
./inc/nsmldmsyncprofilelist.h \
./inc/nsmldmsyncutil.h \
./inc/nsmldmuidefines.h \
./inc/serversettingsview.h\
./inc/dmadvancedview.h\
./inc/settingsdataformcustomitem.h\
./inc/dmfotaview.h\
./inc/nsmldmdbnotifier.h

SOURCES += ./src/dmadvancedview.cpp \
			./src/main.cpp \
			./src/nsmldmsyncutil.cpp \
			./src/nsmldmsyncprofilelist.cpp \
			./src/nsmldmsyncprofileitem.cpp \
			./src/nsmldmsyncprofile.cpp \
			./src/nsmldmsyncappengine.cpp \
			./src/nsmldmsynchandler.cpp \
			./src/dminforetrieval.cpp\
			./src/serversettingsview.cpp\
			./src/settingsdataformcustomitem.cpp\
			../src/dmfotaview.cpp \
			../src/nsmldmdbnotifier.cpp
			
RESOURCES += ./resources/deviceupdates.qrc
CONFIG += hb
TRANSLATIONS += deviceupdates.ts
   

