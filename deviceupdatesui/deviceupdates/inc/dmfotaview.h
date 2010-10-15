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
#include <sysutil.h>
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
#include <fotaengine.h>
#include "dminforetrieval.h"
#include "dmserviceprovider.h"
#include "deviceupdatemoniterobserver.h"
//_LIT( KSmlEOL,      "\n" );
enum {
    EQtHighwayLaunch = 6
};

class HbDialog;
class HbPushButton;
class DmAdvancedView;
class CDeviceUpdateMoniter;

class DMFotaView : public HbView, public MDeviceMoniterObserver
    {
    Q_OBJECT
public:
    DMFotaView(HbMainWindow *aMainWindow,DMServiceProvider* aService=NULL);
    virtual ~DMFotaView();
    bool addFotaView();
    void displayNoteAndDisableButtons();
    void enableButtons();
    
signals :
	/**
	 * applicationReady() - To emit the application ready signal for matti tool.
   */
   void applicationReady();
public slots :
    void OnHelp();
    void OnExit();
	void CheckforUpdate();
	void AdvancedDeviceManager(bool aLaunchadvanceview = true);	
	void backtoMainWindow();
	void readSection( Qt::Orientation aOrientation );
	void ResumeUpdate();
public:
	void UpdateDMUI(TBool aVal);
private:
    void FormatList(QString aVal, QString aStr);
    RFotaEngineSession& FotaEngineL();
    void fotaSupportEnabled();
private:
	HbMainWindow* mMainWindow;
	HbView* mFotaPortraitView;
	HbView* mFotaLandscapeView;
	DmAdvancedView* mProfilesView;
	HbAction    *mSoftKeyBackAction;
    RSyncMLSession iSession;
    RSyncMLDevManProfile iProfile;
    RSyncMLDevManJob iSyncJob;
    QTranslator* mTranslator;
    HbLabel *mLabel,*mLabel2,*mLabel3,*mLabel4;
    QStringList mList1,mList2;
    TInt iCount;   
    DMServiceProvider* mService;
    DmInfo* iMainDmInfo;
    HbDocumentLoader mLoader,mLoader2;
    HbPushButton *mUpdateButton;
    TInt iFotaValue;
    HbPushButton *mAdvancedButton;
    HbPushButton *mUpdateButtonLandscape;
    HbPushButton *mAdvancedButtonLandscape;
    
    RFotaEngineSession iFotaEngine;
    RFotaEngineSession::TState iFotaState;
    
    CDeviceUpdateMoniter * iMoniter;
    TBool iConnected;
    };
    

#endif
