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
            -> selectObject( Path { "connections", aConnection });

            std::string host = "127.0.0.1";
            int port = 21021;
            int timeoutMcs = 100000;

            if( config != NULL )
            {
                host = config -> getString( Path{ "host" }, host );
                port = config -> getInt( Path{ "port" }, port );
                timeoutMcs = config -> getInt( Path{ "timeout-mcs" }, timeoutMcs );
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
            Synchronize net layers

            1. засовываем в реквест:
                1. все слои которые надо записать у которых
                    отправленные хэши != текущим в Net
                    c данными values
                2. все слои которые надо прочитать read-values
                    с их текущими хэшами из Net
            2. вызываем обмен
                1. сервер
                    1. проверяет все для записи
                        1. если хэш присланный не свопадает с server.Net
                            1. засовываем в Net данные
                            2. все засунутые idLayer отправляет в ответ
                    2. провет все для чтения
                        1. засосвывает в ответ из server.Net данные
                            если хэши не совпадают
            3. клиет оплучив ответ
               1. созраняет все hash для write для idLayers как отправленные
               2. сохраняет все data для read

            Contract:
                Request:
                {
                    "read":
                    {
                        "<layerId>":<layerHash>,
                        ...
                    },
                    "write":
                    {
                        "<layerId>":
                        {
                            "hash":"<layerClientHash>",
                            "values":"<layerValuesData>"
                        },
                        ...
                    }
                }

                Answer:
                {
                    "read":
                    {
                        "<layerId>":
                        {
                            "hash":"<layerServerHash>",
                            "values":"<layerValuesData>"
                        },
                        ...
                    },
                    "write":
                    {
                        "<layerId>":<layerHash>,
                        ...
                    }
                }
        */
        ShoggothRpcClient* netSyncLayers
        (
            /* Net obejct */
            Net*,
            /* Old values hashes id layer:hash */
            std::unordered_map<std::string, uint64_t>&,
            Mon*
        );



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
