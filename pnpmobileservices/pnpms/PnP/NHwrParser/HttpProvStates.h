/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of PnPMS components
*
*/


#ifndef HTTP_PROV_STATES_H
#define HTTP_PROV_STATES_H

// INCLUDES
#include <e32base.h>

class THttpProvStates
    {
public:
    /*!
    The available status codes from the specs
    OK = "0"
    User rejected Document = "1"
    Signature failed = "2"
    Document parsing failed = "3"
    Wrapper parsing failed = "4"
    Document storage failed = "5"
    Document activation failed = "6"
    TOKEN validity expired = "7"

    EStatusUnknown should not be sent to the service at all.
    If the status is unknown do not send any status code.
    */
    enum TProvisioningStatus
        {
        EStatusUnknown = -1,
        EStatusOk = 0,
        EStatusRejected = 1,
        EStatusSignatureFailed = 2,
        EStatusDocumentParsingFailed = 3,
        EStatusWrapperParsingFailed = 4,
        EStatusStorageFailed = 5,
        EStatusActivationFailed = 6,
        EStatusTokenExpired = 7
        };
    };

#endif // HTTP_PROV_STATES_H
