/*
    Loader payload
*/

/* Vanilla libraris */
#include <filesystem>
#include <iostream>
#include <cmath>

/* Shoggoth libraries */
#include "../../../../../lib/core/str.h"
#include "../../../../../lib/core/moment.h"

/* User libraries */
#include "loader_payload.h"



using namespace std;



/*
    Constructor
*/
LoaderPayload::LoaderPayload
(
    ShoggothApplication* aApp,
    std::string aId
)
/* Call parent constructor */
: PayloadEngine
(
    (Application*) aApp,
    aId
)
{
    /* Set main net */
    net = aApp -> getNet();

    /* Define mon file */
    auto monFile = aApp -> getMonPath( aId + ".json" );

    /* Begin log */
    aApp -> getLog()
    -> trace( "Loader creating" )
    -> prm( "id", aId )
    -> prm( "log", monFile );

    /* Create Loader monitor */
    mon = Mon::create( monFile )
    -> setString( Path{ "start", "source" }, aId )
    -> now( Path{ "start", "moment" } )
    -> setString( Path{ "start", "pwd" }, std::filesystem::current_path() )
    -> flush()
    ;

    aApp -> getLog() -> trace( "Loader created" );
}



/*
    Destructor
*/
LoaderPayload::~LoaderPayload()
{
    waitStop();

    /* Destroy Processor monitor */
    mon -> destroy();

    /* Log report */
    getLog() -> trace( "Loader destroyed" );
}



/*
    Destructor
*/
void LoaderPayload::destroy()
{
    delete this;
}



/*
    Return application object
*/
ShoggothApplication* LoaderPayload::getApplication()
{
    return ( ShoggothApplication* ) PayloadEngine::getApplication();
}




/******************************************************************************
    Payload events
*/



/*
    Main loop event
*/
void LoaderPayload::onEngineLoop
(
    /* true if application config updated */
    const bool
)
{
    /* Monitoring */
    mon
    -> startTimer( Path{ "last", "processing" } )
    -> now( Path{ "last", "moment" } )
    -> addInt( Path{ "last", "count" })
    -> setString( Path{ "config", "log" }, getLog() -> getFileName() )

    ;

    /* Check server net config */
    auto netConfig = ParamList::create();

    /* Read net config from server */
    auto reloadNet = readNetFromFile( netConfig );

    if( reloadNet )
    {
        /* Apply config */
        net -> lock();
        net -> applyNet( netConfig );
        net -> unlock();
    }

    netConfig -> destroy();

    mon
    -> setString( Path{ "net", "id" }, net -> getId() )
    -> setString( Path{ "net", "version" }, net -> getVersion() )
    -> setString( Path{ "last", "code" }, getCode() )
    -> setInt( Path{ "net", "layers" }, net -> getLayerList() -> getCount() )
    -> setInt( Path{ "net", "nerves" }, net -> getNerveList() -> getCount() )
    -> stopTimer( Path{ "last", "processing" } )
    -> timerToString( Path{ "last", "processing" }, Path{ "last", "processing" } )
    -> flush()
    ;
}



bool LoaderPayload::readNetFromFile
(
    ParamList* aAnswer
)
{
    bool result = false;

    /* Buid file for next version */
    string file = net -> getNetConfigFile( getApplication() -> getNetVersion());

    mon -> setString( Path{ "last", "file" }, file );

    if( fileExists( file ))
    {
        auto lastUpdate = (long) net -> getConfig() -> getInt( Path{ "lastUpdate" }, 0 );
        auto aUpdated = checkFileUpdate( file, lastUpdate );
        if( aUpdated || net -> isVersionChanged() )
        {
            getLog()
            -> begin( "Read net config" )
            -> prm( "file", file )
            -> prm( "pwd", std::filesystem::current_path() )
            -> lineEnd();

            Json::create()
            -> fromFile( file )
            -> include()
            -> copyTo( aAnswer )
            -> resultTo( this )
            -> destroy();

            if( isOk() )
            {
                aAnswer -> setInt( "lastUpdate", lastUpdate );
                result = true;
            }
            else
            {
                getLog()
                -> warning( this -> getCode() )
                -> prm( "message", this -> getMessage())
                ;
            }

            getLog() -> end();
        }
    }

    return result;
}



/******************************************************************************
    Getters and setters
*/
