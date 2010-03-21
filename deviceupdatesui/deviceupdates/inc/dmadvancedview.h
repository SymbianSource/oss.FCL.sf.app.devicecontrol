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
* Description:  Launches Advanced view
*
*/

#ifndef DMADVANCEDVIEW_H
#define DMADVANCEDVIEW_H

#include <hbwidget.h>
#include <QList>
#include <hbinstance.h>
#include <hbview.h>
#include <hbaction.h>
#include <hbgroupbox.h>
#include <hblistview.h>
#include <hbpushbutton.h>
#include <hbmenu.h>
#include <hblabel.h>
#include <hbdocumentloader.h>
#include <hblistviewitem.h>
#include <hbanchorlayout.h>
#include <hbicon.h>
#include <QStandardItem>
#include <qtranslator.h>
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <wlansdkpskeys.h>
#include <btengdomaincrkeys.h>
#include <QStandardItemModel>
#include <centralrepository.h>
#include <hbglobal.h>
#include <etelpckt.h>
#include <etel3rdparty.h>
#include <etelmm.h>
#include <etel.h>
#include "serversettingsview.h"
#include "dminforetrieval.h"
#define DOCML_FILE_NAME ":xml/serversview.docml"
#define VIEW_NAME "view"
#define LIST_NAME "listView"
#define GROUP "groupBox"
#define BANNER_NAME "banner"
#define NEWSERVERBUTTON "pushButton"
#define WIDGET "content"
#define LABEL "label"
#define LISTOTHERDETAILS "listView_1"
#define OTHERDETAILSGROUP "groupBox_1"
#define LANDSCAPE "landscape"
#define PORTRAIT "portrait"

const TInt KWlanMacAddrLength = 100;
const TInt KBTAddrLength = 64; 
class DmAdvancedView : public HbView
{
    Q_OBJECT

public:
    explicit DmAdvancedView(HbMainWindow *mainWindow, HbView *mainView, QGraphicsItem *parent=0);
    virtual ~DmAdvancedView();   
    bool displayItems();
    void saveProfile(QStringList& itemdata, bool& sessmode, QString& currap,unsigned int& portnum, bool& nauth );
    bool checkServerId(QString& serverid);
    void setBackBehavior();
    void syncCompleted(int jobstatus);
    void PacketService(TInt val, QString& string);
    void NetworkBand(RMobilePhone::TMobilePhoneNetworkBandInfo val, QString& string); 
    void NetworkCiphering(RMobilePhone::TMobilePhoneNetworkSecurity val, QString& string);  
    void mainCalltoUpdateView();
    
private:
    void updateEarlierdefaultProfileIcon();
    void updateListview();
    
public slots:        
    void handleLongPress(HbAbstractViewItem* item,QPointF coOrdinates);
    void handleClicked(QModelIndex index);
    void createNewProfile();
    void otherDetailsGroupClicked(bool state);
    void serversListGroupClicked(bool state);
    void reLayout(Qt::Orientation layoutType);
    void backButtonClicked();
    
private:        
    HbGroupBox *serversListGroup;
    HbGroupBox *otherDetailsGroup;
    HbListView* list;
    HbListView* otherdetailslist;
    HbLabel *label; 
    HbMenu* csmenu;
    HbMenu* viewspecificmenu;
    HbAction* help;
    HbAction* exit;
    HbWidget* viewwidget;
    HbDocumentLoader *docmlLoader;
    HbPushButton* newserverprofile;
    int count;
    QTranslator *translator;
    DmInfo* dminfo;
    QStandardItemModel *model;
    QStandardItemModel *otherdetailsmodel;
    int currentdefaultprofile;
    bool connectionRequested;
    bool backbehaviorset;
    QIcon bluetooth;
    QIcon internet;
    QIcon defaultprofileicon;
    HbMainWindow *iMainWindow;
    ServerSettingsView* serverSetView;
    HbAnchorLayout* layout;
    int currentview;
    HbView* iMainView;
    HbAction* backaction;
    RMobilePhone imobPhone;
    RTelServer iServer;
    RTelServer::TPhoneInfo info;	
	RMobilePhone::TMobilePhoneNetworkSecurity idispSecurity;
};

#endif // DMADVANCEDVIEW_H

