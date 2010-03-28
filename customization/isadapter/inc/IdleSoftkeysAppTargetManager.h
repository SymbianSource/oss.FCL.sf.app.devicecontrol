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
* Description:  DM Idle SoftKey Adapter
*
*/


#ifndef __IdleSoftkeysAppTargetManager_H__
#define __IdleSoftkeysAppTargetManager_H__

// symbian
#include <e32base.h>
#include <smldmadapter.h>

class CUiSettingsUtil;

class CIdleSoftkeysAppTargetManager : public CBase
    {

    public:

    
    CIdleSoftkeysAppTargetManager(MSmlDmCallback *& aCallBack);
    ~CIdleSoftkeysAppTargetManager();
    
    //
    // Targets
    //
    TInt        ListTargetsL                            (
                                                        CBufFlat &aList
                                                        );

    HBufC8*     GenerateNodeNameL                       (
                                                        TInt aIndex
                                                        );

    void        GetTargetFromNodeNameL                  (
                                                        const TDesC8& aNodeName,
                                                        TInt &aIndex,
                                                        TDes8 &aTargetCaption
                                                        );

    void        SetCaptionL                             (
                                                        const TDesC8& aURI,
                                                        const TDesC8& aCaption
                                                        );

    private:
	
	TBool					iUiSettingsNotInitialized;
    MSmlDmCallback*         &iCallBack;
    CUiSettingsUtil 		*iUiSettings;
    };

#endif //__IdleSoftkeysAppTargetManager_H__


