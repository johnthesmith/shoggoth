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
    /* Application object */
    EvolutionApplication* a,
    /* Payload id */
    string aPayloadId,
    /* Net id */
    string aNetId,
    /* Net version */
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
        TASK_EVOLUTION
    );
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
    string aPayloadId,
    string aNetId,
    string aNetVersion
)
{
    return new EvolutionPayload( a, aPayloadId, aNetId, aNetVersion );
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
void EvolutionPayload::onEngineLoop
(
    bool,
    bool aEnabled
)
{
    if( aEnabled )
    {
        /* Reset net state */
        net -> setOk();

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

            /* Get server netMode */
            NetMode netMode;

            Io::create( net )
            -> getNetMode( netMode )
            -> resultTo( this )
            -> destroy();

            /* Retrive the config structure */
            auto localConfig = getApplication() -> getConfig();

            /* Read default net mode from string */
            auto directNetMode = netModeFromString
            (
                localConfig
                -> getString
                (
                    Path
                    {
                        "engine",
                        "evolution",
                        "netMode",
                        "direct"
                    }
                )
            );

            getMon()
            -> setString( Path{ "config", "netMode", "server" }, netModeToString( netMode ) )
            -> setString( Path{ "config", "netMode", "direct" }, netModeToString( directNetMode ) );

            if( directNetMode != NET_MODE_UNKNOWN && directNetMode != netMode )
            {
                switch( directNetMode )
                {
                    case NET_MODE_UNKNOWN: break;
                    case NET_MODE_WORK: break;
                    case NET_MODE_LEARN: netSwitchToLearn(); break;
                    case NET_MODE_TEST: netSwitchToTest(); break;
                }
            }
            else
            {
                if( netMode == NET_MODE_UNKNOWN )
                {
                    netSwitchToLearn();
                }
                else
                {
                    auto configPath = Path
                    {
                        "engine",
                        "evolution",
                        "netMode",
                        "config",
                        netModeToString( netMode )
                    };

                    /* Retrive the configuration for curent net mode */
                    auto modeConfig = localConfig -> getObject( configPath );
                    if( modeConfig == NULL )
                    {
                        setResult( "NoConfigForNetMode" )
                        -> getDetails() -> setString( "path", implode( configPath, "." ));
                    }
                    else
                    {
                        auto cfgMaxTick = modeConfig -> getInt( "maxTick" );
                        /* Loop for the each layer of the net mode configuration */
                        modeConfig -> objectsLoop
                        (
                            Path{ "layers" },
                            [ this, &netMode, &cfgMaxTick ]
                            (
                                ParamList* cfg,
                                string layerId
                            )
                            {
                                auto layer = net -> getLayerList()
                                -> getById( layerId );

                                if( layer == NULL )
                                {
                                    setResult( "UnknownLayer" )
                                    -> getDetails()
                                    -> setString( "path", layerId );
                                }
                                else
                                {
                                    /* Decision making */
                                    switch( netMode )
                                    {
                                        case NET_MODE_WORK:
                                        case NET_MODE_UNKNOWN:
                                        break;

                                        case NET_MODE_LEARN:
                                        {
                                            if( isnan( layer -> calcSumValue()) )
                                            {
                                                /* Checking the NAN criterion */
                                                netRollback();
                                                netSwitchToLearn();
                                                getLog() -> info( "Result" )
                                                -> prm( "layer", layer -> getId() )
                                                -> prm( "reason", "is_nan" );
                                            }
                                            else
                                            {
                                                /* Let config params */
                                                auto cfgTickSmoth = cfg -> getDouble( "tickSmoth" );
                                                auto cfgTickDeltaLimit = cfg -> getDouble( "tickDeltaLimit" );
                                                auto cfgTickSmothCount = cfg -> getInt( "tickSmothCount" );

                                                /* Ticks smothing */
                                                auto smoth = ChartData::create()
                                                -> setMaxCount( cfgTickSmothCount );

                                                layer
                                                -> getChartTick()
                                                -> smoth( cfgTickSmoth, smoth );

                                                getMon()
                                                -> setString
                                                (
                                                    Path
                                                    {
                                                        "modes",
                                                        netModeToString( netMode ),
                                                        "tickssmoth",
                                                        strAlign( layer -> getId(), ALIGN_LEFT, 20 )
                                                    },
                                                    smoth -> toString( 40 )
                                                );

/*
TODO 
Эволюшен должен собрать статистику и информаировать сервер который меняетсеть
указать причину завершения обучения сети
указать результат тестирования сети
сервер должен это все бережно собрать и сохранить в конфигурацию сети как отчет
те для мутации и для переключения моды должно быть пояснение со стороны того кто это делает
почему выполняется мутация
почему переключается режим
почему выполняется ролбек
с какими параметрами.
*/

                                                if
                                                (
                                                    cfgMaxTick < net -> getTick() ||
                                                    (
                                                        smoth -> getCount() == cfgTickSmothCount &&
                                                        smoth -> deltaMinMax() < cfgTickDeltaLimit
                                                    )
                                                )
                                                {
                                                    netSwitchToTest();
                                                    getLog() -> trace( "Result" )
                                                    -> prm( "layer", layer -> getId() )
                                                    -> prm( "reason", "learning_limit" );
                                                }

                                                /* Destroy the smoth */
                                                smoth -> destroy();
                                            }
                                        }
                                        break;

                                        case NET_MODE_TEST:
                                        {
                                            /* Read config */
                                            auto configTestTickCount = cfg -> getInt( "testTickCount" );
                                            auto configTestErrorLimit = cfg -> getInt( "testErrorLimit" );
                                            auto chart = layer -> getChartErrorsBeforeChange();

                                            if( chart -> getMaxY() > configTestErrorLimit )
                                            {
                                                netRollback();
                                                netSwitchToLearn();
                                            }

                                            if( chart -> getCount() > configTestTickCount )
                                            {
                                                netMutate();
                                                netSwitchToLearn();
                                            }
                                        }
                                        break;
                                    }   /*  End of net mode case */
                                } /* Unknown layer */
                                return false;
                            }
                        ); /* Loop of layers in config mode */
                    }
                }
            } /* Net mode is not unknown */

            /* Layers stat out */
            net -> getLayerList() -> loop
            (
                [ this ]
                ( void* item )
                {
                    auto layer = (Layer*) item;

                    /* Monitoring */
                    getMon()
                    -> setString
                    (
                        Path{ "ticks", strAlign( layer -> getId(), ALIGN_LEFT, 20 ) },
                        layer -> getChartTick() -> toString( 40 )
                    )
                    -> setString
                    (
                        Path
                        {
                            "errorsBeforeChange",
                            strAlign( layer -> getId(), ALIGN_LEFT, 20 )
                        },
                        layer -> getChartErrorsBeforeChange() -> toString( 40 )
                    );

                    return false;
                }
            );

            getLog() -> end();
        }

        getMon()
        -> setInt( Path{ "net", "tick" }, net -> getTick() )
        -> setString( Path{ "net", "id" }, net -> getId() )
        -> setString( Path{ "net", "version" }, net -> getVersion() )
        -> setInt( Path{ "net", "count", "layers" }, net -> getLayerList() -> getCount() )
        -> setInt( Path{ "net", "count", "nerves" }, net -> getNerveList() -> getCount() )
        ;

    }
}




/******************************************************************************
    Actions methods
*/



/*
    Switch server net in to Learn mode
*/
EvolutionPayload* EvolutionPayload::netSwitchToLearn()
{
    Io::create( net )
    -> setNetMode( NET_MODE_LEARN )
    -> resultTo( this )
    -> destroy();

    return this;
}



/*
    Switch server net in to Learn mode
*/
EvolutionPayload* EvolutionPayload::netSwitchToTest()
{
    Io::create( net )
    -> setNetMode( NET_MODE_TEST )
    -> resultTo( this )
    -> destroy();

    return this;
}



/*
    Rollback net
*/
EvolutionPayload* EvolutionPayload::netRollback()
{
    getLog() -> info( "action" ) -> prm( "state", "Rollback" );

    /* Clone parent net */
    Io::create( net )
    -> mutateAndSwitch( 1 /*.... ? 1 : 2 */ )
    -> resultTo( this )
    -> destroy();

    if( isOk() )
    {
        setResult( "evolution_rollback" );
    }

    return this;
}



/*
    Mutate net
*/
EvolutionPayload* EvolutionPayload::netMutate()
{
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

    return this;
}
