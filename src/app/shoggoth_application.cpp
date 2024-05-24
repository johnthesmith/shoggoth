/* System libraries */
#include <iostream>



/* Libraryes */
#include "../../../../lib/core/utils.h"
#include "../../../../lib/core/log.h"
#include "../../../../lib/json/param_list.h"
#include "../../../../lib/core/buffer_to_hex.h"

/* Application libraryes */
#include "shoggoth_application.h"
#include "loop.h"



using namespace std;



ShoggothApplication::ShoggothApplication
(
    int aCount,        /* cli argumends count */
    char** aList       /* cli arguments */
)
:Application
(
    aCount,
    aList
)
{
    sockManager = SockManager::create();
}



ShoggothApplication::~ShoggothApplication()
{
    sockManager -> destroy();
}



/*
    Creator
*/
ShoggothApplication* ShoggothApplication::create
(
    int aCount,        /* cli argumends count */
    char** aList       /* cli arguments */
)
{
    return new ShoggothApplication( aCount, aList );
}



/*
    Destroy
*/
void ShoggothApplication::destroy()
{
    delete this;
}




/*
    Return Role from string
*/
Role ShoggothApplication::roleFromString
(
    string aRole
)
{
    if( aRole == roleToString( ROLE_TEACHER ))  return ROLE_TEACHER;
    if( aRole == roleToString( ROLE_UI ))       return ROLE_UI;
    if( aRole == roleToString( ROLE_SERVER ))   return ROLE_SERVER;
    return ROLE_PROCESSOR;
}



string ShoggothApplication::roleToString
(
    Role aRole
)
{
    switch( aRole )
    {
        case ROLE_TEACHER   : return "teacher";
        case ROLE_UI        : return "ui";
        case ROLE_SERVER    : return "server";
        default:
        case ROLE_PROCESSOR : return "processor";
    }
}



/*
    Run application
*/
ShoggothApplication* ShoggothApplication::run()
{
    getLog()
    -> begin( "Application start" );

    /* Output cli arguments */
    getLog() -> begin( "Start CLI parameters" );
    for( int i = 0; i < getCli() -> getCount(); i++ )
    {
        auto param = getCli() -> getByIndex( i );
        getLog()
        -> trace( "" )
        -> prm( param -> getName(), param -> getString() );
    }
    getLog() -> end();

    getLog()
    -> trace( "Config source" )
    -> prm( "file", getConfigFileName() );

    netId = getCli() -> getString( "net_id", "alpha" );
    netVersion = getCli() -> getString( "net_version", "zero" );

    /* Main loop */
    while( true )
    {
        Loop::create( this, netId, netVersion )
        -> resume()
        -> loop()
        -> destroy();
    }

    getLog() -> end( "Application stop" );

    return this;
}



string ShoggothApplication::getConfigFileName()
{
    return getCli() -> getString( "config" );
}



/*
    Check update moment of the config file.
    If file was updated, then the config object is rebuilding.
*/
ShoggothApplication* ShoggothApplication::checkConfigUpdate()
{
    string configFileName = getConfigFileName();
    bool cfgUpdated = checkFileUpdate( configFileName, lastConfigUpdate );

    if( cfgUpdated )
    {
        /*
            Load config and cli
        */
        getLog() -> trace( "Load config file" ) -> prm( "name", configFileName );

        getConfig()
        -> clear()
        -> fromJsonFile( configFileName )
        -> copyFrom( getCli() );
    }

    configUpdated = configUpdated || cfgUpdated;

    return this;
}



bool ShoggothApplication::getConfigUpdated()
{
    bool result = configUpdated;
    configUpdated = false;
    return result;
}



SockManager* ShoggothApplication::getSockManager()
{
    return sockManager;
}



/*
    End of thread
*/
ShoggothApplication* ShoggothApplication::onThreadAfter()
{
    getSockManager() -> closeHandlesByThread( "" );
    return this;
}

