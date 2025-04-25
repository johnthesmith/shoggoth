#include "net.h"

#include "../io.h"
#include "../shoggoth_consts.h"
#include "../../../../../lib/json/param_list_file.h"
#include "../../../../../lib/graph/param_point.h"



/*
    Constructor
*/
Net::Net
(
    /* Application object */
    Application*    aApplication,
    /* Socket manager object */
    SockManager*    aSockManager,
    /* The net id */
    string          aId,
    /* The net version */
    string          aVersion,
    /* The net version */
    Task            aTask
)
:Limb( aApplication -> getLogManager() )
{
    application = aApplication;
    sockManager = aSockManager;
    id          = aId;

    /* Set versions */
    version     = aVersion;
    nextVersion = aVersion;

    task        = aTask;

    application -> getLog() -> trace( "Create net" );

    rnd = Rnd::create();

    mon = Mon::create( getMonFile() );

    config = ParamList::create();
    weightsExchange = WeightsExchange::create();
}



/*
    Constructor
*/
Net::~Net()
{
    /* Destroy monitoring */
    mon -> destroy();

    /* Weights exchanger destoy */
    weightsExchange -> destroy();

    /* Config object clear and destroy */
    config -> destroy();

    /* Destroy rnd object */
    rnd -> destroy();

    getLog() -> trace( "Destroy net" );
}



/*
    Static constructor
*/
Net* Net::create
(
    /* Application object */
    Application*    aApplication,
    /* Socket manager object */
    SockManager*    aSockManager,
    /* The net id */
    string          aId,
    /* The net version */
    string          aVersion,
    /* The net version */
    Task            aTask
)
{
    return new Net
    (
        aApplication,
        aSockManager,
        aId,
        aVersion,
        aTask
    );
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
        io -> getAnswer() -> getData( Path{ "data" }, buffer, size );
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
                    )
                    ;
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

            /* Set tick from server net */
            setTick( io -> getAnswer() -> getInt( Path{ "tick" } ));

            /* Loop for values */
            for( auto id : aValues )
            {
                auto layer = getLayerList() -> getById( id );
                if( layer != NULL )
                {
                    char* buffer = NULL;
                    size_t size = 0;

                    io
                    -> getAnswer()
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
    vector<string> aStatValue,              /* Layer list for stat request */
    vector<string> aStatError,              /* Layer list for stat request */
    vector<string> aStatTick,               /* Layer list for stat request */
    vector<string> aStatErrorsBeforeChange  /* Layer list for stat request */
)
{
    if
    (
        aStatValue.size() > 0 ||
        aStatError.size() > 0 ||
        aStatTick.size() > 0 ||
        aStatErrorsBeforeChange.size() > 0
    )
    {
        getLog() -> begin( "Read stat" );

        /* Create IO object and define request */
        auto io = Io::create( this );
        auto request = io -> getRequest();

        request -> setPath( Path{ "value" }) -> pushVector( aStatValue );
        request -> setPath( Path{ "error" }) -> pushVector( aStatError );
        request -> setPath( Path{ "tick" }) -> pushVector( aStatTick );
        request -> setPath( Path{ "errorsBeforeChange" }) -> pushVector( aStatErrorsBeforeChange );

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

            /* Loop for errors */
            for( auto id : aStatErrorsBeforeChange )
            {
                auto layer = getLayerList() -> getById( id );
                if( layer != NULL )
                {
                    char* buffer = NULL;
                    size_t size = 0;

                    io -> getAnswer() -> getData
                    (
                        Path{ "errorsBeforeChange", layer -> getId() },
                        buffer,
                        size
                    );

                    if( buffer != NULL )
                    {
                        layer
                        -> getChartErrorsBeforeChange()
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
    getLog() -> begin( "Read net config" ) -> lineEnd();

    /* Read net */
    Io::create( this, aAnswer )
    -> call( CMD_READ_NET )
    -> resultTo( this )
    -> destroy();

    getLog() -> end();

    return this;
}



Net* Net::readNetFromFile
(
    ParamList* aAnswer
)
{
    /* Buid file for next version */
    string file = getNetConfigFile( getNextVersion() );

    if( fileExists( file ))
    {
        auto lastUpdate = (long) getConfig() -> getInt( Path{ "lastUpdate" }, 0 );
        auto aUpdated = checkFileUpdate( file, lastUpdate );

        if( aUpdated )
        {
            getLog()
            -> begin( "Read net config" )
            -> prm( "file", file )
            -> lineEnd();

            Json::create()
            -> fromFile( file )
            -> include()
            -> copyTo( aAnswer )
            -> resultTo( this )
            -> destroy();

            if( isOk() )
            {
                aAnswer -> setInt( "lastUpdate", lastUpdate );
            }
            else
            {
                getLog()
                -> warning( this -> getCode() )
                -> prm( "message", this -> getMessage())
                ;
            }

            getLog() -> end();
        }
    }

    return this;

}




bool Net::isConfigUpdate
(
    ParamList* aConfig
)
{
    return getLastUpdate() != aConfig -> getInt( Path{ "lastUpdate" }, 0 );
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
    string aChildVersion,
    double aSurvivalErrorAvg,
    Rnd* aMutationRnd
)
{
    getLog() -> begin( "Net clone" );

    /* Set current id and version for not specified */
    aParentNetId = aParentNetId == "" ? id : aParentNetId;
    aParentNetVersion = aParentNetVersion == "" ? version : aParentNetVersion;

    /* Define net files */
    string parentNetFile = getNetConfigFile( aParentNetVersion );


    auto json = Json::create() -> fromFile( parentNetFile );
    json -> include();

    /* Create JSON structure */
    auto mutateConfig = NetConfig::create();
    mutateConfig -> copyFrom( json -> getParamList());

    if( aMutationRnd != NULL )
    {
        /* Mutation */
        auto mutations = mutateConfig -> getObject( Path{ "mutations" });

        if( mutations != NULL )
        {
            /* calculate sum of rnd of all mutation */
            double sumRnd = mutations -> calcSum( Path{ "rnd" } );
            double dice = aMutationRnd -> get( 0.0, sumRnd );

            double prevDice = 0.0;

            getLog()
            -> trace( "Select mutation" )
            -> prm( "Rnd sum", sumRnd )
            -> prm( "Dice", dice );

            /* Loop for each mutation */
            mutations -> loop
            (
                [
                    &mutateConfig,
                    this,
                    &dice,
                    &prevDice,
                    &aMutationRnd
                ]
                ( Param* iParam )
                {
                    if( iParam -> isObject() )
                    {
                        /* Processing mutation */
                        auto mutation = iParam -> getObject();
                        if
                        (
                            dice >= prevDice &&
                            dice < prevDice + mutation -> getDouble( Path{ "rnd" })
                        )
                        {
                            auto operation = mutation -> getString
                            (
                                Path{ "operation" },
                                "changeParam"
                            );

                            if( operation == "insertLayer" )
                                mutateConfig -> mutateInsertLayer( mutation, aMutationRnd );
                            else
                                mutateChangeParam( mutateConfig, mutation, aMutationRnd );
                        }
                        prevDice = prevDice + mutation -> getDouble( Path{ "rnd" });
                    }
                    return false;
                }
            );
        }
    }

    getLog()
    -> info( "Copy net files" )
    -> prm( "from", aParentNetVersion )
    -> prm( "to", aChildVersion );

    /* Write child version like current */
    mutateConfig
    -> setString( Path{ "id" }, aParentNetId )
    -> setString( Path{ "version", "current" }, aChildVersion )
    -> setString( Path{ "version", "parent" }, aParentNetVersion )
    -> setDouble( Path{ "survival", "error", "avg" }, aSurvivalErrorAvg );

    string childNetFile = getNetConfigFile( aChildVersion );

    /* Save children file */
    if( checkPath( getPath( childNetFile )))
    {
        json -> getParamList() -> clear() -> copyFrom( mutateConfig );
        json -> uninclude();
        json -> toFile( childNetFile );
    }

    mutateConfig -> destroy();
    json -> destroy();

    getLog() -> end();

    return this;
}



Net* Net::mutateChangeParam
(
    ParamList* config,
    ParamList* mutation,
    Rnd* aMutationRnd
)
{
    /* Get path for mutation */
    auto path = mutation -> getPath( Path{ "path" });
    /* Set default mutation path */
    double mutationValue = 0.0;
    double mutationValueParent = 0.0;

    getLog()
    -> trace( "Mutation" )
    -> prm( "param", implode( path, "." ));

    /* Get mutating parameter */
    auto mutated = config -> getByName( path );

    if( mutated != NULL )
    {
        switch( mutated -> getType() )
        {
            case KT_DOUBLE:
            {
                auto mul = mutation -> getDouble( Path{ "mul" }, 1.0 );
                auto add = mutation -> getDouble( Path{ "add" }, 0.0 );
                auto val = mutated -> getDouble();
                auto rMul = aMutationRnd -> get( 1.0 / mul, mul );
                auto rAdd = aMutationRnd -> get( -add, +add );
                auto vMax = mutation -> getDouble( Path{ "max" }, val );
                auto vMin = mutation -> getDouble( Path{ "min" }, val );

                mutationValueParent = val;

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
                    mutationValue =
                    min
                    (
                        vMax,
                        max( vMin, val * rMul + rAdd )
                    );
                    mutated -> setDouble( mutationValue );
                }

                getLog() -> prm( "result", mutated -> getDouble() );

            }
            break;
            case KT_INT:
            {
                auto mul = mutation -> getDouble( Path{ "mul" }, 1.0 );
                int add = mutation -> getInt( Path{ "add" }, 0 );
                auto val = mutated -> getInt();
                auto rMul = aMutationRnd -> get( 1.0 / mul, mul );
                auto rAdd = aMutationRnd -> get( -add, +add );
                auto vMax = mutation -> getDouble( Path{ "max" }, val );
                auto vMin = mutation -> getDouble( Path{ "min" }, val );

                mutationValueParent = val;

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
                    mutationValue = min
                    (
                        vMax,
                        max( vMin, val * rMul + rAdd )
                    );

                    mutated -> setInt( ( int ) mutationValue );
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

    config -> pushObject
    (
        Path{ "version", "mutation" },
        ParamList::create()
        -> setString
        (
            "operation",
            mutation -> getString( Path{ "operation" }, "changeParam" )
        )
        -> setString( "key", implode( path, "." ) )
        -> setDouble( "from", mutationValueParent )
        -> setDouble( "to", mutationValue )
    );

    return this;
}





/*
    Apply net
*/
Net* Net::applyNet
(
    ParamList* aConfig
)
{
    config -> copyFrom( aConfig );
    auto configLayers = config -> getObject( Path{ "layers" });

    if( configLayers != NULL )
    {
        /* Set net version from config */
        setNextVersion( config -> getString( Path{ "version", "current" } ) );

        /* Set rnd seed version from config */
        setRndSeedFromConfig();

        /* Remove layers absents in the use list */
        purgeLayers( configLayers );

        auto taskName = taskToString( task );

        getLog()
        -> begin( "Layers load for task" )
        -> prm( "task", taskName );

        /* Create layers */
        configLayers -> objectsLoop
        (
            [ this, &taskName ]
            (
                ParamList* iParam,
                string iName
            )
            {
                getLog() -> trace( iName );

                /* Create layer if its in used list */
                auto used = ParamList::create();

                /* Layer creates */
                if
                (
                    iParam -> getObject
                    (
                        Path{ "actions", taskName }
                    ) != NULL
                )
                {
                    auto layerId = iName;
                    auto layer = createLayer( layerId );
                    loadLayer( layer, iParam );
                    layer -> setStoragePath( storagePath );
                }
                else
                {
                    getLog()
                    -> trace( "Layer skiped" )
                    -> prm( "id", iName )
                    ;
                }

                used -> destroy();
                return false;
            }
        );

        getLog() -> end( "" ); /* End of layers load */

        /* Nerves */
        auto jsonNerves = config -> getObject( Path{ "nerves" });

        if( jsonNerves != NULL )
        {
            auto layers = getLayerList();
            auto nerves = getNerveList();


            getLog()
            -> begin( "Nerves load for task" )
            -> prm( "task", taskName );

            jsonNerves -> loop
            (
                [ this, &layers, &nerves, &taskName ]
                ( Param* aItem )
                {
                    /* Check the json layer */
                    if( aItem -> isObject() )
                    {
                        auto jsonNerve      = aItem -> getObject();
                        auto idFrom         = jsonNerve -> getString( Path{ "idFrom" });
                        auto idTo           = jsonNerve -> getString( Path{ "idTo" });
                        auto bindType       = bindTypeFromString( jsonNerve -> getString( Path{ "bindType" } ));
                        auto nerveType      = nerveTypeFromString( jsonNerve -> getString( Path{ "nerveType" } ));
                        auto nerveDelete    = jsonNerve -> getBool( Path{ "delete" });

                        if
                        (
                            ! aItem
                            -> getObject()
                            -> valueExists( Path{ "uses" }, taskName )
                        )
                        {
                            getLog()
                            -> trace( "Nerve skiped for the task" )
                            -> prm( "idFrom", idFrom )
                            -> prm( "idTo", idTo )
                            -> lineEnd()
                            ;
                        }
                        else
                        {
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

                                if( nerve != NULL )
                                {
                                    if
                                    (
                                        nerve -> getParent() != from ||
                                        nerve -> getChild() != to ||
                                        nerve -> getBindType() != bindType ||
                                        nerve -> getNerveType() != nerveType ||
                                        nerveDelete
                                    )
                                    {
                                        deleteNerve( nerve );
                                        nerve = NULL;
                                    }
                                    else
                                    {
                                        getLog()
                                        -> trace( "Nerve exists and stay" )
                                        -> prm( "idFrom", idFrom )
                                        -> prm( "idTo", idTo )
                                        -> lineEnd()
                                        ;
                                    }
                                }

                                if( nerve == NULL && !nerveDelete )
                                {
                                    auto minW = jsonNerve -> getDouble( Path{ "minWeight" } , 0 );
                                    auto maxW = jsonNerve -> getDouble( Path{ "maxWeight" }, 0 );
                                    auto mulW = config -> getDouble( Path{ "weightMul" }, 1 );

                                    createNerve
                                    (
                                        from,
                                        to,
                                        nerveType,
                                        bindType
                                    )
                                    -> setMinWeight
                                    (
                                        minW * ( minW == maxW ? 1 : mulW )
                                    )
                                    -> setMaxWeight
                                    (
                                        maxW * ( minW == maxW ? 1 : mulW )
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
                                -> lineEnd()
                                ;
                            }
                        }
                    }
                    return false;
                }
            );

            getLog()
            -> end();

        } /* End of nerves load */
    }

    /* Update last update net moment */
    setLastUpdate( aConfig -> getInt( Path{ "lastUpdate" }, 0 ));

    getLayerList() -> dump();

    if( version != nextVersion )
    {
        /* Set nextVersion in to version */
        version = nextVersion;

        /* Drop tick */
        tick = 0;

        /* Clear tick stat for each layer */
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

        mon
        -> setFile( getMonFile() )
        -> now( Path{ "created" })
        -> setString( Path{ "id" }, id )
        -> setString( Path{ "version" }, version )
        -> flush();
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
    return getNetPath
    (
        "ver/" + aVersion  + ( aSubpath == "" ? "" : "/" + aSubpath ),
        aId
    );
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



/*
    Return net monitoring file
*/
string Net::getMonFile
(
    string aVersion,    /* Specific version */
    string aId          /* Net id */
)
{
    return getNetVersionPath
    (
        "/mon/" + taskToString( task ) + ".json",
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
            != aParams -> getString( Path{ "id" }, aLayer -> getId() )
        )
        {
            setCode( "InvalidLayerID" );
        }
        else
        {
            /* Apply neuron functions for layer */
            aLayer
            -> setFrontFunc
            (
                strToFunc( aParams -> getString( Path{ "functionFront" }, "NULL" ))
            )
            -> setBackFunc
            (
                strToFunc( aParams -> getString( Path{ "functionBack" }, "NULL" ))
            )
            -> setBackFuncOut
            (
                strToFunc( aParams -> getString( Path{ "functionBackOut" }, "NULL" ))
            )
            -> setErrorCalc
            (
                errorCalcFromString( aParams -> getString( Path{ "errorCalc" }, "NONE" ))
            )
            -> setWeightCalc
            (
                weightCalcFromString( aParams -> getString( Path{ "weightCalc" }, "NONE" ))
            );

            /* Set Size from params */
            auto size = ParamPoint::point3i( aParams -> getObject( Path{ "size" } ));

            /* Remove nerves for size changed layer */
            if( size.mulComponents() != aLayer -> getCount() )
            {
                getNerveList() -> removeByLayer( aLayer );
            }

            /* Update layer */
            aLayer -> setSize( size );
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
            if( aLayers -> getObject( Path{ layerId }) == NULL )
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
                auto layerId = params -> getString( Path{ "layerId" });
                auto neuronIndex = params -> getInt( Path{ "neuronIndex" });

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
    Limb*   aLimb,      /* Participant */
    bool    aSkip
)
{
    if( lock( aSkip ))
    {
        if( aLimb -> lock( aSkip ))
        {
            /* Loop for layers configuration */
            getLayerList() -> loop
            (
                [ this, &aLimb, &aActions ]
                (
                    void* item
                )
                {
                    auto netLayer = ( Layer* ) item;
                    /* For each of action */
                    for( auto iAction : aActions )
                    {
                        if( checkLayerAction( netLayer, iAction ))
                        {
                            /* ... finds layer by id in net and participant ... */
                            auto participantLayer = aLimb
                            -> getLayerList()
                            -> getById( netLayer -> getId() );

                            /* ... if both layers fonded ... */
                            if( participantLayer != NULL )
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
                                        if( task != TASK_PROC )
                                        {
                                            addChangedValues( netLayer );
                                        }
                                    break;
                                    case READ_ERRORS:
                                        participantLayer -> copyErrorsFrom( netLayer );
                                    break;
                                    case WRITE_ERRORS:
                                        netLayer -> copyErrorsFrom( participantLayer );
                                        if( task != TASK_PROC )
                                        {
                                            addChangedErrors( netLayer );
                                        }
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

    return this;
}




bool Net::syncToLimb
(
    Limb* targetLimb,
    bool aSkip
)
{
    auto result = targetLimb -> getLastUpdate() != getLastUpdate();
    if( result )
    {
        /* Rebuild structure layers and nervs */
        copyTo( targetLimb, true, aSkip, false );
        /* Apply specific config */
        targetLimb -> onAfterReconfig( getConfig() );
        /* Commit last  update */
        targetLimb -> setLastUpdate( getLastUpdate() );
    }
    return result;
}



/*
    Synchronaize with server
    For the modified layer, write to the server
    else read from the server.
*/
Net* Net::syncWithServer()
{
    /* The application does not have to be a processor */
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
    vector<string> readStatErrorsBeforeChange;

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
            &readStatErrorsBeforeChange,
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
                /* Check application rules for write values of the layer */
                if( checkLayerAction( layer, WRITE_VALUES ))
                {
                    writeValues.push_back( layerId );
                }
            }
            else
            {
                /* Check application rules for write values of the layer */
                if( checkLayerAction( layer, READ_VALUES ))
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
                if( checkLayerAction( layer, WRITE_ERRORS ))
                {
                    writeErrors.push_back( layerId );
                }
            }
            else
            {
                /* Check application rules for read errors of the layer */
                if( checkLayerAction( layer, READ_ERRORS ))
                {
                    readErrors.push_back( layerId );
                }
            }

            /* Check application rules stat requests */
            if( checkLayerAction( layer, READ_STAT_VALUE ))
            {
                readStatValue.push_back( layerId );
            }
            /* Check application rules stat requests */
            if( checkLayerAction( layer, READ_STAT_ERROR ))
            {
                readStatError.push_back( layerId );
            }
            /* Check application rules stat requests */
            if( checkLayerAction( layer, READ_STAT_ERRORS_BEFORE_CHANGE ))
            {
                readStatErrorsBeforeChange.push_back( layerId );
            }
            /* Check application rules stat requests */
            if( checkLayerAction( layer, READ_STAT_TICK ))
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

    requestStat
    (
        readStatValue,
        readStatError,
        readStatTick,
        readStatErrorsBeforeChange
    );


    /* Read net stat */
    auto io = Io::create( this ) -> call( CMD_READ_NET_INFO );

    if( io -> isOk() )
    {
        lock();
        setTick( io -> getAnswer() -> getInt( Path{ "tick" }));
        unlock();
    }
    io -> destroy();

    /* Finalize */
    getLog()
    -> end()
    -> trapOff();

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



/*
    Return new next version by argument version and
    list of names from ./net_names.json file.
    Format:
        generation number
        parent name
        person name
*/
string Net::generateVersion
(
    /* Id of the net */
    string aId,
    /* Version of the net */
    string aVersion,
    /* Success */
    bool aParentSuccess
)
{
    auto lexems = explode( aVersion, "." );
    auto lexemsSize = lexems.size();

    /* Read version of current net */
    auto generation = toInt( lexemsSize > 0 ? lexems[ 0 ] : "0" );
    auto parent = toInt( lexemsSize > 1 ? lexems[ 1 ] : "0" );
    auto name = toInt( lexemsSize > 2 ? lexems[ 2 ] : "0" );

    if( aParentSuccess )
    {
        generation ++;
        parent = name;
        name = 0;
    }
    else
    {
        name ++;
    }

    char buffer[21];
    std::snprintf(buffer, sizeof(buffer), "%06lld.%06lld.%06lld", generation, parent, name);
    return std::string(buffer);
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
    Return true value if layer contains action for current net task
*/
bool Net::checkLayerAction
(
    Layer* aLayer,
    Action aAction
)
{
    auto layerActions = getConfig() -> getObject
    (
        Path
        {
            "layers",
            aLayer -> getId(),
            "actions",
            taskToString( task )
        }
    );

    return
    layerActions != NULL &&
    layerActions -> contains( actionToString( aAction ));
}



/*
    Return the tick of the net
*/
long long int Net::getTick()
{
    lock();
    auto result = tick;
    unlock();

    return result;
}



/*
    Return the tick of the net
*/
Net* Net::setTick
(
    /* Tick number */
    long long int a
)
{
    lock();
    tick = a;
    unlock();
    return this;
}



/*
    Return the tick of the net
*/
Net* Net::incTick()
{
    lock();
    tick++;
    unlock();
    return this;
}



Mon* Net::getMon()
{
    return mon;
}




Rnd* Net::getRnd()
{
    return rnd;
}



Net* Net::setRndSeedFromConfig()
{
    getRnd() -> setSeed( config -> getInt( Path{ "seed" } ) );
    return this;
}
