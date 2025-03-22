#include <fstream>
#include <thread>
#include <unistd.h> /* usleep */

#include "limb_processor.h"

#include "../../shoggoth/func.h"
#include "../../shoggoth/shoggoth_consts.h"

#include "../../../../../lib/core/math.h"
#include "../../../../../lib/core/moment.h"
#include "../../../../../lib/core/str.h"
#include "../../../../../lib/graph/param_point.h"




/*
    Constructor
*/
LimbProcessor::LimbProcessor
(
    Net* aNet /* Net limb object*/
)
:Limb( aNet -> getLogManager() )
{
    net             = aNet;

    mon = Mon::create( net -> getMonPath( "procesor.json" ))
    -> setString( Path{ "start", "Source" }, "Limb processor" )
    -> now( Path{ "start", "now" });

    weightsChart = ChartList::create();
    dumpConf = ParamList::create();

    fps = ChartData::create() -> setMaxCount( 100 );

    threadManager = ThreadManager::create( aNet -> getLogManager() );


    calcTableValues = CalcTable::create();
    calcTableErrors = CalcTable::create();
}



/*
    Destructor
*/
LimbProcessor::~LimbProcessor()
{
    calcTableValues -> destroy();
    calcTableErrors -> destroy();

    threadManager -> destroy();

    fps -> destroy();

    dumpConf -> destroy();
    weightsChart -> destroy();

    mon -> destroy();
}



/*
    Create
*/
LimbProcessor* LimbProcessor::create
(
    Net* aNet       /* Limb object*/
)
{
    return new LimbProcessor( aNet );
}



/**********************************************************************
    Main methods
*/



/*
    Set terminated signal
*/
LimbProcessor* LimbProcessor::setTerminated
(
    bool a
)
{
    terminated = a;
    return this;
}



LimbProcessor* LimbProcessor::setMaxThreadCount
(
    int a
)
{
    maxThreadCount = a;
    return this;
}



int LimbProcessor::getMaxThreadCount()
{
    return maxThreadCount;
}



LimbProcessor* LimbProcessor::setMinNeuronPerThread
(
    int a
)
{
    minNeuronPerThread = a;
    return this;
}



int LimbProcessor::getMinNeuronPerThread()
{
    return minNeuronPerThread;
}



/*
    Calculate all layers
*/
LimbProcessor* LimbProcessor::calc()
{
    auto fpsBegin = now();

    /* Increase the tick */
    net -> incTick();

    mon -> now( Path{ "trace", "start" }, true );

    /* Check structure with net */
    if( net -> syncToLimb( this, false ))
    {
        /* Updated */
        buildCalcTables();
        /* Calculae thread count for each layer */
        calcThreadCount();
    }

    mon -> now( Path{ "trace", "syncToLimb" }, true );

    /*
        Calculation begin
    */
    mon
    -> setInt( Path{ "net", "seed" }, net -> getRnd() -> getSeed() )
    -> setDouble( Path{ "config", "learningSpeed" }, learningSpeed )
    -> setDouble( Path{ "config", "minWeight" }, minWeight )
    -> setDouble( Path{ "config", "maxWeight" }, maxWeight )
    -> setDouble( Path{ "config", "maxError" }, maxError )
    -> setInt( Path{ "current", "tick" }, net -> getTick() )
    ;

    mon -> now( Path{ "trace", "mon" }, true);

    /*   */
    nerveControl();

    mon -> now( Path{ "trace", "nerveControl" }, true );

    auto netLastChangeValues = net -> getLastChangeValues();

    /* Swap values and errors */
    net
    -> swapValuesAndErrors
    (
        /* Action */
        {
            /* Upload start values from Net */
            READ_VALUES
        },
        /* Participant object */
        this,
        false
    );

    mon -> now( Path{ "trace" , "swap" }, true );

    /* Drop learing mode flag */
    learning = false;
    calcDebugDump( CALC_STAGE_START );

    mon -> now( Path{ "trace", "debugDumpStageStart" }, true );

    /*
        Forward calculation (neuron values)
    */
    calcTableValues
    -> reset()
    -> loop
    (
        [ this, &netLastChangeValues ]
        ( CalcTable* table, Layer* layer, bool &terminate )
        {
            terminate = terminated || net -> getLastChangeValues() != netLastChangeValues;

            if( !terminate )
            {
                /* Exclude calculation for layers without parents */
                if( ! getNerveList() -> parentsExists( layer ))
                {
                    return true;
                }
                /* Calculate all layers with parents calculating */
                if( table -> isParentsCalculated( layer, getNerveList() ))
                {
                    /* Set default thread id */
                    layerCalc( layer, DATA_VALUES );
                    return true;
                }
            }

            return false;
        }
    );

    mon -> now( Path{ "trace" , "calcFront" }, true );

    calcDebugDump( CALC_STAGE_AFTER_FRONT );

    mon -> now( Path{ "trace", "debugDumpStageAfterFront" }, true);

    /*
        Backward calculation (neuron errors)
    */
    calcTableErrors
    -> reset()
    -> loop
    (
        [ this, &netLastChangeValues ]
        ( CalcTable* table, Layer* layer, bool& terminate )
        {
            terminate = terminated || net -> getLastChangeValues() != netLastChangeValues;

            if( !terminate )
            {
                if( table -> isChildrenCalculated( layer, getNerveList() ))
                {
                    /* Calculate errors for layers with parents */
                    layerCalc( layer, DATA_ERRORS );
                    return true;
                }
            }

            return false;
        }
    );

    mon -> now( Path{ "trace", "calc_back" }, true );

    calcDebugDump( CALC_STAGE_AFTER_BACK );

    mon -> now( Path{ "trace", "debug_dump_stage_after_back" }, true);

    /*
        Learning calculation (nerve weights)
    */

    getLayerList()
    -> loop
    (
        [ this, &netLastChangeValues ]
        ( void* item )
        {
            auto layer = (Layer*) item;
            auto terminate = terminated || net -> getLastChangeValues() != netLastChangeValues;
            if( !terminate )
            {
                layerCalc( layer, DATA_WEIGHTS );
            }
            return terminate;
        }
    );

    mon -> now( Path{ "trace", "calc_learning" }, true );

    calcDebugDump( CALC_STAGE_AFTER_LEARNING );

    mon -> now( Path{ "trace", "debug_dump_stage_after_learning" }, true );

    /*
        End of calculation
    */

    /* Write the nerves weights in to storage */
    if
    (
        !terminated &&
        net -> getLastChangeValues() == netLastChangeValues &&
        tickWrite != 0 &&
        net -> getTick() % tickWrite == 0
    )
    {
        getNerveList() -> loop
        (
            [ this ]
            ( void* item )
            {
                auto nerve = (Nerve*) item;
                if
                (
                    !nerve
                    -> saveWeight( net -> getNervesPath() )
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
    }

    mon -> now( Path{ "trace", "write_nerves" }, true);

    /* Move calculated data to net */
    if
    (
        !terminated  &&
        net -> getLastChangeValues() == netLastChangeValues
    )
    {
        net
        /* Swap values and errors */
        -> swapValuesAndErrors
        (
            /* Action */
            {
                /* Load values and errors to Net */
                WRITE_VALUES,
                WRITE_ERRORS
            },
            /* Participant object */
            this,
            false
        )
        /* Load weights to Net from this limb */
        -> loadWeightsFrom( this )
        /* Write stat for Net */
        -> stat()
        ;
    }

    mon -> now( Path{ "trace", "move_data_to_net" }, true );

    /* Write charts in to monitoring */
    if
    (
        !terminated &&
        net -> getLastChangeValues() == netLastChangeValues &&
        tickChart != 0 &&
        net -> getTick() % tickChart == 0
    )
    {
        net -> lock();

        net
        -> getLayerList()
        -> loop
        (
            [ this ]
            (
                void* aItem
            )
            {
                auto layer = (Layer*) aItem;

                mon
                -> setString
                (
                    Path{ "values", strAlign( layer -> getId(), ALIGN_LEFT, 20 ) },
                    layer -> getChartValues() -> toString( 40 )
                )
                -> setString
                (
                    Path{ "errors", strAlign( layer -> getId(), ALIGN_LEFT, 20 ) },
                    layer -> getChartErrors() -> toString( 40 )
                )

                -> setString
                (
                    Path{ "ticks", strAlign( layer -> getId(), ALIGN_LEFT, 20 ) },
                    layer -> getChartTick() -> toString( 40 )
                )

                -> setString
                (
                    Path{ "errorsBeforeChange", strAlign( layer -> getId(), ALIGN_LEFT, 20 ) },
                    layer -> getChartErrorsBeforeChange() -> toString( 40 )
                )
                ;

                return false;
            }
        );

        net -> unlock();
    }

    mon
    -> now( Path{ "trace", "write_charts" }, true )
    -> trace( Path{ "trace" } );

    /* Write final monitoring */
    mon
    -> now( Path{ "current", "last" } )
    -> setDouble( Path{ "current", "fps" }, fps -> avg() )
    -> setBool( Path{ "current", "learning" }, learning )
    -> setInt( Path{ "current", "thread_count" }, threadManager -> getCount() )
    -> flush();

    auto fpsEnd = now();
    fps -> createLast( (double) SECOND / ( fpsEnd - fpsBegin ));

    return this;
}



/*
    Controling the nerve list
*/
LimbProcessor* LimbProcessor::nerveControl()
{
    bool reallocated = false;

    /* Allocate weigth */
    getNerveList() -> weightsAllocate
    (
        [ &reallocated ]
        ( Nerve* nerve )
        {
            reallocated = true;
        }
    );

    if( reallocated )
    {
        /* Network was realocated and have to loading */
        auto loadingError = false;
        getNerveList() -> loop
        (
            [ this, &loadingError ]
            ( void* item )
            {
                auto nerve = ( Nerve* ) item;
                if( !nerve -> loadWeight( net -> getNervesPath() ) -> isOk() )
                {
                    loadingError = true;
                }
                return loadingError;
            }
        );
        /* Generage weights */
        if( loadingError )
        {
            getNerveList() -> loop
            (
                [ this ]
                ( void* item )
                {
                    auto nerve = ( Nerve* ) item;
                    nerve -> setOk();
                    nerve -> fill( net -> getRnd() );
                    return false;
                }
            );
        }
    }
    return this;
}



/**********************************************************************
    Setters and getters
*/



/*
    Set learning speed
*/
LimbProcessor* LimbProcessor::setLearningSpeed
(
    double a
)
{
    learningSpeed = a;
    return this;
}



/*
    Set wakeup weight
*/
LimbProcessor* LimbProcessor::setMinWeight
(
    double a
)
{
    minWeight = a;
    return this;
}



/*
    Get wakeup weight
*/
double LimbProcessor::getMinWeight()
{
    return minWeight;
}



/*
    Set min weight
*/
LimbProcessor* LimbProcessor::setMaxWeight
(
    double a
)
{
    maxWeight = a;
    return this;
}



/*
    Get max weight
*/
double LimbProcessor::getMaxWeight()
{
    return maxWeight;
}



/*
    Set min error
*/
LimbProcessor* LimbProcessor::setMaxError
(
    double a
)
{
    maxError = a;
    return this;
}



/*
    Get max error
*/
double LimbProcessor::getMaxError()
{
    return maxError;
}



/*
    Get learning speed
*/
double LimbProcessor::getLearningSpeed()
{
    return learningSpeed;
}



/*
    Calculate value and error for neuron of layer
*/
LimbProcessor* LimbProcessor::neuronCalcValue
(
    Layer* aLayer,
    int aIndex
)
{
    /* Additive accum */
    double add = 0.0;
    /* Multiplexor accum */
    double mul = 1.0;
    /* Additive count */
    int count = 0;

    /* Neuron has a binds and it is not a Receptor */
    parentsLoop
    (
        aLayer,
        aIndex,
        BT_ALL,
        [
            this, &add, &mul, &count
        ]
        (
            Layer*  aParentLayer,
            int     aParentIndex,
            Nerve*  aNerve,
            double  aWeight,
            int     aWeightIndex /* not use */
        ) -> bool
        {
            /* The incoming value from parent to current neuron */
            auto w = aParentLayer -> getNeuronValue( aParentIndex ) * aWeight;
            /* Calculate summ */
            switch( aNerve -> getBindType() )
            {
                default:
                break;
                case BT_ADD:
                    count ++;
                    add += w;
                break;
                case BT_MUL:
                    mul *= w;
                break;
            }
            return terminated;
        }
    );

    if( count > 0 )
    {
        aLayer -> setNeuronValue
        (
            aIndex,
            /* Main value calculation */
            (*( aLayer -> frontFunc ))( add * mul )
        );
    }

    return this;
}



/*
    Calculate neuron error
*/
LimbProcessor* LimbProcessor::neuronCalcError
(
    Layer*  aLayer, /* Layer for calculation */
    int     aIndex  /* Neuron index of layer */
)
{
    /* Define variables */
    double error    = 0.0;
    switch( aLayer -> getErrorCalc() )
    {
        default:
        case EC_NONE:
            error = 0;
            /* Error not changing */
        break;
        case EC_LEARNING:
        {
            /*
                caclulate error form all children for the current neuron
            */
            childrenLoop
            (
                aLayer,
                aIndex,
                BT_ADD,
                [ this, &error ]
                (
                    Layer* aChild,
                    int aChildIndex,
                    Nerve* aNerve,
                    double aWeight,
                    int aWeightIndex    /* Not use */
                ) -> bool
                {
                    /* Use the back func out for outgoing error */
                    error += (*( aChild -> backFuncOut ))
                    (
                         aChild -> getNeuronError( aChildIndex )
                    ) * aWeight;
                    return terminated;
                }
            );
        }
        break;
        case EC_VALUE:
            error = - aLayer -> getNeuronValue( aIndex );
        break;
    }

    /*
        Calculate current neuron error
        https://habr.com/ru/articles/313216/
    */
    auto currentError = error * ( * ( aLayer -> backFunc ))
    (
        aLayer -> getNeuronValue( aIndex )
    );

    /* Write error in to neuron */
    aLayer -> setNeuronError( aIndex, currentError );

    return this;
}



/*
    Learning a neuron by recalculating
    the weights of parent connections.
*/
LimbProcessor* LimbProcessor::neuronCalcWeight
(
    Layer*  aLayer, /* Layer for calculation */
    int     aIndex  /* Neuron index of layer */
)
{
    /* Learnin, calculating weights */
    switch( aLayer -> getErrorCalc() )
    {
        default:
            /* EC_NONE */
            /* EC_VALUE */
        break;
        case EC_LEARNING:
            auto error = aLayer -> getNeuronError( aIndex );
            parentsLoop
            (
                aLayer,
                aIndex,
                BT_ADD,
                [
                    this,
                    &error
                ]
                 (
                    Layer*  aParentLayer,
                    int     aParentIndex,
                    Nerve*  aNerve,
                    double  aWeight,
                    int     aWeightIndex
                )
                {
                    /*
                        Emplementate the https://habr.com/ru/articles/313216/
                    */
                    double gradient =
                    aParentLayer -> getNeuronValue( aParentIndex )
                    * error;

                    double deltaWeight = gradient * learningSpeed
                    + aNerve -> getDeltaWeight( aWeightIndex ) * 0.3;

                    double newWeight = aWeight + deltaWeight;

                    aNerve -> setWeight( aWeightIndex, newWeight );
                    aNerve -> setDeltaWeight( aWeightIndex, deltaWeight );

                    return terminated;
                }
            );
        break;
    }

    return this;
}



/*
    Calculate neurons in the layer using threads
*/
LimbProcessor* LimbProcessor::layerCalc
(
    /* Layer for calculation */
    Layer*  aLayer,
    /* Type of calculation */
    Data    aData
)
{
    /* Calculate thread count */
    int threadCount = aLayer -> getThreadCount();

    /* Check thread count */
    if( threadManager -> prepare( threadCount ))
    {
        /* Set handlers for thread */
        for
        (
            size_t threadNumber = 0;
            threadNumber < threadCount && !threadManager -> isTerminating();
            threadNumber++
        )
        {
            threadManager -> setHandler
            (
                threadNumber,
                (void*) new LayerCalcTask
                {
                    this,
                    aLayer,
                    calcNeuronFrom( aLayer, threadCount, threadNumber ),
                    calcNeuronTo( aLayer, threadCount, threadNumber ),
                    aData
                },
                []
                (
                    void* aTaskPtr
                )
                {
                    auto task = (LayerCalcTask*) aTaskPtr;
                    /* Thread body */
                    for( size_t n = task -> neuronFrom; n < task -> neuronTo; n++ )
                    {
                        switch( task -> data )
                        {
                            /* Unknown data */
                            default:
                            case DATA_UNKNOWN:break;
                            case DATA_WEIGHTS:
                                task -> limb -> neuronCalcWeight( task -> layer, n );
                            break;
                            case DATA_VALUES:
                                task -> limb -> neuronCalcValue( task -> layer, n );
                            break;
                            case DATA_ERRORS:
                                task -> limb -> neuronCalcError( task -> layer, n );
                            break;
                        }
                    }
                    delete task;
                }
            );

        }

        /* Run calculation and wait */
        threadManager
        -> run()
        -> wait();
    }

    if( aData == DATA_VALUES )
    {
        onChangeValues();
    }

    return this;
}



/*
    Get tick fo write weight
*/
int LimbProcessor::getTickWrite()
{
    return tickWrite;
}



/*
    Set tick for write weights
*/
LimbProcessor* LimbProcessor::setTickWrite
(
    int a
)
{
    tickWrite = a;
    return this;
}



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

*/
LimbProcessor* LimbProcessor::setDumpConf
(
    ParamList* a
)
{
    if( a != NULL )
    {
        dumpConf -> clear() -> copyFrom( a );
    }
    return this;
}



LimbProcessor* LimbProcessor::down()
{
    threadManager -> terminate();
    return this;
}



LimbProcessor* LimbProcessor::up()
{
    terminated = false;
    return this;
}



LimbProcessor* LimbProcessor::buildCalcTables()
{
    calcTableValues -> clear();
    calcTableErrors -> clear();

    getLayerList() -> loop
    (
        [ this ]
        ( void* item )
        {
            calcTableValues
            -> addLayer( (Layer*) item )
            ;

            calcTableErrors
            -> addLayer( (Layer*) item )
            ;
            return false;
        }
    );

    calcTableValues
    -> dump( getLog(), "Layers before forward sort" )
    -> sortForward( getNerveList() )
    -> dump( getLog(), "Layers after forward sort" )
    ;

    calcTableErrors
    -> dump( getLog(), "Layers before backward sort" )
    -> sortBackward( getNerveList() )
    -> dump( getLog(), "Layers after backward sort" )
    ;

    return this;
}



/*
    Write debug monitoring

    File name keys for replacing
        %stage%
        %tick%
        %layer%
        %neuron%
        %direction%
*/
LimbProcessor* LimbProcessor::calcDebugDump
(
    CalcStage aStage
)
{
    /* Convert stage */
    auto stage = calcStageToString( aStage );
    auto stageAll = calcStageToString( CALC_STAGE_ALL );

    /* Loop for monitoring rules from config */
    dumpConf -> objectsLoop
    (
        [ this, &aStage, &stage, &stageAll ]
        ( ParamList* params, string )
        {
            /* Enabled check */
            if( params -> getBool( Path{ "enabled" }, true ))
            {
                /* Stage check */
                auto stages = params -> getObject( Path{ "stages" });
                if
                (
                    stages != NULL &&
                    (
                        stages -> contains( stage ) ||
                        stages -> contains( stageAll )
                    )
                )
                {
                    /* Layer defining */
                    auto layers = params -> getObject( Path{ "layers" });
                    if( layers != NULL )
                    {
                        /* Read file name */
                        auto file = net -> getMonPath
                        (
                            params -> getString
                            (
                                Path{ "file" },
                                "tick_%tick%/%stage%/%layer%%neuron%-%direction%-%data%.txt"
                            )
                        );

                        /* Read dataview type */
                        auto dataview = dataviewFromString
                        (
                            params -> getString( Path{ "dataview" }),
                            DATAVIEW_FLOAT
                        );

                        layers -> loop
                        (
                            [
                                this, &params, &aStage,
                                &stage, &file, &dataview
                            ]
                            ( Param* param )
                            {
                                /* Get layer by index */
                                auto layer = getLayerList()
                                -> getById( param -> getString());

                                if( layer == NULL )
                                {
                                    getLog()
                                    -> warning( "Layer monitoring not found" );
                                }
                                else
                                {
                                    /* Data type loop VALUES ERROR WEIGHTS */
                                    auto datas = params -> getObject( Path{ "data" });
                                    if( datas != NULL )
                                    {
                                        datas -> loop
                                        (
                                            [
                                                this,
                                                &params,
                                                &aStage,
                                                &stage,
                                                &file,
                                                &layer,
                                                &dataview
                                            ]
                                            ( Param* item )
                                            {
                                                auto data = dataFromString( item -> getString());
                                                /* Let directions */
                                                auto directions = params
                                                -> getObject( Path{ "directions" });

                                                if( directions != NULL )
                                                {
                                                    directions -> loop
                                                    (
                                                        [
                                                            this,
                                                            &params,
                                                            &aStage,
                                                            &stage,
                                                            &file,
                                                            &layer,
                                                            &data,
                                                            &dataview
                                                        ]
                                                        ( Param* param )
                                                        {
                                                            /* Extract the config arguments */
                                                            auto direction = directionFromString( param -> getString() );
                                                            switch( direction )
                                                            {
                                                                case DIRECTION_PARENT:
                                                                case DIRECTION_CHILD:
                                                                {
                                                                    auto neurons = params -> getObject( Path{ "neurons" });
                                                                    if( neurons != NULL )
                                                                    {
                                                                        neurons -> objectsLoop
                                                                        (
                                                                            [
                                                                                this,
                                                                                &layer,
                                                                                &direction,
                                                                                &data,
                                                                                &file,
                                                                                &stage,
                                                                                &dataview
                                                                            ]
                                                                            ( ParamList* item, string )
                                                                            {
                                                                                auto pos = ParamPoint::point3i( item );

                                                                                /* Replace file name */
                                                                                auto f = replace
                                                                                (
                                                                                    file,
                                                                                    vector <string>
                                                                                    {
                                                                                        "%tick%",
                                                                                        "%stage%",
                                                                                        "%layer%",
                                                                                        "%direction%",
                                                                                        "%data%",
                                                                                        "%neuron%"
                                                                                    },
                                                                                    vector <string>
                                                                                    {
                                                                                        toString( net -> getTick() ),
                                                                                        stage,
                                                                                        layer -> getId(),
                                                                                        directionToString( direction ),
                                                                                        dataToString( data ),
                                                                                        pos.toString()
                                                                                    }
                                                                                );

                                                                                /* Dump */
                                                                                dump
                                                                                (
                                                                                    f,
                                                                                    layer,
                                                                                    pos,
                                                                                    direction,
                                                                                    data,
                                                                                    dataview,
                                                                                    net -> getTick()
                                                                                );
                                                                                return false;
                                                                            } /* Neuron list and data exists */
                                                                        ); /* Neurns loop */
                                                                    } /* Neurons exists */
                                                                }
                                                                break;

                                                                default:
                                                                case DIRECTION_UNKNOWN:
                                                                case DIRECTION_NONE:
                                                                {
                                                                    /* Replace file name */
                                                                    auto f = replace
                                                                    (
                                                                        file,
                                                                        vector <string>
                                                                        {
                                                                            "%tick%",
                                                                            "%stage%",
                                                                            "%layer%",
                                                                            "%direction%",
                                                                            "%data%",
                                                                            "%neuron%"
                                                                        },
                                                                        vector <string>
                                                                        {
                                                                            toString( net -> getTick() ),
                                                                            stage,
                                                                            layer -> getId(),
                                                                            directionToString( direction ),
                                                                            dataToString( data ),
                                                                            ""
                                                                        }
                                                                    );
                                                                    /* Dump */
                                                                    dump
                                                                    (
                                                                        f,
                                                                        aStage,
                                                                        layer,
                                                                        data,
                                                                        dataview,
                                                                        net -> getTick()
                                                                    );
                                                                }
                                                                break;

                                                            } /* Switch direction */
                                                            return false;
                                                        } /* Direction lambda */
                                                    ); /* Direction loop */
                                                } /* Direction exists */
                                                else
                                                {
                                                    setResult( "dump_directions_key_not_exists" );
                                                }
                                                return false;
                                            }/* Data loop laymbda */
                                        ); /* Data loop */
                                    } /* Datas exists */
                                } /* Layer exists */
                                return false;
                            } /* Layers loop lambda */
                        ); /* Layers loop */
                    } /* Layers exists */
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

                    auto file = net -> getMonPath( "processor_thread_calc_lock.json" );

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




/*
    Method return count of thread using following property
        maxThreadCount
        minNeuronPerThread

*/
LimbProcessor* LimbProcessor::calcThreadCount()
{
    getLayerList() -> loop
    (
        [ & ]
        ( void* item )
        {
            auto layer = (Layer*) item;
            layer -> setThreadCount
            (
                min
                (
                    maxThreadCount,
                    max
                    (
                        1,
                        layer -> getCount() / minNeuronPerThread
                    )
                )
            );
            return false;
        }
    );
    return this;
}



