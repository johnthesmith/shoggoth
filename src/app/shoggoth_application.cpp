/* System libraries */
#include <iostream>

/* Libraryes */
#include "../../../../lib/core/utils.h"

/* Application libraryes */
#include "shoggoth_application.h"



using namespace std;



/*
    Constructor of the Shogoth application
*/
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



/*
    Destructor of the Shogoth application
*/
ShoggothApplication::~ShoggothApplication()
{
    sockManager -> destroy();
}



/*
    Destroy of the Shoogoth
*/
void ShoggothApplication::destroy()
{
    delete this;
}



/*
    End of thread
*/
ShoggothApplication* ShoggothApplication::onThreadAfter()
{
    getSockManager() -> closeHandlesByThread( "" );
    return this;
}



/*
    Prepare configuration for application running
*/
ShoggothApplication* ShoggothApplication::prepareConfiguration()
{
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

    /* Set net attributes for the begining */
    netId = getCli() -> getString( "net_id", "alpha" );
    netVersion = getCli() -> getString( "net_version", "zero" );

    return this;
}



/**********************************************************************
    Setters and getters
*/



/*
    Return the sock manager from application
*/
SockManager* ShoggothApplication::getSockManager()
{
    return sockManager;
}



/*
    Return the net identifier
*/
string ShoggothApplication::getNetId()
{
    return netId;
}




/*
    Return the net version
*/
string ShoggothApplication::getNetVersion()
{
    return netVersion;
}


