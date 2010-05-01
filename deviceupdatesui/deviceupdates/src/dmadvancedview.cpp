
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
* Description:  Methods for DmAdvancedView.
*
*/
#include <hbnamespace.h>
#include "dmadvancedview.h"
#include "serversettingsview.h"

DmAdvancedView::DmAdvancedView(HbMainWindow *mainWindow, HbView *mainView, QGraphicsItem *parent):HbView(parent),
    bluetooth(":/icons/qgn_prop_sml_bt.svg"),
    internet(":/icons/qgn_prop_sml_http.svg"), 
    defaultprofileicon(":/icons/qtg_large_avatar.svg")                
    {
    iMainWindow = mainWindow;
    serverSetView = NULL;
    iMainView = mainView;
    dminfo = new DmInfo(this);
    currentdefaultprofile = -1;
    currentview = 0;
    connectionRequested = false;    
    backbehaviorset = false;
    currentselecteditem = -1;
    }

DmAdvancedView::~DmAdvancedView()
    {
    qDebug("omadm DmAdvancedView::~DmAdvancedView");
    if(docmlLoader)
        delete docmlLoader;  
    if(dminfo)
        delete dminfo;
    qDebug("omadm DmAdvancedView::~DmAdvancedView end"); 
    }

void DmAdvancedView::handleLongPress(HbAbstractViewItem* item , QPointF coOrdinates)
    {
    if (connectionRequested)
        return;
    if (item)
        {
        modelItem = model->itemFromIndex(item->modelIndex());
        currentselecteditem = modelItem->row();

        HbMenu *csmenu = new HbMenu();
        csmenu->setAttribute( Qt::WA_DeleteOnClose);
        HbAction *defaultprofileAction = 0;
        HbAction *deleteaction = 0;
        HbAction *connectaction = 0;
        if (!dminfo->Isdefaultprofile(currentselecteditem))
            {
            defaultprofileAction = csmenu->addAction(hbTrId(
                    "txt_device_update_menu_set_as_default"));
            connect(defaultprofileAction, SIGNAL(triggered()), this,
                    SLOT(defaultMenuItemSelected()));
            }
        if (!dminfo->Isprofilelocked(currentselecteditem))
            {
            deleteaction = csmenu->addAction(hbTrId(
                    "txt_device_update_menu_delete"));
            connect(deleteaction, SIGNAL(triggered()), this,
                    SLOT(deleteMenuItemSelected()));
            }
        connectaction = csmenu->addAction(hbTrId(
                "txt_device_update_menu_connect"));
        connect(connectaction, SIGNAL(triggered()), this,
                SLOT(connectMenuItemSelected()));
        csmenu->setPreferredPos(coOrdinates);
        csmenu->open();
        }
    }


void DmAdvancedView::handleClicked(QModelIndex index)
    {
    if(connectionRequested)
        return;
    //Stop listening DB events for profile addition
    dminfo->DisableDbNotifications(true);
    //If profile is not locked then take to edit server view
    int itemnum = 0;
    QStandardItem *selectedItem = model->itemFromIndex(index);
    itemnum = selectedItem->row();                  
    if(itemnum >= 0 && !dminfo->Isprofilelocked(itemnum))
        {        
        //read profile items
        QStringList itemdata;
        bool sessmode = 0;
        QStringList iaplist;
        int curriap = 0;
        int portnum = 0;
        bool nwauth = false ;
        dminfo->getProfiledata(itemnum, itemdata, sessmode,iaplist,curriap,portnum, nwauth);	
        QString editserverprof(itemdata[0]);
        if(!serverSetView)
            {
            serverSetView = new ServerSettingsView(iMainWindow, this , editserverprof);
            }
        else // view already created
            {
            //Just update the setting items as per this profile            
            serverSetView->makeviewItemsVisible(editserverprof);		  
            }        
        serverSetView->setProfileValues(itemdata,sessmode,iaplist,curriap,portnum,nwauth);
        iMainWindow->setCurrentView(serverSetView);
  
        }
    }

bool DmAdvancedView::displayItems()
    {    	
    docmlLoader = new HbDocumentLoader;
    bool ok ;    
    docmlLoader->load( DOCML_FILE_NAME,  &ok  );
    if(ok)
    	{
        if(iMainWindow->orientation() == Qt::Vertical)
            docmlLoader->load( DOCML_FILE_NAME,  PORTRAIT,&ok  );
        else
            docmlLoader->load( DOCML_FILE_NAME, LANDSCAPE,&ok  );            
    	}
    else
    	{
    	 qDebug("omadm docml loading failed");
    	 return ok;
    	}
    if(ok)
        {        	
        connect(iMainWindow, SIGNAL(orientationChanged(Qt::Orientation)),
                this,  SLOT(reLayout(Qt::Orientation)));    
        viewwidget = qobject_cast<HbView *>(docmlLoader->findWidget( VIEW_NAME ) );    
        Q_ASSERT_X(viewwidget != 0, "OMADM View not found","Not found");        
        viewspecificmenu = new HbMenu();    
        help = viewspecificmenu->addAction(hbTrId("txt_common_menu_help")); 
        exit = viewspecificmenu->addAction(hbTrId("txt_common_menu_exit"));
        connect(exit, SIGNAL(triggered()), QCoreApplication::instance(), SLOT(quit()));
        setMenu(viewspecificmenu);            
        serversListGroup = qobject_cast<HbGroupBox *>(docmlLoader->findWidget( GROUP ) );
        serversListGroup->setHeading(hbTrId("txt_device_update_setlabel_device_update_services"));
        serversListGroup->setCollapsable( true );        
        connect(serversListGroup, SIGNAL(toggled(bool)), this, SLOT(serversListGroupClicked(bool)));
        otherDetailsGroup = qobject_cast<HbGroupBox *>(docmlLoader->findWidget( OTHERDETAILSGROUP ) );
        connect(otherDetailsGroup, SIGNAL(toggled(bool)), this, SLOT(otherDetailsGroupClicked(bool)));        
        otherDetailsGroup->setHeading(hbTrId("txt_device_update_setlabel_other_details"));
        otherDetailsGroup->setCollapsable( true );
        otherDetailsGroup->setCollapsed( true );                            
        list = qobject_cast<HbListView *>(docmlLoader->findWidget( LIST_NAME ) );
        HbListViewItem *prototype = list->listItemPrototype();
        prototype->setGraphicsSize(HbListViewItem::LargeIcon);
        prototype->setSecondaryTextRowCount(1,2);
        prototype->setStretchingStyle(HbListViewItem::StretchLandscape);
        dminfo->refreshProfileList();    
        int IndicatorCount =dminfo->profilescount();
        model = new QStandardItemModel();            
        for (int i = 0; IndicatorCount > 0 && i < IndicatorCount; ++i) {        
        QStringList liststr;
        bool lock = false;
        int icon = 0;
        int ret = dminfo->profileinfo(i,liststr,icon, lock);
        if(ret <= 0)
            {
            break;
            }
        QStandardItem* item ;    
        item = new QStandardItem();    
        HbIcon icon1;
        icon1.setIconName(":/icons/qgn_prop_sml_http.svg");
        HbIcon icon2;
        icon2.setIconName(":/icons/qgn_prop_sml_bt.svg");
        HbIcon icon3;
        icon3.setIconName(":/icons/qgn_indi_sett_protected_add.svg");   
        HbIcon defaultprofile;
        defaultprofile.setIconName(":/icons/qtg_large_avatar.svg");            
        QList<QVariant> Iconl;    
        if(icon == 0)//Internet
            Iconl.insert(0,icon1);
        else if(icon == 1)//Bluetooth
            Iconl.insert(0,icon2); 
        else 
            {
            Iconl.insert(0,defaultprofile);
            currentdefaultprofile = i;
            }
        icon = 0;
        if(lock)
            {
            Iconl.insert(1,icon3);
            lock = false; 
            }
        QVariant iconlist(Iconl);
        iconlist.setValue(Iconl);
        item->setData(liststr , Qt::DisplayRole);
        item->setData(iconlist , Qt::DecorationRole);
        model->setItem(i, item);    
        }        
        list->setModel(model,prototype);
        list->setItemRecycling(false);    
        connect(list, SIGNAL(longPressed(HbAbstractViewItem*,QPointF)),
                this,  SLOT(handleLongPress(HbAbstractViewItem*,QPointF)));
    
        connect(list, SIGNAL(activated(QModelIndex)), this, SLOT(handleClicked(QModelIndex)));            
        
        qDebug("omadm launching other details list");
        otherdetailslist = qobject_cast<HbListView *>(docmlLoader->findWidget( LISTOTHERDETAILS ) );    
        otherdetailslist->listItemPrototype()->setStretchingStyle(HbListViewItem::StretchLandscape);
		otherdetailslist->setLongPressEnabled(EFalse);
        qDebug("omadm launching other details list done");    
        mainCalltoUpdateView();           
        
    
        newserverprofile = qobject_cast<HbPushButton *>(docmlLoader->findWidget(NEWSERVERBUTTON));
        newserverprofile->setText(hbTrId("txt_device_update_button_new_server_profile"));
        connect(newserverprofile, SIGNAL(clicked()),this, SLOT(createNewProfile()));
              
        label = qobject_cast<HbLabel *>(docmlLoader->findWidget(LABEL));        
        label->setPlainText(hbTrId("txt_device_update_subhead_advanced_device_updates"));        
        
        setWidget(viewwidget);        
        }
    else 
        {
        qDebug("omadm docml section loading failed");
        }    
    return ok;
    }

void DmAdvancedView::updateEarlierdefaultProfileIcon()
    {
    if(currentdefaultprofile >= 0)
        {
        QStandardItem *earlierDefaultProfile = model->item(currentdefaultprofile);
        //Find the transport type & set the icon
        int transporttype = 0;
        dminfo->profileTransport(currentdefaultprofile,transporttype);
        if(transporttype == 0)//Internet
            {
            earlierDefaultProfile->setIcon(internet);
            }
        else
            {
            earlierDefaultProfile->setIcon(bluetooth);
            }        
        }
    }

void DmAdvancedView::createNewProfile()
    {
    if(dminfo->createNewprofile())
        {
        //Open server settings view    
        QString newserverprof(hbTrId("txt_device_update_setlabel_new_server_profile"));    
        if(!serverSetView)
            {
            serverSetView = new ServerSettingsView(iMainWindow, this, newserverprof);        
            }
        else // view already created
            {
            //Just update the setting items as per this profile                
            serverSetView->makeviewItemsVisible(newserverprof);       
            }
        QStringList serveritems;    
        serveritems<< "" <<"" <<"" <<"" <<""<<""<<""<<"";
        bool sessmode = true;
        bool nauth = true;
        QStringList apdata;        
        int currap =-1;
        int destap = 0;
        dminfo->getIaplist(apdata,currap,destap);    
        int portnum = 8080;    
        serverSetView->setProfileValues(serveritems,sessmode,apdata,currap,portnum,nauth);
        iMainWindow->setCurrentView(serverSetView);        
        }
    else // profile not getting created
        {
        qDebug("OMADM New server profile creation failed");
        }
    }

void DmAdvancedView::saveProfile(QStringList& itemdata, bool& sessmode, QString& currap,unsigned int& portnum, bool& nauth )
    {
    dminfo->DisableDbNotifications(true);
    dminfo->saveProfile(itemdata,sessmode,currap,portnum,nauth);
    updateListview();
    dminfo->DisableDbNotifications(false);
    }

void DmAdvancedView::updateListview()
    {
    model->clear();
    int IndicatorCount =dminfo->profilescount();    
    for (int i = 0; IndicatorCount > 0 && i < IndicatorCount; ++i) {    
    QStringList liststr;
    bool lock = false;
    int icon = 0;
    int ret = dminfo->profileinfo(i,liststr,icon, lock);
    if(ret <= 0)
        {
        break;
        }    
    QStandardItem* item ;
    item = new QStandardItem();
    HbIcon icon1;
    icon1.setIconName(":/icons/qgn_prop_sml_http.svg");
    HbIcon icon2;
    icon2.setIconName(":/icons/qgn_prop_sml_bt.svg");
    HbIcon icon3;
    icon3.setIconName(":/icons/qgn_indi_sett_protected_add.svg");   
    HbIcon defaultprofile;
    defaultprofile.setIconName(":/icons/qtg_large_avatar.svg");    
    QList<QVariant> Iconl;
    if(icon == 0)//Internet
        Iconl.insert(0,icon1);
    else if(icon == 1)//Bluetooth
        Iconl.insert(0,icon2); 
    else //Default profile
        {
        Iconl.insert(0,defaultprofile);
        currentdefaultprofile = i;
        }
    icon = 0;
    if(lock)
        {
        Iconl.insert(1,icon3);
        lock = false; 
        }
    QVariant iconlist(Iconl);
    iconlist.setValue(Iconl);
    item->setData(liststr , Qt::DisplayRole);
    item->setData(iconlist , Qt::DecorationRole);
    model->setItem(i, item);    
    }    
    model->sort(0);
    }

void DmAdvancedView::serversListGroupClicked(bool state)
    {
    Q_UNUSED(state);
    if(serversListGroup->isCollapsed())
        otherDetailsGroup->setCollapsed(false);
    else
        otherDetailsGroup->setCollapsed(true);
    }

void DmAdvancedView::otherDetailsGroupClicked(bool state)
    {
    Q_UNUSED(state);
    if(otherDetailsGroup->isCollapsed())
        serversListGroup->setCollapsed(false);
    else
        serversListGroup->setCollapsed(true);
    }

bool DmAdvancedView::checkServerId(QString& serverid)
    {
    return dminfo->checksrvid(serverid);
    }

void DmAdvancedView::reLayout(Qt::Orientation orientation)
    {
    qDebug("OMADM servers view DmAdvancedView::reLayout");
    if(orientation == Qt::Horizontal)
        {
        qDebug("OMADM servers view landscape");
        docmlLoader->load(DOCML_FILE_NAME,LANDSCAPE); 
        }
    else
        {
        qDebug("OMADM servers view portrait");	        
        bool ok;
        docmlLoader->load( DOCML_FILE_NAME,  PORTRAIT, &ok  );
        }
    
    
    }

void DmAdvancedView::setBackBehavior()
    {
    if (!backbehaviorset)
        {
        qDebug("OMADM servers view back behavior setting");
        backaction = new HbAction(Hb::BackNaviAction, this);
        connect(backaction, SIGNAL(triggered()), this,
                SLOT(backButtonClicked()));
        setNavigationAction(backaction);
        backbehaviorset = true;
        }
    qDebug("OMADM servers view back behavior setting done");
    }

void DmAdvancedView::backButtonClicked()
    {    
    QList <HbView*> views = iMainWindow->views();    
    if(iMainWindow->orientation()==Qt::Vertical)
            {
            iMainWindow->setCurrentView(views[0]);
            }
        else
            {
            iMainWindow->setCurrentView(views[1]);
            }    
    }


void DmAdvancedView::mainCalltoUpdateView()
{   
    int IndicatorCount =6;
    otherdetailsmodel = new QStandardItemModel(IndicatorCount,0);
    TInt i=0;
    TRequestStatus status;
    QStringList liststr;
    QStandardItem* item;
    QString val;
    QString str;
    
    iServer.Connect();
    iServer.GetPhoneInfo(0, info);
    imobPhone.Open(iServer, info.iName);
    //Bands supported
    
    RMobilePhone::TMobilePhoneNetworkInfoV1 nwInfov1;
    RMobilePhone::TMobilePhoneNetworkInfoV1Pckg nwInfov1Pckg(nwInfov1);						
    imobPhone.GetCurrentNetwork(status, nwInfov1Pckg);
    User::WaitForRequest( status );
    status = KRequestPending;
    nwInfov1 = nwInfov1Pckg();
    NetworkBand(nwInfov1.iBandInfo, str);
    item = new QStandardItem();
    val = hbTrId("txt_device_update_dblist_gsm_bands");
    liststr << val;
    liststr << str;
    item->setData(liststr , Qt::DisplayRole);
    otherdetailsmodel->setItem(i++, item);
    
    //Packet Service
    
    TInt packetsrvc =0;
    RMobilePhone::TMobilePhoneNetworkInfoV5 nwInfov5;
    RMobilePhone::TMobilePhoneNetworkInfoV5Pckg nwInfov5Pckg(nwInfov5);
    imobPhone.GetCurrentNetwork( status, nwInfov5Pckg );
    User::WaitForRequest( status );
    status = KRequestPending;
    nwInfov5 = nwInfov5Pckg();
    
    if(nwInfov5.iHsdpaAvailableIndicator) packetsrvc =1;
    
    if(nwInfov5.iEgprsAvailableIndicator) packetsrvc =2;
    
    if(!packetsrvc)
    {
        RMobilePhone::TMobilePhoneNetworkInfoV8 nwInfov8;
        RMobilePhone::TMobilePhoneNetworkInfoV8Pckg nwInfov8Pckg(nwInfov8);
        imobPhone.GetCurrentNetwork( status, nwInfov8Pckg );
        User::WaitForRequest( status );
        status = KRequestPending;
        nwInfov8 = nwInfov8Pckg();
        if(nwInfov8.iHsupaAvailableIndicator) packetsrvc =3;
    }	  
    liststr.clear();
    val = hbTrId("txt_device_update_dblist_wcdma_uldl_data_rates");
    liststr << val;
    item = new QStandardItem();
    PacketService(packetsrvc, str);
    liststr << str;
    item->setData(liststr , Qt::DisplayRole);
    otherdetailsmodel->setItem(i++, item);
				
    // Ciphering
    
    liststr.clear();
    item = new QStandardItem();
    val = hbTrId("txt_device_update_dblist_gsm_cipherings");
    liststr << val;
    imobPhone.GetNetworkSecurityLevel(status,idispSecurity);
    User::WaitForRequest( status );
    NetworkCiphering(idispSecurity, str);
    liststr << str;
    item->setData(liststr, Qt::DisplayRole);
    otherdetailsmodel->setItem(i++, item);
    
    // WLAN MAC
    
    TUint KPhoneWlanSeparator (':');
    _LIT( KWLanMACDataFormat, "%02x");  
    // Fetch WLAN MAC address
    TBuf<KWlanMacAddrLength> address;
    RProperty::Get( KPSUidWlan, KPSWlanMacAddress, address );   
    // Format fetched address
    TBuf<KWlanMacAddrLength> wlanMACAddress;        
    for ( TInt i( 0 ); i < address.Length(); i++ )
        {
        // Set separator
        if( i > 0 )
            {
            wlanMACAddress.Append( KPhoneWlanSeparator );
            }
        // Set data
        TBuf<50> tmp;
        tmp.Format( KWLanMACDataFormat, address[i] );
        wlanMACAddress.Append( tmp );
        } 
    liststr.clear();
    item = new QStandardItem();
    val = hbTrId("txt_device_update_dblist_wlan_mac_address");
    liststr << val;
    str = QString::fromUtf16(wlanMACAddress.Ptr(), wlanMACAddress.Length());
    liststr << str;
    item->setData(liststr, Qt::DisplayRole);
    otherdetailsmodel->setItem(i++, item);
      
    // BT MAC
      
    TBuf<KBTAddrLength> addressBuffer;
    // Fetch from Cenrep
    CRepository* repository = NULL;
    TRAPD( err, repository = CRepository::NewL( KCRUidBluetoothLocalDeviceAddress ) );
    if ( err == KErrNone )
        {
        qDebug("KCRUidBluetoothLocalDeviceAddress errnone");
        err = repository->Get( KBTLocalDeviceAddress, addressBuffer );
        
        if (err == KErrNone)
        	qDebug("KBTLocalDeviceAddress errnone");
        else
        	qDebug("KBTLocalDeviceAddress Error");
        	
        delete repository;
        }
    else
    	{
    		qDebug("KCRUidBluetoothLocalDeviceAddress Error openin cenrep");
    	}
    liststr.clear();
    val = hbTrId("txt_device_update_dblist_bt_mac_address");
    liststr << val;
    item = new QStandardItem();
    str = QString::fromUtf16(addressBuffer.Ptr(), addressBuffer.Length());
    liststr << str;
    item->setData(liststr , Qt::DisplayRole);
    otherdetailsmodel->setItem(i++, item);
    
    otherdetailslist->setModel(otherdetailsmodel);
		
}

void DmAdvancedView::PacketService(TInt val, QString& string)
    {
     switch (val)
         {
         case (0):
                 string = hbTrId("txt_device_update_dblist_wcdma_uldl_data_val_l1");
                 break;
         case (1):
                 string = hbTrId("txt_device_update_dblist_wcdma_uldl_data_val_l2");
                 break;
         case (2):
                 string = hbTrId("txt_device_update_dblist_wcdma_uldl_data_val_l3");
                 break;
         case (3):
                 string = hbTrId("txt_device_update_dblist_wcdma_uldl_data_val_l4");
                 break;
         case (4):
                 string = hbTrId("txt_device_update_dblist_gsm_cipherings_val_l4");
                 break;
         case (5):
         default :
                 string = hbTrId("txt_device_update_dblist_wcdma_uldl_data_val_l5");
                 break;
         }
     }


void DmAdvancedView::NetworkCiphering(RMobilePhone::TMobilePhoneNetworkSecurity val, QString& string)
    {
     switch (val)
         {
         case (RMobilePhone::ECipheringGSM):
                 string = hbTrId("txt_device_update_dblist_gsm_cipherings_val_l2");
                 break;
         case (RMobilePhone::ECipheringWCDMA):
                 string = hbTrId("txt_device_update_dblist_gsm_cipherings_val_l3");
                 break;
         case (RMobilePhone::ECipheringCDMA):
                 string = hbTrId("txt_device_update_dblist_gsm_cipherings_val_l4");
                 break;
         case (RMobilePhone::ECipheringOff):
         default :
                 string = hbTrId("txt_device_update_dblist_gsm_cipherings_val_l1");
                 break;
         }
    }
    
void DmAdvancedView::NetworkBand(RMobilePhone::TMobilePhoneNetworkBandInfo val, QString& string)
    {
    switch (val)
        {
        case (RMobilePhone::E800BandA):
                string = hbTrId("txt_device_update_dblist_gsm_bands_val_l2");
                break;
        case (RMobilePhone::E800BandB):
                string = hbTrId("txt_device_update_dblist_gsm_bands_val_l3");
                break;
        case (RMobilePhone::E800BandC):
                string = hbTrId("txt_device_update_dblist_gsm_bands_val_l4");
                break;
        case (RMobilePhone::E1900BandA):
                string = hbTrId("txt_device_update_dblist_gsm_bands_val_l5");
                break;
        case (RMobilePhone::E1900BandB):
                string = hbTrId("txt_device_update_dblist_gsm_bands_val_l6");
                break;
        case (RMobilePhone::E1900BandC):
                string = hbTrId("txt_device_update_dblist_gsm_bands_val_l7");
                break;
        case (RMobilePhone::E1900BandD):
                string = hbTrId("txt_device_update_dblist_gsm_bands_val_l8");
                break;
        case (RMobilePhone::E1900BandE):
                string = hbTrId("txt_device_update_dblist_gsm_bands_val_l9");
                break;
        case (RMobilePhone::E1900BandF):
                string = hbTrId("txt_device_update_dblist_gsm_bands_val_l10");
                break;
        case (RMobilePhone::EBandUnknown):
        default :
                string = hbTrId("txt_device_update_dblist_gsm_bands_val_l1");
                break;
        }   
    }

void DmAdvancedView::syncCompleted(int jobstatus)
    {
    Q_UNUSED(jobstatus);
    connectionRequested = false;    
    updateListview();
    }
	
void DmAdvancedView::defaultMenuItemSelected()
    {
    dminfo->setDefaultProfile(currentselecteditem);
    updateEarlierdefaultProfileIcon();
    modelItem->setIcon(defaultprofileicon);
    currentdefaultprofile = currentselecteditem;
    }

void DmAdvancedView::deleteMenuItemSelected()
    {
    dminfo->DisableDbNotifications(true);
    //check currentdefaultprofile is current item
    if(currentselecteditem == currentdefaultprofile) // deleting default profile
        currentdefaultprofile = -1;
    if(dminfo->DeleteProfile(currentselecteditem) >=0 )
        {
    //Update the profile list
    model->removeRow(currentselecteditem);
        }
    dminfo->DisableDbNotifications(false);
    }

void DmAdvancedView::connectMenuItemSelected()
    {
    dminfo->synchronize(currentselecteditem);
    connectionRequested = true;
    }
	
