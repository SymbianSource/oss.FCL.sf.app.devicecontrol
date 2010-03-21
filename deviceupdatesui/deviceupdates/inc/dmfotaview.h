/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DMFotaView header.
*
*/

#ifndef DMFOTAVIEW_H_
#define DMFOTAVIEW_H_


#include <hbapplication.h>
#include <hbwidget.h>
#include <QGraphicsLinearLayout>
#include <hbgroupbox.h>
#include <hbview.h>
#include <hblabel.h>
#include <hbdocumentloader.h>
#include <hbmenu.h>
#include <SyncMLClient.h>
#include <SyncMLClientDM.h>
#include <QStandardItemModel>
#include <QLocale>
#include <centralrepository.h>
#include <QTranslator>
#include <nsmldmdef.h>
//_LIT( KSmlEOL,      "\n" );


class HbDialog;
class DmAdvancedView;
class DMFotaView : public HbView
    {
    Q_OBJECT
public:
    DMFotaView(HbMainWindow *mainWindow);
    virtual ~DMFotaView();
    bool addFotaView();
    //HbDialog* createDialog() const;
private:
    void FormatList(QString val, QString str);
public slots :
    void OnHelp();
    void OnExit();
	void CheckforUpdate();
	void AdvancedDeviceManager();
	void backtoMainWindow();
	void readSection( Qt::Orientation orientation );
private:
	HbMainWindow* mMainWindow;
	HbView* fotaPortraitView;
	HbView* fotaLandscapeView;
	DmAdvancedView* profilesView;
	HbAction    *mSoftKeyBackAction;
    RSyncMLSession iSession;
    RSyncMLDevManProfile profile;
    RSyncMLDevManJob iSyncJob;
    QTranslator* mTranslator;
    HbLabel *label,*label2,*label3,*label4;
    QStringList list1,list2;
    TInt i;
    HbDocumentLoader loader,loader2;
    };
    

#endif
