/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*
*/

#include <hbaction.h>
#include <hbdialog.h>

#include "devicemanagementnotifierwidget_p.h"


// Constructor
devicemanagementnotifierwidget::devicemanagementnotifierwidget(const QVariantMap &parameters):devicemanagementnotifierutils(parameters)
{
    
}

// Set parameters
bool devicemanagementnotifierwidget::setDeviceDialogParameters(
    const QVariantMap &parameters)
{
    
    return true;
}

// Get error
int devicemanagementnotifierwidget::deviceDialogError() const
{
    
    return 0;
}

// Close device dialog
void devicemanagementnotifierwidget::closeDeviceDialog(bool byClient)
{
    close();
    emit deviceDialogClosed();
}

// Return display widget
HbDialog *devicemanagementnotifierwidget::deviceDialogWidget() const
{
   
    return const_cast<devicemanagementnotifierwidget*>(this);
}
