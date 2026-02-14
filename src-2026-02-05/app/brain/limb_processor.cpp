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
:Limb
(
    aNet -> getLogManager(),
    /* Empty version */
    ""
)
{
    net             = aNet;

    mon = Mon::create( net -> getMonPath( "procesor.json" ))
    -> setString( Path{ "start", "Source" }, "Limb processor" )
    -> now( Path{ "start", "now" });

//    weightsChart = ChartList::create();
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
//    weightsChart -> destroy();

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
    Create new layer for this limb and copy parameters from source layer.
    This method have to overriden at children Limbs.
*/
Layer* Limb::copyLayerFrom
(
    Layer* aLayerFrom
)
{
    return
    LayerProcessor::create( this, aLayerFrom -> getId() )
    -> setErrorCalc( aLayerFrom -> getErrorCalc() )
    -> setWeightCalc( aLayerFrom -> getWeightCalc() )
    -> setFrontFunc( aLayerFrom -> getFrontFunc() )
    -> setBackFunc( aLayerFrom -> getBackFunc() )
    -> setBackFuncOut( aLayerFrom -> getBackFuncOut() )
    -> setSize( aLayerFrom -> getSize() );
}




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
    mon -> now( Path{ "trace", "start" }, true );

    net -> lock();

    if
    (
        /* Net is not locked for writing */
        !net -> getWeightWriteLock()
        &&
        (
            ( getVersion() != net -> getVersion() && getVersion() != "" )
            ||
            ( tickWrite != 0 && net -> getTick() % tickWrite == 0 )
        )
    )
    {
        /* Writes weights for current version */
        weightsWrite( net -> getNervesPath( "", getVersion() ));
    }

    mon -> now( Path{ "trace", "write_nerves" }, true);

    /* Increase the tick */
    net -> incTick();

    /* Detect changes of config */
    auto reload = net -> getLastUpdate() != this -> getLastUpdate();

    if( net -> syncToLimb( this, false ))
    {
        /* Reallocates and fill weights */
        nerveControl();
    }

    /* Rebuild calculations */
    if( reload )
    {
        /* Updated */
        buildCalcTables();
        /* Calculae thread count for each layer */
        calcThreadCount();
    }


    if( getVersion() != net -> getVersion() )
    {
        /* Set version from net */
        setVersion( net -> getVersion() );
    }
    net ->  unlock();


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
    getNerveList()
    -> loop
    (
        [ this, &netLastChangeValues ]
        ( void* item )
        {
            auto nerve = (Nerve*) item;
            auto terminate =
            terminated ||
            net -> getLastChangeValues() != netLastChangeValues;
            if( !terminate )
            {
                nerve -> calcWeights( learningSpeed );
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
        mon -> remove( Path{ "mon" } );


        auto monConfig = net
        -> getApplication()
        -> getConfig()
        -> getObject( Path{  "engine",  "brain", "mon", "charts" });

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
                    mon -> setString
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
                    mon -> setString
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
                    mon -> setString
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

    mon
    -> now( Path{ "trace", "write_charts" }, true )
    -> trace( Path{ "trace" } );

    /* Write final monitoring */
    mon
    -> now( Path{ "current", "last" } )
    -> setDouble( Path{ "current", "fps" }, fps -> avg() )
    -> setInt( Path{ "current", "thread_count" }, threadManager -> getCount() )
    -> flush();

    auto fpsEnd = now();
    fps -> createLast( (real) SECOND / ( fpsEnd - fpsBegin ));

    return this;
}



/*
    Controling the nerve list
*/
LimbProcessor* LimbProcessor::nerveControl()
{
    /* Allocate weigth */
    getNerveList() -> weightsAllocate();

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



/*
    Set learning speed
*/
LimbProcessor* LimbProcessor::setLearningSpeed
(
    real a
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
    real a
)
{
    minWeight = a;
    return this;
}



/*
    Get wakeup weight
*/
real LimbProcessor::getMinWeight()
{
    return minWeight;
}



/*
    Set min weight
*/
LimbProcessor* LimbProcessor::setMaxWeight
(
    real a
)
{
    maxWeight = a;
    return this;
}



/*
    Get max weight
*/
real LimbProcessor::getMaxWeight()
{
    return maxWeight;
}



/*
    Set min error
*/
LimbProcessor* LimbProcessor::setMaxError
(
    real a
)
{
    maxError = a;
    return this;
}



/*
    Get max error
*/
real LimbProcessor::getMaxError()
{
    return maxError;
}



/*
    Get learning speed
*/
real LimbProcessor::getLearningSpeed()
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
    /* Max accum */
    real max = -INFINITY;
    Nerve* maxNerve = NULL;
    int maxWeightIndex = -1;

    /* Additive accum */
    real add = 0.0;
    /* Multiplexor accum */
    real mul = 1.0;
    /* Additive count */
    int count = 0;

    /* Neuron has a binds and it is not a Receptor */
    parentsLoop
    (
        aLayer,
        aIndex,
        BT_ALL,
        [
            this,
            &add,
            &mul,
            &count,
            &max,
            &maxNerve,
            &maxWeightIndex
        ]
        (
            Layer*  aParentLayer,
            int     aParentIndex,
            Nerve*  aNerve,
            real    aWeight,
            int     aWeightIndex /* not use */
        ) -> bool
        {
            /* The incoming value from parent to current neuron */
            auto w
            = aParentIndex == -1
            ? 0
            : aParentLayer -> getNeuronValue( aParentIndex ) * aWeight;

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
                case BT_MAX:
                    aNerve -> setControl( aWeightIndex, 0.0 );
                    if( w > max )
                    {
                        maxNerve = aNerve;
                        maxWeightIndex = aWeightIndex;
                        max = w;
                    }
                break;
            }
            return terminated;
        }
    );

    /* MAX postprocessing hi priority */
    if( maxWeightIndex > -1 )
    {
        maxNerve -> setControl( maxWeightIndex, 1.0 );
        aLayer -> setNeuronValue
        (
            aIndex,
            (*( aLayer -> frontFunc ))( max )
        );
    }
    else
    {
        /* ADD postprocessing */
        if( count > 0 )
        {
            aLayer -> setNeuronValue
            (
                aIndex,
                /* Main value calculation */
                (*( aLayer -> frontFunc ))( add * mul )
            );
        }
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
    real error    = 0.0;
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
                BT_ALL,
                [ this, &error ]
                (
                    Layer* aChild,
                    int aChildIndex,
                    Nerve* aNerve,
                    real aWeight,
                    int aWeightIndex    /* Not use */
                ) -> bool
                {
                    /* Calculate summ */
                    switch( aNerve -> getBindType() )
                    {
                        default:
                        break;
                        case BT_ADD:
                            /* Use the back func out for outgoing error */
                            error += (*( aChild -> backFuncOut ))
                            (
                                 aChild -> getNeuronError( aChildIndex )
                            )
                            * aWeight;
                        break;
                        case BT_MAX:
                            error += aNerve -> getControl( aWeightIndex )
                            * (*( aChild -> backFuncOut ))
                            (
                                 aChild -> getNeuronError( aChildIndex )
                            )
                            * aWeight;
                        break;
                    }
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
    for( size_t n = 0; n < aLayer -> getCount(); n++ )
    {
        switch( aData )
        {
            /* Unknown data */
            default:
            break;
            case DATA_VALUES:
                neuronCalcValue( aLayer, n );
            break;
            case DATA_ERRORS:
                neuronCalcError( aLayer, n );
            break;
        }
    }

    if( aData == DATA_VALUES )
    {
        onChangeValues();
    }

    return this;
}



///*
//    Calculate neurons in the layer using threads
//      Это многопоточный расчет одного слоя
//      Делит слой на множество потоков
//      Устанавлиает что слой расчитан после завершения всех потоков
//      Закомментирвоани 2026-02-04 при реализации деления расчета
//      разных слове по потокам
//*/
//LimbProcessor* LimbProcessor::layerCalc
//(
//    /* Layer for calculation */
//    Layer*  aLayer,
//    /* Type of calculation */
//    Data    aData
//)
//{
//    /* Calculate thread count */
//    int threadCount = aLayer -> getThreadCount();
//    /* Check thread count */
//    if( threadManager -> prepare( threadCount ))
//    {
//        /* Set handlers for thread */
//        for
//        (
//            size_t threadNumber = 0;
//            threadNumber < threadCount && !threadManager -> isTerminating();
//            threadNumber++
//        )
//        {
//            threadManager -> setHandler
//            (
//                threadNumber,
//                (void*) new LayerCalcTask
//                {
//                    this,
//                    aLayer,
//                    calcNeuronFrom( aLayer, threadCount, threadNumber ),
//                    calcNeuronTo( aLayer, threadCount, threadNumber ),
//                    aData
//                },
//                []
//                (
//                    void* aTaskPtr
//                )
//                {
//                    auto task = (LayerCalcTask*) aTaskPtr;
//                    /* Thread body */
//                    for
//                    (
//                        size_t n = task -> neuronFrom;
//                        n < task -> neuronTo;
//                        n++
//                    )
//                    {
//                        switch( task -> data )
//                        {
//                            /* Unknown data */
//                            default:
//                            break;
//                            case DATA_VALUES:
//                                task -> limb -> neuronCalcValue
//                                (
//                                    task -> layer, n
//                                );
//                            break;
//                            case DATA_ERRORS:
//                                task -> limb -> neuronCalcError
//                                (
//                                    task -> layer, n
//                                );
//                            break;
//                        }
//                    }
//                    delete task;
//                }
//            );
//
//        }
//
//        /* Run calculation and wait */
//        threadManager
//        -> run();
//        threadManager
//        -> wait();
//    }
//
//    if( aData == DATA_VALUES )
//    {
//        onChangeValues();
//    }
//
//    return this;
//}




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

    /* Loop for monitoring rules from config */
    dumpConf -> objectsLoop
    (
        [ this, &stage, &stageAll, &aStage ]
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
                    file = net -> getMonPath( file );

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
                                                net -> getTick()
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
                                                [ this, file1, data, dataview ]
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
                                                                    file1,
                                                                    dataview
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
                                                                        data,
                                                                        dataview
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
