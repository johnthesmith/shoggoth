#include <iostream>
#include <cmath>

/* User libraries */
#include "evolution_application.h"
#include "evolution_payload.h"
#include "../../shoggoth/io.h"
#include "../../../../../lib/core/str.h"



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
: PayloadEngine( a ) /* Call parent constructor */
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
void EvolutionPayload::onEngineLoop()
{
    /* Reset net state */
    net -> setOk();

    getMon()
    -> setString( Path{ "net", "id" }, net -> getId() )
    -> setString( Path{ "net", "version" }, net -> getVersion() )
    ;

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
        -> begin( "Net config updated" )
        -> prm( "File", getApplication() -> getConfigFileName() )
        -> dump( netConfig, "Net config" )
        -> lineEnd();
        net -> applyNet( netConfig );
        getLog() -> end();
    }
    netConfig -> destroy();

    /* Synchronize net from the server */
    net
    -> syncWithServer()
    -> resultTo( this );

    if( isOk() )
    {
        getLog() -> begin( "Processing" );

        /* Processing evolution */
        enum EvolutionResult
        {
            CONTINUE,   /* Learning continue */
            ROLLBACK,   /* Kill current and paent mutate (new child) */
            MUTATE      /* New from current (new child) */
        };

        EvolutionResult action = CONTINUE;

        net -> getLayerList() -> loop
        (
            [ this, &action ]
            ( void* item )
            {
                auto layer = (Layer*) item;

                /* Read config */
                auto cfg = getApplication()
                -> getConfig()
                -> getObject( Path{ "criterions", layer -> getId() } );

                if( isnan( layer -> calcSumValue()) )
                {
                    /* Checking the NAN criterion */
                    action = ROLLBACK;
                    getLog() -> info( "Result" )
                    -> prm( "layer", layer -> getId() )
                    -> prm( "reason", "is_nan" );
                }
                else if
                (
                    /* Checking the tick delta criterion */
                    cfg != NULL &&
                    cfg -> exists( Path{ "tickDeltaLimit" })
                )
                {
                    /* Let config params */
                    auto cfgTickSmoth = cfg -> getDouble( "tickSmoth" );
                    auto cfgTickDeltaLimit = cfg -> getDouble( "tickDeltaLimit" );
                    auto cfgTickSmothCount = cfg -> getInt( "tickSmothCount" );

                    /* Ticks smothing */
                    auto smoth = ChartData::create() -> setMaxCount( cfgTickSmothCount );
                    layer
                    -> getChartTick()
                    -> smoth( cfgTickSmoth, smoth );

                    /* Monitoring */
                    getMon()
                    -> setDouble( Path{ "settings", layer -> getId(), "tickDeltaLimit" }, cfgTickDeltaLimit )
                    -> setDouble( Path{ "settings", layer -> getId(), "tickSmothCount" }, cfgTickSmothCount )
                    -> setDouble( Path{ "settings", layer -> getId(), "tickSmoth" }, cfgTickSmoth )
                    -> setString
                    (
                        Path{ "ticks", strAlign( layer -> getId(), ALIGN_LEFT, 20 ) },
                        layer -> getChartTick() -> toString( 40 )
                    )
                    -> setString
                    (
                        Path{ "tickssmoth", strAlign( layer -> getId(), ALIGN_LEFT, 20 ) },
                        smoth -> toString( 40 )
                    );

                    if
                    (
                        smoth -> deltaMinMax() < cfgTickDeltaLimit &&
                        smoth -> getCount() == cfgTickSmothCount
                    )
                    {
                        action = MUTATE;
                        getLog() -> trace( "Result" )
                        -> prm( "layer", layer -> getId() )
                        -> prm( "reason", "learning_limit" );
                    }

                    /* Destroy the smoth */
                    smoth -> destroy();
                }

                return false;
            }
        );


        /* Result processing */
        switch( action )
        {
            default:
            case CONTINUE:
                /* None changes, net will be teaching */
                setResult( "evolution_continue" );
            break;
            case ROLLBACK:
                getLog() -> info( "action" ) -> prm( "state", "Rollback" );
                    /* Clone parent net */


// Поймать ошибку разрушения процеса при отсутсвии сервера
// В тичерезаковырять limb но с оглядкой на ниже написанное...

// TODO Обдумать весь цикл обучения и контроля на статичных картинках КАК?
// Возможно понадобится коммуникая между еволюшеном и тичером??!!
// присобачить условие отката сети в случае если она училась дольше #272

                Io::create( net )
                -> mutateAndSwitch( 1 /*.... ? 1 : 2 */ )
                -> resultTo( this )
                -> destroy();

                if( isOk() )
                {
                    setResult( "evolution_rollback" );
                }
            break;
            case MUTATE:
                getLog() -> info( "action" ) -> prm( "state", "Mutate" );
                /* Clone net */
                Io::create( net )
                -> mutateAndSwitch( 0 )
                -> resultTo( this )
                -> destroy();

                if( isOk() )
                {
                    setResult( "evolution_mutate" );
                }
            break;
        }
    }

    getLog() -> end();
}

