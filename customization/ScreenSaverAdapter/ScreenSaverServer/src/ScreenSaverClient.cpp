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
 


#include "ScreenSaverClientServer.h"
#include "ScreenSaverClient.h"
#include "debug.h"
#include <f32file.h>


TInt StartServer()
	{
	RDEBUG("RScreenSaverClient: StartServer()...");	
	const TUidType serverUid(KNullUid,KNullUid,KDMSSServerUid);

	RProcess server;
	TInt r=server.Create(KDMSSServerImg,KNullDesC);

	if (r!=KErrNone)
		{
		RDEBUG_2("RScreenSaverClient: server start failed %d",r);
		return r;
		}
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	RDEBUG("RScreenSaverClient: Started");
	User::WaitForRequest(stat);		// wait for start or death
	r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
	}

EXPORT_C TInt RScreenSaverClient::Connect()
	{
	RDEBUG("RScreenSaverClient::Connect - start");
	TInt retry=2;
	for (;;)
		{
		TInt r=CreateSession(KDMSSServerName,TVersion(0,0,0),50);
		if (r!=KErrNotFound && r!=KErrServerTerminated)
			return r;
		if (--retry==0)
			return r;
		r=StartServer();
		if (r!=KErrNone && r!=KErrAlreadyExists)
			return r;
		}
	}


EXPORT_C void RScreenSaverClient::Close()
	{
	RSessionBase::Close();  //basecall
	}
	
	
//EXPORT_C void RScreenSaverClient::GetScreenSaverListL(RArray< TUid >& aUids,CDesCArray& aIds)
EXPORT_C void RScreenSaverClient::GetScreenSaverListL(CDesCArray& aIds)
	{
	RDEBUG("	-> RScreenSaverClient: GetScreenSaverList ... STARTED!");
	TInt ssCount(0);
	TPckgBuf<TInt>count(ssCount);
	
	User::LeaveIfError(SendReceive(EScreenSaversCount,TIpcArgs(&count)));
	ssCount = count();
	RDEBUG_2(" RScreenSaverClient: GetScreenSaverList **SSCount : %d",ssCount);
	
	HBufC8* buffer = HBufC8::NewLC((sizeof(TUid)*ssCount)+sizeof(TInt8));
    TPtr8 bufPtr = buffer->Des();
    TInt err =  SendReceive(EGetAvailableScreenSavers,TIpcArgs(&bufPtr));
	
	if ( err == KErrNone || err == KErrOverflow )
		{
		RDesReadStream stream( bufPtr );
		CleanupClosePushL( stream );
		TInt count = stream.ReadInt8L();

		for ( TInt i = 0; i < count; ++i )
        	{
	        TUid uid = TUid::Uid(stream.ReadInt32L());
	        //aUids.AppendL( uid );
	        aIds.AppendL( uid.Name() );
        	}
        
		//aUids.SortSigned( );
		aIds.Sort( );
		
    	CleanupStack::PopAndDestroy(&stream);
		CleanupStack::PopAndDestroy(buffer);
		}
	RDEBUG("	-> RScreenSaverClient: GetScreenSaverList ... END!");
	}
	
	
EXPORT_C void RScreenSaverClient::GetScreenSaverListInfoL(RSSListInfoPtrArray &aScreenSaverList)
	{
	RDEBUG("	-> RScreenSaverClient: GetScreenSaverListInfo ... STARTED!");
	
	TInt ssCount(0);
	TPckgBuf<TInt>count(ssCount);
	User::LeaveIfError(SendReceive(EScreenSaversCount,TIpcArgs(&count)));
	
	ssCount = count();
	RDEBUG_2(" RScreenSaverClient: GetScreenSaverListInfo **SSCount : %d",ssCount);

	HBufC8* buffer = HBufC8::NewLC(KSSInfoBufferLength * ssCount);
    TPtr8 bufPtr = buffer->Des();

	TInt err =  SendReceive(EGetscreensaverInfo,TIpcArgs(&bufPtr));
	TInt lenght = bufPtr.Length();
	if ( err == KErrNone || err == KErrOverflow )
		{
		RDesReadStream stream( bufPtr );
		CleanupClosePushL( stream );
		TInt count = stream.ReadInt8L();
		
			
		for ( TInt i = 0; i < count; ++i )
    		{
	        TUid uid = TUid::Uid(stream.ReadInt32L());
	        TInt version = stream.ReadInt32L();
	        
	        TInt length = stream.ReadUint32L();
	        HBufC* name = HBufC::NewLC( stream,length  );
	        
	        length = stream.ReadUint32L();
	        HBufC8* opaqueData = HBufC8::NewLC( stream, length );
	        
	        
	        CScreenSaverListInfo *ScreensaverInfo = CScreenSaverListInfo::NewL(uid,
	        																   version,
	        																   name,
	 	       																   opaqueData);
	        //TInt error = aScreenSaverList.Append(ScreensaverInfo);
	        User::LeaveIfError(aScreenSaverList.Append(ScreensaverInfo));
	        
	        CleanupStack::PopAndDestroy(opaqueData); 
	        CleanupStack::PopAndDestroy(name); 
    		}
  	   	CleanupStack::PopAndDestroy(&stream);
		CleanupStack::PopAndDestroy(buffer);
		}
	RDEBUG("	-> RScreenSaverClient: GetScreenSaverListInfo ... Ended!");
	}



CScreenSaverListInfo* CScreenSaverListInfo::NewL(TUid aUid, 
												   TInt aVersion, 
												   TDesC*  aName, 
												   TDesC8* aOpaqueData)
	{
	CScreenSaverListInfo* self = CScreenSaverListInfo::NewLC(aUid, 
															aVersion, 
															aName, 
															aOpaqueData);
    CleanupStack::Pop(self);
    return self;
	}
												   

CScreenSaverListInfo* CScreenSaverListInfo::NewLC(TUid aUid, 
												   TInt aVersion, 
												   TDesC*  aName, 
												   TDesC8* aOpaqueData)
	{
	RDEBUG("CScreenSaverListInfo::NewLC - start");
	
	CScreenSaverListInfo* self = new(ELeave)CScreenSaverListInfo(aUid,aVersion);
	CleanupStack::PushL(self);
	self->ConstructL(aName,aOpaqueData);
	RDEBUG("CScreenSaverListInfo::NewLC - end");
	return self;
	}


CScreenSaverListInfo::CScreenSaverListInfo(TUid aUid, 
											TInt aVersion)
											
		
	:CBase(),
	iUid(aUid),
	iVersion(aVersion)								
	{
	//Do nothing here
	}

CScreenSaverListInfo::~CScreenSaverListInfo()
	{
	delete iName;
	iName = NULL;
	delete iOpaqueData;
	iOpaqueData = NULL;
	}

void CScreenSaverListInfo::ConstructL(TDesC*  aName, 
									  TDesC8* aOpaqueData)
	{
	iName = aName->AllocL();
	iOpaqueData = aOpaqueData->AllocL();
	}


EXPORT_C const TDesC& CScreenSaverListInfo::DisplayName() const
	{
	if(iName == NULL)
		return KNullDesC();
	else
		return *iName;
	}



EXPORT_C const TDesC8& CScreenSaverListInfo::OpaqueData() const
	{
	if(iOpaqueData == NULL)
		return KNullDesC8();
	else
		return *iOpaqueData;
	}


EXPORT_C TUid CScreenSaverListInfo::ImplementationUid() const
	{
	return iUid;
	}


EXPORT_C TInt CScreenSaverListInfo::Version() const
	{
	return iVersion;
	}
	
//End of file

