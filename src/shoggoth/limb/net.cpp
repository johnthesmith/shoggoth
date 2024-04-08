#include "net.h"

#include "../io.h"
#include "../shoggoth_consts.h"
#include "../../../../../lib/json/param_list_file.h"



/*
    Constructor
*/
Net::Net
(
    Application*    aApplication,  /* Application object */
    SockManager*    aSockManager,  /* Socket manager object */
    string          aId,            /* The net id */
    string          aVersion        /* The net version */
)
:Limb( aApplication -> getLogManager() )
{
    application = aApplication;
    sockManager = aSockManager;
    id          = aId;
    version     = aVersion;

    application -> getLog() -> trace( "Create net" );

    tasks = ParamList::create();
    config = ParamList::create();
    weightsExchange = WeightsExchange::create();
}




/*
    Constructor
*/
Net::~Net()
{
    /* Weights exchanger destoy */
    weightsExchange -> destroy();

    /* Config object clear and destroy */
    config -> destroy();

    /* Clear layers lists */
    tasks -> destroy();

    getLog() -> trace( "Destroy net" );
}



/*
    Static constructor
*/
Net* Net::create
(
    Application*    aApplication,  /* Application object */
    SockManager*    aSockManager,  /* Socket manager object */
    string          aId,            /* The net id */
    string          aVersion        /* The net version */
)
{
    return new Net( aApplication, aSockManager, aId, aVersion );
}



/*
    Destructor
*/
void Net::destroy()
{
    delete ( this );
}



/*
    Read nerve weights array from Io
*/
Net* Net::readWeights
(
    Nerve* aNerve
)
{
    auto io = Io::create( this );

    io
    -> getRequest()
    -> setString( "idFrom", aNerve -> getParent() -> getId())
    -> setString( "idTo", aNerve -> getChild() -> getId());
    if( io -> call( CMD_READ_WEIGHTS ) -> isOk() )
    {
        char* buffer = NULL;
        size_t size = 0;
        io -> getAnswer() -> getData( "data", buffer, size );
        if( buffer != NULL && size > 0 )
        {
            aNerve -> readFromBuffer( buffer, size );
        }
    }
    io -> destroy();

    return this;
}



/*
    Write the nerve weight array to io
*/
Net* Net::writeWeights
(
    Nerve* aNerve
)
{
    getLog()
    -> begin( "Write weights" )
    -> prm( "parent", aNerve -> getParent() -> getId())
    -> prm( "child", aNerve -> getChild() -> getId());

    auto io = Io::create( this );

    io
    -> getRequest()
    -> setString( "idFrom", aNerve -> getParent() -> getId() )
    -> setString( "idTo", aNerve -> getChild() -> getId() )
    -> setData
    (
        "data",
        ( char* ) aNerve -> getWeights(),
        sizeof(double) * aNerve -> getWeightsCount()
    );

    io
    -> call( CMD_WRITE_WEIGHTS )
    -> destroy();

    getLog() -> end();

    return this;
}



/*
    Write value to io
*/
Net* Net::writeLayers
(
    LayerList* aValues,
    LayerList* aErrors
)
{
    if( aValues -> getCount() > 0 || aErrors -> getCount() > 0 )
    {
        getLog() -> begin( "Write layers" );

        auto io = Io::create( this );
        auto request = io -> getRequest();

        /* Build values */
        aValues -> loop
        (
            [ &request ]
            ( void* aLayer)
            {
                auto layer = (Layer*) aLayer;

                char* buffer = NULL;    /* Buffer pointer */
                size_t size = 0;        /* Size of buffer */

                layer -> getValuesBuffer( buffer, size );

                if( buffer != NULL )
                {
                    request
                    -> setPath( Path{ "values" })
                    -> setData
                    (
                        layer -> getId(),
                        (char*) buffer,
                        size
                    );
                }
                return false;
            }
        );

        /* Build errors */
        aErrors -> loop
        (
            [ &request ]
            ( void* aLayer)
            {
                auto layer = (Layer*) aLayer;

                char* buffer = NULL;    /* Buffer pointer */
                size_t size = 0;        /* Size of buffer */

                layer -> getErrorsBuffer( buffer, size );

                if( buffer != NULL )
                {
                    request
                    -> setPath( Path{ "errors" })
                    -> setData
                    (
                        layer -> getId(),
                        (char*) buffer,
                        size
                    );
                }
                return false;
            }
        );

        io
        -> call( CMD_WRITE_LAYERS )
        -> destroy();

        getLog() -> end();
    }

    return this;
}



/*
    Request weights for neurons
*/
Net* Net::requestWeights()
{
    if( weightsExchange -> needRequest() )
    {
        getLog() -> begin( "Request weights" );

        /* Create IO object and define request */
        auto io = Io::create( this );
        auto request = io -> getRequest();
        weightsExchange -> prepareRequest( request );

        /* Call server and apply the answer */
        if( io -> call( CMD_REQUEST_WEIGHTS ) -> isOk() )
        {
            weightsExchange
            -> checkClearSignal()
            -> readAnswer( io -> getAnswer() );
        }
        else
        {
            /* Call error */
        }

        io -> destroy();

        getLog() -> end();
    }
    return this;
}


/*
    Read value from io
*/
Net* Net::readLayers
(
    LayerList* aValues,
    LayerList* aErrors
)
{
    if( aValues -> getCount() > 0 || aErrors -> getCount() > 0 )
    {
        getLog() -> begin( "Read layers" );

        /* Create IO object and define request */
        auto io = Io::create( this );
        auto request = io -> getRequest();

        /* Build request values */
        aValues -> loop
        (
            [ &request ]
            ( void* aLayer)
            {
                auto layer = (Layer*) aLayer;

                request
                -> setPath( Path{ "values" })
                -> pushString( layer -> getId());

                return false;
            }
        );

        /* Build request errors */
        aErrors -> loop
        (
            [ &request ]
            ( void* aLayer)
            {
                auto layer = (Layer*) aLayer;

                request
                -> setPath( Path{ "errors" })
                -> pushString( layer -> getId());

                return false;
            }
        );

        /* Call server and apply the answer */
        if( io -> call( CMD_READ_LAYERS ) -> isOk() )
        {
            /* Loop for values */
            aValues -> loop
            (
                [ &io ]
                ( void* aLayer)
                {
                    auto layer = (Layer*) aLayer;

                    char* buffer = NULL;
                    size_t size = 0;

                    io -> getAnswer()
                    -> getData
                    (
                        Path{ "values", layer -> getId() },
                        buffer,
                        size
                    );

                    if( buffer != NULL )
                    {
                        layer -> setValuesFromBuffer( buffer, size );
                    }
                    return false;
                }
            );

            /* Loop for errors */
            aErrors -> loop
            (
                [ &io ]
                ( void* aLayer)
                {
                    auto layer = (Layer*) aLayer;

                    char* buffer = NULL;
                    size_t size = 0;


                    io -> getAnswer()
                    -> getData
                    (
                        Path{ "errors", layer -> getId() },
                        buffer,
                        size
                    );

                    if( buffer != NULL )
                    {
                        layer -> errorsFromBuffer( buffer, size );
                    }
                    return false;
                }
            );
        }
        else
        {
            /* Call error */
        }

        io -> destroy();

        getLog() -> end();
    }
    return this;
}



/*
    Read from server
*/
Net* Net::readWeightsList()
{
    getNerveList() -> loop
    (
        [ this ]
        ( void* aNerve )
        {
            readWeights( ( Nerve* ) aNerve );
            return false;
        }
    );
    return this;
}



/*
    Write to server
*/
Net* Net::writeWeightsList()
{
    getNerveList() -> loop
    (
        [ this ]
        ( void* aNerve )
        {
            writeWeights( ( Nerve* ) aNerve );
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




Net* Net::readNet
(
    ParamList* aAnswer
)
{
    getLog() -> begin( "Read net config" );

    /* Read net */
    Io::create( this, aAnswer )
    -> call( CMD_READ_NET )
    -> destroy();

    getLog() -> end();

    return this;
}



bool Net::isConfigUpdate
(
    ParamList* aConfig
)
{
    return getLastUpdate() < aConfig -> getInt( "lastUpdate", 0 );
}



Net* Net::applyNet
(
    ParamList* aConfig
)
{
    config -> copyFrom( aConfig );

    buildTasks();

    auto configLayers = config -> getObject( "layers" );

    if( configLayers != NULL )
    {
        /* Remove layers absents in the use list */
        purgeLayers( configLayers );

        /* Create layers */
        configLayers -> loop
        (
            [ this ]
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
                    loadLayer( layer, iParam -> getObject() );
                    layer -> setStoragePath( storagePath );
                }
                return false;
            }
        );

        /* Nerves */
        auto jsonNerves = config -> getObject( "nerves" );
        if( jsonNerves != NULL )
        {
            auto layers = getLayerList();
            auto nerves = getNerveList();
            jsonNerves -> loop
            (
                [ this, &layers, &nerves ]
                ( Param* aItem )
                {
                    /* Check the json layer */
                    if( aItem -> getType() == KT_OBJECT )
                    {
                        auto jsonNerve      = aItem -> getObject();
                        auto idFrom         = jsonNerve -> getString( "idFrom" );
                        auto idTo           = jsonNerve -> getString( "idTo" );
                        auto bindType       = bindTypeFromString( jsonNerve -> getString( "bindType" ));
                        auto nerveType      = Nerve::nerveTypeFromString( jsonNerve -> getString( "nerveType" ));
                        auto nerveDelete    = jsonNerve -> getBool( "delete" );

                        /* Find the layers */
                        auto from = layers -> getById( idFrom );
                        auto to = layers -> getById( idTo );

                        if( from != NULL && to != NULL )
                        {
                            auto nerve = nerves -> find
                            (
                                idFrom,
                                idTo,
                                bindType
                            );
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
                                deleteNerve( nerve );
                                nerve = NULL;
                            }

                            if( nerve == NULL && !nerveDelete )
                            {
                                createNerve
                                (
                                    from,
                                    to,
                                    nerveType,
                                    bindType
                                )
                                -> setMinWeight
                                (
                                    jsonNerve -> getDouble( "minWeight" , 0 )
                                )
                                -> setMaxWeight
                                (
                                    jsonNerve -> getDouble( "maxWeight", 0 )
                                )
                                ;
                            }
                        }
                        else
                        {
                            getLog()
                            -> info( "Layers not found for nerve" )
                            -> prm( "idFrom", idFrom )
                            -> prm( "idTo", idTo )
                            ;
                        }
                    }
                    return false;
                }
            );
        }
    }

    /* Update last update net moment */
    setLastUpdate( aConfig -> getInt( "lastUpdate", 0 ));

    return this;
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



/******************************************************************************
    Pathes
*/



/*
    Return net path
*/
string Net::getNetPath
(
    string aSubpath
)
{
    return getStoragePath() + "/" + id + ( aSubpath == "" ? "" : "/" + aSubpath );
}




/*
    Return net path
*/
string Net::getNetVersionPath
(
    string aSubpath
)
{
    return getNetPath( version  + ( aSubpath == "" ? "" : "/" + aSubpath ));
}



/*
    Return net config
*/
string Net::getNetConfigFile()
{
    return getNetPath( "net.json" );
}



/*
    Return log path
*/
string Net::getLogPath
(
    string aSubpath
)
{
    return getNetVersionPath( "log" + ( aSubpath == "" ? "" : "/" + aSubpath ));
}



/*
    Return mon path
*/
string Net::getMonPath
(
    string aSubpath
)
{
    return getNetVersionPath( "mon" + ( aSubpath == "" ? "" : "/" + aSubpath ));
}



/*
    Return dump path
*/
string Net::getDumpPath
(
    string aSubpath
)
{
    return getNetVersionPath( "dump" + ( aSubpath == "" ? "" : "/" + aSubpath ));
}



/*
    Return path for nerves weights
*/
string Net::getNervesPath
(
    string aSubpath
)
{
    return getNetVersionPath( "nerves" + ( aSubpath == "" ? "" : "/" + aSubpath ));
}



/*
    Return weights dump path
*/
string Net::getWeightsPath
(
    string aSubpath
)
{
    return getDumpPath( "weights" + ( aSubpath == "" ? "" : "/" + aSubpath ));
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
    auto layers = getLayerList();
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
                auto newCount =
                paramsSize -> getInt( 0 ) *
                paramsSize -> getInt( 1 ) *
                paramsSize -> getInt( 2 );

                /* Remove nerves for size changed layer */
                if( newCount != aLayer -> getCount() )
                {
                    getNerveList() -> removeByLayer( aLayer );
                }
                /* Update layer */
                aLayer -> setCount( newCount );
            }
        }
    }
    return this;
}



/*
    Remove layers absent in the list
*/
Net* Net::purgeLayers
(
    ParamList* aLayers  /* List from config */
)
{
    lock();
    /* Build pure list */
    vector <string> purgeList = {};
    getLayerList() -> loop
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
    for( long unsigned int i = 0; i<c; i++ )
    {
        deleteLayer( purgeList[ i ] );
    }

    unlock();
    return this;
}



/*
    Return socket manager object
*/

SockManager* Net::getSockManager()
{
    return sockManager;
}




/*
    Return config of the net
*/
ParamList* Net::getConfig()
{
    return config;
}



/*
    Load selected weights to this net from the limb argument
*/
Net* Net::loadWeightsFrom
(
    Limb*   aLimb       /* sorce */
)
{
    lock();
    aLimb -> lock();

    weightsExchange -> loop
    (
        [ &aLimb ]
        (
            Param* iParam
        )
        {
            if( iParam -> isObject() )
            {
                auto params = iParam -> getObject();
                auto layerId = params -> getString( "layerId" );
                auto neuronIndex = params -> getInt( "neuronIndex" );

                auto layer = aLimb -> getLayerList() -> getById( layerId );
                if( layer != NULL )
                {
                    /* Loop for the arguments limb nerves */
                    aLimb -> getNerveList() -> loop
                    (
                        [ &layer, &params, &neuronIndex ]
                        ( void* item )
                        {
                            auto nerve = (Nerve*) item;

                            /* Parents processing */
                            if( nerve -> getParent() == layer )
                            {
                                char* buffer = NULL;
                                size_t  size = 0;
                                nerve -> extractChildWeightsBuffer
                                (
                                    neuronIndex,
                                    buffer,
                                    size
                                );

                                /* collect child weights */
                                params -> setData
                                (
                                    Path
                                    {
                                        "children",
                                        nerve -> getChild() -> getId()
                                    },
                                    buffer,
                                    size,
                                    false
                                );
                            }

                            /* Children processing */
                            if( nerve -> getChild() == layer )
                            {
                                char* buffer = NULL;
                                size_t  size = 0;
                                nerve -> extractParentsWeightsBuffer
                                (
                                    neuronIndex,
                                    buffer,
                                    size
                                );
                                /* collect child weights */
                                params -> setData
                                (
                                    Path
                                    {
                                        "parents",
                                        nerve -> getParent() -> getId()
                                    },
                                    buffer,
                                    size,
                                    false
                                );
                            }
                            return false;
                        }
                    );
                }
            }
            return false;
        }
    );

    aLimb -> unlock();
    unlock();

    return this;
}



/*
    Swap layers between net and other participants for actions
*/
Net* Net::swapValuesAndErrors
(
    Actions aActions,   /* Action for participant */
    Task    aTask,      /* Task (role) of participant */
    Limb*   aLimb       /* Participant */
)
{
    auto configLayers = config -> getObject( "layers" );
    if( configLayers != NULL )
    {
        /* Loop for layers configuration */
        configLayers -> loop
        (
            [ this, &aLimb, &aActions, &aTask ]
            (
                Param* iParam
            )
            {
                /* For each of action */
                for( auto iAction : aActions )
                {
                    /* Find action */
                    auto action = iParam -> getObject()
                    -> getObject( Path{ "actions", actionToString( iAction ) });
                    /* If action exists in a layer... */
                    if
                    (
                        action != NULL &&
                        action -> contains( taskToString( aTask ))
                    )
                    {
                        /* ... finds layer by id in net and participant ... */
                        auto id = iParam -> getName();
                        auto netLayer = getLayerList() -> getById( id );
                        auto participantLayer = aLimb -> getLayerList() -> getById( id );
                        /* ... if both layers fonded ... */
                        if( netLayer != NULL && participantLayer != NULL )
                        {
                            /* ... then swap values and errors */
                            switch( iAction )
                            {
                                default: break;
                                case READ_VALUES:
                                    participantLayer -> copyValuesFrom( netLayer );
                                break;
                                case WRITE_VALUES:
                                    netLayer -> copyValuesFrom( participantLayer );
                                    addChangedValues( netLayer );
                                break;
                                case READ_ERRORS:
                                    participantLayer -> copyErrorsFrom( netLayer );
                                break;
                                case WRITE_ERRORS:
                                    netLayer -> copyErrorsFrom( participantLayer );
                                    addChangedErrors( netLayer );
                                break;
                            }
                        }
                    }
                }
                return false;
            }
        );
    }
    return this;
}




Net* Net::syncToLimb
(
    Limb* targetLimb
)
{
    if( targetLimb -> getLastUpdate() != getLastUpdate() )
    {
        /* Rebuild structure layers and nervs */
        copyTo( targetLimb, true );
        /* Apply specific config */
        targetLimb -> onAfterReconfig( getConfig() );
        /* Commit last  update */
        targetLimb -> setLastUpdate( getLastUpdate() );
    }
    return this;
}



/*
    Synchronaize with server
    For the modified layer, write to the server
    else read from the server.
*/
Net* Net::syncWithServer()
{
    auto tasksSection = getApplication()
    -> getConfig()
    -> getObject( "tasks" );

    /* The application does not have to be a processor */
    if
    (
        tasksSection != NULL &&
        tasksSection -> getObject( taskToString( TASK_PROC )) == NULL
    )
    {
        getLog()
        -> trapOn()
        -> begin( "Synchronize layers with server" );

        /* Create lists of layers */
        auto readValues = LayerList::create( this );
        auto readErrors = LayerList::create( this );
        auto writeValues = LayerList::create( this );
        auto writeErrors = LayerList::create( this );

        lock();

        /* */
        getLayerList() -> loop
        (
            [
                this,
                &readValues,
                &readErrors,
                &writeValues,
                &writeErrors
            ]
            ( void* aLayer )
            {

                auto layer = ( Layer* ) aLayer;

                /* Values were changed and must be written to the server */
                if
                (
                    find
                    (
                        changedValues.begin(),
                        changedValues.end(),
                        layer -> getId()
                    ) != changedValues.end()
                )
                {
                    /* Check application rules for write values of the  layer */
                    if( layer -> checkTasks( tasks, WRITE_VALUES ))
                    {
                        writeValues -> push( layer );
                    }
                }
                else
                {
                    /* Check application rules for write values of the layer */
                    if( layer -> checkTasks( tasks, READ_VALUES ))
                    {
                        readValues -> push( layer );
                    }
                }

                /* Errors was changed and must be write to server */
                if
                (
                    find
                    (
                        changedErrors.begin(),
                        changedErrors.end(),
                        layer -> getId()
                    ) != changedErrors.end()
                )
                {
                    /* Check application rules for write erros of the layer */
                    if( layer -> checkTasks( tasks, WRITE_ERRORS ))
                    {
                        writeErrors -> push( layer );
                    }
                }
                else
                {
                    /* Check application rules for read errors of the layer */
                    if( layer -> checkTasks( tasks, READ_ERRORS ))
                    {
                        readErrors -> push( layer );
                    }
                }
                return false;
            }
        );
        changedValues.clear();
        changedErrors.clear();

        /* Exchange with server */
        writeLayers( writeValues, writeErrors );
        readLayers( readValues, readErrors );
        requestWeights();

        /* Unlock Net */
        unlock();

        /* Destroy lists */
        readValues -> destroy();
        readErrors -> destroy();
        writeValues -> destroy();
        writeErrors -> destroy();

        getLog()
        -> end()
        -> trapOff();
    }

    return this;
}



Net* Net::addChangedValues
(
    Layer* aLayer
)
{
    changedValues.push_back( aLayer -> getId() );
    return this;
}



Net* Net::addChangedErrors
(
    Layer* aLayer
)
{
    changedErrors.push_back( aLayer -> getId() );
    return this;
}



/*
    Create roles strung of the process
*/
Net* Net::buildTasks()
{
    auto tasksSection = getApplication()
    -> getConfig()
    -> getObject( "tasks" );

    tasks -> clear();

    if( tasksSection != NULL )
    {
        if( tasksSection -> getObject( taskToString( TASK_PROC )) != NULL )
        {
            tasks -> pushString( taskToString( TASK_PROC ));
        }
        if( tasksSection -> getObject( taskToString( TASK_TEACHER )) != NULL )
        {
            tasks -> pushString( taskToString( TASK_TEACHER ));
        }
        if( tasksSection -> getObject( taskToString( TASK_UI )) != NULL )
        {
            tasks -> pushString( taskToString( TASK_UI ));
        }
    }
    return this;
}



/*
    Return net id
*/
string Net::getId()
{
    return id;
}



/*
    Return net version
*/
string Net::getVersion()
{
    return version;
}



WeightsExchange* Net::getWeightsExchange()
{
    return weightsExchange;
}
