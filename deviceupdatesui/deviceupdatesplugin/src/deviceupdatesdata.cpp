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
#include <centralrepository.h>
#include "deviceupdatesdata.h"


DeviceUpdateData::DeviceUpdateData(CpItemDataHelper &itemDataHelper,
													   const QString &text /*= QString()*/,
													   const QString &description /*= QString()*/,
													   const HbIcon &icon /*= HbIcon()*/,
													   const HbDataFormModelItem *parent /*= 0*/)
													   : CpSettingFormEntryItemData(itemDataHelper,
													   text,
													   description,
													   icon,
													   parent)
{
}

DeviceUpdateData::~DeviceUpdateData()
{
	CloseDmUi();
}

// -----------------------------------------------------------------------------
// DeviceUpdateData::OpenDmAppL()
// -----------------------------------------------------------------------------
//
void DeviceUpdateData::OpenDmAppL()
	{	
		
    	// Create DM UI Process
			RProcess rp;
			TInt cpLaunch(1);
			CRepository *cenrep = CRepository::NewL(KUidSmlSyncApp);
			if(cenrep)
				{
					cenrep->Set( KNsmlDmUILaunch, cpLaunch);
					delete cenrep;
					cenrep = NULL;
				}	
    	TInt err = rp.Create(KDMUIProcess,KNullDesC);
            
    	User::LeaveIfError(err);  
    
		
      	rp.Resume();    // logon OK - start the server
      
    		
	}

// ---------------------------------------------------------------------------------------------
// DeviceUpdateData::CloseDmUi
// closes DM Ui
// ---------------------------------------------------------------------------------------------	
void DeviceUpdateData:: CloseDmUi()
{
	// Create DM UI Process
	 
	TFullName processName;		
	TFindProcess process;
	while ( process.Next( processName ) != KErrNotFound )
	{
		if ( ( processName.Find( KDMUIName ) != KErrNotFound ) )
		{
			RProcess rprocess;
			if (rprocess.Open(process, EOwnerProcess) == KErrNone)
			{				
				rprocess.Terminate(KErrNone);
				rprocess.Close();			
			}
		}
	}	
}	

void DeviceUpdateData::onLaunchView()
{
	TRAP_IGNORE( OpenDmAppL() );
}

CpBaseSettingView *DeviceUpdateData::createSettingView() const
{
	return 0;
}


