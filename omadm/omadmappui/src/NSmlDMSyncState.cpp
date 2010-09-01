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
* Description:  Sync state handler
*
*/


// INCLUDE FILES

#include "NSmlDMSyncState.h"
#include "NSmlDMSyncUtil.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNSmlDMSyncState* CNSmlDMSyncState::NewL()
    {
    CNSmlDMSyncState* self = new(ELeave) CNSmlDMSyncState();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
    }

// -----------------------------------------------------------------------------
// ~CNSmlDMSyncState
//
// Destructor.
// -----------------------------------------------------------------------------
//
CNSmlDMSyncState::~CNSmlDMSyncState()
    {
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncState
//
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNSmlDMSyncState::CNSmlDMSyncState()
    {
    }

// -----------------------------------------------------------------------------
// ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncState::ConstructL()
    {
	Reset();
    }

// -----------------------------------------------------------------------------
// Reset
//
// Resets all state variables.
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncState::Reset()
    {
	iSyncPhase = EPhaseNone;
    }


// -----------------------------------------------------------------------------
// SetSyncPhase
//
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncState::SetSyncPhase(TInt aPhase)
	{
	iSyncPhase = aPhase;
	}


// -----------------------------------------------------------------------------
// SyncPhase
//
// -----------------------------------------------------------------------------
//
TInt CNSmlDMSyncState::SyncPhase()
	{
	return iSyncPhase;
	}


// -----------------------------------------------------------------------------
// SetProgress
//
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncState::SetProgress(TInt aCount)
	{
	iProgressCount = aCount;
	}

// -----------------------------------------------------------------------------
// IncreaseProgress
//
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncState::IncreaseProgress()
	{
	if (iProgressCount < iTotalProgressCount)
		{
		iProgressCount++;
		}
	}

// -----------------------------------------------------------------------------
// Progress
//
// -----------------------------------------------------------------------------
//
TInt CNSmlDMSyncState::Progress()
	{
	return iProgressCount;
	}

// -----------------------------------------------------------------------------
// SetTotalProgress
//
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncState::SetTotalProgress(TInt aCount)
	{
	iTotalProgressCount = aCount;
	}

// -----------------------------------------------------------------------------
// TotalProgress
//
// -----------------------------------------------------------------------------
//
TInt CNSmlDMSyncState::TotalProgress()
	{
	return iTotalProgressCount;
	}

// -----------------------------------------------------------------------------
// SetContent
//
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncState::SetContent(const TDesC& aContent)
	{
	TUtil::StrCopy(iSyncContent, aContent);
	}

// -----------------------------------------------------------------------------
// Content
//
// -----------------------------------------------------------------------------
//
const TDesC& CNSmlDMSyncState::Content()
	{
	return iSyncContent;
	}

// -----------------------------------------------------------------------------
// ResetProgress
//
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncState::ResetProgress()
	{
	iProgressCount = 0;
	iTotalProgressCount = 0;
	iSyncContent = KNullDesC;
	}

// -----------------------------------------------------------------------------
// ProgressKnown
//
// -----------------------------------------------------------------------------
//
TBool CNSmlDMSyncState::ProgressKnown()
	{
	if (iTotalProgressCount == -1 )
		{
		return EFalse;
		}

	return ETrue;
	}


