#include "shoggoth_rpc_server.h"
#include "io.h"



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

    mon = Mon::create( aNet -> getMonPath( "shoggoth_server_rpc.txt" ))
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

    auto method = aArguments -> getInt( "method" );
    auto methodStr = commandToString( ( Command ) method );

    mon
    -> now( Path{ "last", "momentStr" } )
    -> setString( Path{ "last", "Method" }, methodStr )
    -> addInt( Path{ "stat", methodStr })
    -> flush();

    getLog()
    -> trace( "Called method" )
    -> prm( "Name", methodStr )
    -> prm( "Code", method )
    -> lineEnd();

    switch( method )
    {
        case CMD_READ_NET           :readNet( aArguments, aResults); break;
        case CMD_WRITE_LAYERS       :writeLayers( aArguments, aResults); break;
        case CMD_READ_LAYERS        :readLayers( aArguments, aResults); break;
        case CMD_REQUEST_WEIGHTS    :requestWeights( aArguments, aResults); break;

//        case CMD_WRITE_WEIGHTS  :writeWeights( aArguments, aResults); break;
//        case CMD_READ_WEIGHTS   :readWeights( aArguments, aResults); break;
        default                 :unknownMethod( aArguments, aResults); break;
    }

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

    auto values = aArguments -> getObject( "values" );
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
                            layer -> setValuesFromBuffer( buffer, size );
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
        auto errors = aArguments -> getObject( "errors" );
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
*/
ShoggothRpcServer* ShoggothRpcServer::readLayers
(
    ParamList* aArguments,
    ParamList* aResults
)
{
    /* Return positive answer */
    setAnswerResult( aResults, RESULT_OK );

    auto values = aArguments -> getObject( "values" );
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
        auto errors = aArguments -> getObject( "errors" );
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

    auto clientId = aResults -> getString( "clientId" );
    auto neurons = aResults -> getObject( "neurons" );


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
