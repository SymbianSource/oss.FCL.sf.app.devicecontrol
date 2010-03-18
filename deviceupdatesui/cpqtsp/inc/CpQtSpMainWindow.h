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

#ifndef CPQTSPMAINWINDOW_H
#define CPQTSPMAINWINDOW_H

#include <hbmainwindow.h>

class CpQtSpView;
class CpQtSp;

class CpQtSpMainWindow : public HbMainWindow
{
    Q_OBJECT
    
public:
     CpQtSpMainWindow(QWidget *parent=0);
     ~CpQtSpMainWindow();   
public slots:
    void showView();
    
private:
    CpQtSp* iServiceProvider;
    CpQtSpView* iView;  
};

#endif // CPQTSPMAINWINDOW_H
