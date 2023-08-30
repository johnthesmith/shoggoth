#include "shoggoth_rpc_server.h"


/*
    Constructor
*/
ShoggothRpcServer::ShoggothRpcServer
(
    ShoggothApplication*    aApp,
    SocketDomain            aDomain,
    SocketType              aType
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
        getApp()
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
    ShoggothApplication*    aApp,
    SocketDomain            aDomain,
    SocketType              aType

)
{
    return new ShoggothRpcServer( aApp, aDomain, aType );
}




ShoggothApplication* ShoggothRpcServer::getApplication()
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
        case CMD_GET_SYNC       :getSync( aArguments, aResults); break;
        case CMD_WRITE_LAYER    :writeLayer( aArguments, aResults); break;
        case CMD_READ_LAYER     :readLayer( aArguments, aResults); break;
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
    Request sync
*/
ShoggothRpcServer* ShoggothRpcServer::getSync
(
    ParamList* aArguments,
    ParamList* aResults
)
{
    aResults -> setObject( "sync", ParamList::create() -> copyFrom( sync ));
    setAnswerResult( aResults, "ok" );
    return this;
}



/*
    Remote host send layer data

    Arguments
        idLayer
        idClient
        direction
        from
        to
        data
    Results
        sync
        result
*/
ShoggothRpcServer* ShoggothRpcServer::writeLayer
(
    ParamList* aArguments,
    ParamList* aResults
)
{
    /* Read id layer */
    auto idLayer = aArguments -> getString( "idLayer" );
    auto idClient = aArguments -> getString( "idClient" );
    auto direction = aArguments -> getInt( "direction" );
    auto from = aArguments -> getInt( "from" );
    auto to = aArguments -> getInt( "to" );

    if
    (
        /* Arguments validation */
        validate( idClient != "", "IdClientIsEmpty", aResults ) &&
        validate( idLayer != "", "IdLayerIsEmpty", aResults ) &&
        validate
        (
            direction == CALC_FORWARD ||
            direction == CALC_BACKWARD,
            "UnknownDirection",
            aResults
        )
    )
    {
        /* Get layer data */
        char* buffer = NULL;
        size_t size = 0;
        aArguments -> getData( "data", buffer, size );
        if( validate( buffer != NULL, "DataIsEmpty", aResults ))
        {
            /* Storage data */
            data -> setData( idLayer, buffer, size );

            /* Store to sync */
            switch( direction )
            {
                case CALC_FORWARD: sync -> setForward( idClient, idLayer ); break;
                case CALC_BACKWARD: sync -> setBackward( idClient, idLayer ); break;
            }

            aResults -> copyFrom( "sync", sync );

            /* Output sync to log */
            sync -> toLog();

            /* Return positive answer */
            setAnswerResult( aResults, "ok" );
        }
    }


    if( sync -> isComplete() )
    {
        /* End of loop */
        sync -> fill
        (
            getApp()
            -> getConfig()
            -> getObject( vector<string>{ "net", "layers" } )
        );
    }

    return this;
}



/*
    Remote host request the layer
*/
ShoggothRpcServer* ShoggothRpcServer::readLayer
(
    ParamList* aArguments,
    ParamList* aResults
)
{
    auto idLayer = aArguments -> getString( "idLayer" );

    char* buffer = NULL;
    size_t size = 0;

    data -> getData( idLayer, buffer, size );

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
    Return application
*/
ShoggothApplication* ShoggothRpcServer::getApp()
{
    return app;
}


