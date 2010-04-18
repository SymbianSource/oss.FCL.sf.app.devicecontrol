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
 * Description:  This class provides custom list item for the
 * DataForm
 *
 */

#include <settingsdataformcustomitem.h>
#include <hbdataformmodelitem.h>
#include <hblineedit.h>
#include <hbinputeditorinterface.h>
#include <qgraphicslinearlayout.h>
#include <hblabel.h>

SettingsDataFormCustomItem::SettingsDataFormCustomItem(QGraphicsItem *parent) :
HbDataFormViewItem(parent)
{
}

SettingsDataFormCustomItem::~SettingsDataFormCustomItem()
{
}

HbAbstractViewItem* SettingsDataFormCustomItem::createItem()
{
    return new SettingsDataFormCustomItem(*this);
}

HbWidget* SettingsDataFormCustomItem::createCustomWidget()
{
    HbDataFormModelItem::DataItemType itemType =
    static_cast<HbDataFormModelItem::DataItemType> 
    ( modelIndex().data(HbDataFormModelItem::ItemTypeRole).toInt() );    
    switch (itemType)
    {
      case HbDataFormModelItem::CustomItemBase :
            {             
            HbLineEdit* mLineEdit = new HbLineEdit();
            mLineEdit->setText(QString("8080"));
            HbEditorInterface editorInterface(mLineEdit);
            editorInterface.setInputMode(HbInputModeNumeric); 
            editorInterface.setConstraints(HbEditorConstraintFixedInputMode);            
            return mLineEdit;            
            }                
        default:
            return 0;
    }
}

 bool SettingsDataFormCustomItem::canSetModelIndex( const QModelIndex& aIndex ) const
 {
     int itemType = aIndex.data(HbDataFormModelItem::ItemTypeRole).toInt();
     return itemType == HbDataFormModelItem::CustomItemBase;
 }
