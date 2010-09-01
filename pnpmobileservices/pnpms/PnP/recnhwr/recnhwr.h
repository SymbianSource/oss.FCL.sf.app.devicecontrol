/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of PnPMS components
*
*/



#ifndef __REC_NHWR_H__
#define __REC_NHWR_H__

class CApaNhwrRecognizer : public CApaDataRecognizerType
    {
public: // from CApaDataRecognizerType
    CApaNhwrRecognizer();
    ~CApaNhwrRecognizer();
    TUint PreferredBufSize();
    TDataType SupportedDataTypeL(TInt aIndex) const;

    static CApaDataRecognizerType* CreateRecognizerL();

private: // from CApaDataRecognizerType
    void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);

    };

#endif // #ifndef __REC_NHWR_H__
