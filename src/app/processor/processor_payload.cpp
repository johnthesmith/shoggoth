#include <iostream>

/* User libraries */
#include "processor_payload.h"




using namespace std;



/*
    Constructor
*/
ProcessorPayload::ProcessorPayload
(
    ProcessorApplication* a,
    string aNetId,
    string aNetVersion
)
: PayloadEngine( a ) /* Call parent constructor */
{
    net = Net::create( a, a -> getSockManager(), aNetId, aNetVersion );
    net -> addTask( TASK_TEACHER );
//    limb = LimbProcessor::create( net );

    /* Create server and processor payloads */
    server      = Server::create( net );
    processor   = Processor::create( net );
}





/*
    Destructor
*/
ProcessorPayload::~ProcessorPayload()
{
    processor -> destroy();
    server -> destroy();

//    limb -> destroy();

    net -> destroy();
}



/*
    Creator
*/
ProcessorPayload* ProcessorPayload::create
(
    ProcessorApplication* a,
    string aNetId,
    string aNetVersion
)
{
    return new ProcessorPayload( a, aNetId, aNetVersion );
}



/*
    Destructor
*/
void ProcessorPayload::destroy()
{
    delete this;
}



ProcessorApplication* ProcessorPayload::getApplication()
{
    return ( ProcessorApplication* ) Payload::getApplication();
}




/******************************************************************************
    Events
*/



/*
    Net calculateion
*/
void ProcessorPayload::onEngineLoop()
{
    /* Processing processor */

    /* Write log */
    getLog() -> begin( "Processor processing" );

    /* Begin of monitoring */
    getMon()
    -> startTimer( Path{ "currentMks" })
    -> addInt( Path{ "count" })
    -> now( Path{ "last", "moment" } )
    ;

    /* Check net config */

    /* Check server net config */
    auto netConfig = ParamList::create();

    /* Monitoring */
    getMon()
    -> setInt
    (
        Path{ "loop", "netConfigUpdate" },
        netConfig -> getInt( "lastUpdate" ) * SECOND
    )
    -> setInt
    (
        Path{ "loop", "lastNetConfig" },
        net -> getLastUpdate() * SECOND
    )
    -> interval
    (
        Path{ "loop", "netConfigUpdateStr" },
        Path{ "loop", "moment" },
        Path{ "loop", "netConfigUpdate" }
    )
    -> interval
    (
        Path{ "loop", "lastNetConfigStr" },
        Path{ "loop", "moment" },
        Path{ "loop", "lastNetConfig" }
    );

    auto netConfigUpdated = net -> isConfigUpdate( netConfig );
    auto netVersionChanged = net -> isVersionChanged();
    auto appConfigUpdated = getApplication() -> getConfigUpdated(); !!!!!!!

    if( netConfigUpdated || netVersionChanged || appConfigUpdated )
    {
        getLog()
        -> begin( "Restarting" )
        -> info( "by reason" )
        -> prm( "Net updated", netConfigUpdated )
        -> prm( "Net version changed", netVersionChanged )
        -> prm( "App config updated", appConfigUpdated)
        -> prm( "File", getApplication() -> getConfigFileName() )
        -> lineEnd();

        /* Dump net config if changed */
        if( netConfigUpdated )
        {
            getLog() -> dump( netConfig, "Net config" );
        }

        if( getApplication() -> getConfig() -> isOk())
        {
            getLog() -> begin( "Threads stoping" );

            /* Paused processes */
            processor -> pause();
            server -> pause();

            /* Process pause waiting */
            processor -> waitPause();
            server -> waitPause();

            getLog() -> end( "" );

            if( netConfigUpdated || netVersionChanged )
            {
                /* Apply config */
                net -> applyNet( netConfig );
            }

            /* Run server and processor thread */
            server -> setId
            (
                net -> getLogPath( "server_thread" )
            ) -> loop( true );

            processor -> setId
            (
                net -> getLogPath( "processor_thread" )
            ) -> loop( true );

            /* Settings of the processor */
            processor -> getLimb() -> setLearningSpeed
            (
                net
                -> getConfig()
                -> getDouble( Path{ "processor", "learningSpeed" }, 0.001 )
            );

            /* Apply config for processor */
            processor
            -> getLimb()
            -> setMinWeight( taskProc -> getDouble( "minWeight", 1.0e-5 ))
            -> setMaxWeight( taskProc -> getDouble( "maxWeight", 1.0e5 ))
            -> setMaxError( taskProc -> getDouble( "maxError", 0.01 ))
            -> setTickWrite( taskProc -> getInt( "tickWrite", 0 ))
            -> setTickChart( taskProc -> getInt( "tickChart", 0 ))
            -> setDumpConf( taskProc -> getObject( "dump" ))
            ;

            /* Apply config for server */
            server -> setLoopTimeoutMcs( 1000000 );

            server -> resume();
            processor -> resume();
        }

        getLog() -> end();
    }

    netConfig -> destroy();

    getLog() -> end();
}
