#include <iostream>
#include <thread>   /* For server thread */

/* Core libraryes */
#include "../../../../lib/core/mon.h"

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

    /* Create server monitor */
    mon = Mon::create( aNet -> getMonPath( "server.txt" ))
    -> setString( Path{ "start", "source" }, "Server payload" )
    -> startTimer( Path{ "start", "moment" });

    /* Log report */
    net -> getApplication() -> getLog() -> trace( "Create server" );
}



/*
    Destructor
*/
Server::~Server()
{
    /* Destroy server monitor */
    mon -> destroy();

    /* Log report */
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



ShoggothRoleApplication* Server::getApplication()
{
    return ( ShoggothRoleApplication* ) Payload::getApplication();
}



/******************************************************************************
    Events
*/



/*
    Server main loop event
*/
void Server::onLoop()
{
    mon
    -> now( Path{ "current", "now" } )
    -> startTimer( Path{ "current", "moment" } )
    -> interval( Path{ "current", "uptime" }, Path{ "current", "moment" }, Path{ "start", "moment" })
    -> interval( Path{ "resume", "uptime" }, Path{ "current", "moment" }, Path{ "resume", "moment" })
    -> addInt( Path{ "current", "loop" } )
    -> setInt( Path{ "config", "loopTimeoutMcs" }, getLoopTimeoutMcs() )
    -> dumpResult( Path{ "result" }, this )
    -> flush();
}



/*
    Server resume action
*/
void Server::onResume()
{
    getLog() -> begin( "Server start" );

    auto config = getApplication() -> getConfig();

    /* Read port */
    auto listenPort = config -> getInt
    (
        Path{ "tasks",  taskToString( TASK_PROC ), "listen", "port" },
        11120
    );

    auto  readWaitingTimeoutMcs = config -> getInt
    (
        Path{ "tasks",  taskToString( TASK_PROC ), "listen", "readWaitingTimeoutMcs" },
        READ_WAITING_TIMEOUT_MCS
    );

    getLog() -> prm( "Listenen port", listenPort );

    srv = ShoggothRpcServer::create( net, listenPort );
    srv -> setReadWaitingTimeoutMcs( readWaitingTimeoutMcs );

    mon
    -> startTimer( Path{ "resume", "moment" })
    -> setInt( Path{ "resume", "port" }, listenPort )
    -> setString( Path{ "resume", "result", "code" }, "" )
    -> flush();

    serverThread = new thread
    (
        [ this ]
        ()
        {
            /* Thread Log create */
            getApplication() -> createThreadLog( net -> getLogPath( "server_listener" ));
            getLog() -> begin( "Listen thread" ) -> lineEnd();
            /* Up the server lisener and destroy it after close */
            srv -> up();
            srv -> destroy();
            srv = NULL;
            getLog() -> end() -> lineEnd();
            /* Thread log destroy */
            getApplication() -> destroyThreadLog();
        }
    );

    getLog() -> end();
}



/*
    Server pause action
*/
void Server::onPause()
{
    getLog() -> begin( "Server stop" ) -> lineEnd();
    if( srv != NULL )
    {
        /* Stop socket and destroy server */
        srv -> down();
        /* Finalize and destroy server thread */
        serverThread -> join();
        delete serverThread;
        serverThread = NULL;
    }
    getLog() -> end();
}
