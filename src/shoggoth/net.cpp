#include "net.h"
#include "io.h"
#include "shoggoth_consts.h"
#include "neuron.h"
#include "../json/param_list_file.h"
#include "../json/param_list_log.h"


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

    /* Create layers and nerves structures */
    layers = LayerList::create();
    nerves = NerveList::create();

    /* Read actions */
    actions = ParamListFile::create() -> fromJsonFile( "actions.json" );
    tasks = ParamList::create();

    forwardList = LayerList::create();
    backwardList = LayerList::create();
}




/*
    Constructor
*/
Net::~Net()
{
    /* Clear layers lists */
    clear();

    forwardList -> destroy();
    backwardList -> destroy();

    tasks -> destroy();
    nerves -> destroy();
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
    Build layers calculation sequence list
    Method fills the next lists:
        forwardList
        backwardList
*/
Net* Net::precalc()
{
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



/*
    Calculate all layers
*/
Net* Net::calc()
{
    if( getCalcTick() )
    {
        if( getCalcStage( CALC_ALL ) == CALC_COMPLETE )
        {
            /* Reset calc stages for all layers */
            calcReset();
            /* Event */
            // event( THINKING_BEGIN );
        }

        Layer* layer = NULL;

        if( layers -> getCount() > 0 )
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
                    layer -> calcValue( idThread, threadCount );
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

                    layer -> learning
                    (
                        errorNormalize,
                        learningSpeed,
                        wakeupWeight,
                        idThread,
                        threadCount
                    );

                    /* Set local sync for local works */
                    layer -> calcCompleteBackward();
                    /* --- thread end --- */
                }

                if( layer -> getBackwardStage( threadCount ) == CALC_COMPLETE )
                {
                    calcLayerIndex++;
                }

                if( getCalcStage( CALC_BACKWARD ) == CALC_COMPLETE )
                {
                    // event( LEARNING_END );
                }
            }

            /* Dump sync to log */
            syncToLog( layer );
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
            -> getBackwardStage( threadCount ) == CALC_COMPLETE;
            return !result;
        }
    );
    children -> destroy();
    return result;
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



Net* Net::readNet()
{
    getLog() -> begin( "Read net" );

    /* Read net */
    ParamList* json = NULL;
    auto io = Io::create( this );
    if( io -> call( CMD_READ_NET ) -> isOk() )
    {
        json = io -> getAnswer();
        /* Apply net */
        if( json != NULL )
        {

            buildPtuAndTasks();

            auto configLayers = json -> getObject( "layers" );

            if( configLayers != NULL )
            {
                /* Remove layers absents in the use list */
                purgeLayers( configLayers );

                /* Create layers */
                configLayers -> loop
                (
                    [ this, &configLayers ]
                    (
                        Param* iParam
                    )
                    {
                        /* Create layer if its in used list */
                        auto used = iParam
                        -> getObject()
                        -> getObject( "used" );

                        if( used != NULL && used -> isIntersect( tasks ))
                        {
                            auto layerId = iParam -> getName();
                            auto layer = createLayer( layerId );
                            loadLayerFromConfig( layerId, configLayers );
                        }
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
                                    getLog()
                                    -> info( "Layers not found for nerve" )
                                    -> prm( "id", idNerve );
                                }
                            }
                            return false;
                        }
                    );
                }
            }
            event( READ_NET );
        }
    }
    io -> destroy();

    /* Build layers calculation sequence list */
    precalc();

    getLog() -> end();

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
        auto ui = getApplication()
        -> getConfig()
        -> getBool( Path{ "tasks", taskToString( TASK_UI ), "enabled" });

        /* Create new layer object */
        result = Layer::create
        (
            this,
            a,
            true,
            true,
            ui,
            ui,
            ui
        );
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
    ParamList*  aParams     /* Layer configuration */
)
{
    if( this -> isOk() )
    {
        /* Set ID from params */
        if
        (
            aLayer-> getId()
            != aParams -> getString( "id", aLayer -> getId() )
        )
        {
            setCode( "InvalidLayerID" );
        }
        else
        {
            /* Set event actions */
            aLayer
            -> getActions()
            -> copyFrom( aParams -> getObject( "actions" ) );
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
                -> setDrawSize( aParams )
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



Net* Net::setThreadCount
(
    int a
)
{
    threadCount = a;
    return this;
}



int Net::getThreadCount()
{
    return threadCount;
}




/*
    Reset forward and backward counts for layers
*/
Net* Net::calcReset()
{
    layers -> loop
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



/*
    Return calc stage for net layers:
        CALC_UNKNOWN
        CALC_NOT_START
        CALC_START
        CALC_COMPLETE
*/
CalcStage Net::getCalcStage
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
                layer -> getBackwardStage( threadCount ) :
                layer -> getForwardStage( threadCount );
            }

            if
            (
                aDirection != CALC_BACKWARD &&
                result != layer -> getForwardStage( threadCount ) ||
                aDirection != CALC_FORWARD &&
                result != layer -> getBackwardStage( threadCount )
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
    Set id for net exemplar
*/
Net* Net::setId
(
    string aValue
)
{
    id = aValue;
    return this;
}



/*
    Create roles strung of the process in Ptu format
    S - process works with server otherwice with local net
    U - process has a UI interface
    P - process uses as processor
    T - process uses as teacher
*/
Net* Net::buildPtuAndTasks()
{
    auto tasksSection = getApplication()
    -> getConfig()
    -> getObject( "tasks" );

    ptu = "***";
    tasks -> clear();

    if( tasksSection != NULL )
    {
        if( tasksSection -> getObject( taskToString( TASK_PROC )) != NULL )
        {
            ptu[ 0 ] = 'P';
            tasks -> pushString( taskToString( TASK_PROC ));
        }
        if( tasksSection -> getObject( taskToString( TASK_TEACHER )) != NULL )
        {
            ptu[ 1 ] = 'T';
            tasks -> pushString( taskToString( TASK_TEACHER ));
        }
        if( tasksSection -> getObject( taskToString( TASK_UI )) != NULL )
        {
            ptu[ 2 ] = 'U';
            tasks -> pushString( taskToString( TASK_UI ));
        }
    }
    return this;
}



/*
    Main event handler
*/
Net* Net::event
(
    Event aEvent
)
{
    getLog()
    -> trace( "Neural net event" )
    -> begin( "Neural net event" )
    -> prm( "name", eventToString( aEvent ))
    -> prm( "ptu", ptu )
    -> lineEnd()
    ;

    if( actions -> isOk() )
    {
        auto actionsList = actions -> getObject
        (
            Path{ ptu, eventToString( aEvent ) }
        );
        if( actionsList != NULL )
        {
            actionsList -> loop
            (
                [ this ]
                ( Param* aParam)
                {
                    switch( stringToAction( aParam -> getString()))
                    {
                        case READ_VALUES    :layers -> readValues( tasks ); break;
                        case WRITE_VALUES   :layers -> writeValues( tasks ); break;
                        case READ_ERRORS    :layers -> readErrors( tasks ); break;
                        case WRITE_ERRORS   :layers -> writeErrors( tasks ); break;
                        case READ_WEIGHTS   :nerves -> readWeights(); break;
                        case WRITE_WEIGHTS  :nerves -> writeWeights(); break;
                        case SYNC_RESET     :calcReset(); break;
                    }
                    return false;
                }
            );
        }
    }
    else
    {
        getLog()
        -> warning( "Actions Is Empty with error" )
        -> prm( "code",  actions -> getCode());
    }

    getLog()
    -> end()
    -> lineEnd()
    ;

    return this;
}



Net* Net::syncToLog
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



bool Net::isNextLoop()
{
    auto result = false;
    auto currentMoment = now();

    if( lastLoopMoment + loopTimeoutMcs < currentMoment )
    {
        result = true;
        lastLoopMoment = currentMoment;
    }
    return result;
}




bool Net::nerveWeightLoop
(
    NeuronList* a,
    IndexWeightLambda aCallback
)
{
    a -> loop
    (
        [ &a, this, &aCallback ]
        (
            Neuron* fromNeuron
        )
        {
            a -> loop
            (
                [ &fromNeuron, this, &aCallback ]
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



/*
    Set calculation stop flag
*/
Net* Net::setCalcTick()
{
    calcTick = true;
    return this;
}



/*
    Return calculation stop flag
*/
bool Net::getCalcTick()
{
    bool result = !calcDebug || calcTick;
    calcTick = false;
    return result;
}



/*
    Set calculation debug mode flag
*/
Net* Net::setCalcDebug
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
bool Net::getCalcDebug()
{
    return calcDebug;
}



/*
    Return the current calculated layer index
*/
int Net::getCalcLayerIndex()
{
    return calcLayerIndex;
}



/*
    Return pointer of forward calulation list
*/

LayerList* Net::getForwardList()
{
    return forwardList;
}



/*
    Return pointer of backward calulation list
*/

LayerList* Net::getBackwardList()
{
    return backwardList;
}
