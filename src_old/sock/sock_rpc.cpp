#include "sock_rpc.h"

#include <iostream>
#include <cstring>


using namespace std;



/*
    Constructor
*/
SockRpc::SockRpc
(
    LogManager*     aLogManager,
    SockManager*    aSockManager,
    SocketDomain    aDomain,
    SocketType      aType,
    string          aIp,
    int             aPort
):
Sock
(
    aSockManager,
    aDomain,
    aType,
    aIp,
    aPort
)
{
    logManager = aLogManager;
}



/*
    Create socket
*/
SockRpc* SockRpc::create
(
    LogManager*     aLogManager,
    SockManager*    aSockManager,
    SocketDomain    aDomain,
    SocketType      aType,
    string          aIp,
    int             aPort
)
{
    return new SockRpc
    (
        aLogManager,
        aSockManager,
        aDomain,
        aType,
        aIp,
        aPort
    );
}



/*
    Return Log object
*/
Log* SockRpc::getLog()
{
    return logManager -> getLog();
}



/*
    Return current code to log
*/
SockRpc* SockRpc::resultToLog()
{
    if( !isOk() )
    {
        getLog() -> warning( getCode() ) -> lineEnd();
    }
    return this;
}



/******************************************************************************
    Events
*/



/*
    On before read
*/
bool SockRpc::onReadBefore
(
    string aIp
)
{
    return true;
}




/*
    On before read
    Method may be overrided
*/
bool SockRpc::onRead
(
    SockBuffer* aBuffer /* buffers parts */
)
{
    auto header = SockRpcHeader::create( aBuffer );
    return header.isValid() && !header.isFull( aBuffer );
}




/*
    On before read
    Method may be overrided
*/
SockRpc* SockRpc::onConnectBefore()
{
//    getLog()
//    -> trace( "onConnectBefore" )
//    -> prm( "Address", getIp() )
//    -> prm( "Port", getPort() );
    return this;
}



/*
    On error up
    Method may be overrided
*/
SockRpc* SockRpc::onConnectError()
{
    getLog()
    -> warning( "onConnectError" )
    -> prm( "Address", getIp() )
    -> prm( "Port", getPort() )
    -> prm( "Error", getCode());
    return this;
}



/*
    On success up
    Method may be overrided
*/
SockRpc* SockRpc::onConnectSuccess()
{
//    getLog()
//    -> trace( "onConnectSuccess" )
//    -> prm( "Address", getIp() )
//    -> prm( "Port", getPort() );
    return this;
}



/*
    On after up
    Method may be overrided
*/
SockRpc* SockRpc::onConnectAfter()
{
//    getLog()
//    -> trace( "onConnectAfter" )
//    -> prm( "Address", getIp() )
//    -> prm( "Port", getPort() );
    return this;
}



/*
    Write buffer to socket with RPC header
*/
SockRpc* SockRpc::write
(
    ParamList* aParams,    /* ParamList for writing */
    int aHandle            /* Handle for writing */
)
{
    /* Build answer buffer */
    void* buffer = NULL; // TODO поменять на char*[]
    size_t bufferSize = 0;
    aParams -> toBuffer( buffer, bufferSize );

    /* Create net header */
    auto header = SockRpcHeader::create( bufferSize );

    /* Calculate size of buffer */
    size_t netBufferSize = header.getFullSize();

    /* Create memory for net buffer */
    char* netBuffer = new char[ netBufferSize ];

    /* Buld buffer */
    unsigned int shift = 0;
    memcpy( &netBuffer[ shift ], &header, sizeof( SockRpcHeader ));
    shift += sizeof( SockRpcHeader );
    memcpy( &netBuffer[ shift ], (void*) buffer, bufferSize );

    /* Write to socket */
    Sock::write( netBuffer, netBufferSize, aHandle );

    /* Free memory */
    delete [] netBuffer;
    ::operator delete( buffer );

    return this;
}


/******************************************************************************
    Header
*/


/*
    Return filled SockRpcHeader
*/
SockRpcHeader SockRpcHeader::create
(
    size_t aArgumentsSize
)
{
    SockRpcHeader result;

    result.prefix[ 0 ]      = 'R';
    result.prefix[ 1 ]      = 'P';
    result.prefix[ 2 ]      = 'C';
    result.argumentsSize    = aArgumentsSize;

    return result;
}



/*
    Return filled header from buffer
*/
SockRpcHeader SockRpcHeader::create
(
    SockBuffer* aBuffer
)
{
    SockRpcHeader result;

    auto firstBlock = aBuffer -> getFirstBuffer();
    unsigned int headerSize = sizeof( SockRpcHeader );

    if( firstBlock != NULL && firstBlock -> getReadSize() >= headerSize )
    {
        auto pointer = firstBlock -> getPointer();
        memcpy( &result, pointer, sizeof( SockRpcHeader ));
    }

    return result;
}



/*
    Check valid of SockRpcHeader
*/
bool SockRpcHeader::isValid()
{
    return prefix[ 0 ] == 'R' && prefix[ 1 ] == 'P' && prefix[ 2 ] == 'C';
}



/*
    Check valid of SockRpcHeader
*/
bool SockRpcHeader::isFull
(
    SockBuffer* aBuffer
)
{
    return getFullSize() <= aBuffer -> calcReadSize();
}



/*
    Return full size of rpc
*/
size_t SockRpcHeader::getFullSize()
{
    return sizeof( SockRpcHeader ) + argumentsSize;
}



