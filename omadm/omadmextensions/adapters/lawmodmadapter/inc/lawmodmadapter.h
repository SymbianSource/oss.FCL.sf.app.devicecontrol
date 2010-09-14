/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  LAWMO DM Adapter
*
*/

#ifndef __LAWMODMADAPTER_H
#define __LAWMOMADAPTER_H

// INCLUDES
#include <e32base.h>
#include <smldmadapter.h>
#include <centralrepository.h>
#include <lawmoclient.h>
#include <dcmoconst.h>

const TInt KMaxBufferLength = 256;

_LIT8( KMimeType, "text/plain" );
_LIT8( KNSmlLAWMOAdapterTitle,      "LAWMO DM Adapter" );
_LIT8( KNSmlTrue,					"true" );
_LIT8( KNSmlFalse,					"false" );
_LIT8( KLAWMODMAdapterVersion, "1.0" ); // Adapter version

//DDF accepted DM-fieldnames
_LIT8( KNSmlDdfState,				"State" );
_LIT8( KNSmlDdfAvailWipeList,		"AvailableWipeList" );
_LIT8( KNSmlDdfListItemName,		"ListItemName" );
_LIT8( KNSmlDdfToBeWiped,           "ToBeWiped" );
_LIT8( KNSmlDdfOperations,			"Operations" );
_LIT8( KNSmlDdfFullyLock,		    "FullyLock" );
_LIT8( KNSmlDdfPartLock,			"PartiallyLock" );
_LIT8( KNSmlDdfUnlock,	            "UnLock" );
_LIT8( KNSmlDdfFactoryReset,		"FactoryReset" );
_LIT8( KNSmlDdfWipe,			    "Wipe" );
_LIT8( KNSmlDdfEnable,				"Enable" );
_LIT8( KNSmlDdfDisable,				"Disable" );
_LIT8( KNSmlDdfOpExt,				"Ext" ); 
_LIT8( KNSmlDdfOpExtWipeAll,	    "WipeAll" );
_LIT8( KNSmlDdfLawMoConfig,         "LAWMOConfig" );

//DDF DM-field descriptions
_LIT8( KNSmlDdfStateDescription,             "State of LAWMO" );
_LIT8( KNSmlDdfAWLDescription,               "Available WipeList node" );
_LIT8( KNSmlDdfDynNodeDescription,           "Dynamic node" );
_LIT8( KNSmlDdfListDescription,              "Name of List Item" );
_LIT8( KNSmlDdfToWipeDescription,            "Item to be wiped or not" );
_LIT8( KNSmlDdfOperationsDescription,        "Operations node" );
_LIT8( KNSmlDdfFullLockDescription,          "Fully lock operation" );
_LIT8( KNSmlDdfPartLockDescription,          "Partially lock operation" );
_LIT8( KNSmlDdfUnlockDescription,            "Unlock operation" );
_LIT8( KNSmlDdfFactResetDescription,         "Factory Reset operation" );
_LIT8( KNSmlDdfWipeDescription,              "Wipe operation" );
_LIT8( KNSmlDdfExtDescription ,              "Extra Operations " );
_LIT8( KNSmlDdfOpExtDescription,             "Wipe All Operation" );
_LIT8( KNSmlDdfLawConfigDescription,         "Configuration node" );


_LIT8( KLawmoMatch2,                                          "/" );
_LIT8( KSegmLawmo,                                            "AvailableWipeList/Ext/LAWMOConfig/Operations/State");
_LIT8( KLawmoAvWipe,                                          "AvailableWipeList" );
_LIT8( KLawmoAvWipe2,                                         "AvailableWipeList/*" );
_LIT8( KSegmLawmo2,                                            "Messages");
_LIT8( KSegmLawmo3,                                            "ListItemName/ToBeWiped");
_LIT8( KLawmoOperationsMatch,                                   "Operations");
_LIT8( KSegmLawmoOperations,                                    "FactoryReset/FullyLock/PartiallyLock/UnLock/Wipe");
_LIT8( KLawmoOperationsExtMatch,                                "Ext");

_LIT8( KLawmoMetatype,          "urn:oma:at:lawmo:1.0:OperationComplete" );
_LIT8( KLawmoMetaformat,        "text/plain" );
_LIT8( KLawmoMetamark,          "warning" );

// CLASS DECLARATION

/**
* Adapter class
* Provides DM adapter interface
*/
class CLawmoDMAdapter : public CSmlDmAdapter
    {
    
    public: // Constructors, destructor
        /**
        * Create instance of CLawmoDMAdapter
        * @param aDmCallBack
        * @return Instance of CLawmoDMAdapter
        */    
        static CLawmoDMAdapter* NewL( MSmlDmCallback* aDmCallback );

        /**
        * Create instance of CLawmoDMAdapter
        * @param aDmCallBack
        * @return Instance of CLawmoDMAdapter
        */   
        static CLawmoDMAdapter* NewLC( MSmlDmCallback* aDmCallback );

        /**
        * C++ Destructor
        */
        virtual ~CLawmoDMAdapter();

    public:
	    /**
	    * The function returns current version of the DDF.
	    * By asking current DDF versions from adapters DM Module can control
	    * possible changes in the data structure and send the changed DDF
	    * description to a management server.
	    * This function is always called after DDFStructureL.
        * @param aVersion DDF version of the adapter. (filled by the adapter)
	    * @publishedPartner
	    * @prototype
	    */
	    void DDFVersionL( CBufBase& aVersion );
	
	    /**
	    * The function for filling the DDF structure of the adapter
	    * This function is only called once, immediately after the adapter is created.
	    * @param aDDFObject	Reference to root object. A DM adapter starts filling
		         the data structure by calling AddChildObjectL to the root object and
				 so describes the DDF of the adapter. 
	    * @publishedPartner
	    * @prototype
	    */
        void DDFStructureL( MSmlDmDDFObject& aDDF );
	
	    /**
	    * The function creates new leaf objects, or replaces data in existing leaf
	    * objects. The information about the success of the command should be
	    * returned by calling SetStatusL function of MSmlDmCallback callback
	    * interface. This makes it possible to buffer the commands. However, all
	    * the status codes for buffered commands must be returned at the latest when
	    * the adapter's CompleteOutstandingCmdsL() is called.
	    * @param aURI			URI of the object
	    * @param aLUID		LUID of the object (if the adapter has earlier returned a
	 	     				LUID to the DM Module). For new objects, this is the LUID
 			    			inherited through the parent node.
 	    * @param aObject		Data of the object. 
	    * @param aType		MIME type of the object
	    * @param aStatusRef	Reference to correct command, i.e. this reference
	 					must be used when calling the SetStatusL of this command
	    * @publishedPartner
	    * @prototype
	    */
	    void UpdateLeafObjectL( const TDesC8& aURI, 
	                            const TDesC8& aLUID,
			                    const TDesC8& aObject, 
			 				    const TDesC8& aType,
							    TInt aStatusRef 
							  );
		
        /**
	    * The function creates new leaf objects, or replaces data in existing leaf
	    * objects, in the case where data is large enough to be streamed. The
	    * information about the success of the command should be returned by calling
	    * SetStatusL function of MSmlDmCallback callback interface. This makes it
	    * possible to buffer the commands.  However, all the status codes for buffered
	    * commands must be returned at the latest when the CompleteOutstandingCmdsL()
	    * of adapter is called.
	    * @param aURI		URI of the object
	    * @param aLUID		LUID of the object (if the adapter has earlier returned a
    						LUID to the DM Module). For new objects, this is the LUID
	    					inherited through the parent node.
	    * @param aStream	Data of the object. Adapter should create write stream
		    				and return, when data is written to stream by DM agent,
			    			StreamCommittedL() is called by DM engine
	    * @param aType		MIME type of the object
	    * @param aStatusRef	Reference to correct command, i.e. this reference
    						must be used when calling the SetStatusL of this
		    				command.
	    * @publishedPartner
	    * @prototype
	    */
	    void UpdateLeafObjectL( const TDesC8& aURI, 
	                            const TDesC8& aLUID,
								RWriteStream*& aStream, 
								const TDesC8& aType,
		 						TInt aStatusRef 
		 				      );
	    
	    
	    TInt InternalUpdateObjectL( const TDesC8& aURI, 
                                    const TDesC8& /*aLUID*/,
                                    const TDesC8& aObject, 
                                    const TDesC8& /*aType*/,
                                    TInt aStatusRef );
	    /**
	    * The function deletes an object and its child objects. The SetStatusL
	    * should be used as described in UpdateLeafObjectL()
	    * @param aURI		URI of the object
	    * @param aLUID		LUID of the object (if the adapter have earlier returned
    						LUID to the DM Module).
	    * @param aStatusRef	Reference to correct command, i.e. this reference must
						be used when calling the SetStatusL of this command.
        * @publishedPartner
	    * @prototype
	    */
	    void DeleteObjectL( const TDesC8& aURI, 
	                        const TDesC8& aLUID,
							TInt aStatusRef 
					      );
	
	    /**
	    * The function fetches data of a leaf object. The SetStatusL should be used
	    * as described in UpdateLeafObjectL(). The data is returned by using the
	    * SetResultsL function of MSmlCallback callback interface, and may be streamed.
	    * @param aURI			URI of the object
	    * @param aLUID			LUID of the object (if the adapter have earlier
    							returned LUID to the DM Module).   
	    * @param aType 			MIME type of the object
	    * @param aResultsRef	Reference to correct results, i.e. this reference
    							must be used when returning the result by calling
	    						the SetResultsL.
	    * @param aStatusRef		Reference to correct command, i.e. this reference
		    					must be used when calling the SetStatusL of this
    							command.
	    * @publishedPartner
	    * @prototype
	    */
	    void FetchLeafObjectL( const TDesC8& aURI, 
	                           const TDesC8& aLUID,
							   const TDesC8& aType, 
							   TInt aResultsRef,
							   TInt aStatusRef 
							 );
	    
	    TInt InternalFetchObjectL(  const TDesC8& aURI, 
                                    const TDesC8& aLUID, const TDesC8& aType, 
                                    CBufFlat* aObject,
                                    TInt aStatusRef );
	    
	    /**
	    * The function fetches the size of the data of a leaf object. The size is
	    * in bytes, and must reflect the number of bytes that will be transferred
	    * when the framework calls FetchLeafObjectL. The SetStatusL should be used
	    * as described in FetchLeafObjectL(). The size value is returned by using
	    * the SetResultsL function of MSmlCallback callback interface, and must be
	    * a decimal integer expressed as a string, eg. "1234".
	    * Results from this call MUST NOT be streamed.
	    * @param aURI			URI of the object
	    * @param aLUID			LUID of the object (if the adapter have earlier
    							returned LUID to the DM Module).   
	    * @param aType 			MIME type of the object
	    * @param aResultsRef	Reference to correct results, i.e. this reference
    							must be used when returning the result by calling
	    						the SetResultsL.
	    * @param aStatusRef		Reference to correct command, i.e. this reference
     							must be used when calling the SetStatusL of this
	    						command.
	    * @publishedPartner
	    * @prototype
	    */
	    void FetchLeafObjectSizeL( const TDesC8& aURI, 
	                               const TDesC8& aLUID,
								   const TDesC8& aType, 
								   TInt aResultsRef,
								   TInt aStatusRef 
								 );
	    /**
	    * The function fetches URI list. An adapter returns the list of URI segments
	    * under the given URI be separated by slash ("/"). The URI segment names for
	    * new objects must be given by the adapter.
	    * The list is returned by calling the SetResultsL function of MSmlCallback
	    * callback interface.	Results from this call MUST NOT be streamed.
	    * @param aParentURI					URI of the parent object
	    * @param aParentLUID				LUID of the parent object (if the
    										adapter have earlier returned LUID to
		    								the DM Module).   
	    * @param aPreviousURISegmentList	URI list with mapping LUID information,
			    							which is known by DM engine. An adapter
				    						can use this information when verifying
					    					if old objects still exists. An adapter
						    				also knows what objects are new to DM
							    			engine and can provide LUID mapping for
								    		them. aPreviousURISegmentList parameter
									    	(see above) helps to recognise new
    										objects.
	    * @param aResultsRef				Reference to correct results, i.e. this
    										reference must be used when returning
	    									the result by calling the SetResultsL.
	    * @param aStatusRef					Reference to correct command, i.e. this
		    								reference must be used when calling the
			    							SetStatusL of this command.
	    * @publishedPartner
	    * @prototype
	    */
	    void ChildURIListL( const TDesC8& aURI, 
	                        const TDesC8& aLUID,
					        const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
					        TInt aResultsRef, 
					        TInt aStatusRef 
					      );
	
        /**
	    * The function adds node object. In some cases an implementation of the
	    * function may be empty function, if the node object does not need concrete
	    * database update. Still this function may be helpful to an adapter, i.e. in
	    * passing mapping LUID of the node to DM Module. The SetStatusL should be
	    * used as described in UpdateLeafObjectL()
	    * @param aURI			URI of the object
	    * @param aParentLUID	LUID of the parent object (if the adapter have
     							earlier returned LUID to the DM Module).   
	    * @param aStatusRef		Reference to correct command, i.e. this reference
		    					must be used when calling the SetStatusL of this
			    				command.
	    * @publishedPartner
	    * @prototype
	    */
	    void AddNodeObjectL( const TDesC8& aURI, 
	                         const TDesC8& aParentLUID,
		  					 TInt aStatusRef 
		  				   );

	    /**
	    * The function implements execute command. The information about the success
	    * of the command should be returned by calling SetStatusL function of
	    * MSmlDmCallback callback interface. This makes it possible to buffer the
	    * commands.
	    * However, all the status codes for buffered commands must be returned at
	    * the latest when the CompleteOutstandingCmdsL() of adapter is called.
	    * @param aURI			URI of the command
	    * @param aLUID			LUID of the object (if the adapter have earlier
    							returned LUID to the DM Module).   
	    * @param aArgument		Argument for the command
	    * @param aType			MIME type of the object 
	    * @param aStatusRef		Reference to correct command, i.e. this reference
    							must be used when calling the SetStatusL of this
	    						command.
	    * @publishedPartner
	    * @prototype
	    */
	    void ExecuteCommandL( const TDesC8& aURI, 
	                          const TDesC8& aLUID,
							  const TDesC8& aArgument, 
							  const TDesC8& aType,
							  TInt aStatusRef 
							);

	    /**
	    * The function implements execute command. The information about the
	    * success of the command should be returned by calling SetStatusL function
	    * of MSmlDmCallback callback interface. This makes it possible to buffer the
	    * commands.
	    * However, all the status codes for buffered commands must be returned at
	    * the latest when the CompleteOutstandingCmdsL() of adapter is called.
	    * @param aURI			URI of the command
	    * @param aLUID			LUID of the object (if the adapter have earlier
    							returned LUID to the DM Module).   
	    * @param aStream		Argument for the command. Adapter should create
		    					write stream and return, when data is written to
			    				stream by DM agent, StreamCommittedL() is called by
				    			DM engine
	    * @param aType			MIME type of the object 
	    * @param aStatusRef		Reference to correct command, i.e. this reference
    							must be used when calling the SetStatusL of this
	    						command.
	    * @publishedPartner
	    * @prototype
	    */
	    void ExecuteCommandL( const TDesC8& aURI, 
	                          const TDesC8& aLUID,
							  RWriteStream*& aStream, 
							  const TDesC8& aType,
							  TInt aStatusRef 
							);

	    /**
	    * The function implements copy command. The information about the success of
	    * the command should be returned by calling SetStatusL function of
	    * MSmlDmCallback callback interface. This makes it possible to buffer the
	    * commands.
	    * However, all the status codes for buffered commands must be returned at
	    * the latest when the CompleteOutstandingCmdsL() of adapter is called.
	    * @param aTargetURI		Target URI for the command
	    * @param aSourceLUID	LUID of the target object (if one exists, and if the adapter
    							has	earlier returned a LUID to the DM Module).   
	    * @param aSourceURI		Source URI for the command
	    * @param aSourceLUID	LUID of the source object (if the adapter has
    							earlier returned a LUID to the DM Module).   
	    * @param aType			MIME type of the objects
	    * @param aStatusRef		Reference to correct command, i.e. this reference
    							must be used when calling the SetStatusL of this
	    						command.
	    * @publishedPartner
	    * @prototype
	    */
	    void CopyCommandL( const TDesC8& aTargetURI, 
	                       const TDesC8& aTargetLUID,
						   const TDesC8& aSourceURI, 
						   const TDesC8& aSourceLUID,
						   const TDesC8& aType, 
						   TInt aStatusRef 
						 );

	    /**
	    * The function indicates start of Atomic command.
	    * @publishedPartner
	    * @prototype
	    */
	    void StartAtomicL();
	
	    /**
	    * The function indicates successful end of Atomic command. The adapter
	    * should commit all changes issued between StartAtomicL() and
	    * CommitAtomicL()
	    * @publishedPartner
	    * @prototype
	    */
	    void CommitAtomicL();
	
	    /**
	    * The function indicates unsuccessful end of Atomic command. The adapter
	    * should rollback all changes issued between StartAtomicL() and
	    * RollbackAtomicL(). If rollback fails for a command, adapter should use
	    * SetStatusL() to indicate it.
	    * @publishedPartner
	    * @prototype
	    */
	    void RollbackAtomicL();
	
	    /**
        * Returns ETrue if adapter supports streaming otherwise EFalse.
	    * @param aItemSize size limit for stream usage
        * @return TBool ETrue for streaming support
	    * @publishedPartner
	    * @prototype
        */
	    TBool StreamingSupport( TInt& aItemSize );
	
	    /**
        * Called when stream returned from UpdateLeafObjectL or ExecuteCommandL has
	    * been written to and committed. Not called when fetching item.
	    * @publishedPartner
	    * @prototype
        */	
	    void StreamCommittedL();
	
	    /**
	    * The function tells the adapter that all the commands of the message that
	    * can be passed to the adapter have now been passed.  This indicates that
	    * the adapter must supply status codes and results to any buffered commands.
	    * This must be done at latest by the time this function returns.
	    * This function is used at the end of SyncML messages, and during processing
	    * of Atomic.   In the case of Atomic processing, the function will be
	    * followed by a call to CommitAtomicL or RollbackAtomicL.
	    * @publishedPartner
	    * @prototype
	    */
	    void CompleteOutstandingCmdsL();

    private: // New methods
    	
    	/**
	    * The function fills the adapter node with the details
	    * @publishedPartner
	    * @prototype
	    */
        void FillNodeInfoL( MSmlDmDDFObject& aDDFObject, 
                                            TSmlDmAccessTypes& aAccessTypes, 
                                            MSmlDmDDFObject::TScope aScope,
                                            MSmlDmDDFObject::TDFFormat aFormat, 
                                            MSmlDmDDFObject::TOccurence aOccurence,
                                            const TDesC8& aDescription,
                                            const TDesC8& aMIMEType );
        
        /**
	    * This function constructs the DDFnode with the details 
	    * @publishedPartner
	    * @prototype
	    */ 
        void FillLAWMODDFL(MSmlDmDDFObject& aDDF);   
        
        // Fills the DDFnode details with the dynamic node info
        
        void FillDynamicNodeL(MSmlDmDDFObject& aDDF); 
        
        /**
    	* Maps a system wide error code to a TError.
    	* @param aError A system wide error code.
    	* @return A TError value depending on aError.
    	*/
    	CSmlDmAdapter::TError MapErrorToStatus( TInt aError ) const;    
    	
    	// Issues a lock request using tarm 3rd party api
    	TLawMoStatus LockDeviceL();
    	
    	// Issues a Unlock request using tarm 3rd party api
    	TLawMoStatus UnLockDeviceL();
    	
    	// Gets the current Lawmo state store in the cenrep
    	TLawMoStatus GetStateL(TInt& aState);
                         	
    private: // Constructor
        /**
        * Symbian 2nd phase constructor
        */
        void ConstructL();

        /**
        * C++ Constructor
        */    
        CLawmoDMAdapter( TAny* aEcomArguments );
        
        // Sends the Generic alert for wipe command
        void SendGenericAlertL();
        
        // Updates the current server id to the store
        void SetCurrentServerIdL();
        
        // Checks if the current profile doing the operation
        // is a factory profile
        TBool IsFactoryProfileL();
        
    private: // Data
        
        enum TNodeIdentifier
            {
            ENodeLawMo = 0,
            ENodeState,
            ENodeAvWipeList,
            ENodeDynamic,
            ENodeItemName,
            ENodeToBeWiped,
            ENodeOperations,
            ENodePartLock,
            ENodeUnlock,
            ENodeWipe,
            ENodeExt,
            ENodeWipeAll,
            ENodeNotUsedAndAlwaysLast
            };
        
        enum TLawmoState
            {
            EFullLocked = 10,
            EPartialLocked = 20,
            EUnlocked = 30
            };
        
        RLawmoClient ilawmoSession;
        TBool iConnected;
        TBuf8<KMaxBufferLength> iRootNode;        

    private:        
        // Gets the current node based on the URI
        TNodeIdentifier GetNodeIdentifier(const TDesC8& aURI);
        
    };

#endif // __DCMODMADAPTER_H

// End Of File
