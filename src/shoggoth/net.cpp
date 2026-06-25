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
    db = ShoggothDb::create( getLogManager(), "db.sql" );

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
    real aSurvivalErrorAvg
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

    /* Mutation */
    auto mutations = mutateConfig -> getObject( Path{ "mutations" });

    if( mutations != NULL )
    {
        /* calculate sum of rnd of all mutation */
        real sumRnd = mutations -> calcSum( Path{ "rnd" } );
        real dice = getRnd() -> get( 0.0, sumRnd );

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
                            mutateConfig -> mutateInsertLayer( mutation, getRnd() );
                        else
                            mutateChangeParam( mutateConfig, mutation );
                    }

                    prevDice = prevDice + itemMutation -> getDouble( Path{ "rnd" });
                }
                return mutation != NULL;
            }
        );
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
    ParamList* mutation
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
                auto rMul = getRnd() -> get( 1.0 / mul, mul );
                auto rAdd = getRnd() -> get( -add, +add );
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
                auto rMul = getRnd() -> get( 1.0 / mul, mul );
                auto rAdd = getRnd() -> get( -add, +add );
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
