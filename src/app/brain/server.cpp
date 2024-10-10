#include <iostream>
#include <thread>   /* For server thread */

/* Core libraryes */
#include "../../../../../lib/core/mon.h"

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
: PayloadEngine( aNet -> getApplication(), "server" )
{
    net = aNet;

    /* Create server monitor */
    mon = Mon::create( aNet -> getMonPath( "brain_server.json" ))
    -> setString( Path{ "start", "source" }, "Server payload" )
    -> startTimer( Path{ "start", "moment" });

    /* Log report */
    getApplication() -> getLog() -> trace( "Create server" );
}



/*
    Destructor
*/
Server::~Server()
{
    stop();
    waitStop();

    onStopBefore();

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



BrainApplication* Server::getApplication()
{
    return ( BrainApplication* ) Payload::getApplication();
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
    -> dumpResult( Path{ "result" }, this );
    mon -> flush();
}



/*
    Server resume action
    Main app thread
*/
void Server::onStartAfter()
{
    if( serverThread == NULL )
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
}



/*
    Server pause action
*/
void Server::onStopBefore()
{
    if( serverThread != NULL )
    {
        getLog() -> begin( "Server stop" ) -> lineEnd();

        /* Stop socket and destroy server */
        srv -> down();

        /* Finalize and destroy server thread */
        serverThread -> join();
        delete serverThread;
        serverThread = NULL;

        getLog() -> end();
    }
}
