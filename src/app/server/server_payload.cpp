/* Local libraries */
#include "server_payload.h"
#include <filesystem>

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
    /* Log report */
    getLog() -> trace( "Create server" );
    net = aApp -> getNet();

    /* Create server monitor */
    mon = Mon::create( aApp -> getMonPath( aId  + ".json" ))
    -> setString( Path{ "start", "source" }, "Server payload" )
    -> startTimer( Path{ "start", "moment" })
    -> now( Path{ "start", "time" } )
    -> setString( Path{ "start", "pwd" }, std::filesystem::current_path() )
    -> setString( Path{ "start", "log" }, getLog() -> getFileName() )
    ;
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
void ServerPayload::onEngineLoop( bool )
{
    if( serverThread == NULL )
    {
        auto config = getConfig();

        auto listenPort = config -> getInt( Path{ "port" });

        getLog()
        -> begin( "Server starting" )
        -> prm( "Listenen port", listenPort );

        if( listenPort != 0 )
        {
            mon
            -> startTimer( Path{ "resume", "moment" })
            -> setInt( Path{ "resume", "port" }, listenPort )
            -> flush();

            auto readWaitingTimeoutMcs = config -> getInt
            (
                Path{ "readWaitingTimeoutMcs" },
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
        }
        getLog() -> end() -> lineEnd();
    }

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
