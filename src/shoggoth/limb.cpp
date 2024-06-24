#include "limb.h"



/*
    Constructor
*/
Limb::Limb
(
    LogManager* aLogManager
)
{
    /* Set properties */
    logManager = aLogManager;

    /* Create layers and nerves structures */
    layers = LayerList::create( this );
    nerves = NerveList::create( logManager );
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
    LogManager* aLogManager       /* Log object*/
)
{
    return new Limb( aLogManager );
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
    return logManager -> getLog();
}



/*
    Return log object
*/
LogManager* Limb::getLogManager()
{
    return logManager;
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

        lastChange = now();
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

        lastChange = now();
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
        logManager,
        aLayerFrom,
        aLayerTo,
        aNerveType,
        aBindType
    );
    nerves -> push( result );
    lastChange = now();
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

        lastChange = now();
    }
    return this;
}



/**********************************************************************
    Limb synchronization
*/

Limb* Limb::copyTo
(
    Limb* aLimb,
    bool aStrictSync,
    bool aSkipLockThis,
    bool aSkipLockLimb
)
{
    if( aLimb != this )
    {
        if( aSkipLockThis ? tryLock() : ( lock() == this ) )
        {
            if( aSkipLockLimb ? aLimb -> tryLock() : ( aLimb -> lock() == aLimb ))
            {

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
                        -> copyStructureFrom( this -> getNerveList(), aLimb -> getLayerList() );
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
                aLimb -> unlock();
            }
            unlock();
        }
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
    Try lock Limb for operations with layers
*/
bool Limb::tryLock()
{
    return sync.try_lock();
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
    BindType        aBindType,
    parentsLambda   aCallback   /* Callback method */
)
{
    /* Loop by nerves */
    getNerveList() -> loop
    (
        [ &aLayer, &aCallback, &aIndex, &aBindType ]
        ( void* aNerve )
        {
            auto iNerve = ( Nerve* ) aNerve;
            if
            (
                iNerve -> getChild() == aLayer &&
                (
                    aBindType == BT_ALL ||
                    iNerve -> getBindType() == aBindType
                )
            )
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
    BindType        aBindType,
    childrenLambda  aCallback
)
{
    /* Loop by nerves */
    getNerveList() -> loop
    (
        [ &aLayer, &aCallback, &aIndex, &aBindType ]
        ( void* aNerve )
        {
            auto iNerve = ( Nerve* ) aNerve;
            if
            (
                iNerve -> getParent() == aLayer &&
                (
                    aBindType == BT_ALL ||
                    iNerve -> getBindType() == aBindType
                )
            )
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
Limb* Limb::setLastUpdate
(
    long long aValue
)
{
    lastUpdate = aValue;
    return this;
}


long long Limb::getLastUpdate()
{
    return lastUpdate;
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
    -> setFrontFunc( aLayerFrom -> getFrontFunc() )
    -> setBackFunc( aLayerFrom -> getBackFunc() )
    -> setCount( aLayerFrom -> getCount() );
}



long long Limb::getLastChange()
{
    return lastChange;
}



/*
    Configuration postprocessing
*/
void Limb::onAfterReconfig
(
    ParamList* aConfig
)
{
}



/*
    Check layer parents existing by type
*/
bool Limb::layerParentsExists
(
    Layer* aLayer,
    BindType aBindType
)
{
    bool result = false;
    nerves -> loop
    (
        [ &aLayer, &aBindType, &result ]
        ( void* item )
        {
            auto iNerve = ( Nerve* )item;
            result
            = iNerve -> getChild() == aLayer
            && iNerve -> getBindType() == aBindType;
            return result;
        }
    );
    return result;
}
