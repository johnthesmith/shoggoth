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
            -> setString
            (
                Path{ "config", "netMode", "server" }, netModeToString( netMode )
            )
            -> setString
            (
                Path{ "config", "netMode", "direct" }, netModeToString( directNetMode )
            );

            if( directNetMode != NET_MODE_UNKNOWN && directNetMode != netMode )
            {
                switch( directNetMode )
                {
                    case NET_MODE_UNKNOWN: break;
                    case NET_MODE_WORK: break;
                    case NET_MODE_LEARN:
                        netSwitchToLearn
                        (
                            ParamList::shared().get()
                            -> setString( "type", "EVOLUTION_CONFIG" )
                        );
                    break;
                    case NET_MODE_TEST:
                        netSwitchToLearn
                        (
                            ParamList::shared().get()
                            -> setString( "type", "EVOLUTION_CONFIG" )
                        );
                    break;
                }
            }
            else
            {
                if( netMode == NET_MODE_UNKNOWN )
                {
                    netSwitchToLearn
                    (
                        ParamList::shared().get()
                        -> setString( "type", "EVOLUTION_START" )
                    );
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
                                                commitNet
                                                (
                                                    false,
                                                    1e10,
                                                    ParamList::shared().get()
                                                    -> setString( "type", "EVOLUTION_NAN" )
                                                );

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

                                                if
                                                (
                                                    cfgMaxTick < net -> getTick() ||
                                                    (
                                                        smoth -> getCount() == cfgTickSmothCount &&
                                                        smoth -> deltaMinMax() < cfgTickDeltaLimit
                                                    )
                                                )
                                                {
                                                    netSwitchToTest
                                                    (
                                                        ParamList::shared().get()
                                                        -> setString( "type", "LEARNING_LIMIT" )
                                                        -> setInt( "cfgMaxTick", cfgMaxTick )
                                                        -> setString( "layer", layer -> getId() )
                                                    );

                                                    getLog() -> trace( "Result" )
                                                    -> prm( "layer", layer -> getId() )
                                                    -> prm( "reason", "LEARNINT_LIMIT" );
                                                }

                                                /* Destroy the smoth */
                                                smoth -> destroy();
                                            }
                                        }
                                        break;

                                        case NET_MODE_TEST:
                                            testStage( layer, cfg );
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



/*
    Test stage of the evolutio loop
*/
EvolutionPayload* EvolutionPayload::testStage
(
    Layer* aLayer,
    ParamList* aConfig
)
{
    if(  net -> getTick() != lastNetTick )
    {
        /* Change last tick */
        lastNetTick = net -> getTick();

        /* Read config */
        auto configTestTickCount = aConfig -> getInt( "testTickCount" );
        auto chart = aLayer -> getChartErrorsBeforeChange();

        /* Quantity check */
        if( chart -> getCount() > configTestTickCount )
        {
            auto parentSurvivalErrorAvg = net -> getConfig() -> getDouble
            (
                Path{ "survival", "error", "avg" }
            );

            double currentSurvivalErrorAvg = chart -> calcAvgY();
            double success = parentSurvivalErrorAvg > currentSurvivalErrorAvg;

            /* Quality test */
            commitNet
            (
                /* Success or fail */
                success,
                success ? currentSurvivalErrorAvg : parentSurvivalErrorAvg ,
                /* Build reason */
                ParamList::shared().get()
                -> setDouble( "testCount", chart -> getCount() )
                -> setDouble( "parentSurvivalErrorAvg", parentSurvivalErrorAvg )
                -> setDouble( "currentSurvivalErrorAvg", currentSurvivalErrorAvg )
                -> setString
                (
                    Path{ aLayer -> getId(), "errorsBeforeChange" },
                    chart -> toString( 40 )
                )
            );
        }
    }
    return this;
}



/******************************************************************************
    Actions methods
*/



/*
    Switch server net in to Learn mode
*/
EvolutionPayload* EvolutionPayload::netSwitchToLearn
(
    ParamList* aReason
)
{
    Io::create( net )
    -> setNetMode( NET_MODE_LEARN, aReason )
    -> resultTo( this )
    -> destroy();

    return this;
}



/*
    Switch server net in to Learn mode
*/
EvolutionPayload* EvolutionPayload::netSwitchToTest
(
    ParamList* aReason
)
{
    //
    Io::create( net )
    -> setNetMode( NET_MODE_TEST, aReason )
    -> resultTo( this )
    -> destroy();

    if( this -> isOk() )
    {
        /* Reset test success */
        testSuccessCount = 0;
    }

    return this;
}


/*
TODO

сети считаются по 10 лямов раз. это много - надо подправить коэффиуиенты.

Во вторых надо убедиться что ребенок не хуже родителя. Он должен быть лучше.
Если хуже новая сеть не проходит критерий отбора и родитель снова мутирует.
Мутации прикращаются мосле Н количества неуспешных итераций.

*/

/*
    Commit net
*/
EvolutionPayload* EvolutionPayload::commitNet
(
    bool aSuccess,
    double aSurvivalErrorAvg,
    ParamList* aReason
)
{
    getLog()
    -> info( "net commit" )
    -> prm( "success", aSuccess )
    -> dump( aReason );

    /* Clone parent net */
    auto io = Io::create( net );
    io -> getRequest()
    -> setBool( "success", aSuccess )
    -> setDouble( "survivalErrorAvg", aSurvivalErrorAvg )
    -> copyFrom( "reason", aReason )
    -> setString( "id", net -> getId())
    -> setString( "version", net -> getVersion());

    io
    -> call( CMD_COMMIT_NET )
    -> resultTo( this )
    -> destroy();

    if( isOk() )
    {
        setResult( aSuccess ? "evolution_commit" : "evolution_rollback" );
    }

    return this;
}



