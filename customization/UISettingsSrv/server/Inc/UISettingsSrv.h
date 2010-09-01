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
*        class definition for CUISettingsSrvServer and CUISettingsSrvSession
*
*/

#ifndef __UISettingsSrv_H__
#define __UISettingsSrv_H__

// INCLUDES
#include <e32base.h>
#include <f32file.h>
// #include "ScSelectableShortcutListObserver.h"
// #include "ScShortcutUids.h"
// #include "ScShortcutEngineObserver.h"
#include <gsasyncimagehandling.h>
#include <mgsasyncimagehandlingobserver.h>

// CONSTANTS
// MACROS
//#define ?macro ?macro_def

// DATA TYPES

// reasons for server panic
enum TUISettingsSrvPanic
	{
	EBadRequest,
	EBadDescriptor,
	EMainSchedulerError,
	ESvrCreateServer,
	ESvrStartServer,
	ECreateTrapCleanup,
	ENotImplementedYet
	};


// FUNCTION PROTOTYPES

// function to panic the server
GLREF_C void PanicServer(TUISettingsSrvPanic aPanic);
// thread function for server
GLREF_C TInt CountServerThread(TAny *);



// FORWARD DECLARATIONS

// class CScShortcutEngine;
class CScSelectableShortcutList;

// CLASS DECLARATION
const TUint KUISettingsSrvShutdownDelay = 120000000L;

class CShutdown : public CTimer
	{
public:
	inline CShutdown();
	inline void ConstructL();
	inline void Start();
	void RunL();
	};


//**********************************
//CUISettingsSrvServer
//**********************************
//The server class; an active object.
//Contains an instance of RServer; a handle to the kernel server representation which is used 
//to receive messages. 

class CUISettingsSrvServer : public CPolicyServer
                          // , public MScSelectableShortcutListObserver
                          // , public MScEngineObserver
                           , public MGSAsyncImageHandlingObserver
	{
public:
	static CUISettingsSrvServer* NewL();
	virtual ~CUISettingsSrvServer();

	virtual CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
	static TInt ThreadFunction(TAny* aStarted);

    // From MScSelectableShortcutListObserver
    void ScSSLOHandleListChangedL();
    void ScSSLOHandleListConstructionComplete(TInt aError);

    // From MScEngineObserver
    //TBool MScEngineHandleShortcutEvent(
    //                    TScShortcutEvent aEvent,
    //                    TScActionType aType
    //                    );

	void GetShortcutTargetListL();

    void AddListChangeNotifierL(TInt *aFlag);
    void RemoveListChangeNotifierL(TInt *aFlag);
	void SetShortcutListMaskL(CScSelectableShortcutList& aScAppList, TInt aMask);

	void GetShortcutTargetCountL(const RMessage2 &aMessage);
	void GetShortcutTargetTypeL(const RMessage2 &aMessage);
	void GetShortcutTargetAppUidL(const RMessage2 &aMessage);
	void GetShortcutTargetIndexFromATL(const RMessage2 &aMessage);
	void GetShortcutTargetIndexL(const RMessage2 &aMessage);
	void SetShortcutTargetAppIndexL(const RMessage2 &aMessage);
	void GetShortcutRtTypeL(const RMessage2 &aMessage);

	void SetShortcutTargetCaptionL(const RMessage2 &aMessage);
	void GetShortcutTargetCaptionL(const RMessage2 &aMessage);

	void SetShortcutTargetBitmapL(const RMessage2 &aMessage, TBool aIsMask=EFalse);
	void GetShortcutTargetBitmapL(const RMessage2 &aMessage, TBool aIsMask=EFalse);

	void SetSoftkeyBitmapL(const RMessage2 &aMessage, TBool aIsMask=EFalse);
	void GetSoftkeyBitmapL(const RMessage2 &aMessage, TBool aIsMask=EFalse);

    void SetStartupImageL( const TDesC &aFilePath );
	void SetStartupImageL(const RMessage2 &aMessage);
	void GetStartupImagePathL(const RMessage2 &aMessage);
	void SetStartupTextL(const RMessage2 &aMessage);
	void GetStartupTextL(const RMessage2 &aMessage);
	void GetStartupTextSizeL(const RMessage2 &aMessage);
	void SetStartupNoteTypeL(const RMessage2 &aMessage);
	void GetStartupNoteTypeL(const RMessage2 &aMessage);

    CFbsBitmap* LoadBitmapL( const TDesC& aFileName, const TDesC8& aType );

    // From MGSAsyncImageHandlingObserver
	void ImageHandlingCompleteL( TInt aError );

public:
	void AddSession();
	void RemoveSession();
	void Shutdown();

protected:
	CUISettingsSrvServer(TInt aPriority);
	void ConstructL();

private:
    CPolicyServer::TCustomResult CustomSecurityCheckL(const RMessage2& aMsg, TInt& aAction, TSecurityInfo& aMissing);

private:
	RFs         iFs;
	TInt		iActive;
	TInt        iSessionCount;
	CShutdown   iShutdownTimer;
	CActiveSchedulerWait iASWait;
//	CScShortcutEngine* iShortcuts;
	RArray<TInt32> iIndeces;
	RArray<TInt*> iNotifyChanges;
	CGSAsyncImageHandling* iImgHandling;
	CRepository* iStartupRep;
	CScSelectableShortcutList* iAppList;
	const TUid iIdleBitmapUid;
	const TUid iAIBitmapUid;
    TFileName iTemporaryImageFile;
    TInt iImageHandlingError;

#define UISETTINGSSRV_POLICYCOUNT  2
    static const CPolicyServer::TPolicy iConnectionPolicy;
    static const TInt   iRanges[UISETTINGSSRV_POLICYCOUNT];
    static const TUint8 iElementsIndex[UISETTINGSSRV_POLICYCOUNT];
	};

//**********************************
//CUISettingsSrvSession
//**********************************
//This class represents a session in the server.
//CSession::Client() returns the client thread.
//Functions are provided to respond appropriately to client messages.


class CUISettingsSrvSession : public CSession2
	{
public:
	
	// construct / destruct
	static CUISettingsSrvSession* NewL(CUISettingsSrvServer * aServer);
	
	~CUISettingsSrvSession();

	//service request
	virtual void ServiceL(const RMessage2 &aMessage);
	TBool DispatchMessageL(const RMessage2 &aMessage);

	void SetStartupImageL(const RMessage2 &aMessage);
	void GetStartupImagePathL(const RMessage2 &aMessage);
	void SetStartupTextL(const RMessage2 &aMessage);
	void GetStartupTextL(const RMessage2 &aMessage);
	void GetStartupTextSizeL(const RMessage2 &aMessage);
	void SetStartupNoteTypeL(const RMessage2 &aMessage);
	void GetStartupNoteTypeL(const RMessage2 &aMessage);

	void GetShortcutTargetListL( );
	void GetShortcutTargetCountL(const RMessage2 &aMessage);
	void GetShortcutTargetTypeL(const RMessage2 &aMessage);
    void GetShortcutTargetAppUidL(const RMessage2 &aMessage);
	void GetShortcutTargetIndexFromATL(const RMessage2 &aMessage);
	void GetShortcutTargetIndexL(const RMessage2 &aMessage);
	void SetShortcutTargetAppIndexL(const RMessage2 &aMessage);
	void GetShortcutRtTypeL(const RMessage2 &aMessage);

	void SetShortcutTargetCaptionL(const RMessage2 &aMessage);
	void GetShortcutTargetCaptionL(const RMessage2 &aMessage);

	void SetShortcutTargetBitmapL(const RMessage2 &aMessage, TBool aIsMask=EFalse);
	void GetShortcutTargetBitmapL(const RMessage2 &aMessage, TBool aIsMask=EFalse);

	void SetSoftkeyBitmapL(const RMessage2 &aMessage, TBool aIsMask=EFalse);
	void GetSoftkeyBitmapL(const RMessage2 &aMessage, TBool aIsMask=EFalse);

	void AppListCheckL();

	static HBufC8*  CopyParameter8L( const RMessage2 &aMsg, TInt aIndex );
	static HBufC16* CopyParameter16L( const RMessage2 &aMsg, TInt aIndex );

protected:
	// panic the client
	void PanicClient(const RMessagePtr2& aMessage,TInt aPanic);
	// safewrite between client and server
	void Write(const TAny* aPtr,const TDesC8& aDes,TInt anOffset=0);
	
	CUISettingsSrvSession(CUISettingsSrvServer * aServer);
	void ConstructL();

private:
	CUISettingsSrvServer *iServer;
	TInt iCount;
	TInt iListChanged;
	};

#endif // __UISettingsSrv_H__
