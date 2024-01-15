#include "limb.h"
#include "neuron.h"



/*
    Constructor
*/
Limb::Limb
(
    Log* aLog
)
{
    /* Set properties */
    log = aLog;

    /* Create layers and nerves structures */
    layers = LayerList::create( log );
    nerves = NerveList::create();
}



/*
    Destructor
*/
Limb::~Limb()
{
    layers -> clear() -> destroy();
    nerves -> clear() -> destroy();
}



/*
    Create
*/
Limb* Limb::create
(
    Log* aLog       /* Log object*/
)
{
    return new Limb( aLog );
}



/*
    Destroy
*/
void Limb::destroy()
{
    delete this;
}



/*
    Return log object
*/
Log* Limb::getLog()
{
    return log;
}




/*
    Create new nerve
*/
Nerve* Limb::createNerve
(
    string      aId,            /* Nerve id */
    Layer*      aLayerFrom,     /* Layer source */
    Layer*      aLayerTo,       /* Layer destination */
    NerveType   aNerveType,
    BindType    aBindType,
    double      aMinWeight,
    double      aMaxWeight
)
{
    Nerve* result = Nerve::create
    (
        this,
        aId,
        aLayerFrom,
        aLayerTo,
        aNerveType,
        aBindType,
        aMinWeight,
        aMaxWeight
    );
    nerves -> push( result );
    return result;
}



/*
    Delete layer by Id
*/
Limb* Limb::deleteNerve
(
    string a /* Id of layer */
)
{
    Nerve* nerve = nerves -> getById( a );

    if( nerve != NULL )
    {
        /* Remove nerve from net list */
        nerves -> remove( nerve );

        /* Destroy the nerve */
        nerve -> destroy();
    }
    return this;
}



/**********************************************************************
    Limb synchronization
*/



Limb* Limb::copyTo
(
    Limb* aLimb,
    bool aStrictSync
)
{
    aLimb -> lock();
    lock();

    auto layersIsEqual = layers -> compare( aLimb -> getLayerList() );
    auto nervesIsEqual = nerves -> compare( aLimb -> getNerveList() );

    if( aStrictSync )
    {
        if( !layersIsEqual )
        {
            /* Copy structure of layers */
            aLimb
            -> getLayerList()
            -> copyStructureFrom( this -> getLayerList() );

            layersIsEqual = true;
        }
        if( !nervesIsEqual )
        {
            /* Copy structure of nerves */
            aLimb
            -> getNerveList()
            -> copyStructureFrom( this -> getNerveList());

            nervesIsEqual = true;
        }
    }

    if( layersIsEqual && nervesIsEqual )
    {
        aLimb
        -> getLayerList()
        -> copyValuesFrom( this -> getLayerList() )
        -> copyErrorsFrom( this -> getLayerList() )
        ;
    }

    /* Data sinchronization */
    unlock();
    aLimb -> unlock();
    return this;
}



/**********************************************************************
    Setters and getters
*/


/*
    Return LayerList
*/
LayerList* Limb::getLayerList()
{
    return layers;
}



/*
    Return list of nerves
*/
NerveList* Limb::getNerveList()
{
    return nerves;
}



bool Limb::nerveWeightLoop
(
    NeuronList* a,
    IndexWeightLambda aCallback
)
{
    a -> loop
    (
        [ this, &a, &aCallback ]
        (
            Neuron* fromNeuron
        )
        {
            a -> loop
            (
                [ this, &fromNeuron, &aCallback ]
                (
                    Neuron* toNeuron
                )
                {
                    auto fromLayer = fromNeuron -> getLayer();
                    auto toLayer = toNeuron -> getLayer();

                    auto foundedNerves = NerveList::create();
                    nerves -> selectByLayers
                    (
                        fromLayer,
                        toLayer,
                        foundedNerves
                    );

                    foundedNerves -> loop
                    (
                        [
                            &fromNeuron,
                            &toNeuron,
                            &aCallback
                        ]
                        (
                            void* aItem
                        )
                        {
                            auto nerve = (Nerve*)aItem;
                            auto indexFrom = fromNeuron -> getIndex();
                            auto indexTo = toNeuron -> getIndex();
                            auto weightIndex = nerve -> getIndexByNeuronsIndex
                            (
                                indexFrom,  /* in parent layer */
                                indexTo     /* in child layer */
                            );
                            if( weightIndex >= 0 )
                            {
                                aCallback
                                (
                                    weightIndex,
                                    fromNeuron,
                                    toNeuron,
                                    nerve
                                );
                            }
                            return false;
                        }
                    );

                    foundedNerves -> destroy();
                    return false;
                }
            );
            return false;
        }
    );
    return this;
}




/**********************************************************************
    Limb synchronization
*/



/*
    Lock Limb for operations with layers
*/
Limb* Limb::lock()
{
    sync.lock();
    return this;
}



/*
    Unlock Limb after lock method
*/
Limb* Limb::unlock()
{
    sync.unlock();
    return this;
}



/*
    Loop for each parents neuron of this neuron
*/
Limb* Limb::parentsLoop
(
    Layer*          aLayer,     /* Layer */
    int             aIndex,     /* Neuron index */
    parentsLambda   aCallback   /* Callback method */
)
{
    /* Loop by nerves */
    getNerveList() -> loop
    (
        [ &aLayer, &aCallback, &aIndex ]
        ( void* aNerve )
        {
            auto iNerve = ( Nerve* ) aNerve;
            if( iNerve -> getChild() == aLayer )
            {
                int from = 0;
                int to = 0;
                iNerve -> getWeightsRangeByChildIndex
                (
                    aIndex, from, to
                );
                /* Loop by weights */
                for( int i = from; i < to;  i++ )
                {
                    aCallback
                    (
                        iNerve -> getParent(),
                        iNerve -> getParentByWeightIndex( i ),
                        iNerve,
                        iNerve -> getWeight( i ),
                        i
                    );
                }
            }
            return false;
        }
    );
    return this;
}



/*
    Loop for each child of i neuron for Layer
*/
Limb* Limb::childrenLoop
(
    Layer*          aLayer,
    int             aIndex,
    childrenLambda  aCallback
)
{
    /* Loop by nerves */
    getNerveList() -> loop
    (
        [ &aLayer, &aCallback, &aIndex ]
        ( void* aNerve )
        {
            auto iNerve = ( Nerve* ) aNerve;
            if( iNerve -> getParent() == aLayer )
            {
                int from = 0;
                int to = 0;
                int step = 0;

                iNerve -> getWeightsRangeByParentIndex
                (
                    aIndex, from, to, step
                );

                /* Loop by weights */
                for( int i = from; i < to;  i += step )
                {
                    aCallback
                    (
                        iNerve -> getChild(),
                        iNerve -> getChildByWeightIndex( i ),
                        iNerve,
                        iNerve -> getWeight( i ),
                        i
                    );
                }
            }
            return false;
        }
    );
    return this;
}
