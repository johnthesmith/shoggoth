/* System libraries */
#include <iostream>

/* Libraryes */
#include "../lib/utils.h"
#include "../lib/log.h"
#include "../json/param_list.h"
//#include "../graph/scene.h"

/* Application libraryes */
#include "shoggoth_application.h"
#include "shoggoth_rpc_server.h"
#include "ui.h"
#include "processor.h"
#include "teacher.h"

#include "../lib/buffer_to_hex.h"



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



Role ShoggothApplication::roleFromString
(
    string aRole
)
{
    if( aRole == roleToString( ROLE_TEACHER )) return ROLE_TEACHER;
    if( aRole == roleToString( ROLE_UI )) return ROLE_UI;
    if( aRole == roleToString( ROLE_STORAGE )) return ROLE_STORAGE;
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
        case ROLE_STORAGE   : return "storage";
        default:
        case ROLE_PROCESSOR : return "processor";
    }
}



/*
    Run application
*/
ShoggothApplication* ShoggothApplication::run()
{
    getLog() -> begin( "Application start ======================================" );

    /* Output cli arguments */
    getLog() -> begin( "Start CLI parameters" );
    for( int i = 0; i < getCli() -> getCount(); i++ )
    {
        auto param = getCli() -> getByIndex( i );
        getLog() -> trace("") -> prm( param -> getName(), param -> getString() );
    }
    getLog() -> end();

    /* Build config object */
    checkConfigUpdate();

    switch( roleFromString( getConfig() -> getString( "role", "storage" ) ))
    {
        case ROLE_STORAGE:
        {
            getLog() -> trace( "Application role" ) -> prm( "Name", roleToString( ROLE_STORAGE ));
            auto server = ShoggothRpcServer::create( this );
            server -> setPort( getConfig() -> getInt( "port", 11120 ));
            server -> up();
            server -> destroy();
        }
        break;

        case ROLE_TEACHER:
        {
//            getLog()
//            -> trace( "Application role" )
//            -> prm( "Name", roleToString( ROLE_TEACHER ));
//            Teacher::create( this ) -> loop() -> destroy();
        }
        break;

        case ROLE_PROCESSOR:
            getLog()
            -> trace( "Application role" )
            -> prm( "Name", roleToString( ROLE_PROCESSOR ));
            Processor::create( this ) -> loop() -> destroy();
        break;
        case ROLE_UI:
            getLog()
            -> trace( "Application role" )
            -> prm( "Name", roleToString( ROLE_UI ));

            auto scene  = Scene( getLog() );
            auto ui     = Ui::create( this );

            scene.getFont()
            -> setFontName( "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf" )
            -> setGliphSize( 64 )
            -> setCharSet( " !@#$£€¥%^&*()_+=-{}[]\\|/~`<>?.,;:'\"«»†∞1234567890abcdefghijklmnopqarstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXWZабвгдеёжзийклмнопрстуфхцчшщьыъэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЬЫЪЭЮЯ" );

            scene.init() -> setPayload( ui ) -> loop() -> finit();

            ui -> destroy();
        break;
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
