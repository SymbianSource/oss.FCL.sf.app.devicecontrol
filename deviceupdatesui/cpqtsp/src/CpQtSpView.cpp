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
*     Bio control for Provisioning documents.
*
*/

#include <hbapplication.h>
#include <hbtoolbar.h>
#include <hbaction.h>
#include <HbMenu.h>
#include <HbTextItem>
#include <HbFontSpec>
#include <HbMessageBox>
#include <QGraphicsLinearLayout>
#include <xqserviceprovider.h>
#include <xqservicerequest.h>
#include <xqserviceutil.h>
#include <e32base.h>
#include <flogger.h>
#include <CWPNameValue.h>
#include <CWPEngine.h>
#include <CpQtSpView.h>
#include <CpQtSpMainWindow.h>


CpQtSpView::CpQtSpView(CpQtSp* service,QGraphicsItem *parent)
    : HbView(parent), iServiceProvider(service)
{ 
    setTitle(tr("Messaging"));
    HbToolBar* toolBar = this->toolBar(); 
    HbMenu* menu = this->menu();       
    HbAction* saveAction = new HbAction("Save");     
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveProvisoningMessage()));        
    HbAction* DeleteAction = new HbAction("Delete");
    
    toolBar->addAction(saveAction);
    toolBar->addAction(DeleteAction);
    toolBar->setOrientation(Qt::Horizontal);
    iPrimaryFont = new HbFontSpec(HbFontSpec::Primary);
    iSecondaryFont = new HbFontSpec(HbFontSpec::Secondary);
    
    layout = new QGraphicsLinearLayout(Qt::Vertical, this);
    
    QString str = hbTrId("txt_device_update_dblist_configuration_message");
    QString str1 = hbTrId("txt_device_update_dblist_product_code_val_save_to");
    HbTextItem* mPrimaryItem = new HbTextItem(str, this);
    HbTextItem* mSecondaryItem = new HbTextItem(str1, this);
       
    QFont font = iPrimaryFont->font();
    mPrimaryItem->setFont(font);
        
    font = iSecondaryFont->font();
    mSecondaryItem->setFont(font);
    
    layout->addItem(mPrimaryItem);
    layout->addItem(mSecondaryItem);
   
    setLayout(layout);     
    }

void CpQtSpView::constructView()
{
    iBio = CWPBioControl::NewL(iServiceProvider->returnSession(),iServiceProvider->returnId());
    RPointerArray<CWPNameValue>* iArray = iBio->CollectItemsLC();    
    TBufC<200> Name;
    TBufC<200> Value;    
    HbTextItem* mPrimaryItem[100];
    HbTextItem* mSecondaryItem[100];
    HbTextItem* mSecondaryItemBlank = new HbTextItem("", this);
    QFont pfont = iPrimaryFont->font();
    QFont sfont = iSecondaryFont->font();      
   
	TBuf<200> currentTitle(KNullDesC);
	
    for( TInt item = 0; item < iArray->Count(); item++ )
    {  	
    	CWPNameValue* pair = ((*iArray)[item]);
    	Name = pair->Name();
    	if(Name.Compare(currentTitle) !=0 )
    	{
    	QString qname((QChar*)Name.Ptr(),Name.Length());
    	mPrimaryItem[item] = new HbTextItem(qname);    	
    	mPrimaryItem[item]->setFont(pfont);    	
    	layout->addItem(mSecondaryItemBlank);
    	layout->addItem(mPrimaryItem[item]);
    	currentTitle.Copy(Name);
    	}
    	    	
    	Value = pair->Value();
    	QString qvalue((QChar*)Value.Ptr(),Value.Length());   
    	mSecondaryItem[item] = new HbTextItem(qvalue);
    	mSecondaryItem[item]->setFont(sfont);
    	layout->addItem(mSecondaryItem[item]);  
   }
    CleanupStack::PopAndDestroy();

}

void CpQtSpView::saveProvisoningMessage()
{
  CWPEngine* engine = iBio->returnEngine();
  int errSave = KErrNone;
  
  for(TInt i=0; i < engine->ItemCount(); i++)
  {
   TRAP( errSave, engine->SaveL(i) );
  } 
  
  if( errSave == KErrNone )
  {
  	//For testing purpose. Will be removed in future.
    HbMessageBox::launchInformationMessageBox("Message Saved");
  }
}

CpQtSpView::~CpQtSpView()
{
    delete iBio;
}

void CpQtSpView::requestCompleted(const QVariant& value)
{
}


CpQtSp::CpQtSp(QObject* parent)
: XQServiceProvider("com.nokia.services.MDM.Provisioning",parent)
{  
    publishAll();
}

CpQtSp::~CpQtSp()
{
	delete iObserver;
	delete iSession;
}

void CpQtSp::complete()
{
   QString retvalue ="";
   completeRequest(1,retvalue);
}

QString CpQtSp::ProcessMessage(const QString& tmsvid)
{
    iId = tmsvid.toInt();	
    TMsvSelectionOrdering sort;    
    sort.SetShowInvisibleEntries(ETrue); 
    iObserver = new(ELeave) CObserver;
    iSession = CMsvSession::OpenSyncL(*iObserver);    
    emit showView();
    setCurrentRequestAsync();
    return "";    
    }

TMsvId CpQtSp::returnId()
{
	return iId;
}

CMsvSession* CpQtSp::returnSession()
{
  return iSession;
}
