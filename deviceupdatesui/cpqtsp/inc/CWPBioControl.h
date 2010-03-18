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
*     Bio control for Provisioning documents.
*
*/


#ifndef CWPBIOCONTROL_H
#define CWPBIOCONTROL_H

// INCLUDES
#include <e32cmn.h>
#include <MSVAPI.H>
#include "CWPBootstrap.h"

class CWPEngine;
class CWPNameValue;
class CWPPushMessage;
class CWPBootstarp;

// CLASS DECLARATION

/**
 * Bio control for OMA Provisioning messages.
 * @since 2.0
 */
 
 class CDummyObserver : public CBase, public MMsvSessionObserver
{
public:
        void HandleSessionEventL(TMsvSessionEvent, TAny*, TAny*, TAny*) {};
};


class CWPBioControl//: public CMsgBioControl
    {
    public:  // Constructor and destructor

        /**
        * Two-phased constructor.
        * @param aObserver Reference to the Bio control observer.
        * @param aSession Reference to Message Server session.
        * @param aId Id of the message.
        * @param aEditorOrViewerMode Enum for the mode, is it as editor or viewer.
        * @param aFileName The newly created object.
        * @param aFile file handle to provisioning content.
        */
        IMPORT_C static CWPBioControl* NewL( CMsvSession* aSession,TMsvId aId);
       ~CWPBioControl();
        RPointerArray<CWPNameValue>* CollectItemsLC() const;        
	    CWPEngine* returnEngine();	       
	    
    private: // Constructors

        /**
        * Constructor is prohibited.
        * @param aObserver Reference to the Bio control observer.
        * @param aSession Reference to Message Server session.
        * @param aId Id of the message in Message Server.
        * @param aEditorOrViewerMode Enum for the mode, is it as editor or viewer.
        * @param aFileName The newly created object.
        * @param aFile file handle to provisioning content.
        */
        CWPBioControl( CMsvSession* aSession,TMsvId aId);
        void ConstructL(CMsvSession* aSession,TMsvId aId);
	
    private: 
        
        void AddItemsL();
        void CollateAndAddItemsL( RPointerArray<CWPNameValue>& aItems );        
        void DoSaveL();
        void AuthenticateL( CWPPushMessage& aMessage );
        void RestoreMsgL();
        static TInt Compare( const CWPNameValue& aImpl1,const CWPNameValue& aImpl2 );
        void SaveSettingsL();
        void SetCenrepKeyL(const TDesC8& aValue);
        void SaveMessageL();
        static void Cleanup( TAny* aAny );
        
    private: 
        CWPEngine*      iEngine;
        CWPPushMessage* iMessage;
        CDummyObserver* iObserver;
        CMsvSession* iSession;
        TMsvId iId;   
        TMsvEntry iEntry;
    };



#endif // CWPBIOCONTROL_H

// End of File
