#include <cmath>

#include <unistd.h> /* usleep */
#include "limb_processor.h"
#include "../func.h"

#include "../../../../../lib/core/math.h"
#include "../../../../../lib/core/str.h"




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

    mon = Mon::create( net -> getMonPath( "procesor.txt" ))
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

LimbProcessor* LimbProcessor::syncToLog
(
    Layer* aLayer
)
{
    getLog() -> begin( "Sync dump" );

    /* Dump header */
    getLog()
    -> trace()
    -> setWidth( 5 )
    -> text( "Last" )
    -> setWidth( 20 )
    -> text( "Layer" )
    -> setWidth( 15 )
    -> text( "Forward" )
    -> setWidth( 15 )
    -> text( "Backward" );

    auto layers = getLayerList();
    int c = layers -> getCount();

//    getMon()
 //    -> setString( Path{ "current" }, aLayer -> getId()  )

    for( int i=0; i<c; i++ )
    {
        auto layer = layers -> getByIndex( i );

//        getMon()
//        -> setString
//        (
//            Path{ "layer", "forward", layer -> getId() },
//            calcStageToString( layer -> getForwardStage( threadCount )
//        )
//        -> setString
//        (
//            Path{ "layer", "backward", layer -> getId() },
//            calcStageToString( layer -> getBackwardStage( threadCount )
//        );

        getLog()
        -> trace()
        -> setWidth( 5 )
        -> text( aLayer == layer ? ">" : " " )
        -> setWidth( 20 )
        -> text( layer -> getNameOrId() )
        -> setWidth( 15 )
        -> text( calcStageToString( layer -> getForwardStage( threadCount )))
        -> setWidth( 15 )
        -> text( calcStageToString( layer -> getBackwardStage( threadCount )))
        ;
    }

    getLog()
    -> setWidth( 0 )
    -> end();

    return this;
}



/*
    Return calc stage for net layers:
        CALC_UNKNOWN
        CALC_NOT_START
        CALC_START
        CALC_COMPLETE
*/
CalcStage LimbProcessor::getCalcStage
(
    CalcDirection aDirection
)
{
    CalcStage result = CALC_UNKNOWN;

    getLayerList() -> loop
    (
        [ this,  &result, &aDirection ]
        ( void* iLayer )
        {
            auto layer = (( Layer* ) iLayer );

            if( result == CALC_UNKNOWN )
            {
                result = aDirection == CALC_BACKWARD ?
                layer -> getBackwardStage( threadCount ) :
                layer -> getForwardStage( threadCount );
            }

            if
            (
                (
                    aDirection != CALC_BACKWARD &&
                    result != layer -> getForwardStage( threadCount )
                ) ||
                (
                    aDirection != CALC_FORWARD &&
                    result != layer -> getBackwardStage( threadCount ) &&
                    learning
                )
            )
            {
                result = CALC_START;
            }

            return result == CALC_START;
        }
    );

    return result == CALC_UNKNOWN ? CALC_COMPLETE : result;
}



/*
    Reset forward and backward counts for layers
*/
LimbProcessor* LimbProcessor::calcReset()
{
    getLayerList() -> loop
    (
        []
        ( void* iLayer )
        {
            (( Layer* ) iLayer ) -> calcReset();
            return false;
        }
    );
    return this;
}



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
    Check forward calculation
    return true if all parents layers is forward calculated
    otherwise return false
*/
bool LimbProcessor::preparedParents
(
    Layer* aLayer
)
{
    bool result = true;
    auto parents = LayerList::create( this );
    getNerveList() -> getParentsByLayer( aLayer, parents );
    parents -> loop
    (
        [ this,  &result ]( void* parent )
        {
            result = (( Layer* ) parent )
            -> getForwardStage( threadCount ) == CALC_COMPLETE;
            return !result;
        }
    );
    parents -> destroy();
    return result;
}



/*
    Check backward calculation
    return true if all children layers is backward calculated
    otherwise return false
*/
bool LimbProcessor::preparedChildren
(
    Layer* aLayer
)
{
    bool result = true;
    auto children = LayerList::create( this );
    getNerveList() -> getChildrenByLayer( aLayer, children );

    children -> loop
    (
        [ this,  &result ]( void* child )
        {

            result = (( Layer* ) child )
            -> getBackwardStage( threadCount ) == CALC_COMPLETE;
            return !result;
        }
    );
    children -> destroy();
    return result;
}



/*
    Calculate all layers
*/
LimbProcessor* LimbProcessor::calc()
{
    mon
    -> addInt( Path{ "count" })
    -> now( Path{ "last" } )
    -> setDouble( Path{ "config", "learningSpeed" }, learningSpeed )
    -> setDouble( Path{ "config", "minWeight" }, minWeight )
    -> setDouble( Path{ "config", "maxWeight" }, maxWeight )
    -> setDouble( Path{ "config", "maxError" }, maxError )
    ;

    /* Calc begin */
    if( getCalcStage( CALC_ALL ) == CALC_COMPLETE )
    {
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

        /* Allocate weigth */
        getNerveList() -> weightsAllocate
        (
            [ this ]
            ( Nerve* nerve )
            {
                if( !nerve -> loadWeight( net -> getNervesPath() ) -> isOk()  )
                {
                    getLog()
                    -> warning( nerve -> getCode() )
                    -> dump( nerve -> getDetails() );
                    nerve -> setOk();
                    nerve -> fill();
                }
            }
        );

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

        /* Reset calc stages for all layers */
        calcReset();

        /* Fill forward and backward list */
        precalc();

        /* Drop learing mode flag */
        learning = false;

        /*
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
    }


    Layer* layer = NULL;

    if( getLayerList() -> getCount() > 0 )
    {
        if( getCalcStage( CALC_FORWARD ) != CALC_COMPLETE )
        {
            /* All forward calculations is not complete */
            if( calcDirection == CALC_BACKWARD )
            {
                /* Begin of forward calculation */
                calcLayerIndex = 0;
                calcDirection = CALC_FORWARD;
            }

            /* Get calculated layer by index */
            layer = ( Layer* ) forwardList -> getByIndex( calcLayerIndex );

            mon -> startTimer( Path{ "duration", layer -> getId() });

            if
            (
                /* if layer not forward calculated and ... */
                layer -> getForwardStage( threadCount ) == CALC_NOT_START
            )
            {
                /* Set default thread id */
                int idThread = 0;
                /* Layer calculation */
                layer -> calcStartForward();

                /* --- thread begin --- */
                layerCalcValue( layer, idThread, learning );

                /* Set local sync for local works */
                layer -> calcCompleteForward();
                 /* --- thread end --- */
            }

            if( layer -> getForwardStage( threadCount ) == CALC_COMPLETE )
            {
                calcLayerIndex++;
            }

            if( getCalcStage( CALC_FORWARD ) == CALC_COMPLETE )
            {
            }
        }
        else
        {
            /*
                All forward calculation is complete
                Begin the learning process
            */

            if( calcDirection == CALC_FORWARD )
            {
                /* Begin of backward calculation */
                calcLayerIndex = 0;
                calcDirection = CALC_BACKWARD;
            }

            layer = ( Layer* ) backwardList -> getByIndex( calcLayerIndex );
            mon -> startTimer( Path{ "duration", layer -> getId() });

            if
            (
                /* if layer not backward calculated and ... */
                layer -> getBackwardStage( threadCount ) == CALC_NOT_START
            )
            {
                /* Set default thread id */
                int idThread = 0;
                /* Layer calculation */
                layer -> calcStartBackward();
                /* --- thread begin --- */

                /* Calculate errors for layers with parents */
                if( layerParentsExists( layer, BT_VALUE ))
                {
                    layerLearning( layer, idThread );
                }

                /* Set local sync for local works */
                layer -> calcCompleteBackward();
                /* --- thread end --- */
            }

            if( layer -> getBackwardStage( threadCount ) == CALC_COMPLETE )
            {
                calcLayerIndex++;
            }

            /* Finish calculating */
            if( getCalcStage( CALC_BACKWARD ) == CALC_COMPLETE )
            {
                /* Write weight to file */
                if( tick % tickWrite == 0 )
                {
                    getNerveList() -> loop
                    (
                        [ this ]
                        ( void* item )
                        {
                            auto nerve = (Nerve*) item;
                            if( !nerve -> saveWeight( net -> getNervesPath() ) -> isOk()  )
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

            }
        }

        auto id = layer -> getId();

        mon
        -> setBool( Path{ "current", "learning" }, learning )
        -> setInt( Path{ "current", "tick" }, tick )
        -> stopTimer( Path{ "duration", id })
        -> minInt( Path{ "durationMin", id }, Path{ "duration", id })
        -> maxInt( Path{ "durationMax", id }, Path{ "duration", id })
        -> timerToString( Path{ "duration", id } )
        -> flush();
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
        /* Reset calculateion index */
        calcLayerIndex = 0;

        /* Clear forward and backward lists */
        forwardList -> resize( 0 );
        backwardList -> resize( 0 );

        /* Reset calc stages for all layers */
        calcReset();

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
    double summValue        = 0.0;
    double summCommand      = 0.0;
    double summSample       = 0.0;
    double summErrorValue   = 0.0;
    int countSample         = 0;
    int countValue          = 0;
    int countErrorValue     = 0;

    /* Neuron has a binds and it is not a Receptor */
    parentsLoop
    (
        aLayer,
        aIndex,
        BT_ALL,
        [
            &summValue,
            &summSample,
            &summCommand,
            &countSample,
            &countValue,
            &countErrorValue,
            &summErrorValue
        ]
        (
            Layer* aParentLayer,
            int aParentIndex,
            Nerve* aNerve,
            double aWeight,
            int aWeightIndex /* not use */
        ) -> bool
        {
            auto w = aParentLayer -> getNeuronValue( aParentIndex ) * aWeight;

            /* Calculate summ */
            switch( aNerve -> getBindType())
            {
                case BT_ALL:
                break;
                case BT_VALUE:
                    summValue += w;
                    countValue ++;
                break;
                case BT_ERROR_TO_VALUE:
                {
                    auto e = aParentLayer -> getNeuronError( aParentIndex );
                    summErrorValue += e * e * aWeight;
//                    summErrorValue += abs
//                    (
//                        aParentLayer -> getNeuronError( aParentIndex ) * aWeight
//                    );
                    countErrorValue++;
                }
                break;
                case BT_COMMAND:
                    summCommand += w;
                break;
                case BT_SAMPLE:
                    summSample += w;
                    countSample ++;
                break;
            }
            return false;
        }
    );

    if( countValue > 0 )
    {
        /* Value calculation */
        aLayer -> setNeuronValue( aIndex, (*( aLayer -> frontFunc ))( summValue ) );
    }

    if( countErrorValue > 0 )
    {
        /* Write avg error from parent to layer */
//        aLayer -> setNeuronValue
//        (
//            aIndex,
//            summErrorValue / countErrorValue
//        );
        aLayer -> setNeuronValue( aIndex, summErrorValue / 2 );
    }

    /* Start error from Sample */
    if( countSample > 0 )
    {
        auto value = aLayer -> getNeuronValue( aIndex );
        aLayer -> setNeuronError
        (
            aIndex,
            ( summSample - value ) * ( * ( aLayer -> backFunc))( value )
            * ( summCommand > EPSILON_D ? 1.0 : 0.0 )
        );
    }

    /* Teaching detected */
    aLearning = aLearning || summCommand > EPSILON_D;

    return this;
}



/*
    Calculate neuron
*/
LimbProcessor* LimbProcessor::neuronLearning
(
    Layer*  aLayer, /* Layer for calculation */
    int     aIndex  /* Neuron index of layer */
)
{
    /* Define variables */
    int count = 0;
    double summError    = 0.0;

    /*
        caclulate error form all children for current neuron
    */
    childrenLoop
    (
        aLayer,
        aIndex,
        BT_VALUE,
        [
            &count,
            &summError
        ]
        (
            Layer* aChild,
            int aChildIndex,
            Nerve* aNerve,
            double aWeight,
            int aWeightIndex    /* Not use */
        ) -> bool
        {
            summError += aChild -> getNeuronError( aChildIndex ) * aWeight;
            count++;
            return false;
        }
    );

    if( count > 0 )
    {
        auto value = aLayer -> getNeuronValue( aIndex );
        aLayer -> setNeuronError
        (
            aIndex,
            /* Part of derivative */
            summError * ( * ( aLayer -> backFunc ))( value )
        );
    }

    auto currentNeuronError = aLayer -> getNeuronError( aIndex );

    /* Learning */
    parentsLoop
    (
        aLayer,
        aIndex,
        BT_VALUE,
        [
            this,
            &currentNeuronError
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
            * currentNeuronError;

            double deltaWeight = gradient * learningSpeed
            + aNerve -> getDeltaWeight( aWeightIndex ) * 0.3;

            double newWeight = aWeight + deltaWeight;

            aNerve -> setWeight( aWeightIndex, newWeight );
            aNerve -> setDeltaWeight( aWeightIndex, deltaWeight );

            return false;
        }
    );

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
    Calculate neurons error for learning
*/
LimbProcessor* LimbProcessor::layerLearning
(
    Layer*  aLayer, /* Layer for calculation */
    int     aThread /* Current thread */
)
{
    int b = calcNeuronFrom( aLayer, aThread );
    int e = calcNeuronTo( aLayer, aThread );

    for( int i = b; i < e; i ++ )
    {
        neuronLearning( aLayer, i );
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
