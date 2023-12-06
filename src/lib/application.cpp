#include <sstream>
#include <iostream>
#include <cstring>
#include <thread>
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
    log     = Log::create();
    config  = ParamListFile::create();
    cli     = ParamList::create();

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
    cli -> destroy();
    config -> destroy();
    log -> destroy();
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
    Log operations
*/


/*
    Create new log
*/
Log* Application::createThreadLog()
{
    auto threadId = getThreadId();
    auto result = getLog();

    if( result == log && threadId != "" )
    {
        /* Create and retrn new log */
        result = Log::create() -> clone( log );
        /* Registrate in list of the logs */
        logList[ threadId ] = result;
    }
    return result;
}



/*
    Destroy log by id
*/
Application*  Application::destroyThreadLog()
{
    auto threadId = getThreadId();

    auto result = getLog();
    if( result != log )
    {
        logList[ threadId ] -> destroy();
        logList.erase( threadId );
    }
    return this;
}




/*
    Return log object by id log
    or default log application
*/
Log* Application::getLog()
{
    auto threadId = getThreadId();

    return
    threadId != "" && logList.find( threadId ) != logList.end()
    ? logList[ threadId ]
    : log;
}



/*
    Return thread id for current payload
*/
string Application::getThreadId()
{
    auto threadId = this_thread::get_id();
    stringstream s;
    s << threadId;
    return s.str();
}
