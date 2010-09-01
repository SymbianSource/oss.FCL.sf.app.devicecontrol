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
# Description:
#
#
# Build a stub SIS file to go into ROM to allow installing an update.
#

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
INSTALLDIR=..\..\install\wins
SISNAME=Support_S3-0_stub_winscw_udeb
PKGNAME=Support_S3-0_stub_winscw_udeb
else
INSTALLDIR=..\..\install\udeb
ifeq (UDEB,$(findstring UDEB, $(CFG)))
SISNAME=Support_S3-0_stub_udeb
PKGNAME=Support_S3-0_stub_udeb
else
INSTALLDIR=..\..\install
SISNAME=Support_S3-0_stub
PKGNAME=Support_S3-0_stub
endif
endif

# for handling dummy localization resources
LOCALIZATION_RESOURCE_DIR=\Epoc32\data\z\Resource\apps\PnPMS_Temporary_dir

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
TARGETDIR=$(EPOCROOT)EPOC32\RELEASE\$(PLATFORM)\$(CFG)\Z\System\Install
else
TARGETDIR=$(EPOCROOT)EPOC32\Data\Z\System\Install
endif

$(TARGETDIR) :
	@perl -S emkdir.pl "$(TARGETDIR)"

do_nothing :
	rem do_nothing

SISFILE=$(TARGETDIR)\$(SISNAME).sis

$(SISFILE) : $(INSTALLDIR)\$(PKGNAME).pkg
	@echo "
	@echo pkg-file: $? 
	@echo sis-file: $@
	@echo "
#Copy dummy language resources to allow making a sisx-stub
	-mkdir $(LOCALIZATION_RESOURCE_DIR)
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r00
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r01
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r02
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r03
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r04
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r05
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r06
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r07
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r08
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r09
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r10
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r13
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r14
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r15
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r16
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r17
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r18
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r25
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r26
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r27
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r28
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r29
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r30
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r31
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r32
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r33
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r37
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r39
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r42
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r45
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r49
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r51
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r54
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r57
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r59
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r67
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r68
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r70
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r76
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r78
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r79
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r83
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r93
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r157
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r158
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r159
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r160
	-copy onlinesupport.rsc $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r161
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r00
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r01
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r02
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r03
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r04
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r05
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r06
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r07
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r08
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r09
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r10
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r13
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r14
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r15
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r16
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r17
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r18
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r25
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r26
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r27
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r28
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r29
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r30
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r31
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r32
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r33
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r37
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r39
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r42
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r45
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r49
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r51
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r54
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r57
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r59
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r67
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r68
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r70
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r76
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r78
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r79
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r83
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r93
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r157
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r158
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r159
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r160
	-copy pnpprovisioning.rsc $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r161
# Make a SISX-stub file
	makesis -s $? $@ 
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r00
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r01
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r02
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r03
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r04
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r05
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r06
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r07
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r08
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r09
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r10
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r13
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r14
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r15
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r16
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r17
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r18
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r25
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r26
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r27
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r28
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r29
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r30
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r31
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r32
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r33
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r37
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r39
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r42
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r45
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r49
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r51
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r54
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r57
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r59
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r67
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r68
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r70
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r76
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r78
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r79
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r83
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r93
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r157
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r158
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r159
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r160
	-erase $(LOCALIZATION_RESOURCE_DIR)\onlinesupport.r161
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r00
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r01
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r02
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r03
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r04
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r05
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r06
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r07
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r08
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r09
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r10
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r13
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r14
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r15
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r16
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r17
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r18
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r25
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r26
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r27
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r28
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r29
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r30
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r31
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r32
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r33
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r37
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r39
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r42
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r45
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r49
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r51
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r54
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r57
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r59
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r67
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r68
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r70
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r76
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r78
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r79
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r83
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r93
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r157
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r158
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r159
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r160
	-erase $(LOCALIZATION_RESOURCE_DIR)\pnpprovisioning.r161
	-rmdir $(LOCALIZATION_RESOURCE_DIR)

#
# The targets invoked by bld...
#

MAKMAKE : do_nothing

RESOURCE : do_nothing

SAVESPACE : do_nothing

BLD :
#Remove the old sis-files to allow creating new ones
	-erase $(SISFILE)

FREEZE : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

FINAL : $(TARGETDIR) $(SISFILE)

CLEAN : 
	-erase $(SISFILE)

RELEASABLES : 
	@echo $(SISFILE)
