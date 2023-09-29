#include "shoggoth_rpc_server.h"


/*
    Constructor
*/
ShoggothRpcServer::ShoggothRpcServer
(
    Application*    aApp,
    SocketDomain    aDomain,
    SocketType      aType
)
:RpcServer
(
    aApp -> getLog(),
    aDomain,
    aType
)
{
    app = aApp;
    data = ParamList::create();
    netConfig = Json::create();
    getLog() -> trace( "Shoggoth server created" );
}



/*
    Constructor
*/
ShoggothRpcServer::~ShoggothRpcServer()
{
    getLog() -> trace( "Shoggoth server destroy" );
    netConfig -> destroy();
    data -> destroy();
}



/*
    Create socket
*/
ShoggothRpcServer* ShoggothRpcServer::create
(
    Application*    aApp,
    SocketDomain    aDomain,
    SocketType      aType

)
{
    return new ShoggothRpcServer( aApp, aDomain, aType );
}




Application* ShoggothRpcServer::getApplication()
{
    return app;
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

    getLog() -> trace( commandToString( ( Command ) method ));

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
    auto netFile = getApplication()
    -> getConfig()
    -> getString( Path{ "io", "net" } );

    /* Read config if updated */
    if( checkFileUpdate( netFile, lastNetConfigUpdate ))
    {
        netConfig -> fromFile( netFile );
    }

    /* Return answer for client */
    if( netConfig -> isOk() )
    {
        netConfig -> copyTo( aResults );
        setAnswerResult( aResults, "ok" );
    }
    else
    {
        setAnswerResult( aResults, netConfig -> getCode() );
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
        /* Get layer data */
        char* buffer = NULL;
        size_t size = 0;
        aArguments -> getData( "data", buffer, size );
        if( validate( buffer != NULL, "DataIsEmpty", aResults ))
        {
            /* Storage data */
            data
            -> setPath( Path{ "layers", idLayer } )
            -> setData
            (
                "values",
                buffer,
                size
            );
            /* Return positive answer */
            setAnswerResult( aResults, "ok" );
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
    auto idLayer = aArguments -> getString( "idLayer" );

    char* buffer = NULL;
    size_t size = 0;

    data -> getData
    (
        Path{ "layers", idLayer, "values" },
        buffer,
        size
    );

    if( buffer != NULL )
    {
        aResults -> setData( "data", buffer, size );
        setAnswerResult( aResults, "ok" );
    }
    else
    {
        setAnswerResult( aResults, "LayerDataNotFound" );
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
        /* Get layer data */
        char* buffer = NULL;
        size_t size = 0;
        aArguments -> getData( "data", buffer, size );

        if( validate( buffer != NULL, "DataIsEmpty", aResults ))
        {
            /* Storage data */
            data
            -> setPath( Path{ "layers", idLayer } )
            -> setData
            (
                "errors",
                buffer,
                size
            );
            /* Return positive answer */
            setAnswerResult( aResults, "ok" );
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
    auto idLayer = aArguments -> getString( "idLayer" );

    char* buffer = NULL;
    size_t size = 0;

    data -> getData
    (
        Path{ "layers", idLayer, "errors" },
        buffer,
        size
    );

    if( buffer != NULL )
    {
        aResults -> setData( "data", buffer, size );
        setAnswerResult( aResults, "ok" );
    }
    else
    {
        setAnswerResult( aResults, "LayerDataNotFound" );
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
    /* Read id  */
    auto idFrom = aArguments -> getString( "idFrom" );
    auto idTo = aArguments -> getString( "idTo" );

    /* Get layer data */
    char* buffer = NULL;
    size_t size = 0;
    aArguments -> getData( "data", buffer, size );

    if
    (
        /* Arguments validation */
        validate( idFrom != "", "IdFromIsEmpty", aResults ) &&
        validate( idTo != "", "IdFromIsEmpty", aResults ) &&
        validate( buffer != NULL, "DataIsEmpty", aResults )
    )
    {
        /* Storage data */
        data
        -> setPath( Path{ "nerves", idFrom + " " + idTo } )
        -> setData( "weights", buffer, size );
        /* Return positive answer */
        setAnswerResult( aResults, "ok" );
    }
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
    /* Read id  */
    auto idFrom = aArguments -> getString( "idFrom" );
    auto idTo = aArguments -> getString( "idTo" );

    /* Get layer data */
    char* buffer = NULL;
    size_t size = 0;

    data -> getData
    (
        Path{ "nerves", idFrom + " " + idTo, "weights" },
        buffer,
        size
    );

    if( buffer != NULL )
    {
        aResults -> setData( "data", buffer, size );
        setAnswerResult( aResults, "ok" );
    }
    else
    {
        setAnswerResult( aResults, "NerveWeightDataNotFound" );
    }
    return this;
}
