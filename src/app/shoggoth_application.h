/*
    Shoggoth application
*/
#pragma once

/* Local libraries */
#include "../../../../lib/core/application.h"
#include "../../../../lib/sock/sock_manager.h"


using namespace std;



class ShoggothApplication : public Application
{
    private:

        bool            configUpdated       = false;
        long int        lastConfigUpdate    = 0;
        SockManager*    sockManager         = NULL;

        string          netId               = "alpha";
        string          netVersion          = "zero";

    public:

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
            Destroy of the Shogoth
        */
        void destroy();



        /*
            Return the name of configuraion file
        */
        string getConfigFileName();



        /*
            Check update moment of the config file.
            If file was updated, then the config object is rebuilding.
        */
        ShoggothApplication* checkConfigUpdate();



        /*
            End of thread
        */
        ShoggothApplication* onThreadAfter();


        /*
            Prepare configuration for application running
        */
        ShoggothApplication* prepareConfiguration();



        /**********************************************************************
            Setters and getters
        */



        /*
            Return true if config wile was updated
        */
        bool getConfigUpdated();



        /*
            Return the sock manager from application
        */
        SockManager* getSockManager();



        /*
            Return the net identifier
        */
        string getNetId();



        /*
            Return the net version
        */
        string getNetVersion();
};
