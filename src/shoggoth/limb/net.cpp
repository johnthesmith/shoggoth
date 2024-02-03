#include "net.h"

#include "../io.h"
#include "../shoggoth_consts.h"
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
:Limb( aApplication -> getLogManager() )
{
    application = aApplication;
    sockManager = aSockManager;
    application -> getLog() -> trace( "Create net" );

    /* Read actions */
    actions = ParamListFile::create() -> fromJsonFile( "actions.json" );
    tasks = ParamList::create();
    config = ParamList::create();
}




/*
    Constructor
*/
Net::~Net()
{
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
        aLayer -> setValuesFromBuffer( buffer, size );

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
    auto io = Io::create( this );
    if
    (
        io -> call( CMD_READ_NET ) -> isOk() &&
        io -> getAnswer() != NULL
    )
    {
        config -> copyFrom( io -> getAnswer() );

        buildPtuAndTasks();

        auto configLayers = config -> getObject( "layers" );

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
                            auto bindType       = Nerve::bindTypeFromString( jsonNerve -> getString( "bindType" ));
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
                                    -> allocate()
                                    -> fill
                                    (
                                        jsonNerve -> getDouble( "minWeight", 1.0 ),
                                        jsonNerve -> getDouble( "maxWeight", 1.0 )
                                    );
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

        /*
            Next age for net
            Other limbs can reload values and errors
        */
        incAge();
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
    for( int i = 0; i<c; i++ )
    {
        deleteLayer( purgeList[ i ] );
    }

    unlock();
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
    if( targetLimb -> getAge() != getAge() )
    {
        copyTo( targetLimb, true );
        targetLimb -> ageFrom( this );
    }
    return this;
}



/*
    Synchronaize with server
    For the modified layer, write to the server
    else read from the server.
*/
Net* Net::syncToServer()
{
    auto tasksSection = getApplication()
    -> getConfig()
    -> getObject( "tasks" );

    if( tasksSection -> getObject( taskToString( TASK_PROC )) == NULL )
    {
        getLog() -> begin( "Synchronize layers with server" );

        lock();
        getLayerList() -> loop
        (
            [ this ]
            ( void* aLayer )
            {
                auto layer = ( Layer* ) aLayer;
                /* Vslurd */
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
                    writeValues( layer );
                }
                else
                {
                    readValues( layer );
                }

                /* Errors */
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
                    writeErrors( layer );
                }
                else
                {
                    readErrors( layer );
                }
                return false;
            }
        );
        changedValues.clear();
        changedErrors.clear();
        unlock();

        getLog() -> end();
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
