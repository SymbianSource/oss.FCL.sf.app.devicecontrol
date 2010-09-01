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
SISNAME=PnpPaos_S3-0_stub_winscw_udeb
PKGNAME=PnpPaos_S3-0_stub_winscw_udeb
else
ifeq (UDEB,$(findstring UDEB, $(CFG)))
INSTALLDIR=..\..\install\udeb
SISNAME=PnpPaos_S3-0_stub_udeb
PKGNAME=PnpPaos_S3-0_stub_udeb
else
INSTALLDIR=..\..\install
SISNAME=PnpPaos_S3-0_stub
PKGNAME=PnpPaos_S3-0_stub
endif
endif

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
	makesis -s $? $@ 

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
