#pragma once

/*
    Socket work
    https://rsdn.org/article/unix/sockets.xml
*/



#include "log.h"
#include "sock.h"
#include "../json/param_list.h"



/*
    RPC packet header structure
*/
struct SockRpcHeader
{
    char            prefix[ 8 ]     = { 'H', 'R', 'E', 'N', 'O', 'V', 'I', 'N' };    /* Initialize with "hren" */
    size_t          argumentsSize   = 0;


    static SockRpcHeader create
    (
        size_t   /* argumentsSize */
    );



    static SockRpcHeader create
    (
        SockBuffer*
    );



    bool isValid();



    bool isFull
    (
        SockBuffer*
    );



    /*
        Return full size of rpc
    */
    size_t getFullSize();



    /*
        Return params from buffer
    */
    ParamList* getArguments
    (
        SockBuffer*,
        ParamList*
    );
};



/*
    Socket class definition
*/
class SockRpc : public Sock
{
    private:

        Log*            log     = NULL;
        SocketDomain    domain  = SD_INET;
        SocketType      type    = ST_TCP;


        /*
            On before read
            Method may not be overrided
        */
        virtual bool onRead
        (
            SockBuffer* /* buffer */
        ) final;


    public:


        /*
            Constructor
        */
        SockRpc
        (
            Log*,
            SocketDomain        = SD_INET,
            SocketType          = ST_TCP
        );



        /*
            Create socket
        */
        static SockRpc* create
        (
            Log*,
            SocketDomain        = SD_INET,
            SocketType          = ST_TCP
        );



        /*
            Return Log object
        */
        Log* getLog();



        /*
            Return current code to log
        */
        SockRpc* resultToLog();



        /******************************************************************************
            Events
        */

        /*
            On up before
            Method may be overrided
        */
        SockRpc* onConnectBefore();



        /*
            On success up
            Method may be overrided
        */
        virtual SockRpc* onConnectSuccess();



        /*
            On error up
            Method may be overrided
        */
        virtual SockRpc* onConnectError();



        /*
            On after up
            Method may be overrided
        */
        virtual SockRpc* onConnectAfter();



        /*
            On before read
        */
        virtual bool onReadBefore
        (
            string /* Ip */
        );



        /*
            Write buffer to socket with RPC header
        */
        SockRpc* write
        (
            ParamList*,     /* ParamList */
            int = -1        /* Handle for writing */
        );

};
