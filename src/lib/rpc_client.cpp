#include <iostream>
#include "rpc_client.h"
#include <cstring>


using namespace std;



/*
    Constructor
*/
RpcClient::RpcClient
(
    Log*            aLog,
    SocketDomain    aDomain,
    SocketType      aType
):
SockRpc
(
    aLog,
    aDomain,
    aType
)
{
    request = ParamList::create();
    answer = ParamList::create();
}



/*
    Destructor
*/
RpcClient::~RpcClient()
{
    request -> destroy();
    answer -> destroy();
}


/*
    Create client
*/
RpcClient* RpcClient::create
(
    Log*            aLog,
    SocketDomain    aDomain,
    SocketType      aType

)
{
    return new RpcClient( aLog, aDomain, aType );
}



/*
    Create net client
*/
RpcClient* RpcClient::create
(
    Log*            aLog,
    string          aIp,
    unsigned int    aPort

)
{
    auto result = new RpcClient( aLog );
    result -> setIp( aIp) -> setPort( aPort );
    return result;
}




/*
    On before read
    Method may be overrided
*/
bool RpcClient::onReadBefore
(
    string  /* ip income but not use */
)
{
    return onCallBefore();
}



/*
    On after read
    Method may be overrided
*/
RpcClient* RpcClient::onReadAfter
(
    SockBuffer* aBuffer, /* buffer */
    int
)
{
    auto header = SockRpcHeader::create( aBuffer );

    if( header.isValid() )
    {
        auto buffer = aBuffer -> getBuffer();
        void* pointer = &buffer[ sizeof( SockRpcHeader ) ];

        /* Create parms from buffer */
        answer -> clear() -> fromBuffer( pointer, header.argumentsSize );
        onCallAfter();
    }
    else
    {
        getLog() -> warning( "RPC server sent invalid header for RPC client" );
    }
    return this;
}



/*
    Call method
        short int       sizeName        size of method name
        long long int   sizeArguments   size of data
        long            methodName      method name
        char            arguments       data

*/
RpcClient* RpcClient::call()
{
    if( isOk() )
    {
        openHandle();
        if( isOk() )
        {
            connect();
            if( isOk() )
            {
                if( request != NULL )
                {
                    /* Send buffer to server */
                    write( request );
                    /* Read answer from server */
                    read();
                }
            };
        }
        closeHandle();
    }
    return this;
}



RpcClient* RpcClient::call
(
    string aMethod
)
{
    if( isOk() )
    {
        request -> setString( "method", aMethod );
        call();
        if( isOk() )
        {
            /* Result code processing */
            auto code = getAnswer() -> getString( vector<string>{ "result", "code" });
            if( code != "ok" )
            {
                setCode( code );
                getLog()
                -> warning( "RPC call error" )
                -> prm( "method", aMethod )
                -> prm( "code", code );
            }
        }
    }
    return this;
}



RpcClient* RpcClient::call
(
    int aMethod
)
{
    if( isOk() )
    {
        request -> setInt( "method", aMethod );
        call();
        if( isOk() )
        {
            /* Result code processing */
            auto code = getAnswer() -> getString( vector<string>{ "result", "code" });
            if( code != "ok" )
            {
                setCode( code );
                getLog()
                -> warning( "Rpc call error" )
                -> prm( "method", aMethod )
                -> prm( "code", code );
            }
        }
    }
    return this;
}



/******************************************************************************
    Events
*/


/*
    On call event
    Method may be ovverided
*/
RpcClient* RpcClient::onCallBefore()
{
//    getLog() -> trace( "Client RPC on call before" ) -> lineEnd();
    return this;
}



/*
    Servers On call after event
    Method may be ovverided
*/
RpcClient* RpcClient::onCallAfter()
{
//    getLog() -> trace( "Client RPC on call after" ) -> lineEnd();
    return this;
}




/*
    Override set port
*/
RpcClient* RpcClient::setPort
(
    unsigned short int a    /* port */
)
{
    SockRpc::setPort( a );
    return this;
}



/*
    Override set ip
*/
RpcClient* RpcClient::setIp
(
    string a    /* IP address */
)
{
    SockRpc::setIp( a );
    return this;
}



ParamList* RpcClient::getRequest()
{
    return request;
}



ParamList* RpcClient::getAnswer()
{
    return answer;
}


