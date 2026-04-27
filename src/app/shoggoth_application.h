/*
    Shoggoth application
*/
#pragma once

/* Standart libraries */
#include <set>

/* Local libraries */
#include "../../../../lib/core/application.h"
#include "../../../../lib/sock/sock_manager.h"


class Net;



class ShoggothApplication : public Application
{
    private:
        /* Sock manager */
        SockManager*    sockManager     = NULL;
        /* Share net structure */
        Net*            net             = NULL;
        /* Using layers */
        std::map<std::string, std::set<std::string>> layersUsing = {};
        /* Mutex for layer useing list */
        std::mutex mLayersMutex;
    public:

        /*
            Constructor
        */
        ShoggothApplication
        (
            int,        /* cli argumends count */
            char**      /* cli arguments */
        );



        /*
            Destructor of the Shogoth application
        */
        ~ShoggothApplication();



        /*
            Creator for ShoggothApplication
        */
        static inline ShoggothApplication* create
        (
            int     argc,
            char**  argv
        )
        {
            return new ShoggothApplication( argc, argv );
        }



        /*
            Destroy of the Shogoth
        */
        inline void destroy() override
        {
            delete this;
        }



        /*
            End of thread
        */
        virtual ShoggothApplication* onThreadAfter() override;


        /*
            Prepare configuration for application running
        */
        ShoggothApplication* prepareConfiguration();



        /**********************************************************************
            Setters and getters
        */



        /*
            Return the sock manager from application
        */
        inline SockManager* getSockManager()
        {
            return sockManager;
        }



        /*
            Return Net
        */
        inline Net* getNet()
        {
            return net;
        }



        /*
            on signale event handler
        */
        virtual bool onSignal
        (
            int /* aSignal */
        )
        {
            terminate();
            return true;
        }



        /*
            Return net version
        */
        std::string getNetVersion()
        {
            return getConfig() -> getString( Path{ "net-version" }, "zero" );
        }



        /*
            Return monitor path for appliation
        */
        inline std::string getMonPath
        (
            std::string subPath
        )
        {
            return "./mon/" + subPath;
        }



        ShoggothApplication* collectLayersUsing();


        /*
            Return true if layer must be loaded
        */
        bool layerIsUsing
        (
            /* Layer id */
            std::string,
            /* Type of operation */
            std::string = ""
        );



        /*
            Return true value if layer contains action for current net task
        */
        bool checkActionValues
        (
            /* Paylaod id */
            std::string,
            /* Layer id */
            std::string,
            /* Action id */
            std::string
        );



        std::vector<std::string> layersByOperation
        (
            /* Type operation */
            std::string
        );



        /**********************************************************************
            Application events
        */

        /*
            Generate event after config updated
        */
        virtual ShoggothApplication* onConfigUpdated() override
        {
            collectLayersUsing();
            return this;
        }




};
