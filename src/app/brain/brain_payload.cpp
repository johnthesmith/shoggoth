#include <iostream>

/* User libraries */
#include "../../../../../lib/core/moment.h"

/* User libraries */
#include "brain_payload.h"



using namespace std;



/*
    Constructor
*/
BrainPayload::BrainPayload
(
    BrainApplication* a,
    string aNetId,
    string aNetVersion
)
: PayloadEngine( a ) /* Call parent constructor */
{
    net = Net::create
    (
        a,
        a -> getSockManager(),
        aNetId,
        aNetVersion,
        TASK_PROC
    );
//    limb = LimbBrain::create( net );

    /* Create server and Brain payloads */
    server      = Server::create( net );
    processor   = Processor::create( net );
}





/*
    Destructor
*/
BrainPayload::~BrainPayload()
{
    processor -> destroy();
    server -> destroy();

//    limb -> destroy();

    net -> destroy();
}



/*
    Creator
*/
BrainPayload* BrainPayload::create
(
    BrainApplication* a,
    string aNetId,
    string aNetVersion
)
{
    return new BrainPayload( a, aNetId, aNetVersion );
}



/*
    Destructor
*/
void BrainPayload::destroy()
{
    delete this;
}



BrainApplication* BrainPayload::getApplication()
{
    return ( BrainApplication* ) Payload::getApplication();
}



Limb* BrainPayload::getNet()
{
    return net;
}


/******************************************************************************
    Events
*/



/*
    Net calculateion
*/
void BrainPayload::onEngineLoop
(
    bool aConfigUpdated,
    bool aEnabled
)
{
    if( aEnabled )
    {
        /* Processing processor */

        /* Write log */
        getLog() -> begin( "Brain processing" );

        /* Check net config */

        /* Check server net config */
        auto netConfig = ParamList::create();

        /* Read net config from server */
        net -> readNet( netConfig );

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
            Path{ "momentMcs" },
            Path{ "loop", "netConfigUpdate" }
        )
        -> interval
        (
            Path{ "loop", "lastNetConfigStr" },
            Path{ "momentMcs" },
            Path{ "loop", "lastNetConfig" }
        );

        /* Get net variables */
        auto netConfigUpdated = net -> isConfigUpdate( netConfig );
        auto netVersionChanged = net -> isVersionChanged();


        if( netConfigUpdated || netVersionChanged || aConfigUpdated )
        {
            getLog()
            -> begin( "Restarting" )
            -> info( "by reason" )
            -> prm( "Net updated", netConfigUpdated )
            -> prm( "Net version changed", netVersionChanged )
            -> prm( "App config updated", aConfigUpdated)
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
                    processor -> pause() -> waitPause();
                    server -> pause() -> waitPause();
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
                )
// TODO LEAK 2
//                -> loop( true )
                ;

                /* Settings of the processor */
                processor -> getLimb() -> setLearningSpeed
                (
                    net
                    -> getConfig()
                    -> getDouble( Path{ "processor", "learningSpeed" }, 0.001 )
                );

                /* Apply config for processor */
                auto appConfig = getApplication() -> getConfig();
                processor
                -> getLimb()
                -> setMinWeight( appConfig -> getDouble( "minWeight", 1.0e-5 ))
                -> setMaxWeight( appConfig -> getDouble( "maxWeight", 1.0e5 ))
                -> setMaxError( appConfig -> getDouble( "maxError", 0.01 ))
                -> setTickWrite( appConfig -> getInt( "tickWrite", 0 ))
                -> setTickChart( appConfig -> getInt( "tickChart", 0 ))
                -> setDumpConf( appConfig -> getObject( "dump" ))
                ;
                /* Apply config for server */
                server -> setLoopTimeoutMcs( 1000000 );

// TODO LEAK 3
                server -> resume();
// TODO LEAK 4
//                processor -> resume();
            }

            getLog() -> end();
        }
        netConfig -> destroy();
        getLog() -> end();
    }
    else
    {
        if( processor -> getState() == THREAD_STATE_WORK )
        {
            processor -> pause() -> waitPause();
        }
        if( server -> getState() == THREAD_STATE_WORK )
        {
            server -> pause() -> waitPause();
        }
    }

    getMon()
    -> setString( Path{ "current", "processor" }, stateToString( processor -> getState()))
    -> setString( Path{ "current", "server" }, stateToString( server -> getState()));
}
