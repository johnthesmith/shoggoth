#include <cmath>

#include <unistd.h> /* usleep */
#include "limb_processor.h"
#include "../func.h"

#include "../../../../../lib/core/math.h"
#include "../../../../../lib/core/str.h"
#include "./calc_table.h"



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

    forwardList     = LayerList::create( this );
    backwardList    = LayerList::create( this );
}



/*
    Destructor
*/
LimbProcessor::~LimbProcessor()
{
    forwardList -> destroy();
    backwardList -> destroy();
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



LimbProcessor* LimbProcessor::setThreadCount
(
    int a
)
{
    threadCount = a;
    return this;
}



int LimbProcessor::getThreadCount()
{
    return threadCount;
}




/*
    Calculate all layers
*/
LimbProcessor* LimbProcessor::calc()
{
    /*
        Calculation begin
    */
    mon
    -> addInt( Path{ "count" })
    -> now( Path{ "last" } )
    -> setDouble( Path{ "config", "learningSpeed" }, learningSpeed )
    -> setDouble( Path{ "config", "minWeight" }, minWeight )
    -> setDouble( Path{ "config", "maxWeight" }, maxWeight )
    -> setDouble( Path{ "config", "maxError" }, maxError )
    ;

    /* Increase total calc frame */
    frame++;

    /* Increase tick */
    tick ++;

    /* Set learning speed from net config */
    net -> lock();
    setLearningSpeed
    (
        net
        -> getConfig()
        -> getDouble( Path{ "processor", "learningSpeed" }, 0.0 )
    );
    net -> unlock();

    /* Check structure with net */
    net -> syncToLimb( this, false );

    net -> lock();
    auto seed = net -> getSeed();
    net -> unlock();

    /*   */
    nerveControl( seed );

    net
    /* Upload start values from Net */
    -> swapValuesAndErrors
    (
        Actions{ READ_VALUES }, /* Actions */
        TASK_PROC,   /* Role */
        this,        /* Participant object */
        false
    )

    /* Load values and errors to net */
    -> swapValuesAndErrors
    (
        { WRITE_VALUES, WRITE_ERRORS }, /* Action */
        TASK_PROC,                      /* Role */
        this,                           /* Participant object */
        false
    )

    /* Load selected weights to net from this limb */
    -> loadWeightsFrom( this )

    /* Write stat for Net */
    -> stat()
    ;

    /* Drop learing mode flag */
    learning = false;

    /*
        Write monitoring
    */
    net -> lock();
    net -> getLayerList() -> loop
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
            ;

            return false;
        }
    );
    net -> unlock();



    /*
        Forward calculation
    */
    CalcTable::loop
    (
        net,
        [ this ]
        ( calcItem* item, Layer* layer )
        {
            if( item -> isParentCalculated())
            {
                /* Let elapsed begin */
                mon -> startTimer( Path{ "duration", "values", layer -> getId() });
                /* Set default thread id */
                int idThread = 0;
                layerCalcValue( layer, idThread, learning );
                mon -> stopTimer( Path{ "duration", "values", layer -> getId() });
                item -> calculated();
            }
            return false;
        }
    );



    /*
        Backward calculation (neuron errors)
    */
    CalcTable::loop
    (
        net,
        [ this ]
        ( calcItem* item, Layer* layer )
        {
            if( item -> isChildrenCalculated())
            {
                /* Let elapsed begin */
                mon -> startTimer( Path{ "duration", "errors", layer -> getId() });
                /* Set default thread id */
                int idThread = 0;
                /* Calculate errors for layers with parents */
                layerCalcError( layer, idThread );
                mon -> stopTimer( Path{ "duration", "errors", layer -> getId() });
                item -> calculated();
            }
            return false;
        }
    );



    /*
        Learning calculation (nerve weights)
    */
    backwardList -> loop
    (
        [ this ]
        ( void* item )
        {
            auto layer = ( Layer* ) item;
            /* Let elapsed begin */
            mon -> startTimer( Path{ "duration", "weights", layer -> getId() });

            /* Set default thread id */
            int idThread = 0;

            /* Calculate weights in nervs of layers */
            layerCalcWeight( layer, idThread );

            mon -> stopTimer( Path{ "duration", "weights", layer -> getId() });
            return false;
        }
    );



    /*
        End of calculation
    */

    /*
        Write the nerves weights in to storage
    */
    if( tick % tickWrite == 0 )
    {
        getNerveList() -> loop
        (
            [ this ]
            ( void* item )
            {
                auto nerve = (Nerve*) item;
                if
                (
                    !nerve -> saveWeight( net -> getNervesPath() ) -> isOk()
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

    /* Write monitoring */
    mon
    -> setBool( Path{ "current", "learning" }, learning )
    -> setInt( Path{ "current", "tick" }, tick )
    -> flush();

    return this;
}



/*
    Controling the nerve list
*/
LimbProcessor* LimbProcessor::nerveControl
(
    unsigned long long int aSeed
)
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
            auto rnd = RndObj::create( aSeed );
            getNerveList() -> loop
            (
                [ &rnd ]
                ( void* item )
                {
                    auto nerve = ( Nerve* ) item;
                    nerve -> setOk();
                    nerve -> fill( rnd );
                    return false;
                }
            );
            rnd -> destroy();
        }
    }
    return this;
}

/*
    Return pointer of forward calulation list
*/

LayerList* LimbProcessor::getForwardList()
{
    return forwardList;
}



/*
    Return pointer of backward calulation list
*/

LayerList* LimbProcessor::getBackwardList()
{
    return backwardList;
}



/*
    Build layers calculation sequence list
    Method fills the next lists:
        forwardList
        backwardList
*/
LimbProcessor* LimbProcessor::precalc()
{
    auto layers = getLayerList();
    if( layers -> getCount() > 0 )
    {
        /* Clear forward and backward lists */
        forwardList -> resize( 0 );
        backwardList -> resize( 0 );

        /* Calculate forward */
        bool add = false;
        int index = 0;
        auto stop = false;
        while( !stop )
        {
            /* Get calculated layer by index */
            Layer* layer = ( Layer* ) layers -> getByIndex( index );

            if
            (
                /* if layer not forward calculated and ... */
                layer -> getForwardStage( 1 ) == CALC_NOT_START &&
                /* all parents is prepared */
                preparedParents( layer )
            )
            {
                layer -> calcStartForward() -> calcCompleteForward();
                forwardList -> push( layer );
                add = true;
            }

            index++;

            if( index >= layers -> getCount() )
            {
                index = 0;
                if( ! add )
                {
                    stop = true;
                    setCode( "LoopedNetForward" );
                }
                else
                {
                    add = false;
                }
            }

            if( getCalcStage( CALC_FORWARD ) == CALC_COMPLETE )
            {
                stop = true;
            }
        }

        /* Calculate backward */
        add = false;
        index = 0;
        stop = false;
        while( !stop )
        {
            /* Get calculated layer by index */
            Layer* layer = ( Layer* ) layers -> getByIndex( index );

            if
            (
                /* if layer not forward calculated and ... */
                layer -> getBackwardStage( 1 ) == CALC_NOT_START &&
                /* all parents is prepared */
                preparedChildren( layer )
            )
            {
                layer -> calcStartBackward() -> calcCompleteBackward();
                backwardList -> push( layer );
                add = true;
            }

            index++;

            if( index >= layers -> getCount() )
            {
                index = 0;
                if( ! add )
                {
                    stop = true;
                    setCode( "LoopedNetBackward" );
                }
                else
                {
                    add = false;
                }
            }

            if( getCalcStage( CALC_BACKWARD ) == CALC_COMPLETE )
            {
                stop = true;
            }
        }

        calcReset();
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
    Set calculation debug mode flag
*/
LimbProcessor* LimbProcessor::setCalcDebug
(
    bool a
)
{
    calcDebug = a;
    return this;
}



/*
    Return calculation debug mode flag
*/
bool LimbProcessor::getCalcDebug()
{
    return calcDebug;
}



/*
    Return the current calculated layer index
*/
int LimbProcessor::getCalcLayerIndex()
{
    return calcLayerIndex;
}



/*
    Calculate value and error for neuron of layer
*/
LimbProcessor* LimbProcessor::neuronCalcValue
(
    Layer* aLayer,
    int aIndex,
    bool &aLearning
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
            &add, &mul, &count
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
            return false;
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
            /*
                caclulate error form all children for the current neuron
            */
            childrenLoop
            (
                aLayer,
                aIndex,
                BT_ADD,
                [ &error ]
                (
                    Layer* aChild,
                    int aChildIndex,
                    Nerve* aNerve,
                    double aWeight,
                    int aWeightIndex    /* Not use */
                ) -> bool
                {
                    error += aChild -> getNeuronError( aChildIndex ) * aWeight;
                    return false;
                }
            );
        break;
        case EC_CURRENT_VALUE:
            error = 1;
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


// TODO изменить конфиг сети ну и проверить
// Сделать загурзку парамтров слоя
//      ErrorCalc       errorCalc               = EC_NONE;
//      WeightCalc      weightCalc              = WC_NONE;




/*
    Learning a neuron by recalculating
    the weights of parent connections.
*/
LimbProcessor* LimbProcessor::neuronLearning
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

                    return false;
                }
            );
        break;
    }

    return this;
}



/*
    Calculate neurons in the layer
*/
LimbProcessor* LimbProcessor::layerCalcValue
(
    Layer*  aLayer,     /* Layer for calculation */
    int     aThread,    /* Current thread number */
    bool&   aLearning   /* Need learning */
)
{
    int b = calcNeuronFrom( aLayer, aThread );
    int e = calcNeuronTo( aLayer, aThread );
    for( int i = b; i < e; i ++ )
    {
        neuronCalcValue( aLayer, i, aLearning );
    }
    return this;
}



/*
    Calculate neurons error
*/
LimbProcessor* LimbProcessor::layerCalcError
(
    Layer*  aLayer, /* Layer for calculation */
    int     aThread /* Current thread */
)
{
    int b = calcNeuronFrom( aLayer, aThread );
    int e = calcNeuronTo( aLayer, aThread );

    for( int i = b; i < e; i ++ )
    {
        neuronCalcError( aLayer, i );
    }

    return this;
}



/*
    Calculate nerves weights for learning
*/
LimbProcessor* LimbProcessor::layerCalcWeight
(
    Layer*  aLayer, /* Layer for calculation */
    int     aThread /* Current thread */
)
{
    int b = calcNeuronFrom( aLayer, aThread );
    int e = calcNeuronTo( aLayer, aThread );

    for( int i = b; i < e; i ++ )
    {
        neuronCalcWeight( aLayer, i );
    }

    return this;
}




/*
    Calculate start neuron for processors operations
*/
int LimbProcessor::calcNeuronFrom
(
    Layer* aLayer,
    int aNumber
)
{
    return floor
    (
        (double) aLayer -> getCount() * (double) aNumber / (double) threadCount
    );
}



/*
    Caluculate end of neurons for processors operations
*/
int LimbProcessor::calcNeuronTo
(
    Layer* aLayer,
    int aNumber
)
{
    return calcNeuronFrom( aLayer, aNumber + 1 );
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
    int aValue
)
{
    tickWrite = aValue;
    return this;
}
