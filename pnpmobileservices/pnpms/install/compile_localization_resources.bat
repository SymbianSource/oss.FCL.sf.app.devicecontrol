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
rem Description: Implementation of PnPMS components
rem

set EROOT=\epoc32
set RESOURCE_PATH=%EROOT%\data\z\resource\APPS\

rem PnP PROVISIONING
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_00 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r00
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_01 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r01
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_02 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r02
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_03 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r03
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_04 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r04
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_05 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r05
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_06 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r06
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_07 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r07
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_08 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r08
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_09 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r09
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_10 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r10
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_13 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r13
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_14 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r14
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_15 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r15
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_16 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r16
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_17 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r17
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_18 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r18
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_25 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r25
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_26 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r26
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_27 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r27
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_28 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r28
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_29 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r29
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_30 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r30
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_31 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r31
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_32 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r32
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_33 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r33
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_37 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r37
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_39 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r39
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_42 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r42
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_45 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r45
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_49 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r49
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_51 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r51
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_54 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r54
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_57 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r57
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_59 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r59
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_67 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r67
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_68 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r68
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_70 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r70
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_76 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r76
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_78 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r78
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_79 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r79
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_83 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r83
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_93 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r93
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_157 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r157
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_158 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r158
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_159 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r159
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_160 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r160
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_161 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\PnP\PnPProvisioningAppInc "..\PnP\PnpProvisioningAppData\PnPProvisioning.rss" -o%RESOURCE_PATH%pnpprovisioning.r161


rem ONLINE SUPPORT
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_00 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r00
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_01 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r01
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_02 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r02
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_03 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r03
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_04 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r04
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_05 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r05
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_06 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r06
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_07 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r07
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_08 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r08
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_09 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r09
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_10 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r10
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_13 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r13
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_14 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r14
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_15 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r15
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_16 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r16
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_17 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r17
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_18 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r18
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_25 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r25
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_26 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r26
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_27 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r27
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_28 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r28
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_29 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r29
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_30 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r30
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_31 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r31
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_32 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r32
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_33 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r33
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_37 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r37
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_39 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r39
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_42 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r42
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_45 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r45
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_49 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r49
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_51 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r51
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_54 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r54
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_57 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r57
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_59 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r59
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_67 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r67
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_68 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r68
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_70 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r70
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_76 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r76
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_78 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r78
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_79 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r79
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_83 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r83
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_93 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r93
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_157 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r157
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_158 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r158
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_159 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r159
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_160 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r160
perl -S epocrc.pl -u -m045,046,047 -DLANGUAGE_161 -DSIS_RESOURCE_COMPILATION -I%EROOT%\include -I..\OnlineSupport\inc -I..\OnlineSupport\inc\LocalizedFiles "..\OnlineSupport\inc\LocalizedFiles\OnlineSupport.rss" -o%RESOURCE_PATH%onlinesupport.r161

rem -hOnlineSupport.rsg 

