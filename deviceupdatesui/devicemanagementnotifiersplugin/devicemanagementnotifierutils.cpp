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

#include <hbdocumentloader.h>
#include <hbdialog.h>
#include <hblabel.h>
#include <hbmessagebox.h>
#include <hbaction.h>
#include <e32property.h>
#include <qdebug.h>
#include "SyncMLNotifierParams.h"
#include "PnpUtilLogger.h"
#include "devicemanagementnotifierutils.h"

 enum TSyncmlHbNotifierKeys 
		{

     EHbSOSNotifierKeyStatus = 11, // status set will complete the client subscribe
     EHbSOSNotifierKeyStatusReturn = 12, // Return the content of actual status value accepted from UI
     
     EHbDMSyncNotifierKeyStatus = 13,
     EHbDMSyncNotifierKeyStatusReturn = 14
		};


devicemanagementnotifierutils::devicemanagementnotifierutils(
        const QVariantMap &parameters)
    {
	qDebug("devicemanagementnotifierutils");
    syncmlnotifier* notifier = new syncmlnotifier();
    notifier->launchDialog(parameters);

    }

syncmlnotifier::syncmlnotifier()
    {
    qDebug("devicemanagementnotifierutils syncmlnotifier");
    QTranslator *translator = new QTranslator();
    QString lang = QLocale::system().name();
    QString path = "Z:/resource/qt/translations/";
    bool fine = translator->load("deviceupdates_en.qm", path);
    if (fine)
        qApp->installTranslator(translator);

    QTranslator *commontranslator = new QTranslator();

    fine = commontranslator->load("common_" + lang, path);
    if (fine)
        qApp->installTranslator(commontranslator);

    msymnotifier = new syncmlnotifierprivate();
    }

void syncmlnotifier::launchDialog(const QVariantMap &parameters)
    {

    QVariantMap::const_iterator i = parameters.constBegin();
    qDebug("devicemanagementnotifierutils syncmlnotifier launchDialog");
    //i++;

    bool profileidenabled = false;
    bool uiserverinteraction = false;
    bool serverpushinformativeenabled = false;
    bool serverpushinteractiveenabled = false;

    int profileid = 0;
    int uimode = 0;

    QString serverpushinformmessage;
    QString serverpushconfirmmessage;

    QString serverdispname;

    while (i != parameters.constEnd())
        {
        if (i.key().toAscii() == "profileid")
            {

            profileid = i.value().toInt();
            profileidenabled = true;
            }

        if (i.key().toAscii() == "uimode")
            {
            uiserverinteraction = true;
            uimode = i.value().toInt();
            }

        if (i.key().toAscii() == "serverpushinformative")
            {
			qDebug("server push informative enabled");
            serverpushinformativeenabled = true;
            serverpushinformmessage = i.value().toString();
            }

        if (i.key().toAscii() == "serverpushconfirmative")
            {
		    LOGSTRING("Server Push Confirmative");
            serverpushinteractiveenabled = true;
            serverpushconfirmmessage = i.value().toString();
            }

        if (i.key().toAscii() == "serverdisplayname")
            {
            serverdispname = i.value().toString();
            }

        ++i;
        }

    if (profileidenabled && uiserverinteraction)
        {
        qDebug("server interaction packet 0 enabled ");
        qDebug()<<EHbSOSNotifierKeyStatus;
        msymnotifier->setnotifierorigin(EHbSOSNotifierKeyStatus);
        createserverinitnotifier(profileid, uimode, serverdispname);
        }
    else if (serverpushinformativeenabled)
        {
        qDebug("server push informative enabled ");
        qDebug()<<EHbDMSyncNotifierKeyStatus;
        msymnotifier->setnotifierorigin(EHbDMSyncNotifierKeyStatus);
        createserveralertinformative(serverpushinformmessage);
        }
    else if (serverpushinteractiveenabled)
        {
        qDebug("server push informative enabled ");
        qDebug()<<EHbDMSyncNotifierKeyStatus;
         msymnotifier->setnotifierorigin(EHbDMSyncNotifierKeyStatus);
        createserveralertconfirmative(serverpushconfirmmessage);
        }

    }

void syncmlnotifier::createserverinitnotifier(int profileid, int uimode,
        QString& servername)
    {
    
    int inittype = msymnotifier->serverinittype(uimode);
    qDebug("Notification type");
    qDebug()<<inittype;

	LOGSTRING2("Init value = %d", inittype);

    /*For testing purpose*/
    //inittype = ESANUserInteractive;

    if (inittype == ESANUserInteractive)
        {
        HbDocumentLoader loader;
        bool ok = false;
        loader.load(":/xml/dialog.docml", &ok);
        if (!ok)
            {
            return;
            }
        HbDialog *dialog = qobject_cast<HbDialog *> (loader.findWidget(
                "dialog"));

        //set heading content
        HbLabel *contentheading = qobject_cast<HbLabel *> (loader.findWidget(
                "qtl_dialog_pri_heading"));
        QString heading = hbTrId("txt_device_update_title_update_available");
        contentheading->setPlainText(heading);

        //set body content
        HbLabel *contentbody = qobject_cast<HbLabel *> (loader.findWidget(
                "qtl_dialog_pri5"));
        QString serverinitmessage = hbTrId(
                "txt_device_update_info_recommended_update_is_avail").arg(
                servername);
        contentbody->setPlainText(serverinitmessage);

        //set softkeys
        HbAction *primaryAction = dialog->primaryAction();
        QString softkeyok = hbTrId("txt_common_button_ok");
        primaryAction->setText(softkeyok);

        HbAction *secondaryAction = dialog->secondaryAction();
        QString softkeyCancel = hbTrId("txt_common_button_cancel");
        secondaryAction->setText(softkeyCancel);

        //set dialog properties
        dialog->setTimeout(HbPopup::NoTimeout);

        //load appropriate icon from svg file
        HbLabel* label1 = qobject_cast<HbLabel*> (loader.findWidget("icon"));
        HbIcon* icon1 = new HbIcon(":/devman_icon/iconnotifier.svg");
        label1->setIcon(*icon1);
        //label1->setMode(QIcon::Normal);
        //label1->setState(QIcon::Off);
        label1->setToolTip("Mode=Normal, State=Off");

        QObject::connect(primaryAction, SIGNAL(triggered()), this,
                SLOT(okSelected()));

        QObject::connect(secondaryAction, SIGNAL(triggered()), this,
                SLOT(cancelSelected()));

        if (dialog)
            dialog->show();
        }
    else if (inittype == ESANUserInformative)
        {
        HbDocumentLoader loader;
        bool ok = false;
        loader.load(":/xml/dialoginformative.docml", &ok);
        if (!ok)
            {
            return;
            }

        //hbTrId("qtl_dialog_pri_heading");

        HbDialog *dialog = qobject_cast<HbDialog *> (loader.findWidget(
                "dialog"));

        HbLabel *content = qobject_cast<HbLabel *> (loader.findWidget(
                "qtl_dialog_pri5"));

        QString serverinitmessage = hbTrId(
                "txt_device_update_info_update_serverpush_informative").arg(
                servername);

        //getserverspecifictexttodisplay(profileid, uimode, serverinitmessage);
        content->setPlainText(serverinitmessage);

        //get string here /*TODO*/ /*QTranslations*/

        dialog->setTimeout(3000);

        //msymnotifier->setstatus(0);

        if (dialog)
            dialog->show();

        QObject::connect(dialog, SIGNAL(aboutToClose()), this,
                SLOT(okSelected()));

        }
    else if (inittype == ESANSilent)
        {
        msymnotifier->setstatus(0);
        }

    }

void syncmlnotifier::createserveralertinformative(const QString &string)
    {
    qDebug("createserveralertinformative start");
		LOGSTRING("createserveralertinformative start");
    HbDocumentLoader loader;
    bool ok = false;
    loader.load(":/xml/dialoginformative.docml", &ok);
    if (!ok)
        {
        return;
        }
    HbDialog *dialog = qobject_cast<HbDialog *> (loader.findWidget("dialog"));

    HbLabel *content = qobject_cast<HbLabel *> (loader.findWidget(
            "qtl_dialog_pri5"));

    // No translations required
    content->setPlainText(string);

    dialog->setTimeout(3000);

    if (dialog)
        dialog->show();

    QObject::connect(dialog, SIGNAL(aboutToClose()), this, SLOT(okSelected()));

	qDebug("createserveralertinformative end");

    }

void syncmlnotifier::createserveralertconfirmative(const QString &string)
    {
    HbDocumentLoader loader;
    bool ok = false;
    loader.load(":/xml/dialogserverpushconfirm.docml", &ok);
    if (!ok)
        {
        return;
        }
    HbDialog *dialog = qobject_cast<HbDialog *> (loader.findWidget("dialog"));

    //set heading content
    HbLabel *contentheading = qobject_cast<HbLabel *> (loader.findWidget(
            "qtl_dialog_pri_heading"));
    QString heading = hbTrId("txt_device_update_title_server_message");
    contentheading->setPlainText(heading);

    //set body content
    HbLabel *contentbody = qobject_cast<HbLabel *> (loader.findWidget(
            "qtl_dialog_pri5"));

    contentbody->setPlainText(string);

    //set softkeys
    HbAction *primaryAction = dialog->primaryAction();
    QString softkeyok = hbTrId("txt_common_button_yes");
    primaryAction->setText(softkeyok);

    HbAction *secondaryAction = dialog->secondaryAction();
    QString softkeyCancel = hbTrId("txt_common_button_no");
    secondaryAction->setText(softkeyCancel);

    dialog->setTimeout(HbPopup::NoTimeout);

    QObject::connect(primaryAction, SIGNAL(triggered()), this,
            SLOT(okSelected()));

    QObject::connect(secondaryAction, SIGNAL(triggered()), this,
            SLOT(cancelSelected()));

    if (dialog)
        dialog->show();
    }

void syncmlnotifier::okSelected()
    {
    qDebug("ok selected");
    msymnotifier->setstatus(0);
    }

void syncmlnotifier::cancelSelected()
    {
    qDebug("cancel selected");
    msymnotifier->setstatus(-3);

    }

