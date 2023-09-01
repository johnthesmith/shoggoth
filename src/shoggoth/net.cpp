#include "net.h"

/*
    Constructor
*/
Net::Net
(
    Application* a    /* Log object */
)
{
    application = a;
    application -> getLog() -> trace( "Create net" );

    layers = LayerList::create();
    nerves = new NerveList();
}




/*
    Constructor
*/
Net::~Net()
{
    clear();

    delete( nerves );
    layers -> destroy();

    getLog() -> trace( "Destroy net" );
}



/*
    Static constructor
*/
Net* Net::create
(
    Application* a
)
{
    return new Net( a );
}



/*
    Destructor
*/
void Net::destroy()
{
    delete ( this );
}




/*
    Create new nerve
*/
Nerve* Net::createNerve
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
        getLog(),
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
Net* Net::deleteNerve
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



/*
    Clear all layers
*/
Net* Net::clear()
{
    int c = layers -> getCount();
    for( int i = 0; i < c; i++ )
    {
        Layer* layer = ( Layer* ) layers -> getByIndex( i );
        layer -> destroy();
    }
    layers -> resize( 0 );
    return this;
}








/*
    Calculate all enabled layers
*/

Net* Net::calc()
{
    if( getCalcStage( CALC_ALL ) == CALC_COMPLETE )
    {
        /* Begin of calcuation loop */
        readNet();
        /* Reset calc stages for all layers */
        calcReset();
    }

    if( layers -> getCount() > 0 )
    {
        /* Dump sync to log */
        //sync -> toLog( calcLayerIndex );

        if( getCalcStage( CALC_FORWARD ) != CALC_COMPLETE )
        {
            /* All forward calculations is not complete */

            if( !calcDirection )
            {
                /* Begin of forward calculation */
                calcLayerIndex = 0;
                calcDirection = true;
            }

            /* Get calculated layer by index */
            Layer* layer =
            ( Layer* ) layers -> getByIndex( calcLayerIndex );

            if
            (
                /* if layer not forward calculated and ... */
                layer -> getForwardStage( processorCount ) == CALC_NOT_START &&
                /* all parents is prepared */
                preparedParents( layer )
            )
            {
                /* Set default thread id */
                int idThread = 0;
                /* Layer calculation */
                layer -> calcStartForward();
                /* --- thread begin --- */
                layer -> calcValue( idThread, processorCount );
                /* Set local sync for local works */
                layer -> calcCompleteForward();
                 /* --- thread end --- */
           }

            if( layer -> getForwardStage( processorCount ) == CALC_COMPLETE )
            {
                calcLayerIndex++;
                if( calcLayerIndex >= layers -> getCount() )
                {
                    calcLayerIndex = 0;
                }
            }
        }
        else
        {
            /* All forward calculation is complete */

            if( calcDirection )
            {
                /* Begin of backward calculation */
                calcLayerIndex = layers -> getCount() - 1;
                calcDirection = false;
            }

            Layer* layer =
            ( Layer* ) layers -> getByIndex( calcLayerIndex );

            if
            (
                /* if layer not backward calculated and ... */
                layer -> getBackwardStage( processorCount ) == CALC_NOT_START &&
                /* server exists or all children is prepared */
                preparedChildren( layer )
            )
            {
                int idThread = 0;

                layer -> calcStartBackward();

                /* Calculate errors */
                layer -> learning
                (
                    errorNormalize,
                    learningSpeed,
                    wakeupWeight
                );

                /* Set local sync for local works */
                layer -> calcCompleteBackward();
            }

            if( layer -> getBackwardStage( processorCount ) == CALC_COMPLETE )
            {
                calcLayerIndex--;
                if( calcLayerIndex < 0 )
                {
                    calcLayerIndex = layers -> getCount() - 1;
                }
            }
        }
    }

    return this;
}



/*
    Check forward calculation
    return true if all parents layers is forward calculated
    otherwise return false
*/
bool Net::preparedParents
(
    Layer* aLayer
)
{
    bool result = true;
    auto parents = LayerList::create();
    nerves -> getParentsByLayer( aLayer, parents );
    parents -> loop
    (
        [ this,  &result ]( void* parent )
        {
            result = (( Layer* ) parent )
            -> getForwardStage( processorCount ) == CALC_COMPLETE;
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
bool Net::preparedChildren
(
    Layer* aLayer
)
{
    bool result = true;
    auto children = LayerList::create();
    nerves -> getChildrenByLayer( aLayer, children );
    children -> loop
    (
        [ this,  &result ]( void* child )
        {

            result = (( Layer* ) child )
            -> getBackwardStage( processorCount ) == CALC_COMPLETE;
            return !result;
        }
    );
    children -> destroy();
    return result;
}



/*
    Load layers from local storage
*/
Net* Net::loadLayers()
{
    layers -> loop
    (
        []
        ( void* iLayer )
        {
            auto layer = (( Layer* ) iLayer );
            if( layer -> getRead() )
            {
                layer -> loadValue();
            }
            return false;
        }
    );
    return this;
}



/*
    Save layers
*/
Net* Net::saveLayers()
{
    layers -> loop
    (
        []
        ( void* iLayer )
        {
            auto layer = (( Layer* ) iLayer );

            /* Write layer value to local file */
            if( layer -> getWrite() )
            {
                layer -> saveValue();
            }
            return false;
        }
    );
    return this;
}



/*
    Save layers
*/
Net* Net::readLayersFromServer()
{
    layers -> loop
    (
        [ this ]
        ( void* iLayer )
        {
            auto layer = (( Layer* ) iLayer );
            if( layer -> getFromServer() )
            {
                layer -> readFromServer( host, port );
            }
            return false;
        }
    );
    return this;
}



/*
    Return application object
*/
Application* Net::getApplication()
{
    return application;
}



/*
    Return log object
*/
Log* Net::getLog()
{
    return application -> getLog();
}




/*
    Return neuron by screen position
*/
Net* Net::getNeuronsByScreenPos
(
    NeuronList* aList,
    const Point3d& aPosition
)
{
    int c = layers -> getCount();
    for( int i = 0; i < c; i++ )
    {
        Layer* layer = ( Layer* ) layers -> getByIndex( i );
        layer -> getNeuronsByScreenPos( aList, aPosition );
    }
    return this;
}



Net* Net::setSelected
(
    Neuron* a
)
{
    selected = a;
    return this;
}



Neuron* Net::getSelected()
{
    return selected;
}




/*
    On mouse left click event
*/
Net* Net::setSelected
(
    Scene& aScene /* Scene object */
)
{
    auto neurons = NeuronList::create();
    getNeuronsByScreenPos( neurons, aScene.getMouseCurrentScreen() );

    if( neurons -> getCount() > 0 )
    {
        setSelected( neurons -> getByIndex( 0 ));
    }
    else
    {
        setSelected( NULL );
    }

    neurons -> destroy();

    return this;
}




Net* Net::switchShowLayer()
{
    int c = layers -> getCount();
    for( int i = 0; i < c; i++ )
    {
        Layer* layer = ( Layer* ) layers -> getByIndex( i );
        layer -> switchShowLayer();
    }
    return this;
}



/*
    Set learning mode
*/
Net* Net::setLearningMode
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
bool Net::getLearningMode()
{
    return learningMode;
}



/*
    Switch learning mode true/false
*/
Net* Net::switchLearningMode()
{
    learningMode = !learningMode;
    return this;
}



/*
    Apply config from Json
*/
Net* Net::applyConfig
(
    ParamList* json
)
{
    json
    -> loadInt( "processorCount", processorCount, processorCount )
    -> selectObject( vector<string>{ "server" } )
    -> loadString( "host", host, host )
    -> loadInt( "port", port, port )
    ;

//    setLearningSpeed( json -> getDouble( "learningSpeed", getLearningSpeed() ));
//    setWakeupWeight( json -> getDouble( "wakeupWeight", getWakeupWeight() ));
//    setErrorNormalize( json -> getDouble( "errorNormalize", getErrorNormalize() ));
//    setStoragePath( json -> getString( "storagePath", getStoragePath() ));

    auto configLayers = json -> getObject( "layers" );

    if( configLayers != NULL )
    {
        /* Remove layers absents in the use list */
        purgeLayers( configLayers );

        /* Create and reload layers */
        configLayers -> loop
        (
            [ this, &configLayers ]
            (
                Param* iParam
            )
            {
                auto layerId = iParam -> getName();
                auto layer = createLayer( layerId );
                loadLayerFromConfig( layerId, configLayers );
                return false;
            }
        );

        /* Nerves */
        auto jsonNerves = json -> getObject( "nerves" );
        if( jsonNerves != NULL )
        {
            jsonNerves -> loop
            (
                [ this ]
                ( Param* aItem )
                {
                    /* Check the json layer */
                    if( aItem -> getType() == KT_OBJECT )
                    {
                        auto jsonNerve      = aItem -> getObject();
                        auto idFrom         = jsonNerve -> getString( "idFrom" );
                        auto idTo           = jsonNerve -> getString( "idTo" );
                        auto bindType       = Nerve::bindTypeFromString( jsonNerve -> getString( "bindType" ));
                        auto nerveType      = Nerve::nerveTypeFromString( jsonNerve -> getString( "nerveType" ));
                        auto nerveDelete    = jsonNerve -> getBool( "delete" );
                        auto idNerve        = jsonNerve -> getString( "id", idFrom + "_" + idTo + "_" + jsonNerve -> getString( "bindType" ));

                        /* Find the layers */
                        auto from = layers -> getById( idFrom );
                        auto to = layers -> getById( idTo );

                        if( from != NULL && to != NULL )
                        {
                            auto nerve = nerves -> getById( idNerve );
                            if
                            (
                                nerve != NULL &&
                                (
                                    nerve -> getParent() != from ||
                                    nerve -> getChild() != to ||
                                    nerve -> getBindType() != bindType ||
                                    nerve -> getNerveType() != nerveType ||
                                    nerveDelete
                                )
                            )
                            {
                                deleteNerve( idNerve );
                                nerve = NULL;
                            }

                            if( nerve == NULL && !nerveDelete )
                            {
                                createNerve
                                (
                                    idNerve,
                                    from,
                                    to,
                                    nerveType,
                                    bindType,
                                    jsonNerve -> getDouble( "minWeight", 1.0 ),
                                    jsonNerve -> getDouble( "maxWeight", 1.0 )
                                );
                            }
                        }
                        else
                        {
                            getLog() -> warning( "Layers not found for nerve" ) -> prm( "id", idNerve );
                        }
                    }
                    return false;
                }
            );
        }
    }

    return this;
}



/*
    Return layer list
*/
LayerList* Net::getLayers()
{
    return layers;
}





/*
    Set learning speed
*/
Net* Net::setLearningSpeed
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
Net* Net::setWakeupWeight
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
Net* Net::setErrorNormalize
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
double Net::getLearningSpeed()
{
    return learningSpeed;
}



/*
    Get wakeup weight k
*/
double Net::getWakeupWeight()
{
    return wakeupWeight;
}



/*
    Get error normalize
*/
double Net::getErrorNormalize()
{
    return errorNormalize;
}




Net* Net::setStoragePath
(
    const string a
)
{
    storagePath = a;
    return this;
}



string Net::getStoragePath()
{
    return storagePath;
}



/*
    Return list ov nerves
*/
NerveList* Net::getNerves()
{
    return  nerves;
}



/******************************************************************************
    Layers
*/


/*
    Return layer by Id
*/
Layer* Net::getLayerById
(
    string a /* Id of layer */
)
{
    Layer* result = NULL;
    int c = layers -> getCount();
    for( int i=0; i < c && result == NULL; i++ )
    {
        Layer* iLayer = layers -> getByIndex( i );
        if( iLayer -> getId() == a )
        {
            result = iLayer;
        }
    }
    return result;
}



/*
    Create new layer
*/
Layer* Net::createLayer
(
    string a /* Id of layer */
)
{
    Layer* result = NULL;

    int layerIndex = layers -> getIndexById( a );

    if( layerIndex > -1 )
    {
        /* Return exists layer object */
        result = layers -> getByIndex( layerIndex );
    }
    else
    {
        /* Create new layer object */
        result = Layer::create( this, a );
        layers -> push( result );
        result -> setStoragePath( getStoragePath() );
    }

    return result;
}



/*
    Delete layer by Id
*/
Net* Net::deleteLayer
(
    string a /* Id of layer */
)
{
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
    Load layer structure from param list
    Layer may be resized.
*/
Net* Net::loadLayer
(
    Layer*      aLayer,
    ParamList*  aParams
)
{
    if( this -> isOk() )
    {
        /* Set ID from params */
        if( aLayer-> getId() != aParams -> getString( "id", aLayer -> getId() ))
        {
            setCode( "InvalidLayerID" );
        }
        else
        {
            /* Set Size from params */
            auto paramsSize = aParams -> getObject( "size" );
            if( paramsSize != NULL )
            {
                auto newSize = Point3i
                (
                    paramsSize -> getInt( 0 ),
                    paramsSize -> getInt( 1 ),
                    paramsSize -> getInt( 2 )
                );

                if( newSize != aLayer -> getSize() )
                {
                    nerves -> removeByLayer( aLayer );
                }

                /* Update layer */
                aLayer
                -> setSize( aParams )
                -> setPosition( aParams )
                ;
            }
        }

    }
    return this;
}



/*
    Load layer from net.layers section
*/
Layer* Net::loadLayerFromConfig
(
    string aLayerId,    /* ID of the layer */
    ParamList* aParams  /* Param list with layer's parameters */
)
{
    Layer* result = NULL;
    if( aParams != NULL )
    {
        if( this -> isOk() )
        {
            auto params = aParams -> getObject( aLayerId );
            if( params )
            {
                result = createLayer( aLayerId );
                loadLayer( result, params );
            }
            else
            {
                getLog() -> warning( "Layer not found" ) -> prm( "id", aLayerId  );
                setCode( "LayerIdNotFound" );
            }
        }
    }
    else
    {
        setCode( "ParamsIsEmptyForLayer" );
    }
    return result;
}



/*
    Remove layers absent in the list
*/
Net* Net::purgeLayers
(
    ParamList* aLayers  /* List from config */
)
{
    /* Build pure list */
    vector <string> purgeList = {};
    layers -> loop
    (
        [ &purgeList, &aLayers ]
        ( void* iLayer )
        {
            auto layerId = (( Layer* ) iLayer ) -> getId();
            if( aLayers -> getObject( layerId ) == NULL )
            {
                /* Layer is absent in the config and must be delete */
                purgeList.push_back( layerId );
            }
            return false;
        }
    );

    /* Delete layers */
    auto c = purgeList.size();
    for( int i = 0; i<c; i++ )
    {
        deleteLayer( purgeList[ i ] );
    }
    return this;
}



Net* Net::setProcessorCount
(
    int a
)
{
    processorCount = a;
    return this;
}



int Net::getProcessorCount()
{
    return processorCount;
}



Net* Net::readNet()
{
    if( host != "" )
    {
        auto rpc = RpcClient::create( getLog(), host, port );
        if( rpc -> call( CMD_GET_NET ) -> isOk() )
        {
            applyConfig( rpc -> getAnswer() );
            nerves -> readFromServer( host, port );
        }
        rpc -> destroy();
    }
    else
    {
        /* Local read */
        /* nerves -> readFromFile(); */
    }
    return this;
}



/*
    Reset forward and backward counts for layers
*/
Net* Net::calcReset()
{
    layers -> loop
    (
        [  this, &result ]
        ( void* iLayer )
        {
            (( Layer* ) iLayer ) -> calcReset();
            return false;
        }
    );
    return this;
}



/*
    Return calc stage for net layers:
        CALC_UNKNOWN
        CALC_NOT_START
        CALC_START
        CALC_COMPLETE
*/
CalcStage getCalcStage
(
    CalcDirection aDirection
)
{
    CalcStage result = CALC_UNKNOWN;
    layers -> loop
    (
        [ this,  &result, &aDirection ]
        ( void* iLayer )
        {
            auto layer = (( Layer* ) iLayer );

            if( result == CALC_UNKNOWN )
            {
                result = aDirection == CALC_BACKWARD ?
                layer -> getBackwardStage( processorCount ) :
                layer -> getForwardStage( processorCount );
            }

            if
            (
                aDirection != CALC_BACKWARD &&
                result != layer -> getForwardStage( processorCount ) ||
                aDirection != CALC_FORWARD &&
                result != layer -> getBackwardStage( processorCount )
            )
            {
                result = CALC_START;
            }

            return result != CALC_START;
        }
    );

    return result == CALC_UNKNOWN ? CALC_COMPLETE : result;
}
