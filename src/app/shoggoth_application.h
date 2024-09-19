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
