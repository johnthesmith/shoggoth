#include "../lib/rpc_server.h"
#include "../shoggoth/rpc_protocol.h"
#include "../shoggoth/sync.h"

#include "shoggoth_application.h"



class ShoggothRpcServer : public RpcServer
{
    private:

        ShoggothApplication*    app     = NULL; /* Application object */
        ParamList*              data    = NULL; /* List  */
        Sync*                   sync    = NULL; /* Synchronize table */

    public:

        /*
            Constructor
        */
        ShoggothRpcServer
        (
            ShoggothApplication*,
            SocketDomain        = SD_INET,
            SocketType          = ST_TCP
        );



        /*
            Destrouctor
        */
        ~ShoggothRpcServer();



        /*
            Create socket
        */
        static ShoggothRpcServer* create
        (
            ShoggothApplication*,
            SocketDomain        = SD_INET,
            SocketType          = ST_TCP
        );



        ShoggothApplication* getApplication();



        /*
            Shoggoth protocol
        */
        ShoggothRpcServer* onCallAfter
        (
            ParamList* aArguments,
            ParamList* aResults
        ) override;



        /*
            Validate answer condition and set answer if validation is failed
        */
        bool validate
        (
            bool, /* Condition */
            string, /* Code */
            ParamList* /* Answer object */
        );



        /*
            Set code for answer
        */
        ShoggothRpcServer* setAnswerResult
        (
            ParamList*, /* Answer param list */
            string      /* Answer code */
        );


        /**********************************************************************
            Server methods
        */

        /*
            Unknwon method result
        */
        ShoggothRpcServer* unknownMethod
        (
            ParamList*, /* Arguments */
            ParamList*  /* Results */
        );



        /*
            Remote host send net configuration
        */
        ShoggothRpcServer* getNet
        (
            ParamList* aArguments,
            ParamList* aResults
        );



        /*
            Request sync
        */
        ShoggothRpcServer* getSync
        (
            ParamList* aArguments,
            ParamList* aResults
        );



        /*
            Remote host send layre data
        */
        ShoggothRpcServer* writeLayer
        (
            ParamList*, /* Arguments */
            ParamList*  /* Results */
        );



        /*
            Remote host request the layer
        */
        ShoggothRpcServer* readLayer
        (
            ParamList*, /* Arguments */
            ParamList*  /* Results */
        );



        /*
            Return application
        */
        ShoggothApplication* getApp();
};
