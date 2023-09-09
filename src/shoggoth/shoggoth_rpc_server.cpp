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

    /* Create and fill the synchronize object */
    sync = Sync::create( getLog() );

    sync
    -> fill
    (
        getApplication()
        -> getConfig()
        -> getObject( vector<string>{ "net", "layers" } )
    );

    resultFrom( sync );
}



/*
    Constructor
*/
ShoggothRpcServer::~ShoggothRpcServer()
{
    sync -> destroy();
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
    getLog() -> trace( "Method" ) -> prm( "Name", cmdToStr( ( Command ) method ));

    switch( method )
    {
        case CMD_GET_NET        :getNet( aArguments, aResults); break;
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
    -> setPath( vector <string> { "result" })
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
ShoggothRpcServer* ShoggothRpcServer::getNet
(
    ParamList* aArguments,
    ParamList* aResults
)
{
    auto net = getApplication() -> getConfig() -> getObject( "net" );

    if( net != NULL )
    {
        aResults -> copyFrom( net );
        setAnswerResult( aResults, "ok" );
    }
    else
    {
        setAnswerResult( aResults, "NetConfigError" );
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
            -> setPath( vector <string>{ "layers", idLayer } )
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
        vector <string>{ "layers", idLayer, "values" },
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
            -> setPath( vector <string>{ "layers", idLayer } )
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
        vector <string>{ "layers", idLayer, "errors" },
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
