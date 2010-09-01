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
* Description:  $Description: $
*
*/



// INCLUDE FILES
#include "TARMDmStreamAdapter.h"
#include "debug.h"
#include <featmgr.h>
_LIT( KTempFilePath, "C:\\system\\temp\\" );


// ============================ MEMBER FUNCTIONS ===============================


///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CTARMDmStreamAdapter
//
///////////////////////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// CTARMDmStreamAdapter::CTARMDmStreamAdapter
// 
// -----------------------------------------------------------------------------
//
CTARMDmStreamAdapter::CTARMDmStreamAdapter( MSmlDmCallback* aCallback )
	: CTARMDmAdapter( aCallback )
	, iActionInfo( 0 )
	{
	RDEBUG( "CTARMDmStreamAdapter::CTARMDmStreamAdapter" );
	if(!iFs.Connect())
		{
			RDEBUG( "CTARMDmStreamAdapter::CTARMDmStreamAdapter unable to connect to the fileserver" );
		}
	}

// -----------------------------------------------------------------------------
// CTARMDmStreamAdapter::~CTARMDmStreamAdapter
// 
// -----------------------------------------------------------------------------
//
CTARMDmStreamAdapter::~CTARMDmStreamAdapter()
	{
	RDEBUG( "CTARMDmStreamAdapter::~CTARMDmStreamAdapter >" );
	ClearActionInfo();
    iFs.Close();
	RDEBUG( "CTARMDmStreamAdapter::~CTARMDmStreamAdapter <" );
	}

// -----------------------------------------------------------------------------
// CTARMDmStreamAdapter::
// 
// -----------------------------------------------------------------------------
//
void CTARMDmStreamAdapter::UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, RWriteStream*& aStream, const TDesC8& aType, TInt aStatusRef )
	{
	RDEBUG( "CTARMDmStreamAdapter::UpdateLeafObjectL >" );

//#ifdef __SAP_POLICY_MANAGEMENT
TRAPD( errf, FeatureManager::InitializeLibL() );
	if( errf != KErrNone )
	{
		User::Leave(errf);
	}
if(FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement ))
{
	if (CheckPolicyL( aURI ) != KErrNone)
		{
		Callback().SetStatusL( aStatusRef, EError );
		return;
		}
}
FeatureManager::UnInitializeLib();

	
//#endif

	SetActionInfoL(CTARMDmStreamAdapterBuffer::ETARMActionUpdate,
	               aURI,
	               aLUID,
	               aType,
	               aStatusRef,
	               -1); // No result ref

    TInt itemSize;
    TAdapterStreamType streamType = EStreamToNone;
	if( StreamingSupport( itemSize ) )
	    {
	    streamType = StreamType( aURI );
	    }

    if( EStreamToNone != streamType )
        {
        if( EStreamToFile == streamType )
            {
            StreamToFileL( aStream );
            }
        else
        if( EStreamToBuffer == streamType )
            {
            StreamToBufferL( aStream );
            }
        Callback().SetStatusL( aStatusRef, EOk );  
        }
    else
        {
    	_UpdateLeafObjectL( aURI, aLUID, aStream, aType, aStatusRef );
        }

	RDEBUG( "CTARMDmStreamAdapter::UpdateLeafObjectL <" );
	}

// -----------------------------------------------------------------------------
// CTARMDmStreamAdapter::
// 
// -----------------------------------------------------------------------------
//
void CTARMDmStreamAdapter::ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, RWriteStream*& aStream, const TDesC8& aType, TInt aStatusRef )
	{
	RDEBUG( "CTARMDmStreamAdapter::ExecuteCommandL (stream) >" );

//#ifdef __SAP_POLICY_MANAGEMENT
TRAPD( errf, FeatureManager::InitializeLibL() );
	if( errf != KErrNone )
	{
		User::Leave(errf);
	}
if(FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement ))
{
	if (CheckPolicyL( aURI ) != KErrNone)
		{
		Callback().SetStatusL( aStatusRef, EError );
		return;
		}
}
FeatureManager::UnInitializeLib();
//#endif		

	SetActionInfoL(CTARMDmStreamAdapterBuffer::ETARMActionExecute,
	               aURI,
	               aLUID,
	               aType,
	               aStatusRef,
	               -1); // no result ref

    TInt itemSize;
    TAdapterStreamType streamType = CTARMDmStreamAdapter::EStreamToNone;
	if( StreamingSupport( itemSize ) )
	    {
	    streamType = StreamType( aURI );
	    }

    if( CTARMDmStreamAdapter::EStreamToNone != streamType )
        {
        if( CTARMDmStreamAdapter::EStreamToFile == streamType )
            {
            StreamToFileL( aStream );
            }
        else
        if( CTARMDmStreamAdapter::EStreamToBuffer == streamType )
            {
            StreamToBufferL( aStream );
            }
        Callback().SetStatusL( aStatusRef, EOk ); 
        }
    else
        {
        _ExecuteCommandL( aURI, aLUID, aStream, aType, aStatusRef );
        }

	RDEBUG( "CTARMDmStreamAdapter::ExecuteCommandL (stream) <" );
	}

// -----------------------------------------------------------------------------
// CTARMDmStreamAdapter::StreamType
// 
// -----------------------------------------------------------------------------
//
CTARMDmStreamAdapter::TAdapterStreamType CTARMDmStreamAdapter::StreamType( const TDesC8& /*aURI*/ )
    {
	RDEBUG( "CTARMDmStreamAdapter::StreamType ><" );
    
    return CTARMDmStreamAdapter::EStreamToNone;
    }

// -----------------------------------------------------------------------------
// CTARMDmStreamAdapter::StreamToBufferL
// 
// -----------------------------------------------------------------------------
//
void CTARMDmStreamAdapter::StreamToBufferL( RWriteStream*& aStream )
    {
	RDEBUG( "CTARMDmStreamAdapter::StreamToBufferL >" );
    
    User::LeaveIfError( iWriteStream.Temp(
                    iFs,
                    KTempFilePath(),
                    iFileStoreFileName,
                    EFileWrite
                    ));

    aStream = &iWriteStream;
    // StreamCommittedL is called when stream is complete
    iStreamType = CTARMDmStreamAdapter::EStreamToBuffer;


	RDEBUG( "CTARMDmStreamAdapter::StreamToBufferL <" );
    }

// -----------------------------------------------------------------------------
// CTARMDmStreamAdapter::StreamToFileL
// 
// -----------------------------------------------------------------------------
//
void CTARMDmStreamAdapter::StreamToFileL( RWriteStream*& aStream )
    {
	RDEBUG( "CTARMDmStreamAdapter::StreamToFileL >" );


    User::LeaveIfError( iWriteStream.Temp(
                    iFs,
                    KTempFilePath(),
                    iFileStoreFileName,
                    EFileWrite
                    ));

    aStream = &iWriteStream;
    // StreamCommittedL is called when stream is complete
    iStreamType = CTARMDmStreamAdapter::EStreamToFile;
    

	RDEBUG( "CTARMDmStreamAdapter::StreamToFileL <" );
    }

// -----------------------------------------------------------------------------
// CTARMDmStreamAdapter::StreamCommittedL
// 
// -----------------------------------------------------------------------------
//
#ifdef __TARM_SYMBIAN_CONVERGENCY	
void CTARMDmStreamAdapter::StreamCommittedL( RWriteStream& /*aStream*/ )
#else
void CTARMDmStreamAdapter::StreamCommittedL()
#endif	
    {
	RDEBUG( "CTARMDmStreamAdapter::StreamCommittedL >" );

    TInt itemSize = 0;
    if( StreamingSupport( itemSize ) )
        {
        
        TAdapterStreamType type = iStreamType;
        iStreamType = CTARMDmStreamAdapter::EStreamToNone;

        if(CTARMDmStreamAdapter::EStreamToNone == type )
            {
            ClearActionInfo();
            User::Leave( KErrGeneral );
            }

        iWriteStream.Close();

        if(CTARMDmStreamAdapter::EStreamToBuffer == type )
            {           
            // Load file to memory
            RFile f;
            User::LeaveIfError( f.Open( iFs, iFileStoreFileName, EFileRead ) );
            CleanupClosePushL( f );

            TInt size;
            User::LeaveIfError( f.Size( size ) );

            HBufC8* buf = HBufC8::NewLC( size );
            TPtr8 ptr8( buf->Des() );
            User::LeaveIfError( f.Read( ptr8 ) );

            // Pass data as argument
            CommitActionL( ptr8 );

            CleanupStack::PopAndDestroy( buf );
            CleanupStack::PopAndDestroy( &f );
            }
         else
            {
            // Streamed to file, no data in arguments
            CommitActionL( KNullDesC8() );
            }

        iFs.Delete( iFileStoreFileName );
        
        }

    ClearActionInfo();

	RDEBUG( "CTARMDmStreamAdapter::StreamCommittedL <" );
    }

// -----------------------------------------------------------------------------
// CTARMDmStreamAdapter::SetActionInfoL
// 
// -----------------------------------------------------------------------------
//
void CTARMDmStreamAdapter::SetActionInfoL( CTARMDmStreamAdapterBuffer::TTARMDMAction aAction,
                                     const TDesC8& aURI,
                                     const TDesC8& aLUID,
                                     const TDesC8& aType,
                                     TInt aStatusRef,
                                     TInt aResultRef )
    {
	RDEBUG( "CTARMDmStreamAdapter::SetActionInfoL >" );

    ClearActionInfo();
    iActionInfo = CTARMDmStreamAdapterBuffer::NewL(
                        aURI,
                        aLUID,
                        aType,
                        aStatusRef,
                        aResultRef,
                        aAction
                        );

	RDEBUG( "CTARMDmStreamAdapter::SetActionInfoL <" );
    }

// -----------------------------------------------------------------------------
// CTARMDmStreamAdapter::ClearActionInfo
// 
// -----------------------------------------------------------------------------
//
void CTARMDmStreamAdapter::ClearActionInfo()
    {
	RDEBUG( "CTARMDmStreamAdapter::ClearActionInfo >" );

    delete iActionInfo;
    iActionInfo = 0;

	RDEBUG( "CTARMDmStreamAdapter::ClearActionInfo <" );
    }

// -----------------------------------------------------------------------------
// CTARMDmStreamAdapter::ActionInfoL
// 
// -----------------------------------------------------------------------------
//
CTARMDmStreamAdapter::CTARMDmStreamAdapterBuffer& CTARMDmStreamAdapter::ActionInfoL()
    {
	RDEBUG( "CTARMDmStreamAdapter::ActionInfoL >" );

    if( 0 == iActionInfo )
        {
        User::Leave( KErrNotFound );
        }

	RDEBUG( "CTARMDmStreamAdapter::ActionInfoL <" );
    return *iActionInfo;
    }

// -----------------------------------------------------------------------------
// CTARMDmStreamAdapter::CommitActionL
// 
// -----------------------------------------------------------------------------
//
void CTARMDmStreamAdapter::CommitActionL( const TDesC8 &aObject )
    {
	RDEBUG( "CTARMDmStreamAdapter::CommitActionL >" );

    CTARMDmStreamAdapterBuffer& action = ActionInfoL();

    switch( action.Action() )
        {
        case CTARMDmStreamAdapterBuffer::ETARMActionUpdate:
    	_UpdateLeafObjectL(
    	            action.URI(),
    	            action.LUID(),
    	            aObject,
    	            action.Type(),
    	            action.StatusRef()
    	            );
        break;

        case CTARMDmStreamAdapterBuffer::ETARMActionExecute:
        _ExecuteCommandL(
    	            action.URI(),
    	            action.LUID(),
    	            aObject,
    	            action.Type(),
    	            action.StatusRef() 
                    );
        break;

        default:
        User::Leave( KErrNotSupported );
        break;
        }

	RDEBUG( "CTARMDmStreamAdapter::CommitActionL <" );
    }

// End of File
