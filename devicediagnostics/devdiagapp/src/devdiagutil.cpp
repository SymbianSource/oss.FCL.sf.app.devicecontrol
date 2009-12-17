/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
#include <eikenv.h>
#include <bautils.h>
#include <collate.h>
#include <StringLoader.h>
#include <avkon.rsg>


#include "devdiagutil.h"



/*****************************************************************************
 * class TDevDiagUtil
 *****************************************************************************/




// -----------------------------------------------------------------------------
// ConvertUniversalToHomeTime
// -----------------------------------------------------------------------------
//
TTime TDevDiagUtil::ConvertUniversalToHomeTime( const TTime& aUniversalTime )
    {
    TTime time( aUniversalTime );   // time stores UTC time.

    TLocale locale;
    TTimeIntervalSeconds universalTimeOffset( locale.UniversalTimeOffset() );
    
    // Add locale's universal time offset to universal time.
    time += universalTimeOffset;    // time stores Local Time.

    // If home daylight saving in effect, add one hour offset.
    if ( locale.QueryHomeHasDaylightSavingOn() )
        {
        TTimeIntervalHours daylightSaving(1);
        time += daylightSaving;
        }

    return time;
    }

// -----------------------------------------------------------------------------
// IsToDay
// -----------------------------------------------------------------------------
//
TBool TDevDiagUtil::IsToday(TTime aTime)
	{	
	TTime now;
	now.UniversalTime();
	TInt day1 = now.DayNoInYear();
	TInt day2 = aTime.DayNoInYear();
	TTimeIntervalDays daysBetween = now.DaysFrom( aTime );
	
	if ( day1 != day2 )
		{
		return EFalse;
		}
		
	if ( daysBetween.Int() > 0 )
		{
		return EFalse;
		}

	return ETrue; 
	}


// -----------------------------------------------------------------------------
// TDevDiagUtil::GetDateTextL (not done today)
// -----------------------------------------------------------------------------
//
void TDevDiagUtil::GetDateTextL(TDes& aText, TTime aDateTime)
	{
	TTime homeTime = ConvertUniversalToHomeTime( aDateTime );
	HBufC* hBuf = StringLoader::LoadLC( R_QTN_DATE_USUAL_WITH_ZERO );
    homeTime.FormatL( aText, *hBuf );
    CleanupStack::PopAndDestroy( hBuf );
	}


// -----------------------------------------------------------------------------
// TDevDiagUtil::GetTimeTextL (done today)
// -----------------------------------------------------------------------------
//
void TDevDiagUtil::GetTimeTextL( TDes& aText, TTime aDateTime )
	{
	TTime homeTime = ConvertUniversalToHomeTime( aDateTime );
	HBufC* hBuf = StringLoader::LoadLC( R_QTN_TIME_USUAL_WITH_ZERO );
    homeTime.FormatL( aText, *hBuf );
    CleanupStack::PopAndDestroy( hBuf );
	}


// -----------------------------------------------------------------------------
// TDevDiagUtil::GetDateTimeTextL
// -----------------------------------------------------------------------------
//
void TDevDiagUtil::GetDateTimeTextL( TDes& aText, TTime aDateTime )
	{
	TDateTime dt = aDateTime.DateTime();
	aText.Format(_L("%02d.%02d.%04d %02d:%02d:%02d"), dt.Day()+1,
	                                                  dt.Month()+1,
	                                                  dt.Year(),
	                                                  dt.Hour(),
	                                                  dt.Minute(),
	                                                  dt.Second() );
	}


