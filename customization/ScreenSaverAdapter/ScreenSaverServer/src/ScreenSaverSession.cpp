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
 

#include <mmf/common/mmfcontrollerpluginresolver.h>
#include "ScreenSaverSession.h"
#include "ScreenSaverClientServer.h"
#include <badesca.h>
#include "debug.h"
#include <ecom/implementationinformation.h>
#include <e32cmn.h>
#include <f32file.h>

CScreenSaverSession::CScreenSaverSession()
	{
	RDEBUG("CScreenSaverSession::CScreenSaverSession");
	}


CScreenSaverSession::~CScreenSaverSession()
	{
	RDEBUG("CScreenSaverSession::~CScreenSaverSession");
	Server().DropSession();
	}

CScreenSaverServer& CScreenSaverSession::Server()
	{
	return *static_cast<CScreenSaverServer*>(const_cast<CServer2*>(CSession2::Server()));
	}

// ----------------------------------------------------------------------------------------
// CScreenSaverSession::CreateL
// 2nd phase construct for sessions - called by the CServer framework
// ----------------------------------------------------------------------------------------
void CScreenSaverSession::CreateL()
	{
	RDEBUG("CScreenSaverSession::CreateL");
	Server().AddSession();
	
	}


void CScreenSaverSession::ServiceL(const RMessage2& aMessage)
	{
	TRAPD(err,DispatchMessageL(aMessage));
	aMessage.Complete(err);
	}

void CScreenSaverSession::ServiceError(const RMessage2& aMessage,TInt aError)
	{
	RDEBUG_2("CScreenSaverSession::ServiceError %d",aError);
	CSession2::ServiceError(aMessage,aError);
	}
	

void CScreenSaverSession::DispatchMessageL(const RMessage2& aMessage)
	{
	
	switch(aMessage.Function())
		{
		case EGetAvailableScreenSavers:
			{
			RDEBUG("	DispatchMessageL case: EGetAvailableScreenSavers");
			
			
			RImplInfoPtrArray screenSaverList;
			CleanupResetAndDestroyPushL( screenSaverList ); 
			Server().GetAvailableScreenSaversL(screenSaverList);
			
			TInt ssCount(0);
			ssCount = screenSaverList.Count();
			RDEBUG_2("	DispatchMessageL case: EGetAvailableScreenSavers : SSCount : %d",ssCount);
			HBufC8* buffer = HBufC8::NewLC((sizeof(TUid)*ssCount)+sizeof(TInt8));
   			TPtr8 bufPtr = buffer->Des();
   			
   			RDesWriteStream stream;
			stream.Open( bufPtr );
			
			CleanupClosePushL(stream);
			
			const TInt count = screenSaverList.Count();
			
			stream.WriteUint8L(TUint(count));
			for ( TInt i = 0 ; i < count ; i++ )
				{
				CImplementationInformation* implInfo = screenSaverList[i];
				stream.WriteInt32L( implInfo->ImplementationUid().iUid);
				}
			stream.CommitL();
		    aMessage.WriteL( 0, bufPtr, 0 );

		    CleanupStack::PopAndDestroy(&stream);
		    	
		    CleanupStack::PopAndDestroy(buffer);
		    	
		    CleanupStack::PopAndDestroy( &screenSaverList ); // screenSaverList
		    
			}
			break;
		
		case EGetscreensaverInfo:
			{
			RDEBUG("	DispatchMessageL case: EGetscreensaverInfo");
			
			RImplInfoPtrArray screenSaverList;
			CleanupResetAndDestroyPushL( screenSaverList ); 
			
			Server().GetAvailableScreenSaversL(screenSaverList);
			
			TInt ssCount(0);
			ssCount = screenSaverList.Count();
			RDEBUG_2("	DispatchMessageL case: EGetscreensaverInfo : SSCount : %d",ssCount);
			
			HBufC8* buffer = HBufC8::NewLC(KSSInfoBufferLength*ssCount);
   			TPtr8 bufPtr = buffer->Des();
   			
   			RDesWriteStream stream;
			stream.Open( bufPtr );
			CleanupClosePushL(stream);
			
			const TInt count = screenSaverList.Count();
			
			stream.WriteUint8L(TUint(count));
			
			for ( TInt i = 0 ; i < count ; i++ )
				{
					CImplementationInformation* implInfo = screenSaverList[i];
					
					stream.WriteInt32L( implInfo->ImplementationUid().iUid);	//Uid
					stream.WriteInt32L( implInfo->Version());					//Version
					
					stream.WriteInt32L( implInfo->DisplayName().Length());	//Dispaly  Name		
					stream << implInfo->DisplayName();
					
					stream.WriteInt32L( implInfo->OpaqueData().Length());   //Opaque Data
					stream << implInfo->OpaqueData();
					
				}
			stream.CommitL();
		    aMessage.WriteL( 0, bufPtr, 0 );

		    CleanupStack::PopAndDestroy(&stream);
		    CleanupStack::PopAndDestroy(buffer);
		    CleanupStack::PopAndDestroy( &screenSaverList );
		    
			}
			break;
		
		case EScreenSaversCount:
			{
			RDEBUG("	DispatchMessageL case: EScreenSaversCount");
				
			RImplInfoPtrArray screenSaverCount;
			CleanupResetAndDestroyPushL( screenSaverCount ); 
			TInt count(0);
			
			Server().GetAvailableScreenSaversL(screenSaverCount);
			
			count = screenSaverCount.Count();
			TPckgBuf<TInt>ssCount(count);

			aMessage.WriteL(0,ssCount);
			CleanupStack::Pop(&screenSaverCount)	;
		    screenSaverCount.ResetAndDestroy();
			
			}
			break;
		default:
			break;
		}
	}
	
