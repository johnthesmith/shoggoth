/*
    Shoggoth rpc client
*/

#pragma once


#include "../../../../lib/sock/rpc_client.h"
#include "../../../../lib/core/application.h"

#include "net.h"
#include "shoggoth_rpc_contract.h"



/*
    Io class
*/
class ShoggothRpcClient: public RpcClient
{
    private:

        /* Application obejct pointer */
        ShoggothApplication*    app = NULL;

        /*
            Hide parent call
        */
        RpcClient* call() = delete;

        /*
            Call operation
        */
        ShoggothRpcClient* call
        (
            /* Command */
            Command aCommand
        );

    public:


        /*
            Create rpc client for shoggoth
        */
        ShoggothRpcClient
        (
            /* Application */
            ShoggothApplication*,
            /* Host for call */
            std::string,
            /* Port for call */
            int,
            /* External answer */
            ParamList* = NULL
        );


        /*
            Destructor
        */
        ~ShoggothRpcClient();



        /*
            Create new client
        */
        static ShoggothRpcClient* create
        (
            /* Application */
            ShoggothApplication*    aApp,
            /* Conenction alias */
            std::string             aConnection = "default",
            /* External answer */
            ParamList*              aExternalAnswer = NULL
        )
        {
            auto config = aApp
            -> getConfig()
            -> selectObject( Path { "remote", "client", "connections", aConnection });

            std::string host = "127.0.0.1";
            int port = 21021;
            int timeoutMcs = 100000;

            if( config != NULL )
            {
                host = config -> getString( Path{ "host" }, host );
                port = config -> getInt( Path{ "port" }, port );
                timeoutMcs = config -> getInt
                (
                    Path{ "readWaitingTimeoutMcs" },
                    timeoutMcs
                );
            }

            auto result = new ShoggothRpcClient
            (
                aApp,
                host,
                port,
                aExternalAnswer
            )
            ;
            result -> setReadWaitingTimeoutMcs( timeoutMcs );
            return result;
        }



        /*
            Self destroy the rpc client
        */
        inline void destroy()
        {
            delete this;
        }



        /**************************************************************************
            Setters and getters
        */


        /*
            Return the Application object
        */
        inline ShoggothApplication* getApplication()
        {
            return app;
        }



        /******************************************************************************
            Shoggoth Rpc Client methods
        */


        /*
            Clone net from id and version
        */
        ShoggothRpcClient* cloneNet
        (
            /* Net id */
            std::string,
            /* Net version */
            std::string,
            /* Parent generation */
            int,
            /* True for mutation */
            bool
        );



        /*
            Switch net from id and version
        */
        ShoggothRpcClient* switchNet
        (
            /* Net id */
            std::string,
            /* Net version */
            std::string
        );



        /*
            Read net config
        */
        ShoggothRpcClient* readNet();



        /*
            Return server net mode
        */
        NetMode getNetMode();



        /*
            Set server net mode
        */
        ShoggothRpcClient* setNetMode
        (
            /* New net mode */
            NetMode,
            /* Reason of net mode */
            ParamList*
        );



        /*
            Send test result to server
        */
        ShoggothRpcClient* testResult
        (
            /* Net verstion */
            std::string,
            /* Tick number */
            int,
            /* Test result string */
            std::string
        );



        /*
            Read data in to nerve
        */
        ShoggothRpcClient* readWeights
        (
            /* Nerve destination */
            Nerve*
        );



        /*
            Read data in to nerve
        */
        ShoggothRpcClient* writeWeights
        (
            /* Nerve destination */
            Nerve* aNerve
        );



        /*
            Request layers statistics
        */
        ShoggothRpcClient* readLayerStat
        (
            /* Layer list for stat request */
            vector<std::string>,
            /* Layer list for stat request */
            vector<std::string>,
            /* Layer list for stat request */
            vector<std::string>,
            /* Layer list for stat request */
            vector<std::string>
        );


        /*
            Return net information from server
        */
        ShoggothRpcClient* readNetInfo();


        /*
            Write values and errors to server
        */
        ShoggothRpcClient* writeLayers
        (
            /* Net obejct */
            Net* net,
            /* List of layers id for values */
            vector<string> aValues,
            /* List of layers id for errors */
            vector<string> aErrors
        );



        /*
            Read value from server
        */
        ShoggothRpcClient* readLayers
        (
            /* Net obejct */
            Net* net,
            /* List of layers id for values */
            vector<string> aValues,
            /* List of layers id for errors */
            vector<string> aErrors
        );
};
