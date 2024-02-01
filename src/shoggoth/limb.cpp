#include "limb.h"


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
    layers = LayerList::create( this );
    nerves = NerveList::create( log );
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
    Create new layer
*/
Layer* Limb::createLayer
(
    string aId /* Id of layer */
)
{
    Layer* result = NULL;

    lock();

    auto layers = getLayerList();
    int layerIndex = layers -> getIndexById( aId );

    if( layerIndex > -1 )
    {
        /* Return exists layer object */
        result = layers -> getByIndex( layerIndex );
    }
    else
    {
        /* Create new layer object */
        result = Layer::create( this, aId );
        layers -> push( result );
    }

    unlock();

    return result;
}



/*
    Delete layer by Id
*/
Limb* Limb::deleteLayer
(
    string a /* Id of layer */
)
{
    auto layers = getLayerList();
    auto nerves = getNerveList();
    int layerIndex = layers -> getIndexById( a );
    if( layerIndex > -1 )
    {
        /* Define layer for remove */
        Layer* layer = layers -> getByIndex( layerIndex );

        /* Destroy nerves for layer */
        nerves -> removeByLayer( layer );

        /* Remove layer from layer list */
        layers -> remove( layerIndex );

        /* Destroy layer */
        layer -> destroy();
    }
    return this;
}



/*
    Create new nerve
*/
Nerve* Limb::createNerve
(
    Layer*      aLayerFrom,     /* Layer source */
    Layer*      aLayerTo,       /* Layer destination */
    NerveType   aNerveType,
    BindType    aBindType
)
{
    Nerve* result = Nerve::create
    (
        getLog(),
        aLayerFrom,
        aLayerTo,
        aNerveType,
        aBindType
    );
    nerves -> push( result );
    return result;
}



/*
    Delete layer by Id
*/
Limb* Limb::deleteNerve
(
    Nerve* aNerve /* Nerve opbject */
)
{
    if( aNerve != NULL )
    {
        /* Remove nerve from net list */
        nerves -> remove( aNerve );

        /* Destroy the nerve */
        aNerve -> destroy();
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
    if( aLimb != this )
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
                aLimb -> copyStructureFrom( this -> getLayerList() );
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
    }
    else
    {
        getLog() -> warning( "UnableLimbItselfCopyTo" );
    }
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



/**********************************************************************
    Current age of the limbs config
*/

/*
    Increamet age of limb
*/
Limb* Limb::incAge()
{
    age++;
    return this;
}


unsigned int Limb::getAge()
{
    return age;
}



Limb* Limb::ageFrom
(
    Limb* aSource
)
{
    age = aSource -> getAge();
    return this;
}



/*
    Copy list of layers
*/
Limb* Limb::copyStructureFrom
(
    LayerList* aSource
)
{
    layers -> clear();

    aSource -> loop
    (
        [ this ]
        ( void* p )
        {
            /* Create new layer object and push it to this*/
            auto iLayer = (Layer*) p;
            auto nLayer = copyLayerFrom( iLayer );
            nLayer -> setName( iLayer -> getName());
            layers -> push( nLayer );
            return false;
        }
    );
    return this;
}



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
    Layer::create( this, aLayerFrom -> getId() )
    -> setCount( aLayerFrom -> getCount() );
}
