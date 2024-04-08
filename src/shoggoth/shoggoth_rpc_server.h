#include "../../../../lib/sock/rpc_server.h"
#include "../../../../lib/core/mon.h"

#include "shoggoth_consts.h"
#include "limb/net.h"



class ShoggothRpcServer : public RpcServer
{
    private:

        /* Net limb with main layers */
        Net*    net = NULL;

        /* Monitor object */
        Mon*    mon = NULL;

    public:

        /*
            Constructor
        */
        ShoggothRpcServer
        (
            Net*,
            SocketDomain        = SD_INET,
            SocketType          = ST_TCP,
            int                 = 11120
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
            Net*,   /* Net object */
            int     /* Port */
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
            And result details key
        */
        ParamList* setAnswerResult
        (
            ParamList*, /* Answer param list */
            string      /* Answer code */
        );



        /*
            Return true if answer is ok
        */
        bool isAnswerOk
        (
            ParamList*
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
        ShoggothRpcServer* writeLayers
        (
            ParamList*, /* Arguments */
            ParamList*  /* Results */
        );



        /*
            Remote host request layers errors layer
        */
        ShoggothRpcServer* readLayers
        (
            ParamList*, /* Arguments */
            ParamList*  /* Results */
        );



        /*
            Remote host request weights for selected neurons
        */
        ShoggothRpcServer* requestWeights
        (
            ParamList* aArguments,
            ParamList* aResults
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
