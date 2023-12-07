#include <iostream>
#include <cstring>
#include "application.h"
#include "utils.h"


/*
    Constructor
*/
Application::Application
(
    int     aCount, /* cli argumends count */
    char**  aList   /* cli arguments */
)
{
    /* Create base coponents */
    log         = Log::create();
    logManager  = LogManager::create( log );
    config      = ParamListFile::create();
    cli         = ParamList::create();

    /* Fill config from cli */
    for( int i = 1; i < aCount; i++ )
    {
        string arg = aList[ i ];
        auto lexemes = explode( arg, "=", true, 1 );
        string key = "";
        string value = "";

        /* Split on key and value */
        if( lexemes.size() == 1 )
        {
            key = lexemes[ 0 ];
        }
        else
        {
            key = lexemes[ 0 ];
            value = lexemes[ 1 ];
        }

        /* Remove -- from key*/
        if( key.find( "--" ) == 0 )
        {
            key = explode( key, "--", true, 1)[ 1 ];
        }

        /* Write to config */
        cli -> setValue( key, value );
    }

    /* Set log destination */
    getLog() -> setFileName( cli -> getString( "log", "" ) );
}



/*
    Destructor
*/
Application::~Application()
{
    cli         -> destroy();
    config      -> destroy();
    log         -> destroy();
    logManager  -> destroy();
}



/*
    Creator
*/
Application* Application::create
(
    int     aCount, /* cli argumends count */
    char**  aList   /* cli arguments */
)
{
    return new Application( aCount, aList );
}



/*
    Destroy
*/
void Application::destroy()
{
    delete this;
}



/*
    Return cli config object
*/
ParamList* Application::getCli()
{
    return cli;
}



/*
    Return config object
*/
ParamListFile* Application::getConfig()
{
    return config;
}



/**********************************************************************
    Log manager operations
*/


/*
    Return Log manager
*/
LogManager* Application::getLogManager()
{
    return logManager;
}



/*
    Create new log
*/
Log* Application::createThreadLog
(
    string aId
)
{
    return getLogManager() -> createLog( aId );
}



/*
    Destroy log by id
*/
Application*  Application::destroyThreadLog()
{
    getLogManager() -> destroyLog();
    return this;
}



/*
    Return log object by id log
    or default log application
*/
Log* Application::getLog()
{
    return getLogManager() -> getLog();
}
