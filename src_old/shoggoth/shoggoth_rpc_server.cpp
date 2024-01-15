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
    getLog() -> trace( "Shoggoth server created" ) -> lineEnd();
}



/*
    Constructor
*/
ShoggothRpcServer::~ShoggothRpcServer()
{
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




Application* ShoggothRpcServer::getApplication()
{
    return net -> getApplication();
}



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

    getLog() -> trace( commandToString( ( Command ) method )) -> lineEnd();

    switch( method )
    {
        case CMD_READ_NET       :readNet( aArguments, aResults); break;
        case CMD_WRITE_VALUES   :writeValues( aArguments, aResults); break;
        case CMD_READ_VALUES    :readValues( aArguments, aResults); break;
        case CMD_WRITE_ERRORS   :writeErrors( aArguments, aResults); break;
        case CMD_READ_ERRORS    :readErrors( aArguments, aResults); break;
        case CMD_WRITE_WEIGHTS  :writeWeights( aArguments, aResults); break;
        case CMD_READ_WEIGHTS   :readWeights( aArguments, aResults); break;

        default                 :unknownMethod( aArguments, aResults); break;
    }

    return this;
}



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

ShoggothRpcServer* ShoggothRpcServer::setAnswerResult
(
    ParamList* aAnswer,
    string aCode
)
{
    aAnswer
    -> setPath( Path { "result" })
    -> setString(  "code", aCode );
    return this;
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
    ParamList* json = NULL;
    auto io = Io::create( net );
    if( io -> call( CMD_READ_NET ) -> isOk() )
    {
        json = io -> getAnswer();
        /* Apply net */
        if( json != NULL )
        {
            json -> copyTo( aResults );
            setAnswerResult( aResults, "ok" );
        }
        else
        {
            setAnswerResult( aResults, io -> getCode() );
        }
    }
    io -> destroy();

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
ShoggothRpcServer* ShoggothRpcServer::writeValues
(
    ParamList* aArguments,
    ParamList* aResults
)
{
    /* Read id layer */
    auto idLayer = aArguments -> getString( "idLayer" );
    if
    (
        /* Arguments validation */
        validate( idLayer != "", "IdLayerIsEmpty", aResults )
    )
    {
        auto layer = net -> getLayerById( idLayer );
        if( layer == NULL )
        {
            setAnswerResult( aResults, "LayerUnknown" );
        }
        else
        {
            /* Get layer data */
            char* buffer = NULL;
            size_t size = 0;
            aArguments -> getData( "data", buffer, size );

            if( validate( buffer != NULL, "DataIsEmpty", aResults ))
            {
                layer -> valuesFromBuffer( buffer, size );
                /* Return positive answer */
                setAnswerResult( aResults, "ok" );
            }
        }
    }
    return this;
}



/*
    Remote host request the layer
*/
ShoggothRpcServer* ShoggothRpcServer::readValues
(
    ParamList* aArguments,
    ParamList* aResults
)
{
    /* Read id layer */
    auto idLayer = aArguments -> getString( "idLayer" );
    if
    (
        /* Arguments validation */
        validate( idLayer != "", "IdLayerIsEmpty", aResults )
    )
    {
        auto layer = net -> getLayerById( idLayer );
        if( layer == NULL )
        {
            setAnswerResult( aResults, "LayerUnknown" );
        }
        else
        {
            char* buffer = NULL;
            size_t size = 0;

            /* Get pointer of buffer array */
            layer -> getValuesBuffer( buffer, size );

            if( buffer != NULL )
            {
                aResults -> setData( "data", buffer, size );
                /* Return positive answer */
                setAnswerResult( aResults, "ok" );
            }
            else
            {
                setAnswerResult( aResults, "LayerDataNotFound" );
            }
        }
    }

    return this;
}




/*
    Remote host send layer errors data

    Arguments
        idLayer
        data
    Results
        result
*/
ShoggothRpcServer* ShoggothRpcServer::writeErrors
(
    ParamList* aArguments,
    ParamList* aResults
)
{
    /* Read id layer */
    auto idLayer = aArguments -> getString( "idLayer" );
    if
    (
        /* Arguments validation */
        validate( idLayer != "", "IdLayerIsEmpty", aResults )
    )
    {
        auto layer = net -> getLayerById( idLayer );
        if( layer == NULL )
        {
            setAnswerResult( aResults, "LayerUnknown" );
        }
        else
        {
            /* Get layer data */
            char* buffer = NULL;
            size_t size = 0;
            aArguments -> getData( "data", buffer, size );
            if( validate( buffer != NULL, "DataIsEmpty", aResults ))
            {
                layer -> errorsFromBuffer( buffer, size );
                /* Return positive answer */
                setAnswerResult( aResults, "ok" );
            }
        }
    }
    return this;
}



/*
    Remote host request the layer errors
*/
ShoggothRpcServer* ShoggothRpcServer::readErrors
(
    ParamList* aArguments,
    ParamList* aResults
)
{
    /* Read id layer */
    auto idLayer = aArguments -> getString( "idLayer" );
    if
    (
        /* Arguments validation */
        validate( idLayer != "", "IdLayerIsEmpty", aResults )
    )
    {
        auto layer = net -> getLayerById( idLayer );
        if( layer == NULL )
        {
            setAnswerResult( aResults, "LayerUnknown" );
        }
        else
        {
            char* buffer = NULL;
            size_t size = 0;

            /* Get pointer of buffer array */
            layer -> getErrorsBuffer( buffer, size );

            if( buffer != NULL )
            {
                aResults -> setData( "data", buffer, size );
                /* Return positive answer */
                setAnswerResult( aResults, "ok" );
            }
            else
            {
                setAnswerResult( aResults, "LayerDataNotFound" );
            }
        }
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
//        setAnswerResult( aResults, "ok" );
//    }
//    else
//    {
//        setAnswerResult( aResults, "NerveWeightDataNotFound" );
//    }
    setAnswerResult( aResults, "MethodNotEmplimented" );
    return this;
}
