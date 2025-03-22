#include "shoggoth_rpc_server.h"
#include "../../shoggoth/io.h"


/*
    Constructor
*/
ShoggothRpcServer::ShoggothRpcServer
(
    Net*            aNet,
    SocketDomain    aDomain,
    SocketType      aType,
    int             aPort
)
:RpcServer
(
    aNet -> getApplication() -> getLogManager(),
    aNet -> getSockManager(),
    aDomain,
    aType,
    aPort
)
{
    net = aNet;

    mon = Mon::create( aNet -> getMonPath( "shoggoth_server_rpc.json" ))
    -> setString( Path{ "start", "source" }, "shoggoth_server_rpc" )
    -> startTimer( Path{ "start", "moment" })
    -> now( Path{ "start", "momentStr" })
    -> flush();

    getLog() -> trace( "Shoggoth server created" ) -> lineEnd();
}



/*
    Constructor
*/
ShoggothRpcServer::~ShoggothRpcServer()
{
    mon -> destroy();
    getLog() -> trace( "Shoggoth server destroy" );
}



/*
    Create socket
*/
ShoggothRpcServer* ShoggothRpcServer::create
(
    Net*    aNet,
    int     aPort
)
{
    return new ShoggothRpcServer( aNet, SD_INET, ST_TCP, aPort );
}



/******************************************************************************
    Setters and geters
*/


Application* ShoggothRpcServer::getApplication()
{
    return net -> getApplication();
}


/******************************************************************************
    Events
*/


/*
    Servers On call after event
    Main router for server
*/
ShoggothRpcServer* ShoggothRpcServer::onCallAfter
(
    ParamList* aArguments,
    ParamList* aResults
)
{
    auto methodStr = aArguments -> getString( Path{ "method" });
    auto method = commandFromString( methodStr );

    mon
    -> now( Path{ "last", "momentStr" } )
    -> setString( Path{ "last", "Method" }, methodStr )
    -> addInt( Path{ "stat", methodStr })
    -> flush();

    getLog()
    -> trace( "Called method!" )
    -> prm( "Name", methodStr )
    -> prm( "Code", method )
    -> lineEnd();

    net -> lock();

    switch( method )
    {
        case CMD_READ_NET           :readNet( aArguments, aResults); break;
        case CMD_READ_NET_INFO      :readNetInfo( aArguments, aResults); break;
        case CMD_COMMIT_NET         :commitNet( aArguments, aResults); break;
        case CMD_CLONE_NET          :cloneNet( aArguments, aResults); break;
        case CMD_SWITCH_NET         :switchNet( aArguments, aResults); break;
        case CMD_WRITE_LAYERS       :writeLayers( aArguments, aResults); break;
        case CMD_READ_LAYERS        :readLayers( aArguments, aResults); break;
        case CMD_REQUEST_WEIGHTS    :requestWeights( aArguments, aResults); break;
        case CMD_DROP_LAYER_TICK    :dropLayerTick( aArguments, aResults); break;
        case CMD_READ_LAYER_STAT    :readLayerStat( aArguments, aResults); break;
//        case CMD_WRITE_WEIGHTS      :writeWeights( aArguments, aResults); break;
//        case CMD_READ_WEIGHTS       :readWeights( aArguments, aResults); break;
        case CMD_GET_NET_MODE       :getNetMode( aArguments, aResults); break;
        case CMD_SET_NET_MODE       :setNetMode( aArguments, aResults); break;
        default                     :unknownMethod( aArguments, aResults); break;
    }

    net -> unlock();

    return this;
}




/******************************************************************************
*/


/*
    Validate answer condition and set answer if validation is failed
*/
bool ShoggothRpcServer::validate
(
    bool aCondition,
    string aCode,
    ParamList* aAnswer
)
{
    if( !aCondition )
    {
        setAnswerResult( aAnswer, aCode );
    }
    return aCondition;
}



/*
    Set code for answer
*/

ParamList* ShoggothRpcServer::setAnswerResult
(
    ParamList* aAnswer,
    string aCode
)
{
    /* Set code */
    return aAnswer
    -> setPath( Path { "result" })
    -> setString( "code", aCode )
    /* Return details key */
    -> setPath( Path{ "details" });
}



/*
    Return true if answer is ok
*/
bool ShoggothRpcServer::isAnswerOk
(
    ParamList* aAnswer
)
{
    return aAnswer -> getString( Path{ "result", "code" }) == RESULT_OK;
}



/**********************************************************************
    Server methods
*/


/*
    Unknwon method result
*/
ShoggothRpcServer* ShoggothRpcServer::unknownMethod
(
    ParamList* aArguments,
    ParamList* aResults
)
{
    setAnswerResult( aResults, "UnknownMethod" );
    return this;
}



/*
    Request net configuration from remote application
*/
ShoggothRpcServer* ShoggothRpcServer::readNet
(
    ParamList* aArguments,
    ParamList* aResults
)
{
    aResults -> copyFrom( net -> getConfig() );
    setAnswerResult( aResults, RESULT_OK );

    return this;
}



/*
    Request net information
*/
ShoggothRpcServer* ShoggothRpcServer::readNetInfo
(
    ParamList* aArguments,
    ParamList* aResults
)
{
    net -> lock();
    aResults -> setInt( "tick", net -> getTick() );
    net -> unlock();
    setAnswerResult( aResults, RESULT_OK );

    return this;
}



/*
    Remote host send command for clone net configuration
*/
ShoggothRpcServer* ShoggothRpcServer::cloneNet
(
    ParamList* aArguments,
    ParamList* aResults
)
{
    auto parentNetId = aArguments -> getString( Path{ "parentNetId" }, "" );
    auto parentNetVersion = aArguments -> getString( Path{ "parentNetVersion" }, "" );
    auto parentGeneration = aArguments -> getInt( Path{ "parentGeneration" }, 0 );
    auto survivalErrorAvg = aArguments -> getDouble( Path{ "survivalErrorAvg" }, 1e10 );

    getLog() -> info( "Clone net argument" ) -> dump( aArguments );

    /* Return positive answer */
    setAnswerResult( aResults, RESULT_OK );

    /* Version of cloned child */
    string childVersion = "";

    /* Clone network */
    net -> clone
    (
        parentNetId,
        net -> getParentVersion
        (
            parentNetId,
            parentNetVersion,
            parentGeneration
        ),
        childVersion,
        survivalErrorAvg,
        /* No clone */
        NULL
    );

    /* Return result */
    aResults
    -> setString( "id",  parentNetId == "" ? net -> getId() : parentNetId )
    -> setString( "version", childVersion );

    setAnswerResult( aResults, RESULT_OK );

    return this;
}



/*
    Remote host send command for switch to the other net
*/
ShoggothRpcServer* ShoggothRpcServer::switchNet
(
    ParamList* aArguments,
    ParamList* aResults
)
{
    auto id = aArguments -> getString( Path{ "id" }, "" );
    auto version = aArguments -> getString( Path{ "version" }, "" );

    /* Set new value of version */
    net -> setNextVersion( version );

    /* Return positive answer */
    setAnswerResult( aResults, RESULT_OK );

    return this;
}



/*
    Commit net
        generation
            0 - commit success
            1 - commit with rollback net rollback to previous generation
*/
ShoggothRpcServer* ShoggothRpcServer::commitNet
(
    ParamList* aArguments,
    ParamList* aResults
)
{
    /* Dump arguments */
    getLog() -> info( "Commit net argument" ) -> dump( aArguments );

    /* Read arguments */
    auto id = aArguments -> getString( Path{ "id" }, "" );
    auto version = aArguments -> getString( Path{ "version" }, "" );
    auto reason = aArguments -> getByName( Path{ "reason" });
    auto success = aArguments -> getBool( Path{ "success" });
    auto survivalErrorAvg = aArguments -> getDouble( Path{ "survivalErrorAvg" }, 1e10 );
    auto mutationSeed = aArguments -> getInt( Path{ "mutationSeed" });

    if
    (
        /* Arguments validation */
        validate( id != "", "IdNetIsEmpty", aResults ) &&
        validate( version != "", "VersionNetIsEmpty", aResults ) &&
        validate( reason != NULL, "ReasonNotFound", aResults ) &&
        validate( reason -> isObject(), "ReasonIsNotObject", aResults )
    )
    {
        /* Write reason to mon */
        Mon::create
        (
            net -> getNetVersionPath
            (
                "/mon/commit.json",
                version,
                id
            )
        )
        -> setString( Path{ "result" }, success ? "SUCCESS" : "ROLLBACK" )
        -> now( Path{ "moment" })
        -> setInt( Path{ "tick" },  net -> getTick())
        -> copyObject( Path{ "reason" }, reason -> getObject())
        -> flush()
        -> destroy();

        auto parentVersion = net -> getParentVersion( id, version, success ? 0 : 1 );
        auto newVersion = net -> generateVersion( id, version, success );

        auto mutationRnd = Rnd::create() -> setSeed( mutationSeed );

        /* Clone network */
        net -> clone
        (
            id,
            parentVersion,
            newVersion,
            survivalErrorAvg,
            mutationRnd
        );

        mutationRnd -> destroy();

        /* Switch to new version */
        net -> setNextVersion( newVersion );

        /* Change net mode */
        changeNetMode( NET_MODE_LEARN );

        /* Return result */
        aResults
        -> setString( "id", id )
        -> setString( "version", newVersion );

        /* Return positive answer */
        setAnswerResult( aResults, RESULT_OK );
    }

    return this;
}




/*
    Remote host send layer data
    Arguments
        idLayer
        data
    Results
        result
*/
ShoggothRpcServer* ShoggothRpcServer::writeLayers
(
    ParamList* aArguments,
    ParamList* aResults
)
{
    /* Return positive answer */
    setAnswerResult( aResults, RESULT_OK );

    auto values = aArguments -> getObject( Path{ "values" });
    if( values != NULL )
    {
        values -> loop
        (
            [ this, &aResults ]
            ( Param* iParam )
            {
                /* Read id layer */
                auto idLayer = iParam -> getName();
                if
                (
                    /* Arguments validation */
                    validate( idLayer != "", "IdLayerIsEmpty", aResults )
                )
                {
                    auto layer = net -> getLayerById( idLayer );
                    if( layer == NULL )
                    {
                        setAnswerResult( aResults, "LayerUnknown" )
                        -> setString( "idLayer", idLayer );
                    }
                    else
                    {
                        /* Get layer data */
                        char* buffer = iParam -> getValue();
                        size_t size = iParam -> getSize();

                        if( validate( buffer != NULL, "DataIsEmpty", aResults ))
                        {
                            layer
                            /* Write error stat */
                            -> writeErrorsBeforeChange()
                            -> setValuesFromBuffer( buffer, size )
                            -> dropTickCount();
                        }
                    }
                }
                else
                {
                    setAnswerResult( aResults, "LayerIdIsEmpty" );
                }
                return !isAnswerOk( aResults );
            }
        );
    }

    if( isAnswerOk( aResults ))
    {
        auto errors = aArguments -> getObject( Path{ "errors" });
        if( errors != NULL )
        {
            errors -> loop
            (
                [ this, &aResults ]
                ( Param* iParam )
                {
                    /* Read id layer */
                    auto idLayer = iParam -> getName();
                    if
                    (
                        /* Arguments validation */
                        validate( idLayer != "", "IdLayerIsEmpty", aResults )
                    )
                    {
                        auto layer = net -> getLayerById( idLayer );
                        if( layer == NULL )
                        {
                            setAnswerResult( aResults, "LayerUnknown" )
                            -> setString( "idLayer", idLayer );
                        }
                        else
                        {
                            /* Get layer data */
                            char* buffer = iParam -> getValue();
                            size_t size = iParam -> getSize();

                            if( validate( buffer != NULL, "DataIsEmpty", aResults ))
                            {
                                layer -> errorsFromBuffer( buffer, size );
                            }
                        }
                    }
                    else
                    {
                        setAnswerResult( aResults, "LayerIdIsEmpty" );
                    }
                    return !isAnswerOk( aResults );
                }
            );
        }
    }

    return this;
}



/*
    Remote host request the layer

    arguments
    {
        values: [ LayerId ],
        errors: [ LayerId ],
    }

    result
    {
        values:
        {
            LayerId: 0xdata,
            ...
        },
        errors:
        {
            LayerId: 0xdata,
            ...
        }
    }
*/
ShoggothRpcServer* ShoggothRpcServer::readLayers
(
    ParamList* aArguments,
    ParamList* aResults
)
{
    /* Return positive answer */
    setAnswerResult( aResults, RESULT_OK );

    auto values = aArguments -> getObject( Path{ "values" });
    if( values != NULL )
    {
        values -> loop
        (
            [ this, &aResults ]
            ( Param* iParam )
            {
                /* Read id layer */
                auto idLayer = iParam -> getString();
                if
                (
                    /* Arguments validation */
                    validate( idLayer != "", "IdLayerIsEmpty", aResults )
                )
                {
                    auto layer = net -> getLayerById( idLayer );
                    if( layer == NULL )
                    {
                        setAnswerResult( aResults, "LayerUnknown" )
                        -> setString( "idLayer", idLayer );
                    }
                    else
                    {
                        char* buffer = NULL;
                        size_t size = 0;

                        /* Get pointer of buffer array */
                        layer -> getValuesBuffer( buffer, size );

                        if( buffer != NULL )
                        {
                            aResults
                            -> setInt( "tick", net -> getTick() )
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
                else
                {
                    setAnswerResult( aResults, "LayerIdIsEmpty" );
                }
                return !isAnswerOk( aResults );
            }
        );
    }


    if( isAnswerOk( aResults ))
    {
        auto errors = aArguments -> getObject( Path{ "errors" });
        if( errors != NULL )
        {
            errors -> loop
            (
                [ this, &aResults ]
                ( Param* iParam )
                {
                    /* Read id layer */
                    auto idLayer = iParam -> getString();
                    if
                    (
                        /* Arguments validation */
                        validate( idLayer != "", "IdLayerIsEmpty", aResults )
                    )
                    {
                        auto layer = net -> getLayerById( idLayer );
                        if( layer == NULL )
                        {
                            setAnswerResult( aResults, "LayerUnknown" )
                            -> setString( "idLayer", idLayer );
                        }
                        else
                        {
                            char* buffer = NULL;
                            size_t size = 0;

                            /* Get pointer of buffer array */
                            layer -> getErrorsBuffer( buffer, size );

                            if( buffer != NULL )
                            {
                                aResults
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
                    else
                    {
                        setAnswerResult( aResults, "LayerIdIsEmpty" );
                    }
                    return !isAnswerOk( aResults );
                }
            );
        }
    }

    return this;
}



/*
    Remote host request weights for selected neurons
*/
ShoggothRpcServer* ShoggothRpcServer::requestWeights
(
    ParamList* aArguments,
    ParamList* aResults
)
{
    aResults -> copyFrom( aArguments );

    auto clientId = aResults -> getString( Path{ "clientId" });
    auto neurons = aResults -> getObject( Path{ "neurons" });


    if
    (
        validate( neurons != NULL, "NeuronsNotFound", aResults ) &&
        validate( clientId != "", "ClientIDNotFound", aResults )
    )
    {
        /* Return positive answer */
        setAnswerResult( aResults, RESULT_OK );

        net
        -> getWeightsExchange()
        -> synchNeuronsByClient( clientId, neurons )
        -> prepareAnswer( clientId, neurons );
    }

    return this;
}



/*
    Drop layer tick and write it in to stat
*/
ShoggothRpcServer* ShoggothRpcServer::dropLayerTick
(
    ParamList* aArguments,
    ParamList* aResults
)
{
    aResults -> copyFrom( aArguments );

    auto layerId = aResults -> getString( Path{ "layerId" });
    auto layer = net -> getLayerList() -> getById( layerId );

    if( layer != NULL )
    {
        layer -> dropTickCount();
        setAnswerResult( aResults, RESULT_OK );
    }
    else
    {
        setAnswerResult( aResults, "LayerNotFound" );
    }

    return this;
}



/*
    Read layers statistics

    arguments
    {
        value:[ layerId1, ... ],
        error:[ layerId1, ... ],
        tick:[ layerId1, ... ]
    }

    result
    {
        value:
        {
            LayerId1: 0xdata,
            ...
        },
        error:
        {
            LayerId1: 0xdata,
            ...
        },
        tick:
        {
            LayerId1: 0xdata,
            ...
        }
    }
*/
ShoggothRpcServer* ShoggothRpcServer::readLayerStat
(
    ParamList* aArguments,
    ParamList* aResults
)
{
    /* Copy answer in to result */
//    aResults -> copyFrom( aArguments );

    /* Return positive answer */
    setAnswerResult( aResults, RESULT_OK );

    buildLayerStatAnswer( aArguments, aResults, "value" );
    buildLayerStatAnswer( aArguments, aResults, "error" );
    buildLayerStatAnswer( aArguments, aResults, "tick" );
    buildLayerStatAnswer( aArguments, aResults, "errorsBeforeChange" );

    return this;
}



ShoggothRpcServer* ShoggothRpcServer::buildLayerStatAnswer
(
    ParamList* aArguments,
    ParamList* aResults,
    string aType
)
{
    auto list = aArguments -> getObject( Path{ aType });

    if( list != NULL )
    {
        list -> loop
        (
            [ this, &aResults, &aType ]
            ( Param* iParam )
            {
                /* Read id layer */
                auto idLayer = iParam -> getString();
                if
                (
                    /* Arguments validation */
                    validate( idLayer != "", "IdLayerIsEmpty", aResults )
                )
                {
                    auto layer = net -> getLayerById( idLayer );
                    if( layer == NULL )
                    {
                        setAnswerResult( aResults, "LayerUnknown" )
                        -> setString( "idLayer", idLayer );
                    }
                    else
                    {
                        auto buffer = BufferD1::create();

                        if( aType == "value")
                            layer -> getChartValues() -> toBuffer( buffer );

                        if( aType == "error")
                            layer -> getChartErrors() -> toBuffer( buffer );

                        if( aType == "tick")
                            layer -> getChartTick() -> toBuffer( buffer );

                        if( aType == "errorsBeforeChange")
                            layer -> getChartErrorsBeforeChange() -> toBuffer( buffer );

                        if( buffer != NULL )
                        {
                            aResults
                            -> setPath( Path{ aType })
                            -> setData
                            (
                                layer -> getId(),
                                buffer -> getMemBuffer(),
                                buffer -> getMemSize()
                            );
                        }

                        buffer -> destroy();
                    }
                }
                else
                {
                    setAnswerResult( aResults, "LayerIdIsEmpty" );
                }
                return !isAnswerOk( aResults );
            }
        );
    }

    return this;
}


/*
    Remote host send nerve weights data
*/
ShoggothRpcServer* ShoggothRpcServer::writeWeights
(
    ParamList* aArguments,
    ParamList* aResults
)
{
//    /* Read id  */
//    auto idFrom = aArguments -> getString( "idFrom" );
//    auto idTo = aArguments -> getString( "idTo" );
//
//    /* Get layer data */
//    char* buffer = NULL;
//    size_t size = 0;
//    aArguments -> getData( "data", buffer, size );
//
//    if
//    (
//        /* Arguments validation */
//        validate( idFrom != "", "IdFromIsEmpty", aResults ) &&
//        validate( idTo != "", "IdFromIsEmpty", aResults ) &&
//        validate( buffer != NULL, "DataIsEmpty", aResults )
//    )
//    {
//        /* Storage data */
//        data
//        -> setPath( Path{ "nerves", idFrom + " " + idTo } )
//        -> setData( "weights", buffer, size );
//        /* Return positive answer */
//    }
    setAnswerResult( aResults, "MethodNotEmplimented" );
    return this;
}



/*
    Remote host request nerve weights layer
*/
ShoggothRpcServer* ShoggothRpcServer::readWeights
(
    ParamList* aArguments,
    ParamList* aResults
)
{
//    /* Read id  */
//    auto idFrom = aArguments -> getString( "idFrom" );
//    auto idTo = aArguments -> getString( "idTo" );
//
//    /* Get layer data */
//    char* buffer = NULL;
//    size_t size = 0;
//
//    data -> getData
//    (
//        Path{ "nerves", idFrom + " " + idTo, "weights" },
//        buffer,
//        size
//    );
//
//    if( buffer != NULL )
//    {
//        aResults -> setData( "data", buffer, size );
//        setAnswerResult( aResults, RESULT_OK );
//    }
//    else
//    {
//        setAnswerResult( aResults, "NerveWeightDataNotFound" );
//    }
    setAnswerResult( aResults, "MethodNotEmplimented" );
    return this;
}




/*
    Remote host request net mode
*/
ShoggothRpcServer* ShoggothRpcServer::getNetMode
(
    ParamList* aArguments,
    ParamList* aResults
)
{
    /* Reflect arguments */
    aResults -> copyFrom( aArguments );

    /* Return result */
    aResults -> setString( "mode", netModeToString( mode ));

    setAnswerResult( aResults, RESULT_OK );

    return this;
}



/*
    Remote host set net mode
*/
ShoggothRpcServer* ShoggothRpcServer::setNetMode
(
    ParamList* aArguments,
    ParamList* aResults
)
{
    /* Reflect arguments */
    aResults -> copyFrom( aArguments );

    /* Change mode */
    auto strMode = aResults -> getString( Path{ "mode" });
    changeNetMode( netModeFromString( strMode ) );

    /* Extract reason of net mode changing */
    auto reason = aArguments -> getByName( Path{ "reason" });
    if( reason != NULL && reason -> isObject() )
    {
        net -> getMon()
        -> now( Path{ "lastMode", strMode, "moment" })
        -> setInt( Path{ "lastMode", strMode, "tick" },  net -> getTick() )
        -> copyObject
        (
            Path{ "lastMode", strMode, "reason" },
            reason -> getObject()
        )
        -> flush();
    }

    return this;
}



/*
    Change net mode and reset layers charts
*/
ShoggothRpcServer* ShoggothRpcServer::changeNetMode
(
    NetMode aMode
)
{
    net -> lock();
    mode = aMode;

    /* Reset statistics for new mode */
    net -> getLayerList() -> loop
    (
        []
        ( void* item )
        {
            auto layer = ( Layer* ) item;
            layer -> getChartErrorsBeforeChange() -> clear();
            return false;
        }
    );
    net -> unlock();
    return this;
}
