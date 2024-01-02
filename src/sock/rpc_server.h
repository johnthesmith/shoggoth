#pragma once
/*
    RPC server on socket
*/



#include "sock_rpc.h"

#include "../json/param_list.h"



/*
    Server class definition
*/
class RpcServer : public SockRpc
{
    private:


        /*
            On before read
            Method may not be overrided
        */
        virtual bool onReadBefore
        (
            string
        ) final;



        /*
            On after read
            Method may not be overrided
        */
        virtual bool onReadAfter
        (
            SockBuffer*,    /* Buffers */
            int             /* handle of the client socket */
        ) final;



    public:


        /*
            Constructor
        */
        RpcServer
        (
            LogManager*,
            SockManager*        = NULL,
            SocketDomain        = SD_INET,
            SocketType          = ST_TCP,
            int                 = 42
        );



        /*
            Create socket
        */
        static RpcServer* create
        (
            LogManager*,
            SockManager*        = NULL,
            SocketDomain        = SD_INET,
            SocketType          = ST_TCP,
            int                 = 42
        );



        /*
            Up server
        */
        RpcServer* up();



        /*
            Down server
        */
        RpcServer* down();



        /*
            Server on call before event
            Method may be ovverided
        */
        virtual bool onCallBefore
        (
            string  /* client ip address */
        );



        /*
            Server on call after event
            Method may be ovverided
        */
        virtual RpcServer* onCallAfter
        (
            ParamList* = NULL,  /* Arguments */
            ParamList* = NULL   /* Results */
        );
};

