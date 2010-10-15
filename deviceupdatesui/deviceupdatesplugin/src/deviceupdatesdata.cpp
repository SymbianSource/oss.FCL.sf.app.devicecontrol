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
* Description:  Device updates data for control panel
*
*/
#include <cpsettingformitemdata.h>
#include <apgtask.h>
#include "deviceupdatesdata.h"
#include <xqserviceutil.h>
DeviceUpdateData::DeviceUpdateData(CpItemDataHelper &itemDataHelper,
													   const QString &text,
													   const QString &description,
													   const HbIcon &icon,
													   const HbDataFormModelItem *parent)
													   : CpSettingFormEntryItemData(itemDataHelper,
													   text,
													   description,
													   icon,
													   parent)
{
}

DeviceUpdateData::~DeviceUpdateData()
{
	CloseDeviceUpdatesUi();
}

// -----------------------------------------------------------------------------
// DeviceUpdateData::OpenDmAppL()
// -----------------------------------------------------------------------------
//
void DeviceUpdateData::LaunchDeviceUpdatesUi()
	{
    qDebug("DeviceUpdateData::LaunchDeviceUpdatesUi >>");
    RWsSession ws;
    qt_symbian_throwIfError( ws.Connect() );
    CleanupClosePushL( ws );
    // Find the task with uid
    TApaTaskList taskList(ws);
    TApaTask task = taskList.FindApp( KUidSmlSyncApp );    
    if ( task.Exists() )
    {    	    
        task.BringToForeground();        
    }
    else
    {
        qDebug("DeviceUpdateData::LaunchDeviceUpdatesUi >> embedding");
        QScopedPointer<XQAiwRequest> requester (mRequestManager.create(KService,KInterface,KMethod,true)); //false for NON-EMBEDDED 
        qDebug("DeviceUpdateData::LaunchDeviceUpdatesUi >> embed end");
        connect(requester.data(), SIGNAL(requestOk(const QVariant&)), this, SLOT(handleOk(const QVariant&)));
        connect(requester.data(), SIGNAL(requestError(int,const QString&)), this, SLOT(handleError(int,const QString&)));
        if (!(requester.isNull())) 
            {
            m_currentRequest = requester.data();
            requester->setSynchronous(false);
            bool serviceRequestOk = requester->send();
            if (serviceRequestOk)
                {
                m_currentRequest = requester.take();
                }
            else
                {
                // On a controlled error Qt Highway should call requestError,
                // so clean scoped pointer here.
                requester.take();
                }
            }  
        }
    CleanupStack::PopAndDestroy();  // ws    
    qDebug("DeviceUpdateData::LaunchDeviceUpdatesUi >> end");	
	}

// ---------------------------------------------------------------------------------------------
// DeviceUpdateData::CloseDeviceUpdatesUi
// closes DM Ui
// ---------------------------------------------------------------------------------------------	
void DeviceUpdateData:: CloseDeviceUpdatesUi()
{
    if(m_currentRequest)
        {
        delete m_currentRequest;
        m_currentRequest = NULL;
        }
}	

// ---------------------------------------------------------------------------------------------
// DeviceUpdateData::onLaunchView
// ---------------------------------------------------------------------------------------------
void DeviceUpdateData::onLaunchView()
{
	LaunchDeviceUpdatesUi();
}

// ---------------------------------------------------------------------------------------------
// DeviceUpdateData::createSettingView
// ---------------------------------------------------------------------------------------------
CpBaseSettingView *DeviceUpdateData::createSettingView() const
{
		return NULL;
}

// ---------------------------------------------------------------------------------------------
// DeviceUpdateData::handleOk
// Handles when request completes successfully
// ---------------------------------------------------------------------------------------------
void DeviceUpdateData::handleOk(const QVariant &returnValue)
{
    if(m_currentRequest)
        {
        delete m_currentRequest;
        m_currentRequest = NULL;
        }
}

// ---------------------------------------------------------------------------------------------
// DeviceUpdateData::handleError
// Handles when request returns with an error
// ---------------------------------------------------------------------------------------------
void DeviceUpdateData::handleError(int error, const QString& errorMessage)
{   
    if(m_currentRequest)
        {
        delete m_currentRequest;
        m_currentRequest = NULL;
        }
}
