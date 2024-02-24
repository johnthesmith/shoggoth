// TODO необходимо найти место методу
// После изменения состояния сети
/* Build layers calculation sequence list */





#include <cmath>

#include "limb_processor.h"

#include "../func.h"

#include "../../../../../lib/core/math.h"



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
    for( int i=0; i<c; i++ )
    {
        auto layer = layers -> getByIndex( i );

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
                    result != layer -> getBackwardStage( threadCount )
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
    if( getCalcStage( CALC_ALL ) == CALC_COMPLETE )
    {
        /* Check structure with net */
        net -> syncToLimb( this );
        getNerveList() -> weightsAllocate
        (
            []( Nerve* nerve )
            {
                nerve -> fill();
            }
        );

        /* Upload start values from Net */
        net -> swapValuesAndErrors
        (
            Actions{ READ_VALUES }, /* Actions */
            TASK_PROC,   /* Role */
            this         /* Participant object */
        );

        /* Reset calc stages for all layers */
        calcReset();
        /* Fill forward and backward list */
        precalc();
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
            layer = ( Layer* ) forwardList
            -> getByIndex( calcLayerIndex );

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
                layerCalcValue( layer, idThread );

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
                // event( THINKING_END );
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

            layer = ( Layer* ) backwardList
            -> getByIndex( calcLayerIndex );

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
                /* Calculate errors */
                layerLearning( layer, idThread );
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
                /* Load values and errors to net */
                net -> swapValuesAndErrors
                (
                    { WRITE_VALUES, WRITE_ERRORS }, /* Action */
                    TASK_PROC,   /* Role */
                    this         /* Participant object */
                );
            }
        }

        /* Dump sync to log */
        syncToLog( layer );
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
    Set learning mode
*/
LimbProcessor* LimbProcessor::setLearningMode
(
    bool a /* Value */
)
{
    learningMode = a;
    return this;
}



/*
    Get learning mode
*/
bool LimbProcessor::getLearningMode()
{
    return learningMode;
}



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
LimbProcessor* LimbProcessor::setWakeupWeight
(
    double a
)
{
    wakeupWeight = a;
    return this;
}



/*
    Set error normalize
*/
LimbProcessor* LimbProcessor::setErrorNormalize
(
    double a
)
{
    errorNormalize = a;
    return this;
}



/*
    Get learning speed
*/
double LimbProcessor::getLearningSpeed()
{
    return learningSpeed;
}



/*
    Get wakeup weight k
*/
double LimbProcessor::getWakeupWeight()
{
    return wakeupWeight;
}



/*
    Get error normalize
*/
double LimbProcessor::getErrorNormalize()
{
    return errorNormalize;
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
    Calculate neuron of layer
*/
LimbProcessor* LimbProcessor::neuronCalcValue
(
    Layer* aLayer,
    int aIndex
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
        [
            this,
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
                case BT_VALUE:
                    summValue += w;
                    countValue ++;
                break;
                case BT_ERROR_TO_VALUE:
                    summErrorValue += abs
                    (
                        aParentLayer -> getNeuronError( aParentIndex ) * aWeight
                    );
                    countErrorValue++;
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
        aLayer -> setNeuronValue
        (
            aIndex,
            FUNC_SIGMOID_LINE_ZERO_PLUS( summValue, 1.0 )
        );
    }

    if( countErrorValue > 0 )
    {
        /* Write avg error from parent to layer */
        aLayer -> setNeuronValue
        (
            aIndex,
            summErrorValue / countErrorValue
        );
    }

    if( countSample > 0 )
    {
        aLayer -> setNeuronError
        (
            aIndex,
            FUNC_SIGMOID_LINE_MINUS_PLUS
            (
                ( summSample - aLayer -> getNeuronValue( aIndex )) *
                ( summCommand > EPSILON_D ? 1.0 : 0.0 ),
                1.0
            )
        );
    }

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
    double summWeight   = 0.0;
    double summError    = 0.0;
    int countValue      = 0;

    /* Caclulate error form all children for current neuron */
    childrenLoop
    (
        aLayer,
        aIndex,
        [
            this,
            &summWeight,
            &summError,
            &countValue,
            &aIndex
        ]
        (
            Layer* aChild,
            int aChildIndex,
            Nerve* aNerve,
            double aWeight,
            int aWeightIndex    /* Not use */
        ) -> bool
        {
            switch( aNerve -> getBindType())
            {
                case BT_VALUE:
                    summError += aChild -> getNeuronError( aChildIndex ) * aWeight;
                    summWeight += abs( aWeight );
                break;
                default: break;
            }
            return false;
        }
    );

    /* Neuron error is calculated */
    /* If children neurons afected the parent neuron ... */
    if( summWeight > EPSILON_D )
    {
        /*
            Сумму ошибок всех дочерних нейронов и делим
            на сумму весов для дочерних нейронов + 1
            что бы исклюить деление на 0. Считаем что +1 это мелоч
            aErrorNormalize - сила распространения ошибки.
        */
        aLayer -> setNeuronError
        (
            aIndex,
            FUNC_SIGMOID_LINE_MINUS_PLUS
            (
                summError / ( 1.0 + summWeight * errorNormalize ),
                1.0
            )
        );
    }

    /* Learning */
    parentsLoop
    (
        aLayer,
        aIndex,
        [
            this,
            &aLayer,
            &aIndex
        ]
        (
            Layer*  aParentLayer,
            int     aParentIndex,
            Nerve*  aNerve,
            double  aWeight,
            int     aWeightIndex
        ) -> bool
        {
            switch( aNerve -> getBindType())
            {
                default:break;
                case BT_VALUE:
                {
                    /*
                        Calculate delta
                    */
                    double w = abs( aWeight );
                    /* aWakeupWeight epsilon for zero weight подъем нулевых связей */
                    double wv = ( w < wakeupWeight ? wakeupWeight : w )
                    * aParentLayer -> getNeuronValue( aParentIndex );
                    double deltaWeight = aLayer -> getNeuronError( aIndex ) * wv;
                    aNerve -> setWeight
                    (
                        aWeightIndex,
                        FUNC_SIGMOID_LINE_MINUS_PLUS
                        (
                            aWeight + deltaWeight * learningSpeed,
                            1.0
                        )
                    );
                }
            }
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
    Layer*  aLayer, /* Layer for calculation */
    int     aThread /* Current thread number */
)
{
    int b = calcNeuronFrom( aLayer, aThread );
    int e = calcNeuronTo( aLayer, aThread );
    for( int i = b; i < e; i ++ )
    {
        neuronCalcValue( aLayer, i );
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





