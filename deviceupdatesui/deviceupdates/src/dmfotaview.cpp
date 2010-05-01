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
* Description:  Methods for DmFotaView.
*
*/

#include <hbmainwindow.h>
#include <hbpushbutton.h>
#include <hbaction.h>
#include <hbdialog.h>
#include <sysutil.h>
#include <hbscrollarea.h>
#include <etel.h>
#include <featmgr.h>
#include <etelmm.h>
#include <hbnotificationdialog.h>
#include <centralrepository.h>
#include <sysversioninfo.h>
#include "nsmldmsyncprivatecrkeys.h"
#include "dmadvancedview.h"
#include "dmfotaview.h"
// ============================ MEMBER FUNCTIONS ===============================
    
// -----------------------------------------------------------------------------
// DMFotaView::DMFotaView
// -----------------------------------------------------------------------------
//
DMFotaView::DMFotaView(HbMainWindow *mainWindow):
    mMainWindow(mainWindow),profilesView(NULL)
{
    fotaPortraitView=0;
    fotaLandscapeView=0;
}

// -----------------------------------------------------------------------------
// DMFotaView::~DMFotaView
// 
// -----------------------------------------------------------------------------
//
DMFotaView::~DMFotaView()
{ 
}

// -----------------------------------------------------------------------------
// DMFotaView::addFotaView
// Creates and adds the Fota Portrait and Landscape View
// -----------------------------------------------------------------------------
//
bool DMFotaView::addFotaView()
    {
    connect( mMainWindow, SIGNAL( orientationChanged(Qt::Orientation) ), this, SLOT( readSection(Qt::Orientation) ) );
        
    bool ok = false;
    QString val,val2;
    loader.load(":/xml/devman.docml", &ok);
    ok = false;
    loader2.load(":/xml/devman.docml", &ok);
    // Exit if the file format is invalid
    Q_ASSERT_X(ok, "Device Manager", "Invalid docml file");
    
    ok=false;
    loader.load(":/xml/devman.docml","Portrait", &ok);
    ok=false;
    loader2.load(":/xml/devman.docml","Landscape", &ok);
   
    // Load the view by name from the xml file
    fotaPortraitView = qobject_cast<HbView*>(loader.findWidget("p:view"));
    fotaLandscapeView = qobject_cast<HbView*>(loader2.findWidget("l:view"));


    HbAction *help = qobject_cast<HbAction*> (loader.findObject("help")); 
    help->setText(QString("txt_common_menu_help"));
    QObject::connect(help, SIGNAL(triggered()), this, SLOT(OnHelp()));
    HbAction *exit = qobject_cast<HbAction*> (loader.findObject("exit"));
    exit->setText(QString("txt_common_menu_exit"));
    QObject::connect(exit, SIGNAL(triggered()), this, SLOT(OnExit()));

    help = qobject_cast<HbAction*>(loader2.findObject("help"));
    help->setText(QString("txt_common_menu_help"));
    QObject::connect(help, SIGNAL(triggered()), this, SLOT(OnHelp()));
    exit = qobject_cast<HbAction*>( loader2.findObject("exit"));
    exit->setText(QString("txt_common_menu_exit"));
    QObject::connect(exit, SIGNAL(triggered()), this, SLOT(OnExit()));
    
    mSoftKeyBackAction = new HbAction(Hb::BackNaviAction ,this);
    mSoftKeyBackAction->setText("Back");
    connect(mSoftKeyBackAction, SIGNAL(triggered()), this, SLOT(backtoMainWindow()));
        
    
    i=0;
    QString str;
    //Setting title text
    label = qobject_cast<HbLabel*> (loader.findWidget("p:title"));
    label2 = qobject_cast<HbLabel*> (loader2.findWidget("l:title"));
    val = hbTrId("txt_device_update_subhead_device_updates");
    label->setPlainText(val);
    label2->setPlainText(val);
    
    //Setting help text for update button
    label = qobject_cast<HbLabel*> (loader.findWidget("p:updatelabel"));
    label2 = qobject_cast<HbLabel*> (loader2.findWidget("l:updatelabel"));
    val = hbTrId("txt_device_update_setlabel_to_update_your_device_s");
    label->setPlainText(val);
    label2->setPlainText(val);
    
    HbScrollArea* area = qobject_cast<HbScrollArea*> (loader.findWidget("scrollArea"));
    area->setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAlwaysOn);
    
    
    list1 << "label" << "label_1" << "label_2" << "label_3" << "label_4" << "label_5" << "label_6"<<"label_7"<<"label_8"<<"label_9"<<"label_10"<<"label_11";
    list2 << "label_13" << "label_14" << "label_15" << "label_16" << "label_17" << "label_18" << "label_19"<<"label_20"<<"label_21"<<"label_22"<<"label_23"<<"label_24";


    //Product Release
    TBuf< KSysUtilVersionTextLength > prodrelease;
    prodrelease.Zero();
    if(SysUtil::GetPRInformation(prodrelease)==KErrNone)
        {
        val = hbTrId("txt_device_update_dblist_product_release");
        str = QString::fromUtf16(prodrelease.Ptr(), prodrelease.Length());
        FormatList(val,str);
        }
    
    //Software version
    
    TBuf< KSysUtilVersionTextLength > swversion;
    TBuf< KSysUtilVersionTextLength > swversiondate;
    TBuf< KSysUtilVersionTextLength > typedesignator;
    TBuf< KSysUtilVersionTextLength > version;
    swversion.Zero();
    version.Zero();
    typedesignator.Zero();
    swversiondate.Zero();
    if( SysUtil::GetSWVersion(version)==KErrNone)
        {
        TInt len= version.Length();
        TInt pos1 = version.Find(KSmlEOL);
             if( pos1 != KErrNotFound && len > pos1 )
                {
                 TBuf<KSysUtilVersionTextLength> version1;
                 version1.Zero();
                 swversion.Append( version.Left(pos1));
                 version1.Append( version.Right( len-pos1-1 ));
                 len= version1.Length();
                 pos1 = version1.Find(KSmlEOL);
                 if( pos1 != KErrNotFound  && len > pos1 )
                     {
                     swversiondate.Append(version1.Left( pos1 ));
                     version.Zero();
                     version.Append( version1.Right( len-pos1-1 ));
                     len= version.Length();
                     pos1 = version.Find(KSmlEOL);
                     if( pos1 != KErrNotFound  && len > pos1 )
                         {
                         typedesignator.Append(version.Left(pos1));         
                         }
                     }
                }
            val = hbTrId("txt_device_update_dblist_software_version");
            str = QString::fromUtf16(swversion.Ptr(), swversion.Length());
            FormatList(val,str);
            }

    //Model
    RTelServer telServer;
    User::LeaveIfError( telServer.Connect() );
    RTelServer::TPhoneInfo teleinfo;
    User::LeaveIfError( telServer.GetPhoneInfo( 0, teleinfo ) );
    RMobilePhone phone;
    User::LeaveIfError( phone.Open( telServer, teleinfo.iName ) );
    User::LeaveIfError(phone.Initialise()); 
    TUint32 teleidentityCaps;
    phone.GetIdentityCaps( teleidentityCaps );
    RMobilePhone::TMobilePhoneIdentityV1 telid;
    TRequestStatus status;
    phone.GetPhoneId( status, telid );
    User::WaitForRequest( status );
    TBuf <50> imei;
    if (status==KErrNone)
    {
    TBuf<100> phoneName;
    phoneName.Copy( telid.iModel );
    imei.Copy(telid.iSerialNumber);
    if( phoneName.Length()>0 )
        {
        val = hbTrId("txt_device_update_dblist_model");
        str = QString::fromUtf16(phoneName.Ptr(), phoneName.Length());
        FormatList(val,str);
        }
    }
    phone.Close();
    telServer.Close();

    //type
    if(typedesignator.Length()>0)
        {
        val = hbTrId("txt_device_update_dblist_type");
        str = QString::fromUtf16(typedesignator.Ptr(), typedesignator.Length());
        FormatList(val,str);
        }
      
    //Product Code
       /* TInt runtimesupport(0);
        CRepository* cenrep = NULL;
        TRAPD( error, cenrep = CRepository::NewL( KCRUidNSmlDMSyncApp ) );  
        if(error)
            {
            runtimesupport=1;
            }
        if ( cenrep )
                {
                cenrep->Get( KNsmlDmRuntimeVerSupport, runtimesupport );
                delete cenrep; cenrep = NULL;
                }
          if(runtimesupport)
          { */
         TBuf<KSysUtilVersionTextLength> productcode;
         productcode.Zero();
         TInt error = SysVersionInfo::GetVersionInfo(SysVersionInfo::EProductCode, productcode);
         if(error ==KErrNone )
         {            
             if( productcode.Length()>0 )
             {        
             val = hbTrId("txt_device_update_dblist_product_code");
             str = QString::fromUtf16(productcode.Ptr(), productcode.Length());  
             FormatList(val,str);
             }                    
        }

    //IMEI   
    if(imei.Length()>0)
        {
        val = hbTrId("txt_device_update_dblist_imei");
        str = QString::fromUtf16(imei.Ptr(), imei.Length()); 
        FormatList(val,str);
        }

    
    val = hbTrId("txt_device_update_button_update");
    HbPushButton *updateButton = qobject_cast<HbPushButton*>(loader.findWidget("p:update"));
    updateButton->setText(val);
    QObject::connect(updateButton, SIGNAL(clicked()), this, SLOT(CheckforUpdate()));
    updateButton = qobject_cast<HbPushButton*>(loader2.findWidget("l:update"));
    updateButton->setText(val);
    QObject::connect(updateButton, SIGNAL(clicked()), this, SLOT(CheckforUpdate()));
    
    val = hbTrId("txt_device_update_button_advanced");
    HbPushButton *advancedButton = qobject_cast<HbPushButton*>(loader.findWidget("p:advanced")); 
    advancedButton->setText(val);
    QObject::connect(advancedButton, SIGNAL(clicked()), this, SLOT(AdvancedDeviceManager()));
    advancedButton = qobject_cast<HbPushButton*>(loader2.findWidget("l:advanced"));
    advancedButton->setText(val);
    QObject::connect(advancedButton, SIGNAL(clicked()), this, SLOT(AdvancedDeviceManager()));

    
    mMainWindow->addView(fotaPortraitView);
    mMainWindow->addView(fotaLandscapeView);
    
    if(mMainWindow->orientation()==Qt::Vertical)
            mMainWindow->setCurrentView(fotaPortraitView);
        else
            mMainWindow->setCurrentView(fotaLandscapeView);
    return ETrue;
    }

// -----------------------------------------------------------------------------
// DMFotaView::CheckforUpdate
// Checks for updates from the default server
// -----------------------------------------------------------------------------
//
void DMFotaView::CheckforUpdate()
 {
    /*TInt profileId=NULL;
    if(mainDmInfo==0)
        {
        if(!profilesView)
            {
            mainDmInfo = new DmInfo();
            mainDmInfo->refreshProfileList();
            }
        else 
            {
            mainDmInfo = profilesView->dminfo;
            }
        }
    if((profileId = mainDmInfo->DefaultFotaProfileIdL())==KErrNotFound)
        {*/

        HbNotificationDialog* note = new HbNotificationDialog();
        QString val = hbTrId("txt_device_update_info_no_server_configured_to_get");
        note->setTitle(val);
        note->setTitleTextWrapping(Hb::TextWordWrap);
        note->setDismissPolicy(HbPopup::TapAnywhere);
        note->setTimeout(HbPopup::StandardTimeout);
        note->setAttribute(Qt::WA_DeleteOnClose, true);
        note->open();
       /* }
    else
        {
        mainDmInfo->sync(profileId);
        }*/
 }

// -----------------------------------------------------------------------------
// DMFotaView::AdvancedDeviceManager
// Displays the Profiles view
// -----------------------------------------------------------------------------
//
void DMFotaView::AdvancedDeviceManager()
 {
    
    qDebug("omadm DeviceManagerUi::AdvancedDeviceManager");
    if(!profilesView)
        {
        qDebug("omadm DeviceManagerUi::AdvancedDeviceManager 1");
        profilesView = new DmAdvancedView(mMainWindow,this);
        qDebug("omadm DeviceManagerUi::AdvancedDeviceManager 2");
        bool loadingok = profilesView->displayItems();
        if(loadingok)
            {
            qDebug("omadm DeviceManagerUi::AdvancedDeviceManager 3");
            mMainWindow->addView(profilesView);
            qDebug("omadm DeviceManagerUi::AdvancedDeviceManager 4");
            profilesView->setBackBehavior();  
            qDebug("omadm DeviceManagerUi::AdvancedDeviceManager 5");
            mMainWindow->setCurrentView(profilesView);
            qDebug("omadm DeviceManagerUi::AdvancedDeviceManager 6");
            }
        else
            {
            qFatal("omadm Unable to read groupbox.docml");
        
            }
        }
    else
        {
        profilesView->setBackBehavior();                
        mMainWindow->setCurrentView(profilesView);   
        }
 }

// -----------------------------------------------------------------------------
// DMFotaView::FormatList
// Adds data to the details list
// -----------------------------------------------------------------------------
//
void DMFotaView::FormatList(QString val,QString str)
    {
    label = qobject_cast<HbLabel*> (loader.findWidget(list1[i]));
    label2 = qobject_cast<HbLabel*> (loader2.findWidget(list2[i++]));         
    label3 = qobject_cast<HbLabel*> (loader.findWidget(list1[i]));
    label4 = qobject_cast<HbLabel*> (loader2.findWidget(list2[i++]));
    label->setPlainText(val);
    label2->setPlainText(val);
    label3->setPlainText(str);
    label4->setPlainText(str);
    label->setVisible(ETrue);
    label2->setVisible(ETrue);
    label3->setVisible(ETrue);
    label4->setVisible(ETrue);
    }
// -----------------------------------------------------------------------------
// DMFotaView::backtoMainWindow
// Enables switching between views
// -----------------------------------------------------------------------------
//
void DMFotaView::backtoMainWindow()
    {
        qApp->quit();
    }

// -----------------------------------------------------------------------------
// DMFotaView::OnHelp
// Displays Help contents
// -----------------------------------------------------------------------------
//
void DMFotaView::OnHelp()
{   
}

// -----------------------------------------------------------------------------
// DMFotaView::OnExit
// -----------------------------------------------------------------------------
//
void DMFotaView::OnExit()
{
    qApp->quit();
}

void DMFotaView::readSection( Qt::Orientation orientation )
{
    if(mMainWindow->currentView() == fotaPortraitView || mMainWindow->currentView() == fotaLandscapeView)
        {
        if( orientation == Qt::Vertical ) {
        // reading "portrait" section
        mMainWindow->setCurrentView(fotaPortraitView);
        }
     else {
        // reading "landscape" section
        mMainWindow->setCurrentView(fotaLandscapeView);
         }
        }
    else if(mMainWindow->currentView()==profilesView)
        {
        profilesView->reLayout(orientation);
        }

}   



