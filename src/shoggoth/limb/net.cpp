#include "net.h"

#include "../io.h"
#include "../shoggoth_consts.h"
#include "../../../../../lib/json/param_list_file.h"
#include "../../../../../lib/core/rnd.h"



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

    /* Set versions */
    version         = aVersion;
    nextVersion     = aVersion;

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
    vector<string> aValues,
    vector<string> aErrors
)
{
    if( aValues.size() > 0 || aErrors.size() > 0 )
    {
        getLog() -> begin( "Write layers" );

        auto io = Io::create( this );
        auto request = io -> getRequest();

        lock();

        /* Build values */
        for( auto id : aValues )
        {
            auto layer = getLayerList() -> getById( id );
            if( layer != NULL )
            {
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
            }
        }

        /* Build errors */
        for( auto id : aErrors )
        {
            auto layer = getLayerList() -> getById( id );
            if( layer != NULL )
            {
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
            }
        }

        unlock();

        io
        -> call( CMD_WRITE_LAYERS )
        -> destroy();

        getLog() -> end();
    }

    return this;
}



/*
    Read value from io
*/
Net* Net::readLayers
(
    vector<string> aValues,
    vector<string> aErrors
)
{
    if( aValues.size() > 0 || aErrors.size() > 0 )
    {
        getLog() -> begin( "Read layers" );

        /* Create IO object and define request */
        auto io = Io::create( this );
        auto request = io -> getRequest();

        /* Build request values */
        request -> setPath( Path{ "values" } ) -> pushVector( aValues );
        request -> setPath( Path{ "errors" } ) -> pushVector( aErrors );

        /* Call server and apply the answer */
        if( io -> call( CMD_READ_LAYERS ) -> isOk() )
        {
            lock();

            /* Loop for values */
            for( auto id : aValues )
            {
                auto layer = getLayerList() -> getById( id );
                if( layer != NULL )
                {
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
                }
            }

            /* Loop for errors */
            for( auto id : aErrors )
            {
                auto layer = getLayerList() -> getById( id );
                if( layer != NULL )
                {
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
                }
            }

            unlock();
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
    Request layers statistics
*/
Net* Net::requestStat
(
    vector<string> aStatValue,    /* Layer list for stat request */
    vector<string> aStatError,    /* Layer list for stat request */
    vector<string> aStatTick      /* Layer list for stat request */
)
{
    if
    (
        aStatValue.size() > 0 ||
        aStatError.size() > 0 ||
        aStatTick.size() > 0
    )
    {
        getLog() -> begin( "Read stat" );

        /* Create IO object and define request */
        auto io = Io::create( this );
        auto request = io -> getRequest();

        request -> setPath( Path{ "value" }) -> pushVector( aStatValue );
        request -> setPath( Path{ "error" }) -> pushVector( aStatError );
        request -> setPath( Path{ "tick" }) -> pushVector( aStatTick );

        /* Call server and apply the answer */
        if( io -> call( CMD_READ_LAYER_STAT ) -> isOk() )
        {
            lock();

            /* Loop for values */
            for( auto id : aStatValue )
            {
                auto layer = getLayerList() -> getById( id );
                if( layer != NULL )
                {
                    char* buffer = NULL;
                    size_t size = 0;

                    io -> getAnswer() -> getData
                    (
                        Path{ "value", layer -> getId() },
                        buffer,
                        size
                    );

                    if( buffer != NULL )
                    {
                        layer
                        -> getChartValues()
                        -> fromBuffer( buffer, size );
                    }
                }
            }

            /* Loop for errors */
            for( auto id : aStatError )
            {
                auto layer = getLayerList() -> getById( id );
                if( layer != NULL )
                {
                    char* buffer = NULL;
                    size_t size = 0;

                    io -> getAnswer() -> getData
                    (
                        Path{ "error", layer -> getId() },
                        buffer,
                        size
                    );

                    if( buffer != NULL )
                    {
                        layer
                        -> getChartErrors()
                        -> fromBuffer( buffer, size );
                    }
                }
            }


            /* Loop for ticks */
            for( auto id : aStatTick )
            {
                auto layer = getLayerList() -> getById( id );
                if( layer != NULL )
                {
                    char* buffer = NULL;
                    size_t size = 0;

                    io -> getAnswer()
                    -> getData
                    (
                        Path{ "tick", layer -> getId() },
                        buffer,
                        size
                    );

                    if( buffer != NULL )
                    {
                        layer
                        -> getChartTick()
                        -> fromBuffer( buffer, size );
                    }
                }
            }

            unlock();

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
            lock();
            weightsExchange
            -> checkClearSignal()
            -> readAnswer( io -> getAnswer() );
            unlock();
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
    return getLastUpdate() != aConfig -> getInt( "lastUpdate", 0 );
}



/*
    Reurn parent net version
*/
string Net::getParentVersion
(
    /* Net ID (not used) */
    string aNetId,
    /* Net version */
    string aNetVersion,
    /* Parent generation (0 - current net) */
    int aParentGeneration
)
{
    auto result = aNetVersion;

    for( int i = 0; i < aParentGeneration; i++ )
    {
        /* Define net files */
        string netFile = getNetConfigFile( result );

        /* Create JSON structure */
        auto json = Json::create() -> fromFile( netFile );

        if( json != NULL )
        {
            auto current = json -> getString( Path{ "version", "parent" }, result );
            if( current != "" )
            {
                result = current;
            }
        }
    }

    return result;
}



/*
    Clone net form parent to child
*/
Net* Net::clone
(
    string aParentNetId,
    string aParentNetVersion,
    string& aChildVersion,
    bool aMutation
)
{
    getLog() -> begin( "Net clone" );

    /* Set current id and version for not specified */
    aParentNetId = aParentNetId == "" ? id : aParentNetId;
    aParentNetVersion = aParentNetVersion == "" ? version : aParentNetVersion;

    /* Set default mutation path */
    Path path = { "pure" };

    /* Define net files */
    string parentNetFile = getNetConfigFile( aParentNetVersion );

    /* Create JSON structure */
    auto json = Json::create() -> fromFile( parentNetFile );

    if( aMutation )
    {
        /* Mutation */
        auto mutations = json -> getParamList() -> getObject( "mutations" );
        if( mutations != NULL )
        {
            /* calculate sum of rnd of all mutation */
            double sumRnd = mutations -> calcSum( Path{ "rnd" } );
            double dice = Rnd::get( 0.0, sumRnd );
            double prevDice = 0.0;

            getLog()
            -> trace( "Select mutation" )
            -> prm( "Rnd sum", sumRnd )
            -> prm( "Dice", dice );

            /* Loop for each mutation */
            mutations -> loop
            (
                [ &json, this, &dice, &prevDice, &path ]
                ( Param* iParam )
                {
                    if( iParam -> isObject() )
                    {
                        /* Processing mutation */
                        auto mutation = iParam -> getObject();
                        if
                        (
                            dice >= prevDice &&
                            dice < prevDice + mutation -> getDouble( "rnd" )
                        )
                        {
                            /* Get path for mutation */
                            path = mutation -> getPath( Path{ "path" });

                            getLog()
                            -> trace( "Mutation" )
                            -> prm( "param", implode( path, "." ));

                            /* Get mutating parameter */
                            auto mutated = json -> getParamList() -> getByName( path );

                            if( mutated != NULL )
                            {
                                switch( mutated -> getType() )
                                {
                                    case KT_DOUBLE:
                                    {
                                        auto mul = mutation -> getDouble( "mul", 1.0 );
                                        auto add = mutation -> getDouble( "add", 0.0 );
                                        auto val = mutated -> getDouble();
                                        auto rMul = Rnd::get( 1.0 / mul, mul );
                                        auto rAdd = Rnd::get( -add, +add );
                                        auto vMax = mutation -> getDouble( "max", val );
                                        auto vMin = mutation -> getDouble( "min", val );

                                        getLog()
                                        -> prm( "val", val )
                                        -> prm( "mul", mul )
                                        -> prm( "add", add )
                                        -> prm( "min", vMin )
                                        -> prm( "max", vMax )
                                        -> prm( "rndmul", rMul )
                                        -> prm( "rndadd", rAdd )
                                        ;

                                        if( mul < 1.0 || add < 0.0 )
                                        {
                                            getLog()
                                            -> warning( "Multiplexor or additive is low" )
                                            -> prm( "path", implode( path, ".") )
                                            -> prm( "mul", mul )
                                            -> prm( "add", add )
                                            ;
                                        }
                                        else
                                        {
                                            mutated -> setDouble
                                            (
                                                min
                                                (
                                                    vMax,
                                                    max( vMin, val * rMul + rAdd )
                                                )
                                            );
                                        }

                                        getLog() -> prm( "result", mutated -> getDouble() );

                                    }
                                    break;
                                    case KT_INT:
                                    {
                                        auto mul = mutation -> getDouble( "mul", 1.0 );
                                        int add = mutation -> getInt( "add", 0 );
                                        auto val = mutated -> getInt();
                                        auto rMul = Rnd::get( 1.0 / mul, mul );
                                        auto rAdd = Rnd::get( -add, +add );
                                        auto vMax = mutation -> getDouble( "max", val );
                                        auto vMin = mutation -> getDouble( "min", val );

                                        getLog()
                                        -> prm( "val", val )
                                        -> prm( "mul", mul )
                                        -> prm( "add", add )
                                        -> prm( "min", vMin )
                                        -> prm( "max", vMax )
                                        -> prm( "rndmul", rMul )
                                        -> prm( "rndadd", rAdd )
                                        ;

                                        if( mul < 1 || add < 0 )
                                        {
                                            getLog()
                                            -> warning( "Multiplexor or additive is low" )
                                            -> prm( "path", implode( path, ".") )
                                            -> prm( "mul", mul )
                                            -> prm( "add", add )
                                            ;
                                        }
                                        else
                                        {
                                            mutated -> setInt
                                            (
                                                min
                                                (
                                                    vMax,
                                                    max( vMin, val * rMul + rAdd )
                                                )
                                            );
                                        }

                                        getLog() -> prm( "result", mutated -> getInt() );

                                    }
                                    break;
                                    default:
                                        getLog()
                                        -> warning( "Mutaded is not a DOUBLE or INT" )
                                        -> prm( "path", implode( path, ".") );
                                    break;
                                }
                            }
                            else
                            {
                                getLog()
                                -> warning( "Mutated key not found" )
                                -> prm( "path", implode( path, ".") );
                            }
                        }
                        prevDice = prevDice + mutation -> getDouble( "rnd" );
                    }
                    return false;
                }
            );
        }
    }

    /* Create children version */
    aChildVersion = Rnd::getUuid() + "-" + implode( path, "." );

    getLog()
    -> info( "Copy net files" )
    -> prm( "from", aParentNetVersion )
    -> prm( "to", aChildVersion );

    /* Write child version like current */
    json
    -> getParamList()
    -> setString( Path{ "id" }, aParentNetId )
    -> setString( Path{ "version", "current" }, aChildVersion )
    -> setString( Path{ "version", "parent" }, aParentNetVersion );

    string childNetFile = getNetConfigFile( aChildVersion );

    /* Save children file */
    if( checkPath( getPath( childNetFile )))
    {
        json -> toFile( childNetFile );
    }

    json -> destroy();

    getLog() -> end();

    return this;
}




/**/
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
        /* Set net version from config */
        setNextVersion( config -> getString( Path{ "version", "current" } ) );

        /* Set net version from config */
        setSeed( config -> getInt( Path{ "seed" } ) );

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
                auto used = ParamList::create();

                /* Build list of using roles from Actions */
                auto actions = iParam
                -> getObject()
                -> getObject( "actions" );

                if( actions != NULL )
                {
                    actions -> loop
                    (
                        [ &used ]
                        ( Param* item )
                        {
                            if( item -> isObject())
                            {
                                item -> getObject() -> loop
                                (
                                    [ &used ]
                                    ( Param* item )
                                    {
                                        used -> pushString( item -> getString() );
                                        return false;
                                    }
                                );
                            }
                            return false;
                        }
                    );

                    if( used -> isIntersect( tasks ))
                    {
                        auto layerId = iParam -> getName();
                        auto layer = createLayer( layerId );
                        loadLayer( layer, iParam -> getObject() );
                        layer -> setStoragePath( storagePath );
                    }
                }

                used -> destroy();
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

    if( version != nextVersion )
    {
        /* Set nextVersion in to version */
        version = nextVersion;

        /* Clear tick */
        getLayerList() -> loop
        (
            []
            ( void* aLayer )
            {
                auto layer = ( Layer* ) aLayer;
                layer -> getChartTick() -> clear();
                return false;
            }
        );
    }

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
    string aSubpath,
    string aId      /* Net id */
)
{
    aId = aId =="" ? id : aId;
    return getStoragePath() + "/" + aId + ( aSubpath == "" ? "" : "/" + aSubpath );
}




/*
    Return net path
*/
string Net::getNetVersionPath
(
    string aSubpath,    /* Subpath */
    string aVersion,    /* Specific version */
    string aId          /* Net id */
)
{
    aVersion = aVersion == "" ? version : aVersion;
    return getNetPath( "ver/" + aVersion  + ( aSubpath == "" ? "" : "/" + aSubpath ));
}



/*
    Return net config
*/
string Net::getNetConfigFile
(
    string aVersion,    /* Specific version */
    string aId          /* Net id */
)
{
    return getNetVersionPath( "net.json", aVersion, aId );
}



/*
    Return log path
*/
string Net::getLogPath
(
    string aSubpath,
    string aVersion,    /* Specific version */
    string aId          /* Net id */
)
{
    return getNetPath
    (
        "log" + ( aSubpath == "" ? "" : "/" + aSubpath ),
        aId
    );
}



/*
    Return mon path
*/
string Net::getMonPath
(
    string aSubpath,
    string aVersion,    /* Specific version */
    string aId          /* Net id */
)
{
    return getNetPath
    (
        "mon" + ( aSubpath == "" ? "" : "/" + aSubpath ),
        aId
    );
}



/*
    Return dump path
*/
string Net::getDumpPath
(
    string aSubpath,
    string aVersion,    /* Specific version */
    string aId          /* Net id */
)
{
    return getNetVersionPath
    (
        "dump" + ( aSubpath == "" ? "" : "/" + aSubpath ),
        aVersion,
        aId
    );
}



/*
    Return path for nerves weights
*/
string Net::getNervesPath
(
    string aSubpath,
    string aVersion,    /* Specific version */
    string aId          /* Net id */
)
{
    return getNetVersionPath
    (
        "nerves" + ( aSubpath == "" ? "" : "/" + aSubpath ),
        aVersion,
        aId
    );
}



/*
    Return weights dump path
*/
string Net::getWeightsPath
(
    string aSubpath,
    string aVersion,    /* Specific version */
    string aId          /* Net id */
)
{
    return getDumpPath
    (
        "weights" + ( aSubpath == "" ? "" : "/" + aSubpath ),
        aVersion,
        aId
    );
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

            /* Apply neuron functions for layer */
            aLayer -> setNeuronFunc( aParams -> getString( "function", "SIGMOID" ));

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
    Limb*   aLimb,      /* Participant */
    bool    aSkip
)
{
    auto configLayers = config -> getObject( "layers" );
    if( configLayers != NULL )
    {
        if( lock( aSkip ))
        {
            if( aLimb -> lock( aSkip ))
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
                aLimb -> unlock();
            }
            unlock();
        }
    }
    return this;
}




Net* Net::syncToLimb
(
    Limb* targetLimb,
    bool aSkip
)
{
    if( targetLimb -> getLastUpdate() != getLastUpdate() )
    {
        /* Rebuild structure layers and nervs */
        copyTo( targetLimb, true, aSkip, false );
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
    /* The application does not have to be a processor */
    if( !tasks -> valueExists( taskToString( TASK_PROC )))
    {
        getLog()
        -> trapOn()
        -> begin( "Synchronize layers with server" );

        /* Create lists of layers */
        vector<string> readValues;
        vector<string> readErrors;
        vector<string> writeValues;
        vector<string> writeErrors;

        vector<string> readStatValue;
        vector<string> readStatError;
        vector<string> readStatTick;

        lock();

        /* */
        getLayerList() -> loop
        (
            [
                this,
                &readValues,
                &readErrors,
                &writeValues,
                &writeErrors,

                &readStatValue,
                &readStatError,
                &readStatTick
            ]
            ( void* aLayer )
            {

                auto layer = ( Layer* ) aLayer;
                auto layerId = layer -> getId();

                /* Values were changed and must be written to the server */
                if
                (
                    find
                    (
                        changedValues.begin(),
                        changedValues.end(),
                        layerId
                    ) != changedValues.end()
                )
                {
                    /* Check application rules for write values of the  layer */
                    if( layer -> checkTasks( tasks, WRITE_VALUES ))
                    {
                        writeValues.push_back( layerId );
                    }
                }
                else
                {
                    /* Check application rules for write values of the layer */
                    if( layer -> checkTasks( tasks, READ_VALUES ))
                    {
                        readValues.push_back( layerId );
                    }
                }

                /* Errors was changed and must be write to server */
                if
                (
                    find
                    (
                        changedErrors.begin(),
                        changedErrors.end(),
                        layerId
                    ) != changedErrors.end()
                )
                {
                    /* Check application rules for write erros of the layer */
                    if( layer -> checkTasks( tasks, WRITE_ERRORS ))
                    {
                        writeErrors.push_back( layerId );
                    }
                }
                else
                {
                    /* Check application rules for read errors of the layer */
                    if( layer -> checkTasks( tasks, READ_ERRORS ))
                    {
                        readErrors.push_back( layerId );
                    }
                }

                /* Check application rules stat requests */
                if( layer -> checkTasks( tasks, READ_STAT_VALUE ))
                {
                    readStatValue.push_back( layerId );
                }
                /* Check application rules stat requests */
                if( layer -> checkTasks( tasks, READ_STAT_ERROR ))
                {
                    readStatError.push_back( layerId );
                }
                /* Check application rules stat requests */
                if( layer -> checkTasks( tasks, READ_STAT_TICK ))
                {
                    readStatTick.push_back( layerId );
                }

                return false;
            }
        );
        changedValues.clear();
        changedErrors.clear();

        unlock();

        /* Exchange with server */
        writeLayers( writeValues, writeErrors );
        readLayers( readValues, readErrors );
        requestWeights();

        requestStat( readStatValue, readStatError, readStatTick );

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

    if( tasksSection != NULL )
    {
        tasks -> clear();

        if( tasksSection -> getObject( taskToString( TASK_PROC )) != NULL )
        {
            addTask( TASK_PROC );
        }
        if( tasksSection -> getObject( taskToString( TASK_TEACHER )) != NULL )
        {
            addTask( TASK_TEACHER );
        }
        if( tasksSection -> getObject( taskToString( TASK_UI )) != NULL )
        {
            addTask( TASK_UI );
        }
    }
    return this;
}



/*
    Add role for this net
*/
Net* Net::addTask
(
    Task a
)
{
    tasks -> pushString( taskToString( a ));
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
    Set net id
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
    Return net version
*/
string Net::getVersion()
{
    return version;
}



/*
    Return parent net version
*/
string Net::getParentVersion()
{
    return config -> getString( Path{ "version", "parent" });
}




WeightsExchange* Net::getWeightsExchange()
{
    return weightsExchange;
}



/*
    Set the next version of the net
*/
Net* Net::setNextVersion
(
    string aValue
)
{
    nextVersion = aValue;
    return this;
}



/*
    Return the next version of the net
*/
string Net::getNextVersion()
{
    return nextVersion;
}



bool Net::isVersionChanged()
{
    return nextVersion != version;
}



/*
    Calculate statistics for layers of the net
*/
Net* Net::stat()
{
    getLayerList() -> loop
    (
        []
        ( void* iLayer )
        {
            (( Layer* ) iLayer ) -> stat();
            return false;
        }
    );
    return this;
}



/*
    Return net random seed
*/
unsigned long long Net::getSeed()
{
    return seed;
}



/*
    Set net random seed
*/
Net* Net::setSeed
(
    unsigned long long a
)
{
    seed = a;
    return this;
}
