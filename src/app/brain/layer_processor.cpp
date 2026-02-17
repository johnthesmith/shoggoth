#include "../../shoggoth/limb.h"
#include "layer_processor.h"



/*
    Constructor
*/
LayerProcessor::LayerProcessor
(
    Limb* aLimb,
    string aId
)
:Layer( aLimb, aId )
{
    getLog() -> trace( "Cateate processor layer" );
}



/*
    Internal destructor
*/
LayerProcessor::~LayerProcessor()
{
}



/*
    Creator
*/
LayerProcessor* LayerProcessor::create
(
    Limb* aLimb,
    string aId
)
{
    return new LayerProcessor( aLimb, aId );
}



/*
    Destructor
*/
void LayerProcessor::destroy()
{
    delete this;
}



/******************************************************************************
    Specific calculate methods
*/


/*
    Calculate neurns in the thread
*/
LayerProcessor* LayerProcessor::calc
(
    /* Type of data calculation */
    Data aData,
    ThreadManager* aThreadManager
)
{
    if( setCalcState( CALCULATING ))
    {
        switch( aData )
        {
            default:
            break;
            case DATA_VALUES:
                calcValues();
                forwardCalcComplete( aThreadManager );
            break;
            case DATA_ERRORS:
                calcErrors();
                backwardCalcComplete( aThreadManager );
            break;
        }
    }

    return this;
}



/*
    Calculate neurons in the layer using threads
*/
LayerProcessor* LayerProcessor::calcValues()
{
    for( size_t index = 0; index < getCount(); index++ )
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
            index,
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
                return false;
            }
        );

        /* MAX postprocessing hi priority */
        if( maxWeightIndex > -1 )
        {
            maxNerve -> setControl( maxWeightIndex, 1.0 );
            setNeuronValue( index, (*( frontFunc ))( max ) );
        }
        else
        {
            /* ADD postprocessing */
            if( count > 0 )
            {
                setNeuronValue
                (
                    index,
                    /* Main value calculation */
                    (*( frontFunc ))( add * mul )
                );
            }
        }
    }

    return this;
}



/*
    Calculate neurons in the layer using threads
*/
LayerProcessor* LayerProcessor::calcErrors()
{
    for( size_t index = 0; index < getCount(); index++ )
    {
        real error    = 0.0;
        switch( getErrorCalc() )
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
                    index,
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
                        return false;
                    }
                );
            }
            break;
            case EC_VALUE:
                error = - getNeuronValue( index );
            break;
        }

        /*
            Calculate current neuron error
            https://habr.com/ru/articles/313216/
        */
        auto currentError = error * ( * ( backFunc ))
        (
            getNeuronValue( index )
        );

        /* Write error in to neuron */
        setNeuronError( index, currentError );
    }
    return this;
}








