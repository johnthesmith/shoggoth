#include "../lib/rpc_server.h"
#include "../lib/application.h"

#include "../shoggoth/shoggoth_consts.h"
#include "../shoggoth/sync.h"



class ShoggothRpcServer : public RpcServer
{
    private:


        Application*            app     = NULL; /* Application object */
        ParamList*              data    = NULL; /* List  */

        /*
            State
        */
        Json*               netConfig           = NULL;
        long int            lastNetConfigUpdate = 0;

    public:

        /*
            Constructor
        */
        ShoggothRpcServer
        (
            Application*,
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
            Application*,
            SocketDomain        = SD_INET,
            SocketType          = ST_TCP
        );



        Application* getApplication();



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
        ShoggothRpcServer* readNet
        (
            ParamList* aArguments,
            ParamList* aResults
        );



        /*
            Remote host send layers errors data
        */
        ShoggothRpcServer* writeValues
        (
            ParamList*, /* Arguments */
            ParamList*  /* Results */
        );



        /*
            Remote host request layers errors layer
        */
        ShoggothRpcServer* readValues
        (
            ParamList*, /* Arguments */
            ParamList*  /* Results */
        );



        /*
            Remote host send layers errors data
        */
        ShoggothRpcServer* writeErrors
        (
            ParamList*, /* Arguments */
            ParamList*  /* Results */
        );



        /*
            Remote host request layers errors layer
        */
        ShoggothRpcServer* readErrors
        (
            ParamList*, /* Arguments */
            ParamList*  /* Results */
        );



        /*
            Remote host send nerve weights data
        */
        ShoggothRpcServer* writeWeights
        (
            ParamList*, /* Arguments */
            ParamList*  /* Results */
        );



        /*
            Remote host request nerve weights layer
        */
        ShoggothRpcServer* readWeights
        (
            ParamList*, /* Arguments */
            ParamList*  /* Results */
        );
};
