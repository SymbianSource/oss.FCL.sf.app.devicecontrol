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
    #:BLD_INF_RULES.prj_exports += "./deviceupdates_en.qm /epoc32/data/z/resource/qt/translations/deviceupdates_en.qm"    
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_ar.ts /epoc32/include/platform/qt/translations/deviceupdates_ar.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_eu.ts /epoc32/include/platform/qt/translations/deviceupdates_eu.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_bg.ts /epoc32/include/platform/qt/translations/deviceupdates_bg.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_ca.ts /epoc32/include/platform/qt/translations/deviceupdates_ca.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_hr.ts /epoc32/include/platform/qt/translations/deviceupdates_hr.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_cs.ts /epoc32/include/platform/qt/translations/deviceupdates_cs.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_da.ts /epoc32/include/platform/qt/translations/deviceupdates_da.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_nl.ts /epoc32/include/platform/qt/translations/deviceupdates_nl.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_en.ts /epoc32/include/platform/qt/translations/deviceupdates_en.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_en_US.ts /epoc32/include/platform/qt/translations/deviceupdates_en_US.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_et.ts /epoc32/include/platform/qt/translations/deviceupdates_et.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_fi.ts /epoc32/include/platform/qt/translations/deviceupdates_fi.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_fr.ts /epoc32/include/platform/qt/translations/deviceupdates_fr.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_fr_CA.ts /epoc32/include/platform/qt/translations/deviceupdates_fr_CA.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_gl.ts /epoc32/include/platform/qt/translations/deviceupdates_gl.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_de.ts /epoc32/include/platform/qt/translations/deviceupdates_de.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_el.ts /epoc32/include/platform/qt/translations/deviceupdates_el.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_he.ts /epoc32/include/platform/qt/translations/deviceupdates_he.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_hi.ts /epoc32/include/platform/qt/translations/deviceupdates_hi.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_zh_HK.ts /epoc32/include/platform/qt/translations/deviceupdates_zh_HK.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_hu.ts /epoc32/include/platform/qt/translations/deviceupdates_hu.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_is.ts /epoc32/include/platform/qt/translations/deviceupdates_is.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_id.ts /epoc32/include/platform/qt/translations/deviceupdates_id.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_it.ts /epoc32/include/platform/qt/translations/deviceupdates_it.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_ja.ts /epoc32/include/platform/qt/translations/deviceupdates_ja.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_ko.ts /epoc32/include/platform/qt/translations/deviceupdates_ko.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_lv.ts /epoc32/include/platform/qt/translations/deviceupdates_lv.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_lt.ts /epoc32/include/platform/qt/translations/deviceupdates_lt.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_ms.ts /epoc32/include/platform/qt/translations/deviceupdates_ms.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_no.ts /epoc32/include/platform/qt/translations/deviceupdates_no.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_pl.ts /epoc32/include/platform/qt/translations/deviceupdates_pl.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_pt.ts /epoc32/include/platform/qt/translations/deviceupdates_pt.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_pt_BR.ts /epoc32/include/platform/qt/translations/deviceupdates_pt_BR.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_zh.ts /epoc32/include/platform/qt/translations/deviceupdates_zh.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_ro.ts /epoc32/include/platform/qt/translations/deviceupdates_ro.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_ru.ts /epoc32/include/platform/qt/translations/deviceupdates_ru.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_sr.ts /epoc32/include/platform/qt/translations/deviceupdates_sr.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_sk.ts /epoc32/include/platform/qt/translations/deviceupdates_sk.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_sl.ts /epoc32/include/platform/qt/translations/deviceupdates_sl.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_es_419.ts /epoc32/include/platform/qt/translations/deviceupdates_es_419.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_sv.ts /epoc32/include/platform/qt/translations/deviceupdates_sv.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_tl.ts /epoc32/include/platform/qt/translations/deviceupdates_tl.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_zh_TW.ts /epoc32/include/platform/qt/translations/deviceupdates_zh_TW.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_th.ts /epoc32/include/platform/qt/translations/deviceupdates_th.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_tr.ts /epoc32/include/platform/qt/translations/deviceupdates_tr.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_uk.ts /epoc32/include/platform/qt/translations/deviceupdates_uk.ts"      
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_ur.ts /epoc32/include/platform/qt/translations/deviceupdates_ur.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_vi.ts /epoc32/include/platform/qt/translations/deviceupdates_vi.ts"  
    :BLD_INF_RULES.prj_exports += "./translations/deviceupdates_es.ts /epoc32/include/platform/qt/translations/deviceupdates_es.ts"  
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
		-lcmmanager
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
   

