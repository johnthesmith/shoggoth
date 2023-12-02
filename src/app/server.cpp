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
    Activate payload
*/
void Server::onActivate()
{
}



/*
    Run net calculateion
*/
void Server::onRun()
{
    /* Read port */
    auto listenPort = getApplication()
    -> getConfig()
    -> getInt( Path{ "tasks",  taskToString( TASK_PROC ), "listen", "port" }, 11120 );

    auto srv = ShoggothRpcServer::create( getApplication() );
    srv -> setPort( listenPort );
    srv -> up() -> destroy();
}
