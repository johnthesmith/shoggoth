#pragma once


/*
    RPC client on socket
*/




#include "sock_rpc.h"
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
        virtual RpcClient* onReadAfter
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
            Log*,
            SocketDomain        = SD_INET,
            SocketType          = ST_TCP
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
            Log*,
            SocketDomain        = SD_INET,
            SocketType          = ST_TCP
        );




        /*
            Create net socket
        */
        static RpcClient* create
        (
            Log*,
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



        /*
            Override set port
        */
        RpcClient* setPort
        (
            unsigned short int  /* port */
        );



        /*
            Override set ip
        */
        RpcClient* setIp
        (
            string  /* IP address */
        );



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
