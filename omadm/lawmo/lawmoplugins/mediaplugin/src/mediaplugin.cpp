/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Media plug-in for LAWMO
*
*/

#include <f32file.h>
#include <data_caging_path_literals.hrh>
#include <centralrepository.h>
#include "mediaplugin.h"
#include "lawmodebug.h"

// ----------------------------------------------------------------------------------------
// CLAWMOMediaPlugin::NewL
// ----------------------------------------------------------------------------------------
CLAWMOMediaPlugin* CLAWMOMediaPlugin::NewL(MLawmoPluginWipeObserver* aObserver)
	{
        RDEBUG( "CLAWMOMediaPlugin NewL");
        CLAWMOMediaPlugin* self=new(ELeave) CLAWMOMediaPlugin();  
		CleanupStack::PushL(self);
		self->ConstructL(aObserver); 
		if(self)
        RDEBUG( "CLAWMOMediaPlugin NewL self obj created");
	    CleanupStack::Pop();
		return self;
	}

// ----------------------------------------------------------------------------------------
// CLAWMOMediaPlugin::~CLAWMOMediaPlugin
// ----------------------------------------------------------------------------------------
CLAWMOMediaPlugin::~CLAWMOMediaPlugin()
	{
    RDEBUG( "CLAWMOMediaPlugin destructor");
    if(iQuery)
    	{
    	delete iQuery;
    	RDEBUG( "iQuery deleted");
      }
    if(iMdeSession)
    	{
    	delete iMdeSession;
    	RDEBUG( "iMdeSession deleted");
      }
    iRfs.Close();
	}

// ----------------------------------------------------------------------------------------
// CLAWMOMediaPlugin::CLAWMOMediaPlugin
// ----------------------------------------------------------------------------------------
CLAWMOMediaPlugin::CLAWMOMediaPlugin()
	{
	// See ConstructL() for initialisation completion.
	}

// ----------------------------------------------------------------------------------------
// CLAWMOMediaPlugin::ConstructL
// ----------------------------------------------------------------------------------------
void CLAWMOMediaPlugin::ConstructL(MLawmoPluginWipeObserver* aObserver)
{	
    RDEBUG( "CLAWMOMediaPlugin ConstructL");
    iState = EUninitialized;
	iObserver = aObserver;
    User::LeaveIfError(iRfs.Connect());
    RDEBUG( "CLAWMOMediaPlugin ConstructL done");
}

// Implementation of CLAWMOPluginInterface

// ----------------------------------------------------------------------------------------
// CLAWMOMediaPlugin::WipeL
// Wipes the Media.
// ----------------------------------------------------------------------------------------

void CLAWMOMediaPlugin::WipeL ()
{
    RDEBUG( "CLAWMOMediaPlugin WipeL()");
    iMdeSession = CMdESession::NewL( *this ); //MMdESessionObserver
    RDEBUG( "CLAWMOMediaPlugin WipeL() iMdeSession");
    iState = EInitialized;  
}
	
void CLAWMOMediaPlugin::HandleSessionOpened( CMdESession& /*aSession*/, TInt aError )
    {
	RDEBUG( "HandleSessionOpened start");
	iState = ESession;
	if(aError == KErrNone)
	    {
		TRAPD(err,StartMdsQueryL());
		if(err!=KErrNone)
		    iObserver->HandleWipeCompleted(KErrGeneral);
	    }
	else
	    iObserver->HandleWipeCompleted(aError);
    RDEBUG( "HandleSessionOpened done");	
	}

// ----------------------------------------------------------------------------
// CMdsQueryHandler::HandleSessionError
// From MMdESessionObserver
// ----------------------------------------------------------------------------
//
void CLAWMOMediaPlugin::HandleSessionError( CMdESession& /*aSession*/, TInt aError )
    {
    RDEBUG( "CLAWMOMediaPlugin HandleSessionError()");
	iState = EError;
	iObserver->HandleWipeCompleted(aError);
    }
	
void CLAWMOMediaPlugin::StartMdsQueryL()
    {
    RDEBUG( "CLAWMOMediaPlugin StartMdsQuery() start");
    if(iState == ESession)
        {
        iState = ESession;
        TInt counT = iMdeSession->NamespaceDefCount();
        RDEBUG_2("NamespaceDefCount  %d", counT);

        CMdENamespaceDef& defaultNamespaceDef = iMdeSession->GetDefaultNamespaceDefL();
        RDEBUG( "HandleSessionOpened StartMdsQuery() defaultNamespaceDef");
        CMdEObjectDef& imageObjDef = defaultNamespaceDef.GetObjectDefL(MdeConstants::Image::KImageObject);
        RDEBUG( "RunL() imageObjDef");

        //CMdEPropertyDef& itemProperty = imageObjDef.GetPropertyDefL( MdeConstants::Object::KItemTypeProperty );
        CMdEPropertyDef& titleProperty = imageObjDef.GetPropertyDefL( MdeConstants::Object::KTitleProperty );

        RDEBUG( "RunL() titleProperty");
        iQuery = iMdeSession->NewObjectQueryL( iMdeSession->GetDefaultNamespaceDefL(),
                                    imageObjDef,
                                    this );
        iQuery->AddPropertyFilterL(&titleProperty);
        
        RDEBUG( "RunL() NewObjQuery");                                 
        //_LIT(KJpegMime, "image/jpeg");
        //CMdELogicCondition& rootCondition = iQuery->Conditions();
        //rootCondition.AddPropertyConditionL(itemProperty, ETextPropertyConditionCompareEquals,KJpegMime);
        //RDEBUG( "RunL()LogicCondition");
        iState = EImage;
        iQuery->FindL(); // starts the object query
        RDEBUG( "RunL() FindL");    
        }
    else if(iState == EVideo)
        {
        CMdENamespaceDef& defaultNamespaceDef = iMdeSession->GetDefaultNamespaceDefL();
        RDEBUG( "StartMdsQuery StartVideoL() defaultNamespaceDef");
        CMdEObjectDef& vidObjDef = defaultNamespaceDef.GetObjectDefL(MdeConstants::Video::KVideoObject);
        RDEBUG( "StartMdsQuery StartVideoL() imageObjDef");
        
        iQuery = iMdeSession->NewObjectQueryL( iMdeSession->GetDefaultNamespaceDefL(),
                                            vidObjDef,
                                            this );
        RDEBUG( "StartMdsQuery StartVideoL() NewObjQuery");                                 
        CMdEPropertyDef& titleProperty = vidObjDef.GetPropertyDefL( MdeConstants::Object::KTitleProperty );
        iQuery->AddPropertyFilterL(&titleProperty);
        RDEBUG( "StartMdsQuery StartVideoL() BeforeFindL");
        iQuery->FindL(); // starts the object query
        RDEBUG( "StartMdsQuery StartVideoL() FindL");
        }
    RDEBUG( "CLAWMOMediaPlugin StartMdsQuery() done");
    }

void CLAWMOMediaPlugin::HandleQueryCompleted( CMdEQuery& aQuery, TInt aError )
  {
    // query is completed
    RDEBUG( "CLAWMOMediaPlugin HandleQueryCompleted, Begin");
    RDEBUG_2("CLAWMOMediaPlugin error %d", aError);
    RDEBUG_2("aQuery.Count %d", aQuery.Count());
    TBool comp = iQuery->IsComplete();
    RDEBUG_2("Query complete ? %d",comp);
    
    CMdEObjectQuery& query = (CMdEObjectQuery &) aQuery;
    if( aQuery.Count() > 0 && aError == KErrNone )
    {
    // some items were found!
    RDEBUG( "CLAWMOMediaPlugin HandleQueryCompleted, objects found");
    for(TInt i=0;i<aQuery.Count();i++)
        {
        TBuf<255> uri;
        CMdEObject* obj = (CMdEObject*)query.TakeOwnershipOfResult(i);
        RDEBUG( "CLAWMOMediaPlugin HandleQueryCompleted, own one obj");
        uri.Copy(obj->Uri());
        RDEBUG_2("HandleQueryCompleted URI %s", uri.PtrZ());
        if(iRfs.IsValidName(uri))
            {
            RDEBUG( "CLAWMOMediaPlugin HandleQueryCompleted file exists");
            iRfs.Delete ( uri );
            }
        delete obj;
        }
    }
	// After deletion, decide if its video/image and do accordingly
    if(iState == EVideo)
        {
        iObserver->HandleWipeCompleted(aError);
        }
    else if(iState == EImage)
        {
        iState = EVideo;
        RDEBUG( "CLAWMOMediaPlugin HandleQueryCompleted() ****reqcomplete");
        TRAPD(err,StartMdsQueryL());
        if(err!=KErrNone)
            iObserver->HandleWipeCompleted(KErrGeneral);
        }
    RDEBUG( "CLAWMOMediaPlugin HandleQueryCompleted, out of loop");
  }

void CLAWMOMediaPlugin::HandleQueryNewResults(CMdEQuery& aQuery,
						   TInt aFirstNewItemIndex,
						   TInt aNewItemCount)
   {
    RDEBUG( "CLAWMOMediaPlugin HandleQueryNewResults");    
   }

//
