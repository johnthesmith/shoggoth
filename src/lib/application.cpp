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
    Return log object
*/
Log* Application::getLog()
{
    return log;
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
