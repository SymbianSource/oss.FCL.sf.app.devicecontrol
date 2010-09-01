#
# Copyright © 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#    This is a device diagnostics component

ifeq (WINS,$(findstring WINS,$(PLATFORM)))
//ZDIR=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\z
ZDIR=$(EPOCROOT)epoc32\release\$(PLATFORM)\udeb\z
else
ZDIR=$(EPOCROOT)epoc32\data\z
endif

TARGETDIR=$(ZDIR)\resource\apps
ICONTARGETFILENAME=$(TARGETDIR)\devdiagapp.mif
MBGDIR=$(EPOCROOT)epoc32\include
MBGFILENAME=$(MBGDIR)\devdiagapp.mbg

MAKMAKE : 
	mifconv $(ICONTARGETFILENAME) \
		/h$(MBGFILENAME) \
		/c8,8 qgn_prop_cp_diag_not.svg \
		/c8,8 qgn_prop_cp_diag_pass.svg \
		/c8,8 qgn_prop_cp_diag_fail.svg 
		

BLD : ;

CLEAN : ;

LIB : ;

CLEANLIB : ;

RESOURCE : ;

FREEZE : ;

SAVESPACE : ;

RELEASABLES :
	@echo $(ICONTARGETFILENAME) && \
	@echo $(MBGFILENAME)

FINAL : ;
