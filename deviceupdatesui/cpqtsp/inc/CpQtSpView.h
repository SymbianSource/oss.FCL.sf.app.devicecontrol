/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/

#ifndef CPQTSPVIEW_H
#define CPQTSPVIEW_H

#include <hbview.h>
#include <xqserviceprovider.h>
#include <QGraphicsLinearLayout>
#include <CWPNameValue.h>
#include <CWPBioControl.h>
#include <msvapi.h>
#include <hbscrollarea.h>

class CObserver : public CBase, public MMsvSessionObserver
{
public:
        void HandleSessionEventL(TMsvSessionEvent, TAny*, TAny*, TAny*) {};
};


class CpQtSp;
class XQServiceRequest;

class CpQtSpView : public HbView
{
    Q_OBJECT

public:
    CpQtSpView(CpQtSp* service,QGraphicsItem *parent = 0);
    ~CpQtSpView();
    void constructView();

public slots:
    void requestCompleted(const QVariant& value);
    void saveProvisoningMessage();
private:
    CpQtSp* iServiceProvider; 
    QGraphicsLinearLayout *layout;
    HbFontSpec *iPrimaryFont;
    HbFontSpec *iSecondaryFont;
    RPointerArray<CWPNameValue>* iArray;
    CWPBioControl* iBio;   
};

class CpQtSp : public XQServiceProvider
{
    Q_OBJECT
public:
    CpQtSp( QObject *parent = 0 );
    ~CpQtSp();
    void complete();    
		TMsvId returnId();
		CMsvSession* returnSession();
	
Q_SIGNALS:	
    void showView();
    
public slots:
    QString ProcessMessage(const QString& number);
    
private:
    TMsvId iId;
    CObserver* iObserver;
    CMsvSession* iSession;    
};

#endif // CPQTSPVIEW_H
