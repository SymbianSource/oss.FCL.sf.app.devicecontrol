/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef __TARMDmStreamAdapter_H__
#define __TARMDmStreamAdapter_H__

//  INCLUDES
#include <e32base.h>
#include <s32file.h> //RFileWriteStream
#include "TARMDmAdapter.h"

/**
* This class adds streaming support to CTARMDmAdapter
*
* To enable streaming in your adapter:
*
* 1) Inherit from CTARMDmStreamAdapter
* 2) Implement StreamingSupport (to return ETrue and set item size)
* 3) Implement StreamType to return the stream support type
*
* Stream types:
*
* - EStreamToBuffer to get data in aObject argument
* - EStreamToFile to get data in a file, file name is in iFileStoreFileName
* - EStreamToNone to use streaming methods of CTARMDmAdapter
*
* 4) Use CTARMDmStreamAdapter::StreamCommittedL()
*
* StreamCommittedL will complete operation with call to the non-streaming method:
*
* - _UpdateLeafObjectL or
* - _ExecuteCommandL
*
*
*/
class CTARMDmStreamAdapter : public CTARMDmAdapter
	{
public:
    enum TAdapterStreamType
        {
        EStreamToNone,        
        EStreamToBuffer,
        EStreamToFile
        };

    class CTARMDmStreamAdapterBuffer : public CBase
        {
        public:
        
            enum TTARMDMAction
                {
                ETARMActionNone,
                ETARMActionUpdate,
                ETARMActionExecute,
                };
        private:
            HBufC8 *iURI;
            HBufC8 *iLUID;
            HBufC8 *iType;
            TInt iStatusRef;
            TInt iResultRef;
            CTARMDmStreamAdapterBuffer::TTARMDMAction iAction; 
        public:
            CTARMDmStreamAdapterBuffer()
                : iURI( 0 )
                , iLUID( 0 )
                , iType( 0 )
                , iStatusRef( 0 )
                , iResultRef( 0 )
                , iAction( CTARMDmStreamAdapterBuffer::ETARMActionNone )
                {
                }
             static CTARMDmStreamAdapterBuffer* NewL(
                        const TDesC8 &aURI,
                        const TDesC8 &aLUID,
                        const TDesC8 &aType,
                        TInt aStatusRef,
                        TInt aResultRef,
                        CTARMDmStreamAdapterBuffer::TTARMDMAction aAction
                        )
                {
                CTARMDmStreamAdapterBuffer* newObject = new (ELeave) CTARMDmStreamAdapterBuffer();
                newObject->iURI  = aURI.AllocL();
                newObject->iLUID = aLUID.AllocL();
                newObject->iType = aType.AllocL();
                newObject->iStatusRef = aStatusRef;
                newObject->iResultRef = aResultRef;
                newObject->iAction = aAction;
                return newObject;
                }
            ~CTARMDmStreamAdapterBuffer()
                {
                delete iURI;
                delete iLUID;
                delete iType;
                }
            const TDesC8& URI() const
                {
                return *iURI;
                }
            const TDesC8& LUID() const
                {
                return *iLUID;
                }
            const TDesC8& Type() const
                {
                return *iType;
                }
            TInt StatusRef() const
                {
                return iStatusRef;
                }
            TInt ResultRef() const
                {
                return iResultRef;
                }
            CTARMDmStreamAdapterBuffer::TTARMDMAction Action() const
                {
                return iAction;
                }
        };

	// Implementation of MSmlDmAdapter interface functions that make policy checks
	// ===========================================================================
private:
	void UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, RWriteStream*& aStream, const TDesC8& aType, TInt aStatusRef );
	void ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, RWriteStream*& aStream, const TDesC8& aType, TInt aStatusRef );

	// Implementation of MSmlDmAdapter interface functions AFTER policy checks
	// =======================================================================
public:
	virtual void _UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aObject, const TDesC8& aType, TInt aStatusRef ) = 0;
	virtual void _ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aArgument, const TDesC8& aType, TInt aStatusRef ) = 0;
	virtual void _UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, RWriteStream*& aStream, const TDesC8& aType, TInt aStatusRef ) = 0;
	virtual void _ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, RWriteStream*& aStream, const TDesC8& aType, TInt aStatusRef ) = 0;

	// Other
	virtual TAdapterStreamType StreamType( const TDesC8& aURI );
	void StreamToBufferL( RWriteStream*& aStream );
	void StreamToFileL( RWriteStream*& aStream );
#ifdef __TARM_SYMBIAN_CONVERGENCY	
		virtual void StreamCommittedL( RWriteStream& aStream );
#else
		virtual void StreamCommittedL();
#endif	

    void SetActionInfoL( CTARMDmStreamAdapterBuffer::TTARMDMAction aAction,
                         const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aType,
                         TInt aStatusRef, TInt aResultRef );
    void ClearActionInfo();
    CTARMDmStreamAdapterBuffer& ActionInfoL();
    void CommitActionL( const TDesC8& aObject );

protected:
	CTARMDmStreamAdapter( MSmlDmCallback* aCallback );
	virtual ~CTARMDmStreamAdapter();

private:
    RFs                 iFs;
    RFileWriteStream    iWriteStream;
    TFileName           iFileStoreFileName;
    TAdapterStreamType  iStreamType;

    CTARMDmStreamAdapterBuffer *iActionInfo;
	};

#endif      // __TARMDmStreamAdapter_H__

// End of File
