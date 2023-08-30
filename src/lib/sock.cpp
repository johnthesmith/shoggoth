#include <string>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <cstring>
#include "sock.h"

#include "../lib/heap.h"
#include "../lib/buffer_to_hex.h"




/*
    Destructor
*/
Sock::~Sock()
{
    closeHandle();
    deleteBuffer();
}



/*
    Create socket
*/
Sock* Sock::create()
{
    return new Sock();
}



/*
    Destroy socket
*/
void Sock::destroy()
{
    delete this;
}



/*
    Open socket handle
*/
Sock* Sock::openHandle
(
    SocketDomain        aDomain,
    SocketType          aType
)
{
    if( isOk() && !isOpen() )
    {
        domain  = aDomain;
        handle = socket( aDomain, aType, 0 );
        if( !isOpen() )
        {
            setCode( "SocketCreateError" );
        }
    }
    return this;
}



/*
    Close socket handle if they was opened
*/
Sock* Sock::closeHandle()
{
    if( isOpen() )
    {
        close( handle );
        handle = -1;
        connected = false;
    }
    return this;
}



/*
    The object has a socket handle
    Return true when handle contains positive value
*/

bool Sock::isOpen()
{
    return handle >= 0;
}



/*
    Connect to remote host
*/
Sock* Sock::connect()
{
    if( isOk() )
    {
        if( isOpen() )
        {
            /* On before */
            onConnectBefore();

            /* Create address structure */
            struct sockaddr_in addr;
            addr.sin_family = domain;
            addr.sin_port = htons( port );
            addr.sin_addr.s_addr = Sock::stringToIp( ip );

            connected = ::connect( handle, (struct sockaddr *)&addr, sizeof( addr ) ) >= 0;
            if( !isConnected() )
            {
                setCode( "ConnectError" );
                onConnectError();
            }
            else
            {
                onConnectSuccess();
            }

            /* On after */
            onConnectAfter();
        }
        else
        {
            setCode( "SocketIsNotOpen" );
        }
    }

    return this;
}



/*
    Connect to remote host
*/
Sock* Sock::disconnect()
{
    closeHandle();
    return this;
}



/*
    Socket role server
*/
Sock* Sock::listen()
{
    if( isOk() )
    {
        if( isOpen() )
        {
            onListenBefore( port );

            struct sockaddr_in addr;
            addr.sin_family = domain;
            addr.sin_port = htons( port );
            addr.sin_addr.s_addr = htonl( INADDR_ANY );

            auto r = bind
            (
                handle,
                ( struct sockaddr* )&addr,
                sizeof( addr )
            );

            /* bind socket */
            if( r < 0 )
            {
                setCode( "ServerCreateError" );
            }
            else
            {
                r = ::listen( handle, queueSize );
                if( r < 0 )
                {
                    setCode( "ServerListenError" );
                }
                else
                {
                    connected = true;           /* Set connected flag */
                    bool terminated = false;    /* Listening will continue while not trminated */

                    while( !terminated )
                    {
                        /* Define address structiure and his size */
                        struct sockaddr remoteAddressStruct;
                        unsigned int remoteSize = sizeof( remoteAddressStruct );

                        /* The socket waiting request */
                        int request = accept( handle, &remoteAddressStruct, &remoteSize );

                        if( request < 0 )
                        {
                            setCode( "RequestError" );
                            terminated = true;
                        }
                        else
                        {
                            auto remoteAddress = ipToString
                            (
                                (( sockaddr_in* ) &remoteAddressStruct ) -> sin_addr.s_addr
                            );
                            readInternal( request, remoteAddress );
                            close( request );
                        }
                    }
                }
            }
            onListenAfter( port );
        }
        else
        {
            setCode( "SocketIsNotOpen" );
        }
    }
    return this;
}



/*
    Write buffer to socket
*/
Sock* Sock::write
(
    const void*     aBuffer,
    const size_t    aSize,
    int             aHandle
)
{
    if( isOk() )
    {
        if( !isConnected() )
        {
            setCode( "SocketIsNotConnectedForWrite" );
        }
        else
        {
            if
            (
                send
                (
                    aHandle == -1 ? handle : aHandle,
                    aBuffer,
                    aSize,
                    0
                ) < 0
            )
            {
                setCode( "SocketWriteError" );
            }
        }
    }

    return this;
}



/*
    Write string
*/
Sock* Sock::write
(
    string a
)
{
    write( a.c_str(), a.size() );
    return this;
}



/*
    Read buffer
*/
Sock* Sock::read()
{
    readInternal( handle );
    return this;
}



/*
    Read beffer
*/
Sock* Sock::readInternal
(
    int aHandle,    /* request handle */
    string aIp      /* remote ip address */
)
{
    if( isOk() )
    {
        if( onReadBefore( aIp ))
        {
            if( !isConnected() )
            {
                setCode( "NoConnect" );
            }
            else
            {
                /* Create buffer */
                auto buffer = SockBuffer::create();

                /* Read loop */
                bool read = true;
                while( read )
                {
                    auto item = buffer -> add( packetSize );
                    auto bytesRead = recv( aHandle, item -> getPointer(), packetSize, 0 );
                    item -> setReadSize( bytesRead );
                    read = bytesRead > 0 ? onRead( buffer ) : false;
                }

                /* Finall call onReadAfter */
                onReadAfter( buffer, aHandle );

                /* Destroy buffer */
                buffer -> destroy();
            }
        }
    }
    return this;
}



/*
    Return true if socket connected
*/
bool Sock::isConnected()
{
    return connected;
}



/*
    Convert string to numeric IP address
*/
unsigned int Sock::stringToIp
(
    string aString
)
{
    int a, b, c, d;
    char arr[4];

    sscanf( aString.c_str(), "%d.%d.%d.%d", &a, &b, &c, &d );

    arr[0] = a;
    arr[1] = b;
    arr[2] = c;
    arr[3] = d;

    return *( unsigned int *) arr;
}



string Sock::ipToString
(
    unsigned int a
)
{
    auto ip = ( unsigned char* )&a;
    return
    to_string( ip[ 0 ] ) + "." +
    to_string( ip[ 1 ] ) + "." +
    to_string( ip[ 2 ] ) + "." +
    to_string( ip[ 3 ] );
}



Sock* Sock::deleteBuffer()
{
    if( resultBuffer != NULL )
    {
        delete [] resultBuffer;
        resultBuffer = NULL;
        resultBufferSize = 0;
    }
    return this;
}



/******************************************************************************
    Setters and getters
*/


/*
    Set ip address
*/
Sock* Sock::setIp
(
    string a    /* ip address */
)
{
    ip = a;
    return this;
}



/*
    Return ip address
*/
string Sock::getIp()
{
    return ip;
}



/*
    Set port
*/
Sock* Sock::setPort
(
    unsigned short int a    /* port */
)
{
    port = a;
    return this;
}



/*
    Return port
*/
unsigned short int Sock::getPort()
{
    return port;
}



/*
    Set packet size
*/
Sock* Sock::setPacketSize
(
    unsigned int a
)
{
    packetSize = a;
    return this;
}



/*
    Return packet size
*/
unsigned int Sock::getPacketSize()
{
    return packetSize;
}



/*
    Return remote address
*/
string Sock::getRemoteAddress()
{
    return remoteAddress;
}



/******************************************************************************
    Events
*/




/*
    On before read
    Method may be overrided
*/
Sock* Sock::onConnectBefore()
{
    /* Empty */
    return this;
}



/*
    On error up
    Method may be overrided
*/
Sock* Sock::onConnectError()
{
    /* Empty */
    return this;
}



/*
    On success up
    Method may be overrided
*/
Sock* Sock::onConnectSuccess()
{
    /* Empty */
    return this;
}



/*
    On after up
    Method may be overrided
*/
Sock* Sock::onConnectAfter()
{
    /* Empty */
    return this;
}



/*
    On before read
    Method may be overrided
*/
bool Sock::onReadBefore
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
bool Sock::onRead
(
    SockBuffer* /* buffer */
)
{
    return false;
}



/*
    On after read
    Method may be overrided
*/
Sock* Sock::onReadAfter
(
    SockBuffer*,    /* buffer */
    int             /* Handle for remote connection */
)
{
    /* Empty */
    return this;
}



/*
    On Listen before
    Method may be overrided
*/
Sock* Sock::onListenBefore
(
    unsigned short int aPort
)
{
    return this;
}



/*
    On Listen after
    Method may be overrided
*/
Sock* Sock::onListenAfter
(
    unsigned short int aPort
)
{
    return this;
}
