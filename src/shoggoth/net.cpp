#include "net.h"

#include "io.h"
#include "../json/param_list_file.h"


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
}




/*
    Constructor
*/
Net::~Net()
{
    clear();

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
    Calculate all layers
*/
Net* Net::calc()
{
    if( getCalcStage( CALC_ALL ) == CALC_COMPLETE )
    {
        /* Reset calc stages for all layers */
        calcReset();
        /* Event */
        event( THINKING_BEGIN );
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

            if( getCalcStage( CALC_FORWARD ) == CALC_COMPLETE )
            {
                event( THINKING_END );
            }
        }
        else
        {
            /*
                All forward calculation is complete
                Begin the learning process
            */

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

            if( getCalcStage( CALC_BACKWARD ) == CALC_COMPLETE )
            {
                event( LEARNING_END );
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
    /* Read net */
    ParamList* json = NULL;

    auto io = Io::create( this );
    if( io -> call( CMD_READ_NET ) -> isOk() )
    {
        json = io -> getAnswer();
        /* Apply net */
        if( json != NULL )
        {
            buildSupt( json );

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
                        auto uses = iParam
                        -> getObject()
                        -> getObject( "uses" );
                        if( uses -> contains( id ) )
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
                                    getLog() -> warning( "Layers not found for nerve" ) -> prm( "id", idNerve );
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
        -> getString( "role" ) == "ui";

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
    ParamList*  aParams
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
    Create roles strung of the process in SUPT format
    S - process works with server otherwice with local net
    U - process has a UI interface
    P - process uses as processor
    T - process uses as teacher
*/
Net* Net::buildSupt
(
    ParamList* aConfig
)
{
    auto tasks = aConfig -> getObject( "tasks" );
    supt = tasks == NULL ? "****" :
    (
        aConfig -> getString( Path { "io", "source" } ) == "LOCAL" ? "*" :
        (
            (string) "S" +
            ( tasks -> getObject( "UI" ) == NULL ? "*" : "U" ) +
            ( tasks -> getObject( "PROC" ) == NULL ? "*_" : "P" ) +
            ( tasks -> getObject( "TEACHER" ) == NULL ? "*" : "T" )
        )
    );
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
    if( actions != NULL )
    {
        auto actionsList = actions -> getObject
        (
            Path{ supt, eventToString( aEvent ) }
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
                        case READ_VALUES    :layers -> readValues(); break;
                        case WRITE_VALUES   :layers -> writeValues(); break;
                        case READ_ERRORS    :layers -> readErrors(); break;
                        case WRITE_ERRORS   :layers -> writeErrors(); break;
                        case READ_WEIGHTS   :nerves -> readWeights(); break;
                        case WRITE_WEIGHTS  :nerves -> writeWeights(); break;
                        case SYNC_RESET     :calcReset(); break;
                    }
                    return false;
                }
            );
        }
    }
    return this;
}



/*
    Return Action const by string value
*/
Action Net::stringToAction
(
    string aValue
)
{
    if( aValue == "READ_VALUES" )   return READ_VALUES;
    if( aValue == "WRITE_VALUES" )  return WRITE_VALUES;
    if( aValue == "READ_ERRORS" )   return READ_ERRORS;
    if( aValue == "WRITE_ERRORS" )  return WRITE_ERRORS;
    if( aValue == "READ_WEIGHTS" )  return READ_WEIGHTS;
    if( aValue == "WRITE_WEIGHTS" ) return WRITE_WEIGHTS;
    if( aValue == "SYNC_RESET" )    return SYNC_RESET;
    return ACTION_UNKNOWN;
}



/*
    Return event string by Event
*/
string Net::eventToString
(
    Event aValue /* Event enum */
)
{
    switch( aValue )
    {
        case LOOP_BEGIN     : return "LOOP_BEGIN";
        case THINKING_BEGIN : return "THINKING_BEGIN";
        case THINKING_END   : return "THINKING_END";
        case LEARNING_END   : return "LEARNING_END";
        case READ_NET       : return "READ_NET";
        case TICHING_END    : return "TICHING_END";
    }
    return "EVENT_UNKNOWN";
}
