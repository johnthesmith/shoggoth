#pragma once

/*
    Socket work
    https://rsdn.org/article/unix/sockets.xml
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <functional>       /* for lyambda */

#include "result.h"
#include "sock_buffer.h"


/* Predeclaration sock for events definitions */
class Sock;



enum SocketDomain
{
    SD_INET     = AF_INET,  /* TCP/IP V4 */
    SD_UNIX     = AF_UNIX,  /* Local host socket */
    SD_IPX      = AF_IPX,   /* IPX protocol */
    SD_INTE6    = AF_INET6  /* TCP/IP V6 */
};



enum SocketType
{
    ST_TCP      = SOCK_STREAM,
    SD_UDP      = SOCK_DGRAM,
    SD_RAW      = SOCK_RAW
};



/*
    Socket class definition
*/
class Sock : public Result
{
    private:

    int                 handle              = -1;       /* handle of socket */
    SocketDomain        domain              = SD_INET;
    unsigned int        queueSize           = 50;       /* Resuest queue size */
    unsigned int        packetSize          = 512;      /* Data packet size */
    bool                connected           = false;

    char*               resultBuffer        =  NULL;
    unsigned int        resultBufferSize    = 0;
    string              remoteAddress       = "";
    string              ip                  = "127.0.0.1";
    unsigned short int  port                = 42;


    /*
        Read beffer
    */
    Sock* readInternal
    (
        int,        /* request handle */
        string = "" /* remote ip address */
    );


    public:



    /*
        Destructor
    */
    ~Sock();



    /*
        Create socket
    */
    static Sock* create();



    /*
        Destroy socket
    */
    void destroy();



    /*
        Create socket handle
    */
    Sock* openHandle
    (
        SocketDomain        = SD_INET,
        SocketType          = ST_TCP
    );



    /*
        Close socket handle if they was opened
    */
    Sock* closeHandle();



    /*
        Socket is created
        Return true when handle contains positive value
    */
    bool isOpen();



    /*
        Socket connected status
        Return true when socket connected
    */
    bool isConnected();



    /*
        Socket role server
    */
    Sock* listen();



    /*
        Socket connect to server like client
    */
    Sock* connect();



    /*
        Disconnect from remote host or listen mode
    */
    Sock* disconnect();



    /*
        Disconnect
    */
    Sock* down();



    /*
        Write buffer to socket
    */
    Sock* write
    (
        const void*,    /* Buffer */
        const size_t,   /* Size of buffer */
        int = -1        /* Handle for writing */
    );



    /*
        Write string to socket
    */
    Sock* write
    (
        string
    );



    /*
        Read buffer from socket
        Result will return to events
            onReadBefore
            onRead
            onReadAfter
    */
    Sock* read();



    /*
        Convert string to numeric IP address
    */
    static unsigned int stringToIp
    (
        string
    );




    static string ipToString
    (
        unsigned int
    );




    /*
        Set ip address
    */
    Sock* setIp
    (
        string /* ip address */
    );



    /*
        Return ip address
    */
    string getIp();



    /*
        Set port
    */
    virtual Sock* setPort
    (
        unsigned short int  /* port */
    );



    /*
        Return port
    */
    unsigned short int getPort();



    /*
        Set packet size
    */
    Sock* setPacketSize
    (
        unsigned int
    );



    /*
        Return packet size
    */
    unsigned int getPacketSize();


    Sock* deleteBuffer();



    /*
        Return remote address
    */
    string getRemoteAddress();



    /******************************************************************************
        Events
    */


    /*
        On before read
        Method may be overrided
    */
    virtual Sock* onConnectBefore();



    /*
        On error up
        Method may be overrided
    */
    virtual Sock* onConnectError();



    /*
        On success up
        Method may be overrided
    */
    virtual Sock* onConnectSuccess();



    /*
        On after up
        Method may be overrided
    */
    virtual Sock* onConnectAfter();



    /*
        On before read
        Method may be overrided
    */
    virtual bool onReadBefore
    (
        string  /* ip */
    );



    /*
        On before read
        Method may be overrided
    */
    virtual bool onRead
    (
        SockBuffer* /* buffer */
    );



    /*
        On after read
        Method may be overrided
    */
    virtual Sock* onReadAfter
    (
        SockBuffer*,    /* Buffers */
        int             /* Handle for remote connection */
    );



    /*
        On Lisen before
        Method may be overrided
    */
    virtual Sock* onListenBefore
    (
        unsigned short int  /* Port */
    );



    /*
        On Lisen after
        Method may be overrided
    */
    virtual Sock* onListenAfter
    (
        unsigned short int /* Port */
    );
};
