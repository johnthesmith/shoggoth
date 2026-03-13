/* Local libraries */
#include "server_payload.h"

/* Core libraryes */
#include "../../../../../lib/core/mon.h"
#include "../../shoggoth/net.h"



/*
    Constructor
*/
ServerPayload::ServerPayload
(
    ShoggothApplication* aApp,
    std::string aId
)
/* Call parent constructor */
: PayloadEngine( (Application*) aApp, aId )
{
    net = aApp -> getNet();

    /* Create server monitor */
    mon = Mon::create( net -> getMonPath( "server_payload.json" ))
    -> setString( Path{ "start", "source" }, "Server payload" )
    -> startTimer( Path{ "start", "moment" });

    /* Log report */
    getApplication() -> getLog() -> trace( "Create server" );
}



/*
    Destructor
*/
ServerPayload::~ServerPayload()
{
    waitStop();
    /* Destroy server monitor */
    mon -> destroy();
    /* Log report */
    getLog() -> trace( "Server destroyd" );
}




/******************************************************************************
    Events
*/



/*
    Server main loop event
*/
void ServerPayload::onLoop()
{
    mon
    -> now( Path{ "current", "now" } )
    -> startTimer( Path{ "current", "moment" } )
    -> interval
    (
        Path{ "current", "uptime" },
        Path{ "current", "moment" },
        Path{ "start", "moment" }
    )
    -> interval
    (
        Path{ "resume", "uptime" },
        Path{ "current", "moment" },
        Path{ "resume", "moment" }
    )
    -> addInt( Path{ "current", "loop" } )
    -> setInt( Path{ "config", "loopTimeoutMcs" }, getLoopTimeoutMcs() )
    -> dumpResult( Path{ "result" }, this );

    mon -> flush();
}



/*
    Server resume action
*/
void ServerPayload::onStartAfter()
{
    if( serverThread == NULL )
    {
        auto config = getConfig();
        auto listenPort = config -> getInt( Path{ "port" });
        if( listenPort != 0 )
        {
            getLog()
            -> begin( "Server starting" )
            -> prm( "Listenen port", listenPort );

            mon
            -> startTimer( Path{ "resume", "moment" })
            -> setInt( Path{ "resume", "port" }, listenPort )
            -> flush();

            auto readWaitingTimeoutMcs = config -> getInt
            (
                Path{ "listen", "readWaitingTimeoutMcs" },
                READ_WAITING_TIMEOUT_MCS
            );

            srv = ShoggothRpcServer::create( net, listenPort );
            srv -> setReadWaitingTimeoutMcs( readWaitingTimeoutMcs );
            serverThread = new thread
            (
                [ this ]
                ()
                {
                    /* Thread Log create */
                    getApplication()
                    -> createThreadLog
                    (
                        net -> getLogPath( "server_listener" )
                    );
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

            getLog() -> end() -> lineEnd();
        }
    }
}



/*
    Server pause action
*/
void ServerPayload::onStopBefore()
{
    if( serverThread != NULL )
    {
        getLog() -> begin( "Server stopping" ) -> lineEnd();

        /* Stop socket and destroy server */
        srv -> down();

        /* Finalize and destroy server thread */
        serverThread -> join();
        delete serverThread;
        serverThread = NULL;

        getLog() -> end() -> lineEnd();
    }
}
