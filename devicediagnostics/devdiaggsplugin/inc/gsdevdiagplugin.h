/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Launch Diagnostics embedded application.
*                 (DevDiagApp.exe) The UID3 is defined below.
*
*/


#ifndef GSDDPLUGIN_H
#define GSDDPLUGIN_H

// INCLUDES
#include <gsplugininterface.h>
#include <ConeResLoader.h>
#include <AknServerApp.h>

//CONSTANTS
const TUid KGSDDPluginUid = { 0x2000E543 };
const TUid KGsDiagnosticsAppUid  = { 0x2000E542 }; //app to be laucnhed 
//_LIT( KGSDDPluginIconDirAndName, "z:GSDDPlugin.mbm"); // Use KDC_BITMAP_DIR
_LIT( KGSDDPluginIconDirAndName, "z:GSDevDiagPlugin.mif"); // Use KDC_BITMAP_DIR or ?? KDC_APP_BITMAP_DIR
_LIT( KGSDiagnosticsApp,    "z:\\sys\\bin\\DevDiagApp.exe" );


// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CAknNullService;


// CLASS DEFINITION
/**
*  CGSDevDiagPlugin view class 
*  @since S60 5.0
*
*/
class CGSDevDiagPlugin : public CGSPluginInterface,
                         public MAknServerAppExitObserver // Embedding

    {
    
    /**
    * Contains application information
    *
    *  @since S60 5.0
    **/
    class TAppInfo
     {
     public:
         TAppInfo( TUid aUid, const TDesC& aFile )
             : iUid( aUid ), iFile( aFile )
             {}
         TUid iUid; //Uid3
         TFileName iFile; //full filename of the app.
     };

    
public: // Constructors and destructor

    /**
    * Symbian OS two-phased constructor
    * @return GS connection view.
    */
    static CGSDevDiagPlugin* NewL( TAny* aInitParams );

    /**
    * Destructor
    */
    ~CGSDevDiagPlugin();
    
public: // From CGSPluginInterface

    /**
    * @see CGSPluginInterface header file.
    */
    void GetCaptionL( TDes& aCaption ) const;

    /**
    * See base class.
    */
    CGulIcon* CreateIconL( const TUid aIconType );

    /**
    * @see CGSPluginInterface header file.
    */
    TInt PluginProviderCategory() const;
    
    /**
    * GS framework calls this method when plug-in is opened.
    * The method launches the embedded application.
    **/
    void HandleSelection( const TGSSelectionTypes aSelectionType );
    
    /**
    * See base class.
    */
    TGSListboxItemTypes ItemType();

    /**
    * See base class.
    */
    void GetValue( const TGSPluginValueKeys aKey,
                       TDes& aValue );

public: //from MAknServerAppExitObserver
    
    /**
    * Close the GS when embedded application is shutdown.
    **/
    virtual void HandleServerAppExit(TInt aReason);

    
public: //From CAknView

    /**
    * From CAknView.
    * Returns views id.
    **/
    TUid Id() const;


protected:
    /**
    * From CAknView Activates view.
    * @since Series 60 3.1
    * @param aPrevViewId Id of previous view.
    * @param aCustomMessageId Custom message id.
    * @param aCustomMessage Custom message.
    * @return None.
    */
    virtual void DoActivateL (const TVwsViewId &aPrevViewId, 
                                       TUid aCustomMessageId, 
                                       const TDesC8 &aCustomMessage);
    /**
    * From CAknView Deactivates view.        
    * @since Series 60 3.1
    * @param None.
    * @return None.
    */
    virtual void DoDeactivate();     
    
private:
    
    /**
    * ConstructL
    **/
    void ConstructL();
    
    /**
    * Default constructor
    **/
    CGSDevDiagPlugin();
    
    /**
    * Open localised resource file.
    *
    * @aResourceFileName Full path name of the resource file.
    * @aRsourceLoader 
    **/
    void OpenLocalizedResourceFileL(
            const TDesC& aResourceFileName,
            RConeResourceLoader& aResourceLoader );
            
   /**
   * Launch DevDiagApp.exe as an embedded application 
   **/            
   void LaunchDiagnosticsAppL();
         
   /**
   * Calls null service to launch the application.
   *
   * @param aApp The application to be launched.
   **/      
   void EmbedAppL( const TAppInfo& aApp );      
            
private:
               
     // resource loader
    RConeResourceLoader iResourceLoader;
    
    // Mysterious embedded app launcher.
    CAknNullService* iNullService;             
    };

#endif //GSDDPLUGIN_H

// End of File
