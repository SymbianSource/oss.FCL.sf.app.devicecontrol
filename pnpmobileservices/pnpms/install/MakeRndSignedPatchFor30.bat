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

makesis "PnpPaos_S3-0_patch.pkg"
signsis "PnpPaos_S3-0_patch.SIS" PnpPaos_S3-0_signed.sis \s60\AppinstUi\internal\RD_RootCA\rd.cer \s60\AppinstUi\internal\RD_RootCA\rd-key.pem

makesis "CentralRepositoryInit.pkg"
signsis "CentralRepositoryInit.SIS" CentralRepositoryInit_signed.sis \s60\AppinstUi\internal\RD_RootCA\rd.cer \s60\AppinstUi\internal\RD_RootCA\rd-key.pem

makesis "Support_w_Paos_S3-0_patch.pkg"
signsis "Support_w_Paos_S3-0_patch.SIS" ServiceHelp_update_RnD_signed.sis \s60\AppinstUi\internal\RD_RootCA\rd.cer \s60\AppinstUi\internal\RD_RootCA\rd-key.pem

del PnpPaos_S3-0_patch.SIS
del PnpPaos_S3-0_signed.sis
del CentralRepositoryInit.SIS
del CentralRepositoryInit_signed.sis
del Support_w_Paos_S3-0_patch.SIS