#include <fstream>
#include <thread>
#include <unistd.h> /* usleep */

#include "limb_processor.h"
#include "processor_payload.h"

#include "../../shoggoth/func.h"
#include "../../shoggoth/shoggoth_consts.h"

#include "../../../../../lib/core/math.h"
#include "../../../../../lib/core/moment.h"
#include "../../../../../lib/core/str.h"
#include "../../../../../lib/graph/param_point.h"

#include "../../../../../lib/core/payload.h"




/*
    Constructor
*/
LimbProcessor::LimbProcessor
(
    /* Paylaod for limb */
    ProcessorPayload* aPayload
)
:Limb
(
    aPayload -> getApplication() -> getLogManager(),
    aPayload -> getId(),
    ""
)
{
    payload = aPayload;

//    weightsChart = ChartList::create();

    threadManager = ThreadManager::create
    (
        aPayload -> getApplication() -> getLogManager()
    );

    threadManagerWeight = ThreadManager::create
    (
        aPayload -> getApplication() -> getLogManager()
    );

    fps = ChartData::create() -> setMaxCount( 100 );
}




/**********************************************************************
    Main methods
*/



/*
    Calculate all layers
*/

LimbProcessor* LimbProcessor::calc()
{
    auto fpsBegin = now();
    auto net = payload -> getApplication() -> getNet();

    net -> lock();
//    if
//    (
//        /* Net is not locked for writing */
//        !net -> getWeightWriteLock()
//        &&
//        (
//            ( getVersion() != net -> getVersion() && getVersion() != "" )
//            ||
//            ( tickWrite != 0 && net -> getTick() % tickWrite == 0 )
//        )
//    )
//    {
//        /* Writes weights for current version */
//        weightsWrite( net -> getNervesPath( "", getVersion() ));
//    }
    payload -> getMon() -> now( Path{ "trace", "write_nerves" }, true);

    /* Detect changes of config */
    if( net -> syncToLimb( this ))
    {
        /*
            Clear parents and children for each layer
            Filling in the followng nerveControl
        */
        getLayerList() -> loop
        (
            []
            ( void* item )
            {
                (( LayerProcessor* ) item ) -> clearParentsAndChildren();
                return false;
            }
        );

        /* Reallocates and fill weights */
        nerveControl();
    }
    payload -> getMon() -> now( Path{ "trace", "syncToLimb" }, true );


    if( getVersion() != net -> getVersion() )
    {
        /* Set version from net */
        setVersion( net -> getVersion() );
    }
    net ->  unlock();

    /*
        Calculation begin
    */
    payload -> getMon()
    -> setInt( Path{ "config", "seed" }, net -> getRnd() -> getSeed() )
    -> setString( Path{ "config", "learningLayerId" }, getLearningLayerId() )
    -> setDouble( Path{ "config", "learningSpeed" }, learningSpeed )
    ;

    /* TODO need to analize and choise this or cmp in the layer   */
    auto netLastChangeValues = net -> getLastChangeValues();

    /* Swap values and errors */
    auto readedValues = net -> valuesAndErrorsToLimb
    (
        this,
        payload -> getConfig() -> getStringVector( Path{ "layers", "read-values" }),
        false
    );

    payload
    -> getMon()
    -> now( Path{ "trace" , "swapValuesAndErrors" }, true );

    auto learningLayer = getLayerList() -> getById( learningLayerId );

    auto needCalc =
    readedValues
    ||
    (
        learningLayer != NULL
        && learningLayer -> calcSumValue() > EPSILON_D
    );

    if( needCalc )
    {
        /* Increase the tick */
        net -> incTick();
        payload -> getMon() -> setInt( Path{ "current", "tick" }, net -> getTick() );

        /* Drop learing mode flag */
        calcDebugDump( CALC_STAGE_START );

        /*
            Init forward calculation (neuron values)
            from layers without parents
        */
        resetCalcState();
        payload -> getMon() -> now( Path{ "trace" , "resetCalcState" }, true );

        /* Start tasks */
        getLayerList() -> loop
        (
            [ this ]
            ( void* item )
            {
                auto layer = ( LayerProcessor* ) item;
                if( layer -> isInput() )
                {
                    layer -> forwardCalcComplete( threadManager );
                }
                return false;
            }
        );
        payload -> getMon() -> now( Path{ "trace" , "forwardStart" }, true );
        /* Wait tasks */
        threadManager -> wait();
        payload -> getMon() -> now( Path{ "trace" , "forwardWait" }, true );
        onChangeValues();
        calcDebugDump( CALC_STAGE_AFTER_FRONT );
        payload -> getMon() -> now( Path{ "trace", "forwardDump" }, true);

        /*
            Backward calculation (neuron errors)
        */
        resetCalcState();
        getLayerList() -> loop
        (
            [ this ]
            ( void* item )
            {
                auto layer = ( LayerProcessor* ) item;
                if( layer -> isOutput())
                {
                    layer
                    -> calcErrors()
                    -> backwardCalcComplete( threadManager );
                }
                return false;
            }
        );
        payload -> getMon() -> now( Path{ "trace" , "backwardStart" }, true );
        threadManager -> wait();
        payload -> getMon() -> now( Path{ "trace" , "backwardWait" }, true );
        calcDebugDump( CALC_STAGE_AFTER_BACK );
        payload -> getMon() -> now( Path{ "trace", "backwardDump" }, true);

        /*
            Learning calculation (nerve weights)
        */
        struct DataStruct
        {
            Nerve* nerve;
            real learningSpeed;
        };

        getNerveList()
        -> loop
        (
            [ this, &netLastChangeValues, &net ]
            ( void* item )
            {
                auto nerve = (Nerve*) item;
                auto data = DataStruct{ nerve, learningSpeed };

                auto terminate =
                terminated ||
                net -> getLastChangeValues() != netLastChangeValues;

                if( !terminate )
                {
                    threadManagerWeight
                    -> add( nerve -> buildId() )
                    -> run
                    (
                        []
                        ( void* data )
                        {
                            auto [ nerve, learningSpeed ] = *(DataStruct*) data;
                            nerve -> calcWeights( learningSpeed );
                        },
                        &data,
                        sizeof( data )
                    );
                }

                return terminate;
            }
        );

        payload -> getMon() -> now( Path{ "trace", "weightStart" }, true);
        threadManagerWeight -> wait();
        payload -> getMon() -> now( Path{ "trace", "weightWait" }, true );
        calcDebugDump( CALC_STAGE_AFTER_LEARNING );
        payload -> getMon() -> now( Path{ "trace", "weightDump" }, true );

        /*
            End of calculation
        */

        /* Move calculated data to net */
        if
        (
            !terminated  &&
            net -> getLastChangeValues() == netLastChangeValues
        )
        {
            net -> valuesAndErrorsFromLimb
            (
                this,
                payload -> getConfig() -> getStringVector( Path{ "layers", "write-values" }),
                false
            );
            /* Load weights to Net from this limb */
            net -> loadWeightsFrom( this )
            /* Write stat for Net */
            -> stat()
            ;
        }

        payload -> getMon() -> now( Path{ "trace", "move_data_to_net" }, true );

        /* Write charts in to monitoring */
        if
        (
            !terminated &&
            net -> getLastChangeValues() == netLastChangeValues &&
            tickChart != 0 &&
            net -> getTick() % tickChart == 0
        )
        {
            payload -> getMon() -> remove( Path{ "mon" } );

            auto monConfig = payload
            -> getConfig()
            -> getObject( Path{ "mon", "charts" });

            if( monConfig != nullptr )
            {
                net -> lock();
                net
                -> getLayerList()
                -> loop
                (
                    [ this, monConfig ]
                    (
                        void* aItem
                    )
                    {
                        auto layer = (Layer*) aItem;
                        auto layerId = layer -> getId();

                        if( monConfig -> getBool( Path{ "values", layerId } ))
                        {
                            payload -> getMon() -> setString
                            (
                                Path
                                {
                                    "mon",
                                    "values",
                                    strAlign( layer -> getId(), ALIGN_LEFT, 20 )
                                },
                                layer -> getChartValues() -> toString( 40 )
                            );
                        }

                        if( monConfig -> getBool( Path{ "errors", layerId } ))
                        {
                            payload -> getMon() -> setString
                            (
                                Path
                                {
                                    "mon",
                                    "errors",
                                    strAlign( layer -> getId(), ALIGN_LEFT, 20 )
                                },
                                layer -> getChartErrors() -> toString( 40 )
                            );
                        }

                        if( monConfig -> getBool( Path{ "ticks", layerId } ))
                        {
                            payload -> getMon() -> setString
                            (
                                Path
                                {
                                    "mon",
                                    "ticks",
                                    strAlign( layer -> getId(), ALIGN_LEFT, 20 )
                                },
                                layer -> getChartTick() -> toString( 40 )
                            );
                        }

        //                -> setString
        //                (
        //                    Path
        //                    {
        //                        "mon",
        //                        "errorsBeforeChange",
        //                        strAlign( layer -> getId(), ALIGN_LEFT, 20 )
        //                    },
        //                    layer -> getChartErrorsBeforeChange() -> toString( 40 )
        //                )
                        ;

                        return false;
                    }
                );

                net -> unlock();
            }
        }
        payload -> getMon() -> now( Path{ "trace", "write_charts" }, true );
    }
    else
    {
        setResult( "calc_sleep" );
        usleep( 100 );
    }

    payload -> getMon()
    -> trace( Path{ "trace" } )
    -> now( Path{ "current", "last" } )
    -> setString( Path{ "current", "result" }, getCode() )
    -> flush();

    if( needCalc )
    {
        if( fpsStart + SECOND < fpsBegin )
        {
            payload -> getMon() -> setInt( Path{ "current", "fps-fact" }, fpsTick );
            fpsStart = fpsBegin;
            fpsTick = 0;
        }
        else
        {
            fpsTick++;
        }

        /* Calc and out fps */
        auto fpsEnd = now();
        fps -> createLast( (real) SECOND / ( fpsEnd - fpsBegin ));
        payload -> getMon() -> setDouble( Path{ "current", "fps-max" }, fps -> avg() );
    }

    return this;
}



/*
    Create new layer for this limb and copy parameters from source layer.
    This method have to overriden at children Limbs.
*/
LayerProcessor* LimbProcessor::copyLayerFrom
(
    LayerProcessor* aLayerFrom
)
{
    auto result = LayerProcessor::create( this, aLayerFrom -> getId() );

    result
    -> setErrorCalc( aLayerFrom -> getErrorCalc() )
    -> setWeightCalc( aLayerFrom -> getWeightCalc() )
    -> setFrontFunc( aLayerFrom -> getFrontFunc() )
    -> setBackFunc( aLayerFrom -> getBackFunc() )
    -> setBackFuncOut( aLayerFrom -> getBackFuncOut() )
    -> setSize( aLayerFrom -> getSize() );

    return result;
}



/*
    Controling the nerve list and prepare layers
*/
LimbProcessor* LimbProcessor::nerveControl()
{
    /* Allocate weigth */
    getNerveList() -> weightsAllocate();

    auto loadingError = false;
    auto net = payload -> getApplication() -> getNet();

    getNerveList() -> loop
    (
        [ &loadingError, &net ]
        ( void* item )
        {
            auto nerve = ( Nerve* ) item;

            /* Network was realocated and have to loading */
            if( !nerve -> loadWeight( net -> getNervesPath() ) -> isOk() )
            {
                loadingError = true;
            }

            /* Fill parents and children for layers */
            (( LayerProcessor* )(nerve -> getParent()))
            -> addChild( nerve );

            (( LayerProcessor* )( nerve -> getChild()))
            -> addParent( nerve );

            return false; //loadingError;
        }
    );

    /* Generage weights */
    if( loadingError )
    {
        getNerveList() -> loop
        (
            [ net ]
            ( void* item )
            {
                auto nerve = ( Nerve* ) item;
                nerve -> setOk();
                nerve -> fill( net -> getRnd() );

                net -> getDb() -> writeStartWeights
                (
                    net -> getVersion(),
                    nerve -> calcId(),
                    nerve -> calcSumWeights(),
                    nerve -> getWeightsCount()
                );
                return false;
            }
        );
    }


    return this;
}



/**********************************************************************
    Setters and getters
*/



//
///*
//    Get tick fo write weight
//*/
//int LimbProcessor::getTickWrite()
//{
//    return tickWrite;
//}
//
//
//
///*
//    Set tick for write weights
//*/
//LimbProcessor* LimbProcessor::setTickWrite
//(
//    int a
//)
//{
//    tickWrite = a;
//    return this;
//}
//


/*
    Set tick for write charts
*/
LimbProcessor* LimbProcessor::setTickChart
(
    int a
)
{
    tickChart = a;
    return this;
}




/*
    Dump layers
*/
LimbProcessor* LimbProcessor::dumptypeLayerProcessing
(
    string aFile,
    ParamList* aLayers,
    Dataview aDataview,
    Data aData,
    int aTick
)
{
    aLayers -> loop
    (
        [ this, aFile, &aDataview, &aData, aTick ]
        ( Param* param )
        {
            /* Get layer by index */
            auto layer = getLayerList()
            -> getById( param -> getString());
            if( layer == NULL )
            {
                getLog()
                -> warning( "Layer monitoring not found" )
                -> prm( "id", param -> getString() )
                ;
            }
            else
            {
                /* Dump */
                dumpLayer( aFile, layer, aData, aDataview, aTick );
            }
            return false;
        }
    );
    return this;
}




/*
    Dump layers
*/
LimbProcessor* LimbProcessor::dump()
{
    getLayerList() -> loop
    (
        []
        ( void* item )
        {
            /* Get layer by index */
            ((LayerProcessor*) item) -> dump();
            return false;
        }
    );
    return this;
}




/*
    Write debug monitoring
*/
LimbProcessor* LimbProcessor::calcDebugDump
(
    /* Calculation stage CALC_STAGE_* */
    CalcStage aStage
)
{
    /* Convert stage */
    auto stage = calcStageToString( aStage );
    auto stageAll = calcStageToString( CALC_STAGE_ALL );
    auto dumpConf = payload -> getConfig() -> getObject( Path{ "dump" });
    auto net = payload -> getApplication() -> getNet();

    /* Loop for monitoring rules from config */
    dumpConf -> objectsLoop
    (
        [ this, &stage, &stageAll, &aStage, &net ]
        ( ParamList* params, string )
        {
            /* Enabled check */
            if( params -> getBool( Path{ "enabled" }, true ))
            {
                /* File name */
                auto file = params -> getString( Path{ "file" }, "" );

                /* Stage check */
                auto stages = params -> getObject( Path{ "stages" });
                if
                (
                    file != "" &&
                    stages != NULL &&
                    (
                        stages -> contains( stage ) ||
                        stages -> contains( stageAll )
                    )
                )
                {
                    /* Types defenition as DUMP_TYPE_* */
                    auto dumptype = dumptypeFromString
                    (
                        params -> getString( Path{ "dumptype" })
                    );

                    /* Change file to mon path/file */
                    file = payload -> getApplication() -> getMonPath( file );

                    /* Replace stage and tick in the file */
                    file = replace
                    (
                        file,
                        vector <string>
                        {
                            "%stage%",
                            "%tick%",
                            "%dumptype%"
                        },
                        vector <string>
                        {
                            stage,
                            toString( net -> getTick() ),
                            dumptypeToString( dumptype )
                        }
                    );

                    /* Read dataview type */
                    auto dataview = dataviewFromString
                    (
                        params -> getString( Path{ "dataview" }),
                        DATAVIEW_FLOAT
                    );

                    /* Data type loop VALUES ERROR WEIGHTS */
                    auto datas = params -> getObject( Path{ "data" });
                    if( datas != NULL )
                    {
                        datas -> loop
                        (
                            [
                                this,
                                &params,
                                file,
                                dataview,
                                dumptype
                            ]
                            ( Param* item )
                            {
                                auto data = dataFromString( item -> getString());
                                auto file1 = replace( file, "%data%", item -> getString() );

                                switch( dumptype )
                                {
                                    default:
                                    case DUMP_TYPE_LAYER:
                                    {
                                        auto layers = params -> getObject( Path{ "layers" });
                                        if( layers != NULL )
                                        {
                                            dumptypeLayerProcessing
                                            (
                                                file1,
                                                layers,
                                                dataview,
                                                data,
                                                payload -> getApplication()-> getNet() -> getTick()
                                            );
                                        }
                                    }
                                    break;
                                    case DUMP_TYPE_NERVE:
                                    {
                                        auto nervesConfig = params -> getObject( Path{ "nerves" });
                                        if( nervesConfig != NULL )
                                        {
                                            nervesConfig -> loop
                                            (
                                                [ this, file1, data, dataview ]
                                                ( Param* item )
                                                {
                                                    if( item != NULL && item -> isObject() )
                                                    {
                                                        auto nerveRecord = item -> getObject();
                                                        if( nerveRecord -> getCount() > 1 )
                                                        {
                                                            auto from = nerveRecord -> getByIndex( 0 ) -> getString();
                                                            auto to = nerveRecord -> getByIndex( 1 ) -> getString();

                                                            auto file2 = replace
                                                            (
                                                                file1,
                                                                vector <string> {"%layer-from%", "%layer-to%"},
                                                                vector <string> { from, to }
                                                            );

                                                            getNerveList() -> loop
                                                            (
                                                                [
                                                                    this,
                                                                    file2,
                                                                    data,
                                                                    dataview,
                                                                    from,
                                                                    to
                                                                ]
                                                                ( void* item )
                                                                {
                                                                    auto nerve = ( Nerve* )item;

                                                                    if
                                                                    (
                                                                        nerve -> getParent() -> getId() == from &&
                                                                        nerve -> getChild() -> getId() == to
                                                                    )
                                                                    {
                                                                        dumpNerve
                                                                        (
                                                                            file2,
                                                                            nerve,
                                                                            data,
                                                                            dataview
                                                                        );
                                                                    }
                                                                    return false;
                                                                }
                                                            );

                                                        }
                                                    }
                                                    return false;
                                                }
                                            );
                                        }
                                    }
                                    break;
                                    case DUMP_TYPE_NEURON:
                                    {
                                        /* Layer defining */
                                        auto neurons = params -> getObject( Path{ "neurons" });
                                        if( neurons != NULL )
                                        {
                                            neurons -> loop
                                            (
                                                [ this, file1, data ]
                                                ( Param* param )
                                                {
                                                    if( param -> isObject() )
                                                    {
                                                        auto layerId = param -> getName();
                                                        auto layer = getLayerList()
                                                        -> getById( layerId );
                                                        if( layer != NULL )
                                                        {
                                                            auto poses = param -> getObject();
                                                            poses -> objectsLoop
                                                            (
                                                                [
                                                                    this,
                                                                    layer,
                                                                    data,
                                                                    file1
                                                                ]
                                                                ( ParamList* item, string )
                                                                {
                                                                    auto pos = ParamPoint::point3i( item );

                                                                    /* Replace file name */
                                                                    auto file2 = replace
                                                                    (
                                                                        file1,
                                                                        "%neuron%",
                                                                        pos.toString()
                                                                    );
                                                                    /* Dump for neuron */

                                                                    dumpNeuron
                                                                    (
                                                                        file2,
                                                                        layer,
                                                                        pos,
                                                                        data
                                                                    );
                                                                    return false;
                                                                } /* Poses loop callback */
                                                            ); /* Poses loop */
                                                        } /* Layer exists */
                                                    } /* Layer params exists */
                                                    return false;
                                                } /* Neurons params loop callback */
                                            ); /* Neurons loop */
                                        } /* Neurons exists */
                                        break;
                                    } /* case DUMP_TYPE_NEURON */
                                } /* dumptypeswitch */
                                return false;
                            } /* Datas callback  */
                        );  /* datas loop */
                    } /* Datas exists */
                } /* Stage check */

                /* Pause */
                auto pause = params -> getInt( Path{ "pauseMcs" }, 0 );
                if( pause > 0 )
                {
                    usleep( pause );
                }

                /* Lock */
                if( params -> getBool( Path{ "lock" }, false ))
                {
                    params = ParamList::create();
                    params
                    -> setInt( "tick", net -> getTick() )
                    -> setString( "stage", calcStageToString( aStage ) );

                    auto file = payload
                    -> getApplication()
                    -> getMonPath( "processor_thread_calc_lock.json" );

                    getLog()
                    -> info( "Process locked" )
                    -> prm( "file", file )
                    -> lineEnd();

                    net -> getApplication() -> lock
                    (
                        file,
                        params,
                        [ this ](){ return terminated; }
                    );

                    getLog()
                    -> info( "Process unlocked" )
                    -> prm( "file", file );

                    params -> destroy();
                }

            } /* Enabled check */
            return false;
        }
    );

    return this;
}



LimbProcessor* LimbProcessor::weightsWrite
(
    /* Path folder for layers */
    string a
)
{
    getNerveList() -> loop
    (
        [ &a, this ]( void *item )
        {
            auto nerve = ( Nerve* ) item;
            if
            (
                !nerve
                -> saveWeight( a )
                -> isOk()
            )
            {
                getLog()
                -> warning( nerve -> getCode() )
                -> dump( nerve -> getDetails() );
                nerve -> setOk();
            }
            return false;
        }
    );
    return this;
}
