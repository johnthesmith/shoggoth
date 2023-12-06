#include <iostream>

/* File read */
#include <streambuf>
#include <sstream>
#include <fstream>
#include <thread>

/* for config update control */
#include <sys/types.h>
#include <sys/stat.h>


/* Local libraries */
#include "server.h"




using namespace std;



/*
    Constructor
*/
Server::Server
(
    Net* aNet
)
/* Call parent constructor */
: Payload( aNet -> getApplication() )
{
    auto app = aNet -> getApplication();
    aNet -> getLog() -> trace( "Create Server" );
    net = aNet;
}





/*
    Destructor
*/
Server::~Server()
{
    getLog() -> trace( "Server destroyd" );
}



/*
    Creator
*/
Server* Server::create
(
    Net* a
)
{
    return new Server( a );
}



/*
    Destructor
*/
void Server::destroy()
{
    delete this;
}



ShoggothApplication* Server::getApplication()
{
    return ( ShoggothApplication* ) Payload::getApplication();
}



/******************************************************************************
    Methods
*/



/*
    Run net calculateion
*/
void Server::onLoop
(
    bool&           aTreminated,
    bool&           aIdling,
    unsigned int&   aSleep,
    bool&           aReconfig
)
{
    if( !getPause() )
    {
        /* Confirm work mode */
        setPaused( false );

        /* Work loop */
        if( srv == NULL )
        {
            getLog() -> begin( "Server" );

            /* Read port */
            auto listenPort = getApplication()
            -> getConfig()
            -> getInt
            (
                Path{ "tasks",  taskToString( TASK_PROC ), "listen", "port" },
                11120
            );

            getLog() -> trace() -> prm( "Listenen port", listenPort ) -> lineEnd();

            srv = ShoggothRpcServer::create( getApplication(), getLog() );
            srv -> setPort( listenPort );

            serverThread = new thread
            (
                [ this ]
                ()
                {
                    srv -> up() -> destroy();
                }
            );
        }
    }
    else
    {
        if( srv != NULL )
        {
            /* Stop sochet and destroy server */
            srv -> down() -> destroy();
            srv = NULL;

            /* Finalize and destroy server thread */
            serverThread -> join();
            delete serverThread;
            serverThread = NULL;

            /* Close server log section */
            getLog() -> end( "" );
        }
        /* Confirm pause mode */
        setPaused( true );
    }
}
