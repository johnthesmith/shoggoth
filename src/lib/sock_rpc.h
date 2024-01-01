#pragma once

/*
    Socket work
    https://rsdn.org/article/unix/sockets.xml
*/



#include "log_manager.h"
#include "sock.h"
#include "sock_manager.h"
#include "../json/param_list.h"



/*
    RPC packet header structure
*/
struct SockRpcHeader
{
    /* Initialize with "hrenovin" */
    char            prefix[ 8 ]     = { 'H', 'R', 'E', 'N', 'O', 'V', 'I', 'N' };
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

        LogManager*     logManager  = NULL;



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
            LogManager*,
            SockManager*        = NULL,
            SocketDomain        = SD_INET,
            SocketType          = ST_TCP,
            string              = "127.0.0.1",
            int                 = 42
        );



        /*
            Create socket
        */
        static SockRpc* create
        (
            LogManager*,
            SockManager*,
            SocketDomain        = SD_INET,
            SocketType          = ST_TCP,
            string              = "127.0.0.1",
            int                 = 42
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
