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
        if( lock( aSkipLockThis ))
        {
            if( aLimb -> lock( aSkipLockLimb ))
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
bool Limb::lock
(
    bool aSkip
)
{
    /* Let default result */
    bool result = true;
    if( aSkip )
    {
        result = sync.try_lock();
    }
    else
    {
        sync.lock();
    }
    return result;
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
    -> setErrorCalc( aLayerFrom -> getErrorCalc() )
    -> setWeightCalc( aLayerFrom -> getWeightCalc() )
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



/*
    Dump weights from neuron of the layer in to the file
*/
Limb* Limb::dumpWeights
(
    /* The layer */
    Layer*      aLayer,
    /* Neuron Index in the layer */
    int         aNeuronIndex,
    /* Layer link */
    Layer*      aLayerLink,
    /* Type parent or child */
    string      aType,
    /* buffer with weights */
    char*       aBuffer,
    /* size of buffer */
    size_t      aSize
)
{
    auto pos = Point3i();
    auto size = aLayerLink -> getSize();
    for( pos.z = 0; pos.z < size.z; pos.z++ )
    {
        /* Create file name */
        stringstream s;
        s
        << aLayer -> getId()
        << "_"
        << aType
        << "_"
        << aLayerLink -> getId()
        << "_z:"
        << pos.z
        << ".txt";

        auto file = net -> getWeightsPath( s.str() );

        if( checkPath( getPath( file )))
        {
            ofstream f;
            f.open( file );
            if( f.is_open() )
            {
                f
                << "layer: " << aLayer -> getId() << endl
                << "index: " << aNeuronIndex << endl
                << "value: " << aLayer -> getNeuronValue( aNeuronIndex ) << endl
                << "error: " << aLayer -> getNeuronError( aNeuronIndex ) << endl
                ;

                f << endl << "Weights" << endl;
                dumpXY
                (
                    pos.z, f, aLayerLink,
                    [ &aBuffer ]( int aIndex )
                    {
                        return *((double*) &aBuffer[ aIndex * NEURON_WEIGHT_SIZE ]);
                    }
                );

                /* Parent or child values */

                f << endl << ( aType == "parent" ? "Parent" : "Child" ) << " values" << endl;
                dumpXY
                (
                    pos.z, f, aLayerLink,
                    [ &aLayerLink ]( int aIndex )
                    {
                        return aLayerLink -> getNeuronValue( aIndex );
                    }
                );

                /* Parent or child values */
                f << endl << ( aType == "parent" ? "Parent" : "Child" ) << " errors" << endl;
                dumpXY
                (
                    pos.z, f, aLayerLink,
                    [ &aLayerLink ]( int aIndex )
                    {
                        return aLayerLink -> getNeuronError( aIndex );
                    }
                );

                /* Incoming and outcoming values */
                f << endl << ( aType == "parent" ? "Incoming" : "Outcoming" ) << " values" << endl;
                dumpXY
                (
                    pos.z, f, aLayerLink,
                    [ &aType, &aBuffer, &aLayerLink, &aLayer, &aNeuronIndex ]( int aIndex )
                    {
                        return
                        (
                            aType == "parent"
                            ? aLayerLink -> getNeuronValue( aIndex )
                            : aLayer -> getNeuronValue( aNeuronIndex )
                        ) *
                        *((double*) &aBuffer[ aIndex * NEURON_WEIGHT_SIZE ]);
                    }
                );

                f.close();
            }
        }
    }

    return this;
}
