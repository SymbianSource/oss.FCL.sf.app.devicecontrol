/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Utility classes.
*
*/



// INCLUDES
#include "NSmlDMItemData.h"
#include "NSmlDMSyncDebug.h"
#include <nsmldmsync.rsg>
#include "NSmlDMSyncUtil.h"

#include <AknUtils.h>
#include <StringLoader.h>

// -----------------------------------------------------------------------------
// CNSmlDMItemData::ConstructL()
// -----------------------------------------------------------------------------
//
void CNSmlDMItemData::ConstructL()
	{
	FLOG( "[OMADM] CNSmlDMItemData::ConstructL:" );
	
	iHeader = HBufC::NewMaxL(128); 
	iValue = HBufC::NewMaxL(128); 
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CNSmlDMItemData::~CNSmlDMItemData()
	{
	FLOG( "[OMADM] ~CNSmlDMItemData:" );
	
	delete iHeader;
	delete iValue;
	}

// -----------------------------------------------------------------------------
// CNSmlDMItemData::Header
// -----------------------------------------------------------------------------
//
HBufC* CNSmlDMItemData::Header() const
	{
	FLOG( "[OMADM] CNSmlDMItemData::Header:" );
	
	return iHeader;
	}

// -----------------------------------------------------------------------------
// CNSmlDMItemData::Value
// -----------------------------------------------------------------------------
//
HBufC* CNSmlDMItemData::Value() const
	{
	FLOG( "[OMADM] CNSmlDMItemData::Value:" );
	
	return iValue;
	}

// -----------------------------------------------------------------------------
// CNSmlDMItemData::SetHeader
// -----------------------------------------------------------------------------
//
void CNSmlDMItemData::SetHeader( const TDesC& aData )
	{
	FLOG( "[OMADM] CNSmlDMItemData::SetHeader:" );
	
	if ( iHeader->Des().MaxLength() < aData.Length() )
		{
		HBufC* tmp = iHeader->ReAlloc( aData.Length() );
		if (tmp)
			{
			iHeader = tmp;
			}
		}
	TPtr p( iHeader->Des() );
	p.Zero();
	p.Append( aData );
	}

// -----------------------------------------------------------------------------
// CNSmlDMItemData::SetValue
// -----------------------------------------------------------------------------
//
void CNSmlDMItemData::SetValue( const TDesC& aData )
	{
	FLOG( "[OMADM] CNSmlDMItemData::SetValue:" );
	
	if ( iValue->Des().MaxLength() < aData.Length() )
		{
		HBufC* tmp = iValue->ReAlloc( aData.Length() );
		
		if (tmp)
			{
			iValue = tmp;
			}
		}
	TPtr p( iValue->Des() );
	p.Zero();
	p.Append( aData );
	}

void CNSmlDMItemData::SetValue( TInt aPort )
    {
    
	FLOG( "[OMADM] CNSmlDMItemData::SetValue( TInt )" );
	
	TBuf<KBufSize16> portbuf;
	portbuf.Num( aPort );
	SetValue( portbuf );
	
    }
// -----------------------------------------------------------------------------
// CNSmlDMItemData::ItemToListLC
// -----------------------------------------------------------------------------
//
HBufC* CNSmlDMItemData::ItemToListLC( TBool aConvert )
	{
	FLOG( "[OMADM] CNSmlDMItemData::ItemToListLC:" );
	
	CDesCArray* list = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL( list );
	
	list->Reset();
	list->AppendL( iHeader->Des() );
	list->AppendL( iValue->Des() );

	HBufC* row;
	if ( iMandatory )
		{
		row = StringLoader::LoadL( R_QTN_SML_SETTINGROWITEM_MAND, *list );
		}
	else
		{
		row = StringLoader::LoadL( R_QTN_SML_SETTINGROWITEM, *list );
		}
	CleanupStack::PopAndDestroy( list );
	CleanupStack::PushL( row );

	TPtr prow = row->Des();
	if ( aConvert )
		{
		AknTextUtils::DisplayTextLanguageSpecificNumberConversion( prow );
		}
	return row;
	}

// -----------------------------------------------------------------------------
// CNSmlDMProfileData::NewLC
// -----------------------------------------------------------------------------
//
CNSmlDMProfileData* CNSmlDMProfileData::NewLC()
	{
	FLOG( "[OMADM] CNSmlDMProfileData::NewLC():" );
	
	CNSmlDMProfileData* self = new (ELeave) CNSmlDMProfileData;
	CleanupStack::PushL( self);
	self->ConstructL();
	return self;
	}

// End of file
