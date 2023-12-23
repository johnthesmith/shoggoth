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
    net = aNet;
    net -> getApplication() -> getLog() -> trace( "Create server" );
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
void Server::onLoop()
{
    getLog() -> trace( "Server alive" );
}



/*
    Run net calculateion
*/
void Server::onResume()
{
    getLog() -> begin( "Server start" );

    /* Read port */
    auto listenPort = getApplication()
    -> getConfig()
    -> getInt
    (
        Path{ "tasks",  taskToString( TASK_PROC ), "listen", "port" },
        11120
    );

    getLog() -> prm( "Listenen port", listenPort );

    srv = ShoggothRpcServer::create( net );
    srv -> setPort( listenPort );

    serverThread = new thread
    (
        [ this ]
        ()
        {
            /* Thread Log create */
            getApplication() -> createThreadLog( "server_listener" );
            /* Up the therver lisener and destroy it after close  */
            getLog() -> begin( "Listen thread" ) -> lineEnd();
            srv -> up() -> destroy();
            srv = NULL;
            getLog() -> end() -> lineEnd();
            /* Thread log destroy */
            getApplication() -> destroyThreadLog();
        }
    );

    getLog() -> end();
}



void Server::onPause()
{
    getLog() -> begin( "Server stop" );
    if( srv != NULL )
    {
        /* Stop sochet and destroy server */
        srv -> down();
        /* Finalize and destroy server thread */
        serverThread -> join();
        delete serverThread;
        serverThread = NULL;
    }
    getLog() -> end();
}
