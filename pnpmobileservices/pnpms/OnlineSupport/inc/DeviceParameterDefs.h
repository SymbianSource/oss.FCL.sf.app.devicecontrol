/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Definitions for device parameter names and values
*
*/


#ifndef DEVICE_PARAMETER_DEFS_H
#define DEVICE_PARAMETER_DEFS_H

#include <e32base.h>

// Delimeter for URL parameters
_LIT( KParameterDelimeter,"&" );
_LIT( KQuestionMark,"?" );
_LIT( KParenthesisOpen, "(");
_LIT( KParenthesisClose, ")");
_LIT( KSpace," " );

// Version parameter
_LIT( KPnpVersionParameter,"cVersion=" );

// Query parameter
_LIT( KQueryParameter, "hQuery=" );

// Sim MMC parameter
_LIT( KSimMccParameter,"MCC=" );

// Sim MNC parameter
_LIT( KSimMncParameter,"MNC=" );

// current MMC parameter
_LIT( KCurrentMccParameter,"cMCC=" );

// current MNC parameter
_LIT( KCurrentMncParameter,"cMNC=" );

// Token related parameters parameter
_LIT( KTokenParameter,"TOKEN=" );
_LIT( KTokenValidityTimeParameter,"tValid=" );

// PKI related parameters
_LIT( KNonceParameter,"Nonce=" );
_LIT( KKeyInfoParameter,"Key=" );

// Connection reason parameter
_LIT( KConnectReasonParameter,"cReason=" );

// Operator name (from SIM)
_LIT( KOperatorNameParameter,"Oper=" );

// Current Operator name
_LIT( KCurrentOperatorNameParameter,"cOper=" );

// Preferred language
_LIT( KLanguageParameter, "Lang=" );

// MSISDN parameter
_LIT( KMSISDNParameter, "MSISDN=" );

// User Agent profile
_LIT( KUAProfParameter, "UAProf=" );

// Device type parameter
_LIT( KDeviceTypeParameter,"dType=" );
_LIT( KSymbianOs, "Symbian%20" ); // Url encoded format

_LIT(KSW, "SW=" );
_LIT(KHW, "HW=" );

// AutoStart (silent mode) parameters
_LIT( KInteractiveSessionParameter, "iActive=" );
_LIT( KInteractiveSession, "1" );        // the client can display markup pages (this is the default). 
_LIT( KNonInteractiveSession, "0" );    // non-interactive provisioning operation is expected

// IMEI
//_LIT( KImei, "DeviceId=" );

_LIT( KPhoneAlias, "dAlias=" );
_LIT( KPhoneAliasValue, "NOKIA6600" );

#endif // DEVICE_PARAMETER_DEFS_H
