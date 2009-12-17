/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of customization components
*
*/
 


#include "ScreenSaverServer.h"
#include "ScreenSaverClientServer.h"
#include "ScreenSaverSession.h"

#include "debug.h"





// ----------------------------------------------------------------------------------------
// Server startup code
// ----------------------------------------------------------------------------------------
static void RunServerL()
	{
	// naming the server thread after the server helps to debug panics
	__UHEAP_MARK;

	User::LeaveIfError(User::RenameThread(KDMSSServerName)); 

	// create and install the active scheduler

	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	__UHEAP_MARK;

	// create the server (leave it on the cleanup stack)
	CServer2 *t = CScreenSaverServer::NewLC();
	// Initialisation complete, now signal the client

	RProcess::Rendezvous(KErrNone);

	// Ready to run
	RDEBUG("CScreenSaverServer is running");
	CActiveScheduler::Start();


	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(t);
	__UHEAP_MARKEND;

	CleanupStack::PopAndDestroy(s);
	__UHEAP_MARKEND;
	}

// Server process entry-point
TInt E32Main()
	{
	__UHEAP_MARK;
	RDEBUG("CScreenSaverServer: E32Main");
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunServerL());
		RDEBUG_2("CScreenSaverServer closed with code: %d", r);
		delete cleanup;
		}
	__UHEAP_MARKEND;
	return r;
	}




// -----------------------------------------------------------------------------
// CShutDown::CShutdown()
// -----------------------------------------------------------------------------

inline CShutdown::CShutdown()
	:CTimer(-1)
	{
	CActiveScheduler::Add(this);
	}
	
	
// -----------------------------------------------------------------------------
// CShutDown::ConstructL()
// -----------------------------------------------------------------------------

inline void CShutdown::ConstructL()
	{
	CTimer::ConstructL();
	}

// -----------------------------------------------------------------------------
// CShutDown::Start()
// -----------------------------------------------------------------------------

inline void CShutdown::Start()
	{
	
	RDEBUG( "creenSaverServer: starting shutdown timeout" );
	
	After(KScreenSaverServerShutdownDelay);
	//SetActive();
	}

// -----------------------------------------------------------------------------
// CShutDown::RunL()
// -----------------------------------------------------------------------------

void CShutdown::RunL()
	{
	RDEBUG( "creenSaverServer timeout ... closing" );
	CActiveScheduler::Stop();
	}




CServer2* CScreenSaverServer::NewLC()
	{
	RDEBUG("CScreenSaverServer::NewLC - start");
	
	CScreenSaverServer* self=new(ELeave) CScreenSaverServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	RDEBUG("CScreenSaverServer::NewLC - end");
	return self;
	}



void CScreenSaverServer::ConstructL()
	{
	RDEBUG("CScreenSaverServer::ConstructL start");
	
	StartL(KDMSSServerName);
	iShutdown.ConstructL();
	iShutdown.Start();	
	
	RDEBUG("CScreenSaverServer::ConstructL end");
	}


CScreenSaverServer::CScreenSaverServer() :CServer2(0)
	{
	RDEBUG("CScreenSaverServer::CScreenSaverServer()");
	}
	// -----------------------------------------------------------------------------
// CApplicationManagementServer::AddSession()
// -----------------------------------------------------------------------------

void CScreenSaverServer::AddSession()
	{
	// A new session is being created
	RDEBUG( "CScreenSaverServer::AddSession" );
	++iSessionCount;
	
	// Cancel the shutdown timer if it was running
	iShutdown.Cancel();
	}


// -----------------------------------------------------------------------------
// CApplicationManagementServer::DropSession()
// -----------------------------------------------------------------------------

void CScreenSaverServer::DropSession()
	{
	// A session is being destroyed
	RDEBUG( "CScreenSaverServer::DropSession" );

	// Start the shutdown timer if it is the last session
	if ( --iSessionCount == 0 )
		{
			iShutdown.Start();
		}
	}




CScreenSaverServer::~CScreenSaverServer()
	{
	RDEBUG("CScreenSaverServer::CScreenSaverServer()");
	}



CSession2* CScreenSaverServer::NewSessionL(const TVersion&,const RMessage2&) const
	{
	RDEBUG("CScreenSaverServer::NewSessionL");
	return new (ELeave) CScreenSaverSession();
	}


void CScreenSaverServer::GetAvailableScreenSaversL(RImplInfoPtrArray& aScreenSaverList)
	{
	RDEBUG("CScreenSaverServer::GetAvailableScreenSavers()");
	CScreensaverPluginInterfaceDefinition::ListImplementationsL( aScreenSaverList );
	REComSession::FinalClose();
	}
