/* for current_path */
#include <filesystem>
#include <algorithm>
#include <iostream>

#include "net.h"

#include "shoggoth_rpc_client.h"
#include "shoggoth_consts.h"
#include "../../../../lib/json/param_list_file.h"
#include "../../../../lib/graph/param_point.h"



/*
    Constructor
*/
Net::Net
(
    /* Application object */
    ShoggothApplication*    aApplication,
    /* The net id */
    std::string     aId,
    /* The net version */
    std::string     aVersion
)
:Limb
(
    aApplication -> getLogManager(),
    /* Payload id */
    "",
    aVersion
)
{
    application = aApplication;

    id          = aId;

    /* Set versions */
    nextVersion = aVersion;

    application -> getLog() -> trace( "Create net" );

    rnd = Rnd::create();

    db = ShoggothDb::create( getLogManager(), "db.sql" );

    config = ParamList::create();

    weightsExchange = WeightsExchange::create();
}



/*
    Constructor
*/
Net::~Net()
{
    db -> destroy();

    /* Weights exchanger destoy */
    weightsExchange -> destroy();

    /* Config object clear and destroy */
    config -> destroy();

    /* Destroy rnd object */
    rnd -> destroy();

    getLog() -> trace( "Destroy net" );
}



/*
    Read nerve weights array
*/
Net* Net::readWeights
(
    Nerve* aNerve,
    std::string aConnection
)
{
    ShoggothRpcClient::create( getApplication(), aConnection )
    -> readWeights( aNerve )
    -> destroy();
    return this;
}



/*
    Write the nerve weight array
*/
Net* Net::writeWeights
(
    Nerve* aNerve,
    std::string aConnection
)
{
    ShoggothRpcClient::create( getApplication(), aConnection )
    -> writeWeights( aNerve )
    -> destroy();
    return this;
}


/*
    Request layers statistics
*/
Net* Net::requestStat
(
    /* Layer list for stat request */
    vector<string> aStatValue,
    /* Layer list for stat request */
    vector<string> aStatError,
    /* Layer list for stat request */
    vector<string> aStatTick,
    /* Layer list for stat request */
    vector<string> aStatErrorsBeforeChange,
    /* Connection config */
    std::string aConnection
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

        auto client = ShoggothRpcClient::create( getApplication(), aConnection );
        if
        (
            /* Call server and apply the answer */
            client -> readLayerStat
            (
                aStatValue,
                aStatError,
                aStatTick,
                aStatErrorsBeforeChange
            )
            -> isOk()
        )
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

                    client -> getAnswer() -> getData
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

                    client -> getAnswer() -> getData
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

                    client -> getAnswer() -> getData
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

                    client -> getAnswer()
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

        client -> destroy();

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



Net* Net::readNet
(
    ParamList* aAnswer,
    std::string aConnection
)
{
    getLog()
    -> begin( "Read net config" )
    -> lineEnd();

    ShoggothRpcClient::create( getApplication(), aConnection, aAnswer )
    -> readNet()
    -> resultTo( this )
    -> destroy();

    getLog() -> end();

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
    real aSurvivalErrorAvg,
    Rnd* aMutationRnd
)
{
    getLog() -> begin( "Net clone" );

    /* Set current id and version for not specified */
    aParentNetId = aParentNetId == "" ? id : aParentNetId;
    aParentNetVersion = aParentNetVersion == "" ? getVersion() : aParentNetVersion;

    /* Define net files */
    string parentNetFile = getNetConfigFile( aParentNetVersion );


    auto json = Json::create() -> fromFile( parentNetFile );
    json -> include();

    /* Create JSON structure */
    auto mutateConfig = NetConfig::create();
    mutateConfig -> copyFrom( json -> getParamList());

    ParamList* mutation = NULL;

    /* Unlock weightWriteLock for net */
    mutateConfig -> setBool( Path{ "processor", "weightWriteLock" }, false );

    if( aMutationRnd != NULL )
    {
        /* Mutation */
        auto mutations = mutateConfig -> getObject( Path{ "mutations" });

        if( mutations != NULL )
        {
            /* calculate sum of rnd of all mutation */
            real sumRnd = mutations -> calcSum( Path{ "rnd" } );
            real dice = aMutationRnd -> get( 0.0, sumRnd );

            real prevDice = 0.0;

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
                    &aMutationRnd,
                    &mutation
                ]
                ( Param* iParam )
                {
                    if( iParam -> isObject() )
                    {
                        /* Processing mutation */
                        auto itemMutation = iParam -> getObject();

                        if
                        (
                            dice >= prevDice &&
                            dice < prevDice + itemMutation -> getDouble( Path{ "rnd" })
                        )
                        {
                            mutation = itemMutation;

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

                        prevDice = prevDice + itemMutation -> getDouble( Path{ "rnd" });
                    }
                    return mutation != NULL;
                }
            );
        }
    }

    getDb() -> netStart
    (
        aChildVersion,
        mutation == NULL ? "" : mutation -> toString()
    );

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
    real mutationValue = 0.0;
    real mutationValueParent = 0.0;

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
    ParamList*  aConfig
)
{
    config -> copyFrom( aConfig );
    auto configLayers = config -> getObject( Path{ "layers" });

    if( configLayers != NULL )
    {
        /* Set net version from config */
        setNextVersion( config -> getString( Path{ "version", "current" } ));

        /* Read weights write lock flag */
        setWeightWriteLock
        (
            config -> getBool( Path{ "processor", "weightWriteLock" })
        );

        /* Set rnd seed version from config */
        setRndSeedFromConfig();

        /* Remove layers absents in the use list */
        purgeLayers( configLayers );

        /* Create layers */
        getLog() -> begin( "Layers load for task" );
        configLayers -> objectsLoop
        (
            [ this ]
            (
                ParamList* iParam,
                string layerId
            )
            {
                getLog() -> begin( "Layer loading" ) -> prm( "id", layerId );

                /* Create layer if its in used list */
                auto used = ParamList::create();

                /* Layer creates */
                if( getApplication() -> layerIsUsing( layerId ) )
                {
                    auto layer = addLayer( layerId );
                    loadLayer( layer, iParam );
                }
                else
                {
                    getLog()
                    -> trace( "Layer skiped" )
                    -> prm( "id", layerId )
                    ;
                }

                used -> destroy();
                getLog() -> end();
                return false;
            }
        );
        /* End of layers load */
        getLog() -> end( "" );

        loadNerves( config );
    }

    /* Update last update net moment */
    setLastUpdate( aConfig -> getInt( Path{ "lastUpdate" }, 0 ));

    getLayerList() -> dump();
    getNerveList() -> dump();

    if( getVersion() != nextVersion )
    {
        /* Set nextVersion in to version */
        setVersion( nextVersion );

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
    }

    return this;
}



/*
    Load nerves from config
*/
Net* Net::loadNerves
(
    ParamList* aConfig
)
{
    /* Nerves */
    auto jsonNerves = aConfig -> getObject( Path{ "nerves" });
    if( jsonNerves != NULL )
    {
        auto layers = getLayerList();
        auto nerves = getNerveList();

        getLog() -> begin( "Nerves load" );

        jsonNerves -> loop
        (
            [ this, &layers, &nerves ]
            ( Param* aItem )
            {
                /* Check the json layer */
                if( aItem -> isObject() )
                {
                    auto jsonNerve = aItem -> getObject();

                    auto fromList = jsonNerve -> getStringVector( Path{ "idFrom" });
                    auto toList = jsonNerve -> getStringVector( Path{ "idTo" });

                    auto bindType = bindTypeFromString
                    (
                        jsonNerve -> getString( Path{ "bindType" } )
                    );
                    auto nerveType = nerveTypeFromString
                    (
                        jsonNerve -> getString( Path{ "nerveType" } )
                    );
                    auto nerveDelete = jsonNerve -> getBool( Path{ "delete" });
                    auto windowSize = ParamPoint::point3i
                    (
                        jsonNerve -> getObject( Path{ "windowSize" } )
                    );

                    /* Cartesian product for form and to */
                    for( auto& idFrom:fromList )
                    {
                        for( auto& idTo:toList )
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
                                        -> trace( "Nerve exists" )
                                        -> prm( "idFrom", idFrom )
                                        -> prm( "idTo", idTo )
                                        -> lineEnd()
                                        ;
                                    }
                                }

                                if( nerve == NULL && !nerveDelete )
                                {
                                    auto minW = jsonNerve
                                    -> getDouble( Path{ "minWeight" } , 0 );
                                    auto maxW = jsonNerve
                                    -> getDouble( Path{ "maxWeight" }, 0 );
                                    auto mulW = config
                                    -> getDouble( Path{ "weightMul" }, 1 );
                                    nerve = createNerve
                                    (
                                        from,
                                        to,
                                        nerveType,
                                        bindType,
                                        windowSize
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
                                    if( !nerve -> isOk() )
                                    {
                                        getLog()
                                        -> warning( "Nerve error" )
                                        -> prm( "code", nerve -> getCode() )
                                        -> lineEnd();
                                    }
                                }
                            }
                            else
                            {
                                getLog()
                                -> info( "Nerve skiped" )
                                -> prm( "idFrom", idFrom )
                                -> prm( "idTo", idTo )
                                -> lineEnd()
                                ;
                            }
                        }
                    }
                }
                return false;
            }
        );

        getLog()
        -> end();

    } /* End of nerves load */
    return this;
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
    return "./" + aSubpath;
}




/*
    Return net path
*/
string Net::getNetVersionPath
(
    /* Subpath */
    string aSubpath,
    /* Specific version */
    string aVersion
)
{
    aVersion = aVersion == "" ? getVersion() : aVersion;
    return getNetPath
    (
        "ver/" + aVersion  + ( aSubpath == "" ? "" : "/" + aSubpath )
    );
}



/*
    Return net config
*/
string Net::getNetConfigFile
(
    /* Specific version */
    string aVersion
)
{
    return getNetVersionPath( "net.json", aVersion );
}



/*
    Return log path
*/
string Net::getLogPath
(
    string aSubpath
)
{
    return getNetPath
    (
        "log" + ( aSubpath == "" ? "" : "/" + aSubpath )
    );
}



/*
    Return dump path
*/
string Net::getDumpPath
(
    string aSubpath,
    /* Specific version */
    string aVersion
)
{
    return getNetVersionPath
    (
        "dump" + ( aSubpath == "" ? "" : "/" + aSubpath ),
        aVersion
    );
}



/*
    Return path for nerves weights
*/
string Net::getNervesPath
(
    string aSubpath,
    /* Specific version */
    string aVersion
)
{
    return getNetVersionPath
    (
        "nerves" + ( aSubpath == "" ? "" : "/" + aSubpath ),
        aVersion
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
                strToFunc
                (
                    aParams -> getString( Path{ "functionFront" }, "NULL" )
                )
            )
            -> setBackFunc
            (
                strToFunc
                (
                    aParams -> getString( Path{ "functionBack" }, "NULL" )
                )
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

            /* Apply default values */
            auto values = aParams -> getObject( Path{ "values" } );
            if( values != nullptr )
            {
                aLayer -> fillValue( values );
            }

            calcLayerValuesHash( aLayer );
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
bool Net::valuesAndErrorsToLimb
(
    /* Participant */
    Limb*           aLimb,
    vector<string>  aLayers,
    bool            aSkip
)
{
    bool result = false;
    if( lock( aSkip ))
    {
        if( aLimb -> lock( aSkip ))
        {
            /* Loop for layers configuration */
            getLayerList() -> loop
            (
                [ &aLimb, &result, &aLayers ]
                ( void* item )
                {
                    auto netLayer = ( Layer* ) item;
                    /* ... finds layer by id in net and participant ... */
                    auto participantLayer = aLimb
                    -> getLayerList()
                    -> getById( netLayer -> getId() );
                    /* ... if both layers fonded ... */
                    if( participantLayer != NULL )
                    {
                        if
                        (
                            std::find(aLayers.begin(), aLayers.end(), netLayer -> getId())
                            != aLayers.end()
                        )
                        {
                            result |= participantLayer -> copyValuesFrom
                            (
                                netLayer
                            );
                        }
                    }
                    return false;
                }
            );
            aLimb -> unlock();
        }
        unlock();
    }
    return result;
}



/*
    Swap layers between net and other participants for actions
*/
bool Net::valuesAndErrorsFromLimb
(
    /* Participant */
    Limb*           aLimb,
    vector<string>  aLayers,
    bool            aSkip
)
{
    bool result = false;
    if( lock( aSkip ))
    {
        if( aLimb -> lock( aSkip ))
        {
            /* Loop for layers configuration */
            getLayerList() -> loop
            (
                [ this, &aLimb, &result, &aLayers ]
                ( void* item )
                {
                    auto netLayer = ( Layer* ) item;
                    /* ... finds layer by id in net and participant ... */
                    auto layerId = netLayer -> getId();
                    auto participantLayer = aLimb
                    -> getLayerList()
                    -> getById( layerId );
                    /* ... if both layers fonded ... */
                    if( participantLayer != NULL )
                    {
                        if
                        (
                            std::find(aLayers.begin(),
                            aLayers.end(),
                            netLayer -> getId())
                            != aLayers.end()
                        )
                        {
                            result |= netLayer -> copyValuesFrom
                            (
                                participantLayer
                            );
                            /* Calculate hash for the layer */
                            calcLayerValuesHash( netLayer );
                        }
                    }
                    return false;
                }
            );
            aLimb -> unlock();
        }
        unlock();
    }
    return result;
}



bool Net::syncToLimb
(
    Limb* targetLimb
)
{
    auto result = targetLimb -> getLastUpdate() != getLastUpdate();
    if( result )
    {
        /* Rebuild structure layers and nervs */
        copyTo( targetLimb, true /*, aSkip, false */ );
        /* Apply specific config */
        targetLimb -> onAfterReconfig( getConfig() );
        /* Commit last  update */
        targetLimb -> setLastUpdate( getLastUpdate() );
    }
    return result;
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
    return nextVersion != getVersion();
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



Rnd* Net::getRnd()
{
    return rnd;
}



Net* Net::setRndSeedFromConfig()
{
    getRnd() -> setSeed( config -> getInt( Path{ "seed" } ) );
    return this;
}
