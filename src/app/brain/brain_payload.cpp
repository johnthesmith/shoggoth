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
    string aPayloadId,
    string aNetId,
    string aNetVersion
)
: PayloadEngine( a, aPayloadId ) /* Call parent constructor */
{
    net = Net::create
    (
        a,
        a -> getSockManager(),
        aNetId,
        aNetVersion,
        TASK_PROC
    );

    /* Create server and Brain payloads */
    server = Server::create( net );
    processor = Processor::create( net );
}





/*
    Destructor
*/
BrainPayload::~BrainPayload()
{
    processor -> destroy();
    server -> destroy();

    net -> destroy();
}



/*
    Creator
*/
BrainPayload* BrainPayload::create
(
    BrainApplication* a,
    string aPayloadId,
    string aNetId,
    string aNetVersion
)
{
    return new BrainPayload( a, aPayloadId, aNetId, aNetVersion );
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

        string file = net -> getNetConfigFile( net -> getNextVersion() );

        /* Read net config from server */
        net -> readNetFromFile( netConfig );

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
                    processor -> stop();
                    server -> stop();
                    processor -> waitStop();
                    server -> waitStop();
                getLog() -> end( "" );

                if( netConfigUpdated || netVersionChanged )
                {
                    /* Apply config */
                    net -> applyNet( netConfig );
                }

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

                server -> start( true );
                processor -> start( true );
            }

            getLog() -> end();
        }

        netConfig -> destroy();
        getLog() -> end();
    }
    else
    {
        if( processor -> getState() == STATE_LOOP )
        {
            processor -> stop() -> waitStop();
        }
        if( server -> getState() == STATE_LOOP )
        {
            server -> stop() -> waitStop();
        }
    }

    getMon()
    -> setString( Path{ "current", "processor" }, stateToString( processor -> getState()))
    -> setString( Path{ "current", "server" }, stateToString( server -> getState()));

}
