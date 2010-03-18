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

#include <hbmainwindow.h>
#include <CpQtSpMainWindow.h>
#include <CpQtSpView.h>


CpQtSpMainWindow::CpQtSpMainWindow(QWidget *parent)
    : HbMainWindow(parent)
{
    iServiceProvider = new CpQtSp();
    connect(iServiceProvider,SIGNAL(showView()),this, SLOT(showView()));    
    iView = new CpQtSpView(iServiceProvider);    
    }

CpQtSpMainWindow::~CpQtSpMainWindow()
{
    delete iView;
    delete iServiceProvider;
}

void CpQtSpMainWindow::showView()
{
    iView->constructView(); 				  
    addView(iView);    
}
