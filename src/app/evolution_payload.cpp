#include <iostream>
#include <cmath>

/* User libraries */
#include "evolution_application.h"
#include "evolution_payload.h"
#include "../shoggoth/io.h"



using namespace std;


/*
    Constructor
*/
EvolutionPayload::EvolutionPayload
(
    EvolutionApplication* a,
    string aNetId,
    string aNetVersion
)
: Payload( a ) /* Call parent constructor */
{
    net = Net::create( a, a -> getSockManager(), aNetId, aNetVersion );
    net -> addTask( TASK_EVOLUTION );
}



/*
    Destructor
*/
EvolutionPayload::~EvolutionPayload()
{
    net -> destroy();
}



/*
    Creator
*/
EvolutionPayload* EvolutionPayload::create
(
    EvolutionApplication* a,
    string aNetId,
    string aNetVersion
)
{
    return new EvolutionPayload( a, aNetId, aNetVersion );
}



/*
    Destructor
*/
void EvolutionPayload::destroy()
{
    delete this;
}



/*
    Return application object
*/
EvolutionApplication* EvolutionPayload::getApplication()
{
    return ( EvolutionApplication* ) Payload::getApplication();
}




/******************************************************************************
    Payload events
*/



/*
    Main loop event
*/
void EvolutionPayload::onLoop()
{
    getLog() -> trapOn() -> begin( "Loop" );
    getMon() -> startTimer( Path{ "loop", "moment" });

    if( getApplication() -> getCli() -> getString( "config" ) == "" )
    {
        getLog() -> error( "Need the --config cli argument." );
        terminate();
    }
    else
    {
        /* Check local application config */
        getApplication() -> checkConfigUpdate();

        if( getApplication() -> getConfigUpdated() )
        {
            auto cfg = getApplication() -> getConfig();

            if( cfg != NULL )
            {
                setLoopTimeoutMcs( cfg -> getDouble( "loopSleepMcs", 100 ));
            }
        }

        /* Check server net config */
        auto netConfig = ParamList::create();

        /* Read net config from server */
        net -> readNet( netConfig );

        if
        (
            net -> isConfigUpdate( netConfig ) ||
            net -> isVersionChanged()
        )
        {
            getLog()
            -> begin( "Config updated" )
            -> prm( "File", getApplication() -> getConfigFileName() )
            -> dump( netConfig, "Net config" )
            -> lineEnd();

            net -> applyNet( netConfig );

            getLog() -> end();
        }

        netConfig -> destroy();

        /* Synchronize net from the server */
        net -> syncWithServer();

        /* Processing evolution */
        processing();
    }

    getMon() -> flush();
    getLog() -> end( getCode() ) -> trapOff();
}



/******************************************************************************
    Private methods
*/



/*
    Processing of the loop
*/
EvolutionPayload* EvolutionPayload::processing()
{
    enum EvolutionResult
    {
        CONTINUE,
        ROLLBACK,
        MUTATE
    };

    EvolutionResult action = CONTINUE;
    getLog() -> begin( "Processing" );

    /*
        Nan condition
        TODO проверить почему нан не приехал
    */
    net -> getLayerList() -> loop
    (
        [ this, &action ]
        ( void* item )
        {
            auto layer = ( Layer* ) item;
            if( isnan( layer -> calcSumValue()) )
            {
                action = ROLLBACK;
                getLog() -> trace( "Result" )
                -> prm( "layer", layer -> getId() )
                -> prm( "reason", "is nan" );
            }
            return false;
        }
    );

    /*
        Next case of  evolution ...
    */


    /* Define result */
    auto result = Result::create();

    /* Result processing */
    switch( action )
    {
        default:
        case CONTINUE:
            /* None changes, net will be teaching */
            getLog() -> trace( "action" ) -> prm( "state", "Continue" );
        break;
        case ROLLBACK:
            getLog() -> info( "action" ) -> prm( "state", "Rollback" );

            /* Clone parent net */
            Io::create( net )
            -> mutateParentAndSwitch()
            -> resultTo( result )
            -> destroy();
        break;
        case MUTATE:
            getLog() -> info( "action" ) -> prm( "state", "Mutate" );
            /* Clone net */
            Io::create( net )
            -> mutateCurrentAndSwitch()
            -> resultTo( result )
            -> destroy();
        break;
    }

    getLog() -> end( result -> getCode() );

    result -> destroy();

    return this;
}
