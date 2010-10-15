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
 * Description: omacppinquerydialog class implementation
 *
 */

#include <e32property.h>
#include <qdebug.h>
#include <qtranslator.h>
#include <qcoreapplication.h>
#include <hbaction.h>
#include <devicedialogconsts.h>
#include "omacppinquerydialog.h"
#include "pnputillogger.h"

// ----------------------------------------------------------------------------
// omacppinquerydialog ::omacppinquerydialog
// Initialization of member variables; Create Pin query
// ----------------------------------------------------------------------------
//
omacppinquerydialog::omacppinquerydialog(const QVariantMap &parameters)
    {
    qDebug("omacppinquerydialog omacppinquerydialog() start");

    // Do translation
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

    mlineedit = 0;
    mactionok = 0;
    mDialog = 0;
    createcppinquery(parameters);
    qDebug("omacppinquerydialog omacppinquerydialog() end");
    }


// Set parameters
bool omacppinquerydialog::setDeviceDialogParameters(
    const QVariantMap &parameters)
{
    
    return true;
}

// Get error
int omacppinquerydialog::deviceDialogError() const
{
    
    return 0;
}

HbDialog *omacppinquerydialog::deviceDialogWidget() const
    {
    return mDialog;
    }

// ----------------------------------------------------------------------------
// omacppinquerydialog ::createcppinquery
// Create Pin query dialog from docml; Connect the signals to slots
// ----------------------------------------------------------------------------
//
void omacppinquerydialog::createcppinquery(const QVariantMap &parameters)
    {
    qDebug("omacppinquerydialog createcppinquery() start");
    HbDocumentLoader loader;
    bool ok = false;
    loader.load(":/xml/resources/pin.docml", &ok);
    if (!ok)
        {
        qFatal("omacppinquerydialog createcppinquery() Unable to read pin.docml");
        
        }

    
    mDialog = qobject_cast<HbDialog *> (loader.findWidget("dialog"));
    mDialog->setDismissPolicy(HbPopup::NoDismiss);
    mDialog->setTimeout(HbPopup::NoTimeout);

    int tries = -1;

    //Get the reties left
    QVariantMap::const_iterator i = parameters.find(keyparam1);
    if (i != parameters.end())
        {
        tries = i.value().toInt();
        }
    // In the first trial, show label without tries
    HbLabel *label = qobject_cast<HbLabel *> (loader.findObject("label"));
    QString labelstring = hbTrId(
            "txt_device_update_dialog_enter_the_configuration_p");

    // In the second and third trials, show label with tries left
    if ((tries >= 1) && (tries <= (KPinRetries - 1)))
        {
        labelstring = hbTrId(
                "txt_device_update_dialog_enter_the_config_trial");
        label->setNumber(tries);
        }

    label->setPlainText(labelstring);

    mactionok = (HbAction *) mDialog->actions().first();
    QString softkeyok = hbTrId("txt_common_button_ok");
    mactionok->setText(softkeyok);
    mactionok->setEnabled(false);

    HbAction *actioncancel = (HbAction *) mDialog->actions().at(1);
    QString softkeyCancel = hbTrId("txt_common_button_cancel");
    actioncancel->setText(softkeyCancel);

    mlineedit = qobject_cast<HbLineEdit *> (loader.findObject("lineEdit"));
    mlineedit->setEchoMode(HbLineEdit::Password);
    mlineedit->setMaxLength(KCPPinMaxLength);

    mlineedit->setInputMethodHints(Qt::ImhDigitsOnly);
    HbEditorInterface editorInterface(mlineedit);
    editorInterface.setMode(HbInputModeNumeric);
    editorInterface.setInputConstraints(HbEditorConstraintFixedInputMode);
    mDialog->setTimeout(HbPopup::NoTimeout);

    bool bconnect = false;
    // Connection to the slot when PIN text is changed
    bconnect = QObject::connect(mlineedit, SIGNAL(contentsChanged()), this,
            SLOT( pintextChanged()));

    qDebug("omacppinquerydialog::createcppinquery() bconnect value ");
    qDebug() << bconnect;

    // Connections to the slot when Ok and Cancel buttons are clicked
    QObject::connect(mactionok, SIGNAL(triggered()), this, SLOT(okSelected()));
    QObject::connect(actioncancel, SIGNAL(triggered()), this,
            SLOT(cancelSelected()));

       
    

    qDebug("omacppinquerydialog createcppinquery() end");
    }

// ----------------------------------------------------------------------------
// omacppinquerydialog ::pintextChanged
// Slot to enable/disable Ok button when text is entered/deleted
// ----------------------------------------------------------------------------
//
void omacppinquerydialog::pintextChanged()
    {
    qDebug("omacppinquerydialog::pintextChanged start");
    LOGSTRING("omacppinquerydialog::pintextChanged start");
    int textlength = 0;
    textlength = mlineedit->text().length();
    LOGSTRING2("omacppinquerydialog::pintextChanged start %d", textlength);

    if (textlength > 0)
        {
        //If text length is positive, enable Ok button
        qDebug("omacppinquerydialog::pintextChanged text is entered");
        LOGSTRING("omacppinquerydialog::pintextChanged text is entered");
        mactionok->setEnabled(true);
        }
    else if (textlength == 0)
        {
        //If text length is zero, disable Ok button
        qDebug("omacppinquerydialog::pintextChanged zero text");
        LOGSTRING("omacppinquerydialog::pintextChanged zero text");
        mactionok->setEnabled(false);
        }
    qDebug("omacppinquerydialog::pintextChanged end");
    }

// ----------------------------------------------------------------------------
// omacppinquerydialog ::okSelected
// Slot to emit signals when Ok is selected
// ----------------------------------------------------------------------------
//
void omacppinquerydialog::okSelected()
    {
    qDebug("omacppinquerydialog::okSelected() start");
    LOGSTRING("omacppinquerydialog::okSelected() start");

    QString stringpin = mlineedit->text();
    QVariantMap resultMap;
    resultMap.insert(pinquery, stringpin);
    resultMap.insert(returnkey, EHbLSK);

    //Emit signals with the data
    emit
    deviceDialogData(resultMap);
    emit deviceDialogClosed();
    qDebug("omacppinquerydialog::okSelected() end");
    LOGSTRING("omacppinquerydialog::okSelected() end");
    }

// ----------------------------------------------------------------------------
// omacppinquerydialog ::cancelSelected
// Slot to emit signals when Cancel is selected
// ----------------------------------------------------------------------------
//
void omacppinquerydialog::cancelSelected()
    {
    qDebug("omacppinquerydialog::cancelSelected() start");
    LOGSTRING("omacppinquerydialog::cancelSelected() start");
    QVariantMap resultMap;
    resultMap.insert(returnkey, EHbRSK);

    //Emit signals with the data
    emit
    deviceDialogData(resultMap);
    emit deviceDialogClosed();
    
    LOGSTRING("omacppinquerydialog::cancelSelected() end");
    qDebug("omacppinquerydialog::cancelSelected() end");
    }

// Close device dialog
void omacppinquerydialog::closeDeviceDialog(bool byClient)
    {
    Q_UNUSED(byClient);
    mDialog->close();
    emit deviceDialogClosed();
    }

// Return signal source (container) for the dialog
QObject *omacppinquerydialog::signalSender() const
{
    return const_cast<omacppinquerydialog*>(this);
}

//  End of File

