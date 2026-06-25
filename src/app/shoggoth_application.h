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
        /* Net config */
        ParamList*      netConfig       = NULL;

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
            Return Net
        */
        inline ParamList* getNetConfig()
        {
            return netConfig;
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



        /**********************************************************************
            Application events
        */


        /*
            Generate event before main application loop in run method
        */
        ShoggothApplication* onBeforeLoop() override;



        /*
            Return true if application configured and ready
        */
        bool isReady()
        {
            return netConfig -> getCount() > 0;
        }



        /*
            Return net config seed
        */
        unsigned long long int getNetSeed()
        {
            return netConfig -> getUInt( Path{ "seed" }, 0 );
        }
};
