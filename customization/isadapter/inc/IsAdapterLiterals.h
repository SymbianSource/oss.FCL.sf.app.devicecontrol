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


#ifndef __IsAdapterLiterals_H__
#define __IsAdapterLiterals_H__

// the DDF version must be changed if any changes in DDF structure ( built in DDFStructureL() function )
_LIT8( KNSmlISDDFVersion, "1.0" ); 
_LIT8( KMimeTypeImageMbm, "image/x-epoc-mbm");
_LIT8( KNSmlISTextPlain, "text/plain" );
_LIT8( KNSmlISNodeName, "Apps" );
_LIT8( KNSmlISDynamicNode, "" );
_LIT8( KNSmlISSeparator, "/" );

//
// Customization, IdleSoftkeys, Softkeys, Targets, WebTargets
//
_LIT8( KPrefixSoftkeyNodeName,       "Softkey" );
_LIT8( KFormatSoftkeyNodeName,       "Softkey%d" );
_LIT8( KPrefixTargetNodeName,        "Target" );
_LIT8( KFormatTargetNodeName,        "Target%d" );
_LIT8( KPrefixAppTargetNodeName,     "AppTarget" );
_LIT8( KFormatAppTargetNodeName,     "AppTarget%d" );
_LIT8( KPrefixEditorTargetNodeName,  "EditorTarget" );
_LIT8( KFormatEditorTargetNodeName,  "EditorTarget%d" );
_LIT8( KPrefixSpecialTargetNodeName, "OtherTarget" );
_LIT8( KFormatSpecialTargetNodeName, "OtherTarget%d" );
_LIT8( KPrefixWebTargetURI,          "Customization/IdleSoftkeys/WebTargets/" );
_LIT8( KPrefixWebTargetNodeName,     "WebTarget" );
_LIT8( KFormatWebTargetNodeName,     "WebTarget%d" );

_LIT8( KISLeftSoftkeyName,          "Left softkey" );
_LIT8( KISRightSoftkeyName,         "Right softkey" );
_LIT8( KISScrollLeftSoftkeyName,    "Left arrow softkey" );
_LIT8( KISScrollRightSoftkeyName,   "Right arrow softkey" );
_LIT8( KISScrollUpSoftkeyName,      "Up arrow softkey" );
_LIT8( KISScrollDownSoftkeyName,    "Down arrow softkey" );
_LIT8( KISSelectionSoftkeyName,     "Selection softkey" );
_LIT8( KISAISoftkeyName1,           "Active Idle softkey 1" );
_LIT8( KISAISoftkeyName2,           "Active Idle softkey 2" );
_LIT8( KISAISoftkeyName3,           "Active Idle softkey 3" );
_LIT8( KISAISoftkeyName4,           "Active Idle softkey 4" );
_LIT8( KISAISoftkeyName5,           "Active Idle softkey 5" );
_LIT8( KISAISoftkeyName6,           "Active Idle softkey 6" );

_LIT8( KNSmlCustomizationNodeName,  "Customization" );
_LIT8( KNSmlIS_IdleSoftKeyNodeName, "IdleSoftkeys" );
_LIT8( KNSmlIS_SoftKeysNodeName,    "SoftKeys" );
_LIT8( KNSmlIS_TargetsNodeName,     "Targets" );
_LIT8( KNSmlIS_WebTargetsNodeName,  "WebTargets" );

_LIT8( KNSmlISDescription, "The interior object holds all idle softkey objects" );
_LIT8( KNSmlISIdleSoftKeyNodeDescription, "Holds all idle softkey customization nodes");
_LIT8( KNSmlISSoftkeysNodeDescription, "Holds all softkeys objects in idle softkey customization");
_LIT8( KNSmlISTargetsNodeDescription, "Holds all targets objects in idle softkey customization");
_LIT8( KNSmlISWebTargetsNodeDescription, "Holds all web targets objects in idle softkey customization");
_LIT8( KNSmlISSoftkeysXNodeDescription, "Holds all softkey information in a web targets object");
_LIT8( KNSmlISTargetsXNodeDescription, "Holds all target information in a web targets object");
_LIT8( KNSmlISWebTargetsXNodeDescription, "Holds all web target information in a web targets object");

// Softkeys/X/*
_LIT8( KNSmlIS_SoftkeysIDNodeName,        "ID" );
_LIT8( KNSmlIS_SoftkeysTargetRefNodeName, "TargetRef" );
#ifdef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
_LIT8( KNSmlIS_SoftkeysImageNoneName,     "Image" );
#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
_LIT8( KNSmlIS_SoftkeysMaskNodeName,      "Mask" );
#endif
#endif

_LIT8( KNSmlIS_SoftkeysIDDescription,        "This object holds id of a Softkeys node" );
_LIT8( KNSmlIS_SoftkeysTargetRefDescription, "This object holds target reference of a Softkeys node" );
#ifdef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
_LIT8( KNSmlIS_SoftkeysImageDescription,     "This object holds image mask of a Softkeys node" );
#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
_LIT8( KNSmlIS_SoftkeysMaskDescription,      "This object holds image mask of a Softkeys node" );
#endif
#endif

// Targets/X/*
_LIT8( KNSmlIS_TargetsIDNodeName,          "ID" );
_LIT8( KNSmlIS_TargetsDescriptionNodeName, "Description" );
_LIT8( KNSmlIS_TargetsCaptionName,         "Caption" );
#ifndef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
_LIT8( KNSmlIS_TargetsImageNodeName,       "Image" );
#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
_LIT8( KNSmlIS_TargetsMaskNodeName,        "Mask" );
#endif
#endif

_LIT8( KNSmlIS_TargetsIDDescription,          "This object holds id of a Targets node" );
_LIT8( KNSmlIS_TargetsDescriptionDescription, "This object holds description of a Targets node" );
_LIT8( KNSmlIS_TargetsCaptionDescription,     "This object holds caption of a Targets node" );
#ifndef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
_LIT8( KNSmlIS_TargetsImageDescription,       "This object holds image of a Targets node" );
#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
_LIT8( KNSmlIS_TargetsMaskDescription,        "This object holds image mask of a Targets node" );
#endif
#endif

// WebTargets/X/*
_LIT8( KNSmlIS_WebTargetsIDNodeName,          "ID" );
_LIT8( KNSmlIS_WebTargetsDescriptionNodeName, "Description" );
_LIT8( KNSmlIS_WebTargetsURLNodeName,         "URL" );
_LIT8( KNSmlIS_WebTargetsUserNameNodeName,    "UserName" );
_LIT8( KNSmlIS_WebTargetsPasswordNodeName,    "Password" );
_LIT8( KNSmlIS_WebTargetsConRefNodeName,      "ConRef" );
_LIT8( KNSmlIS_WebTargetsCaptionNodeName,     "Caption" );
#ifndef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
_LIT8( KNSmlIS_WebTargetsImageNodeName,       "Image" );
#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
_LIT8( KNSmlIS_WebTargetsMaskNodeName,        "Mask" );
#endif
#endif

_LIT8( KNSmlIS_WebTargetsIDDescription,          "This object holds id of a WebTargets node" );
_LIT8( KNSmlIS_WebTargetsDescriptionDescription, "This object holds description of a WebTargets node" );
_LIT8( KNSmlIS_WebTargetsURLDescription,         "This object holds URL of a WebTargets node" );
_LIT8( KNSmlIS_WebTargetsUserNameDescription,    "This object holds user name of a WebTargets node" );
_LIT8( KNSmlIS_WebTargetsPasswordDescription,    "This object holds password of a WebTargets node" );
_LIT8( KNSmlIS_WebTargetsConRefDescription,      "This object holds connection reference of a WebTargets node" );
_LIT8( KNSmlIS_WebTargetsCaptionDescription,     "This object holds caption of a WebTargets node" );
#ifndef IDLESOFTKEY_ADAPTER_SOFTKEY_IMAGES
_LIT8( KNSmlIS_WebTargetsImageDescription,       "This object holds image of a WebTargets node" );
#ifdef IDLESOFTKEY_ADAPTER_IMAGEMASK
_LIT8( KNSmlIS_WebTargetsMaskDescription,        "This object holds image mask of a WebTargets node" );
#endif
#endif


// Used by UiSettingsUtil

	_LIT(KNewMsg,"localapp:0x100058C5?new=msg");
	_LIT(KNewEmail,"localapp:0x100058C5?new=email");
	_LIT(KNewSyncMLMail,"localapp:0x100058C5?new=syncmlmail");
	_LIT(KNewMMS,"localapp:0x100058C5?new=postcard");
	_LIT(KNewAudioMsg,"localapp:0x100058C5?new=audiomsg");
	_LIT(KNewMsgType,"localapp:0x100058C5?new");
	_LIT(KChangeTheme,"localapp:0x10005A32?view=0x102750A7");
	
	_LIT(KNewMsgCaption,"New Message");
	_LIT(KNewEmailCaption,"New Email");
	_LIT(KNewSyncMLMailCaption,"New Syncml Mail");
	_LIT(KNewMMSCaption,"New Postcard");
	_LIT(KNewAudioMsgCaption,"New Audio Message");
	_LIT(KNewMsgTypeCaption,"Select Message Type");
	_LIT(KChangeThemeCaption,"Change Idle Theme");
	
	   	

	_LIT16(KFormat,"%x");
	_LIT(KAppText,"localapp:0x");
    _LIT(KMailBoxText,"localapp:0x100058C5?mailbox=");
    _LIT(KBookmarkText,"localapp:0x102750FB?bkm=0x");
    

#endif //__IsAdapterLiterals_H__
