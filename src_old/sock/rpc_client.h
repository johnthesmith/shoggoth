#pragma once


/*
    RPC client on socket
*/


#include "sock_rpc.h"
#include "sock_manager.h"

#include "../json/param_list.h"



/*
    Client class definition
*/

class RpcClient : public SockRpc
{
    private:

        ParamList* answer   = NULL;
        ParamList* request  = NULL;

        bool ownerAnswer    = true;
        bool ownerRequest   = true;

        /*
            On before read
            Method may not be overrided
        */
        virtual bool onReadBefore
        (
            string  /* ip */
        ) final;




        /*
            On after read
            Method may not be overrided
        */
        virtual bool onReadAfter
        (
            SockBuffer*,    /* Buffers */
            int             /* handle of server conenction */
        ) final;


    public:

        /*
            Constructor
        */
        RpcClient
        (
            LogManager*,
            SockManager*,
            SocketDomain        = SD_INET,
            SocketType          = ST_TCP,
            string              = "127.0.0.1",
            int                 = 42
        );



        /*
            Destructor
        */
        ~RpcClient();



        /*
            Create socket
        */
        static RpcClient* create
        (
            LogManager*,
            SockManager*,
            SocketDomain        = SD_INET,
            SocketType          = ST_TCP,
            string              = "127.0.0.1",
            int                 = 42

        );




        /*
            Create net socket
        */
        static RpcClient* create
        (
            LogManager*,
            SockManager*,
            string,
            unsigned int
        );



        /*
            Client call
        */
        RpcClient* call();



        /*
            Client call method with result structure return
        */
        RpcClient* call
        (
            string /* Method */
        );



        /*
            Client call method with result structure return
        */
        RpcClient* call
        (
            int /* Method ID */
        );



        /*
            Client on call before event
            Method may be ovverided
        */
        RpcClient* onCallBefore();



        /*
            Client on call after event
            Method may be ovverided
        */
        RpcClient* onCallAfter();



        RpcClient* setRequest
        (
            ParamList*
        );




        ParamList* getRequest();



        RpcClient* setAnswer
        (
            ParamList*
        );



        ParamList* getAnswer();
};
