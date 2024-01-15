#include "net.h"

#include "../io.h"
#include "../shoggoth_consts.h"
#include "../neuron.h"
#include "../../../../../lib/json/param_list_file.h"
#include "../../../../../lib/json/param_list_log.h"



/*
    Constructor
*/
Net::Net
(
    Application* aApplication,  /* Application object */
    SockManager* aSockManager   /* Socket manager object */
)
:Limb( aApplication -> getLog() )
{
    application = aApplication;
    sockManager = aSockManager;
    application -> getLog() -> trace( "Create net" );

    /* Read actions */
    actions = ParamListFile::create() -> fromJsonFile( "actions.json" );
    tasks = ParamList::create();
}




/*
    Constructor
*/
Net::~Net()
{
    /* Clear layers lists */
    tasks -> destroy();

    getLog() -> trace( "Destroy net" );
}



/*
    Static constructor
*/
Net* Net::create
(
    Application* a,
    SockManager* sockManager
)
{
    return new Net( a, sockManager );
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
Net* Net::writeValues
(
    Layer* aLayer
)
{
    char* buffer = NULL;    /* Buffer pointer */
    size_t size = 0;        /* Size of buffer */

    aLayer -> getValuesBuffer( buffer, size );

    if( buffer != NULL )
    {
        auto io = Io::create( this );
        io -> getRequest()
        -> setString( "idLayer", aLayer -> getId() )
        -> setData( "data", (char*) buffer, size );

        io
        -> call( CMD_WRITE_VALUES )
        -> destroy();

        getLog()
        -> trace( "Write layer values" )
        -> prm( "id", aLayer -> getId() );
    }

    return this;
}



/*
    Read value from io
*/
Net* Net::readValues
(
    Layer* aLayer
)
{
    auto io = Io::create( this );

    io
    -> getRequest()
    -> setString( "idLayer", aLayer -> getId() );

    if( io -> call( CMD_READ_VALUES ) -> isOk() )
    {
        char* buffer = NULL;
        size_t size = 0;

        io -> getAnswer() -> getData( "data", buffer, size );
        aLayer -> valuesFromBuffer( buffer, size );

        getLog()
        -> trace( "Read layer values" )
        -> prm( "id", aLayer -> getId() );
    }

    io -> destroy();

    return this;
}



/*
    Write errors to io
*/
Net* Net::writeErrors
(
    Layer* aLayer
)
{
    char* buffer = NULL;    /* Buffer pointer */
    size_t size = 0;        /* Size of buffer */

    aLayer -> getErrorsBuffer( buffer, size );

    if( buffer != NULL )
    {
        getLog()
        -> begin( "Write layer errors" )
        -> prm( "id", aLayer -> getId() );

        auto io = Io::create( this );

        io -> getRequest()
        -> setString( "idLayer", aLayer -> getId() )
        -> setData( "data", buffer, size );

        io
        -> call( CMD_WRITE_ERRORS )
        -> destroy();

        getLog() -> end();
    }

    return this;
}



/*
    Read value from io
*/
Net* Net::readErrors
(
    Layer* aLayer
)
{
    auto io = Io::create( this );

    io
    -> getRequest()
    -> setString( "idLayer", aLayer -> getId() );

    if( io -> call( CMD_READ_ERRORS ) -> isOk() )
    {
        char* buffer = NULL;
        size_t size = 0;

        io -> getAnswer() -> getData( "data", buffer, size );

        aLayer -> errorsFromBuffer( buffer, size );

        getLog()
        -> trace( "Read layer errors" )
        -> prm( "id", aLayer -> getId() );
    }

    io -> destroy();

    return this;
}



/*
    Read from server
*/
Net* Net::readValuesList
(
    ParamList* aTasks
)
{
    getLayerList() -> loop
    (
        [ this, &aTasks ]
        ( void* aLayer )
        {
            auto iLayer = ( Layer* ) aLayer;
            if
            (
                aTasks == NULL ||
                aTasks
                -> isIntersect
                (
                    iLayer
                    -> getActions()
                    -> getObject( actionToString( READ_VALUES ))
                )
            )
            {
                readValues( iLayer );
            }
            return false;
        }
    );
    return this;
}



/*
    Write to server
*/
Net* Net::writeValuesList
(
    ParamList* aTasks
)
{
    getLayerList() -> loop
    (
        [ this, &aTasks ]
        ( void* aLayer )
        {
            auto iLayer = ( Layer* ) aLayer;

            if
            (
                aTasks == NULL ||
                aTasks -> isIntersect
                (
                    iLayer
                    -> getActions()
                    -> getObject( actionToString( WRITE_VALUES ))
                )
            )
            {
                writeValues( iLayer );
            }
            return false;
        }
    );
    return this;
}



/*
    Read from server
*/
Net* Net::readErrorsList
(
    ParamList* aTasks
)
{
    getLayerList() -> loop
    (
        [ this, &aTasks ]
        ( void* aLayer )
        {
            auto iLayer = ( Layer* ) aLayer;
            if
            (
                aTasks == NULL ||
                aTasks
                -> isIntersect
                (
                    iLayer
                    -> getActions()
                    -> getObject( actionToString( READ_ERRORS ))
                )
            )
            {
                readErrors( iLayer );
            }
            return false;
        }
    );
    return this;
}



/*
    Write to server
*/
Net* Net::writeErrorsList
(
    ParamList* aTasks
)
{
    getLayerList() -> loop
    (
        [ this, &aTasks ]
        ( void* aLayer )
        {
            auto iLayer = ( Layer* ) aLayer;
            if
            (
                aTasks == NULL ||
                aTasks
                -> isIntersect
                (
                    iLayer
                    -> getActions()
                    -> getObject( actionToString( WRITE_ERRORS ))
                )
            )
            {
                writeErrors( iLayer );
            }
            return false;
        }
    );
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
                            auto layer = createLayer( layerId )
                            -> setStoragePath( storagePath );
                            loadLayerFromConfig( layerId, configLayers );
                        }
                        return false;
                    }
                );

                /* Nerves */
                auto jsonNerves = json -> getObject( "nerves" );
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

    getLog() -> end();

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
                auto newSize = Point3i
                (
                    paramsSize -> getInt( 0 ),
                    paramsSize -> getInt( 1 ),
                    paramsSize -> getInt( 2 )
                );

                if( newSize != aLayer -> getSize() )
                {
                    getNerveList() -> removeByLayer( aLayer );
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
    for( int i = 0; i<c; i++ )
    {
        deleteLayer( purgeList[ i ] );
    }
    return this;
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
            auto layers = getLayerList();
            auto nerves = getNerveList();
            actionsList -> loop
            (
                [ this, &layers, &nerves ]
                ( Param* aParam)
                {
                    switch( stringToAction( aParam -> getString()))
                    {
                        case READ_VALUES    :readValuesList( tasks ); break;
                        case WRITE_VALUES   :writeValuesList( tasks ); break;
                        case READ_ERRORS    :readErrorsList( tasks ); break;
                        case WRITE_ERRORS   :writeErrorsList( tasks ); break;
                        case READ_WEIGHTS   :readWeightsList(); break;
                        case WRITE_WEIGHTS  :writeWeightsList(); break;
//                        case SYNC_RESET     :calcReset(); break;
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




/*
TODO for remove
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
*/



/*
    Return socket manager object
*/

SockManager* Net::getSockManager()
{
    return sockManager;
}

