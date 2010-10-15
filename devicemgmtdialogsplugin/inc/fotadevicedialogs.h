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

#ifndef FOTADEVICEDIALOGS_H_
#define FOTADEVICEDIALOGS_H_

#include <devicedialogconsts.h>

#include <hbdevicedialoginterface.h>    // HbDeviceDialogInterface
#include <hbdialog.h>
#include <hblabel.h>
#include <hbpushbutton.h>
#include "syncmlnotifierprivate.h"
#include "devicemanagementnotifierwidget_p.h"

#include <QObject>


class fotadevicedialogs:public QObject
{


Q_OBJECT

public:
		/**
		  * Construction and setting of observer happens here
		  *
		  * @since   SF4
		  * @param   ptr - pointer to the devicemanagementnotifierwidget which sends the data to the calling module.
		  * passed to the parent class from the device dialog.
		  */
		
		fotadevicedialogs(devicemanagementnotifierwidget* ptr);
		
		
		/**
		  * Destructor to release the memory
		  *
		  * @since   SF4
		  * @param   None
		  */
		  
		~fotadevicedialogs();

		/**
      * Function to call the appropriate device dialog based on the dialog ID.
      *
      * @since   SF4
      * @param   aType - tell sthe type of the dialog to be shown
      * @param   parameters - contains the dialogID + information to be shown in the dialogs
      */

     void launchFotaDialog(const QVariantMap &parameters);


private:
         
     /**
       * Function to show the informative dialog of FOTA
       *
       * @since   SF4
       * @param   aType - tell sthe type of the dialog to be shown
       * @param   parameters - contains the dialogID + information to be shown in the dialogs
       */

     void createfotainformativedialog(TFwUpdNoteTypes aType,const QVariantMap &parameters);
     
     /**
        * Function to show the error message dialog of FOTA
        *
        * @since   SF4
        * @param   aType - tell sthe type of the dialog to be shown
        * @param   parameters - contains the dialogID + information to be shown in the dialogs
        */

     void createfotamessagedialog(TFwUpdNoteTypes aType,const QVariantMap &parameters);
     
     /**
         * Function to show the confirmative dialog of FOTA
         *
         * @since  SF4
         * @param   aType - tell sthe type of the dialog to be shown
         * @param   parameters - contains the dialogID + information to be shown in the dialogs
         */
     void createfotaconfirmationdialog(TFwUpdNoteTypes aType,const QVariantMap &parameters);

public slots:

		/**
	  * Construction and setting of observer happens here
	  *
	  * @since   SF4
	  * @param   aObserver is the parent class which owns the notifier and information is
	  * passed to the parent class from the device dialog.
	  */
    void fotaLSK();
    
    /**
    * Construction and setting of observer happens here
    *
    * @since   SF4
    * @param   aObserver is the parent class which owns the notifier and information is
    * passed to the parent class from the device dialog.
    */
    void fotaRSK();
    
private:
    
    // The dialog ID of the current shown dialog.
    TInt m_DialogId;
    
    // HbDialog instance which actually shows the dialog from the docml.
    HbDialog* m_dialog;
    
    // widget pointer which is used to send the signals to the calling module.
    devicemanagementnotifierwidget* m_Ptr;
};

#endif
