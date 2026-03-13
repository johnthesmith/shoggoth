#include "shoggoth_rpc_client.h"


/*
    Constructor
*/
ShoggothRpcClient::ShoggothRpcClient
(
    /* Application */
    ShoggothApplication*    aApp,
    /* Host for call */
    std::string             aHost,
    /* Port for call */
    int                     aPort,
    /* External answer */
    ParamList*              aExternalAnswer
)
: RpcClient
(
    aApp -> getLogManager(),
    aApp -> getSockManager(),
    SD_INET,
    ST_TCP,
    aHost,
    aPort
)
{
    app = aApp;

    if( aExternalAnswer != NULL )
    {
        setAnswer( aExternalAnswer );
    }
}



/*
    Destructor
*/
ShoggothRpcClient::~ShoggothRpcClient()
{
}



/*
    Call input output operation
*/
ShoggothRpcClient* ShoggothRpcClient::call
(
    /* Command */
    Command aCommand
)
{
    if( this -> isOk() )
    {
        auto methodStr = commandToString( aCommand );
        getLog() -> begin( "Call method" ) -> prm( "method", methodStr );

        getRequest()
        -> setString( Path{ "method" }, methodStr );

        /* Get config for dump details */
        auto dumpDetails = getApplication()
        -> getConfig()
        -> valueExists
        (
            Path{ "remote", "client", "methodDump" },
            commandToString( aCommand )
        );

        if( dumpDetails )
        {
            getLog()
            -> begin( "Call RPC" )
            -> dump( getRequest(), "Request" )
            -> lineEnd()
            ;
        }

        /* Call */
        RpcClient::call();

        if( dumpDetails )
        {
            getLog()
            -> dump( getAnswer(), "Response" )
            -> end();
        }

        /* Dump error code */
        if( !isOk() )
        {
            getLog()
            -> warning( "Shoggot rpc client error" )
            -> prm( "host", getIp())
            -> prm( "port", getPort())
            -> prm( "command", commandToString( aCommand ))
            -> prm( "code", getCode() )
            -> dump( getDetails(), "error details" )
            -> dump( getRequest(), "request" )
            -> lineEnd();
        }

        getLog() -> end();
    }

    return this;
}



/******************************************************************************
    Shoggoth Rpc Client methods
*/


/*
    Clone net from id and version
*/
ShoggothRpcClient* ShoggothRpcClient::cloneNet
(
    std::string aNetId,
    std::string aNetVersion,
    int aGeneration,
    bool aMutation
)
{
    if( isOk() )
    {
        getRequest()
        -> clear()
        -> setString( "parentNetId", aNetId )
        -> setString( "parentNetVersion", aNetVersion )
        -> setInt( "parentGeneration", aGeneration )
        -> setBool( "mutation", aMutation );
        call( CMD_CLONE_NET );
    }
    return this;
}



/*
    Clone net from id and version
*/
ShoggothRpcClient* ShoggothRpcClient::testResult
(
    std::string aNetVersion,
    int aTick,
    std::string aTestResult
)
{
    if( isOk() )
    {
        getRequest()
        -> clear()
        -> setString( "netVersion", aNetVersion )
        -> setInt( "tick", aTick )
        -> setString( "testResult", aTestResult )
        ;

        call( CMD_TEST_RESULT );
    }
    return this;
}



/*
    Switch net from id and version
*/
ShoggothRpcClient* ShoggothRpcClient::switchNet
(
    std::string aId,
    std::string aVersion
)
{
    if( isOk() )
    {
        getRequest()
        -> clear()
        -> setString( "id", aId )
        -> setString( "version", aVersion );

        call( CMD_SWITCH_NET );
    }
    return this;
}



/*
    Read net config
*/
ShoggothRpcClient* ShoggothRpcClient::readNet()
{
    if( isOk() )
    {
        getRequest() -> clear();
        call( CMD_READ_NET );
    }
    return this;
}



/*
    Return server net mode
*/
NetMode ShoggothRpcClient::getNetMode()
{
    NetMode result = NET_MODE_UNKNOWN;
    if( isOk() )
    {
        call( CMD_GET_NET_MODE );
        result = netModeFromString( getAnswer() -> getString( Path{ "mode" }));
    }
    return result;
}



/*
    Set server net mode
*/
ShoggothRpcClient* ShoggothRpcClient::setNetMode
(
    /* New net mode */
    NetMode aMode,
    /* Reason of net mode */
    ParamList* aReason
)
{
    if( this -> isOk() )
    {
        getRequest()
        -> clear()
        -> setString( "mode", netModeToString( aMode ))
        -> copyFrom( "reason", aReason );

        call( CMD_SET_NET_MODE );
    }
    return this;
}



/*
    Read data in to nerve
*/
ShoggothRpcClient* ShoggothRpcClient::readWeights
(
    /* Nerve destination */
    Nerve* aNerve
)
{
    getRequest()
    -> clear()
    -> setString( "idFrom", aNerve -> getParent() -> getId())
    -> setString( "idTo", aNerve -> getChild() -> getId());

    if( call( CMD_READ_WEIGHTS ) -> isOk() )
    {
        char* buffer = NULL;
        size_t size = 0;
        getAnswer() -> getData( Path{ "data" }, buffer, size );
        if( buffer != NULL && size > 0 )
        {
            aNerve -> readFromBuffer( buffer, size );
        }
    }
    return this;
}



/*
    Read data in to nerve
*/
ShoggothRpcClient* ShoggothRpcClient::writeWeights
(
    /* Nerve destination */
    Nerve* aNerve
)
{
    getRequest()
    -> clear()
    -> setString( "idFrom", aNerve -> getParent() -> getId() )
    -> setString( "idTo", aNerve -> getChild() -> getId() )
    -> setData
    (
        "data",
        ( char* ) aNerve -> getWeights(),
        sizeof(real) * aNerve -> getWeightsCount()
    );

    call( CMD_WRITE_WEIGHTS );

    return this;
}




/*
    Request layers statistics
*/
ShoggothRpcClient* ShoggothRpcClient::readLayerStat
(
    /* Layer list for stat request */
    vector<std::string> aStatValue,
    /* Layer list for stat request */
    vector<std::string> aStatError,
    /* Layer list for stat request */
    vector<std::string> aStatTick,
    /* Layer list for stat request */
    vector<std::string> aStatErrorsBeforeChange
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
        getRequest()
        -> clear()
        -> setPath( Path{ "value" }) -> pushVector( aStatValue )
        -> setPath( Path{ "error" }) -> pushVector( aStatError )
        -> setPath( Path{ "tick" }) -> pushVector( aStatTick )
        -> setPath( Path{ "errorsBeforeChange" }) -> pushVector( aStatErrorsBeforeChange );

        call( CMD_READ_LAYER_STAT );
    }

    return this;
}



ShoggothRpcClient* ShoggothRpcClient::readNetInfo()
{
    getRequest() -> clear();
    call( CMD_READ_NET_INFO );
    return this;
}



/*
    Write values and errors to server
*/
ShoggothRpcClient* ShoggothRpcClient::writeLayers
(
    /* Net obejct */
    Net* net,
    /* List of layers id for values */
    vector<string> aValues,
    /* List of layers id for errors */
    vector<string> aErrors
)
{
    if
    (
        isOk()
        && ( aValues.size() > 0 || aErrors.size() > 0 )
    )
    {
        getRequest() -> clear();

        net -> lock();

        /* Build values */
        for( auto id : aValues )
        {
            auto layer = net -> getLayerList() -> getById( id );
            if( layer != NULL )
            {
                /* Buffer pointer */
                char* buffer = NULL;
                /* Size of buffer */
                size_t size = 0;

                layer -> getValuesBuffer( buffer, size );

                if( buffer != NULL )
                {
                    getRequest()
                    -> setPath( Path{ "values" })
                    -> setData( layer -> getId(), (char*) buffer, size );
                }
            }
        }

        /* Build errors */
        for( auto id : aErrors )
        {
            auto layer = net -> getLayerList() -> getById( id );
            if( layer != NULL )
            {
                /* Buffer pointer */
                char* buffer = NULL;
                /* Size of buffer */
                size_t size = 0;

                layer -> getErrorsBuffer( buffer, size );

                if( buffer != NULL )
                {
                    getRequest()
                    -> setPath( Path{ "errors" })
                    -> setData( layer -> getId(), (char*) buffer, size );
                }
            }
        }
        net -> unlock();
        call( CMD_WRITE_LAYERS );
    }

    return this;
}



/*
    Read value from io
    It locks the net
*/
ShoggothRpcClient* ShoggothRpcClient::readLayers
(
    /* Net obejct */
    Net* net,
    /* List of layers id for values */
    vector<string> aValues,
    /* List of layers id for errors */
    vector<string> aErrors
)
{
    if
    (
        isOk()
        && ( aValues.size() > 0 || aErrors.size() > 0 )
    )
    {
        /* Clear request arguments */
        getRequest() -> clear();

        /* Build request values */
        getRequest()
        -> setPath( Path{ "values" } )
        -> pushVector( aValues );

        getRequest()
        -> setPath( Path{ "errors" } )
        -> pushVector( aErrors );

        /* Call server and apply the answer */
        if( call( CMD_READ_LAYERS ) -> isOk() )
        {
            net -> lock();

            auto layers = net -> getLayerList();

            /* Set tick from server net */
            net -> setTick( getAnswer() -> getInt( Path{ "tick" } ));

            /* Loop for values */
            for( auto id : aValues )
            {
                auto layer = layers -> getById( id );
                if( layer != NULL )
                {
                    char* buffer = NULL;
                    size_t size = 0;

                    getAnswer() -> getData
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
                auto layer = layers -> getById( id );
                if( layer != NULL )
                {
                    char* buffer = NULL;
                    size_t size = 0;

                    getAnswer() -> getData
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

            net -> unlock();
        }
    }
    return this;
}




//
///*
//    Request weights for neurons
//*/
//Net* Net::requestWeights()
//{
//    if( weightsExchange -> needRequest() )
//    {
//        getLog() -> begin( "Request weights" );
//
//        /* Create IO object and define request */
//        auto io = Io::create( this );
//        auto request = io -> getRequest();
//        weightsExchange -> prepareRequest( request );
//
//        /* Call server and apply the answer */
//        if( io -> call( CMD_REQUEST_WEIGHTS ) -> isOk() )
//        {
//            lock();
//            weightsExchange
//            -> checkClearSignal()
//            -> readAnswer( io -> getAnswer() );
//            unlock();
//        }
//        else
//        {
//            /* Call error */
//        }
//
//        io -> destroy();
//
//        getLog() -> end();
//    }
//    return this;
//}
