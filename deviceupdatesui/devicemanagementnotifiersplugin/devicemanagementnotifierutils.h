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

#ifndef DEVICEMANAGEMENTNOTIFIERUTILS_H_
#define DEVICEMANAGEMENTNOTIFIERUTILS_H_

#include <hbpushbutton.h>
#include <qtimeline.h>
#include <hbcheckbox.h>
#include <hblabel.h>
#include <hblineedit.h>
#include <hbtextedit.h>
#include <QGraphicsWidget>
#include <hbdocumentloader.h>
#include <hbinputeditorinterface.h>
#include <hbview.h>
#include <hbprogressbar.h>
#include <hbdialog.h>
#include <hbmainwindow.h>
#include "syncmlnotifierprivate.h"

class syncmlnotifier;

class devicemanagementnotifierutils :public HbDialog  {

Q_OBJECT

public:
devicemanagementnotifierutils(const QVariantMap &parameters);
  
};

class syncmlnotifier:public QObject {

Q_OBJECT

public:
syncmlnotifier();

public:
     void launchDialog(const QVariantMap &parameters);
     void createserverinitnotifier(int profileid, int uimode,QString& servername);
     void createserveralertinformative(const QString &string);
     void createserveralertconfirmative(const QString &string);

public slots:
    void okSelected();
    void cancelSelected(); 

signals:
    void servertextmessage(const QString &string);
 
private:
  syncmlnotifierprivate* msymnotifier;


};
    


#endif
