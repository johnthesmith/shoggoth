/*
    Shoggoth application
*/
#pragma once

/* Local libraries */
#include "../../../../lib/core/application.h"
#include "../../../../lib/sock/sock_manager.h"


class Net;



class ShoggothApplication : public Application
{
    private:
        /* Sock manager */
        SockManager*    sockManager = NULL;
        /* Share net structure */
        string          netVersion  = "zero";
        Net*            net         = NULL;

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
        inline void destroy()
        {
            delete this;
        }



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
        inline SockManager* getSockManager()
        {
            return sockManager;
        }



        /*
            Return the net version
        */
        inline string getNetVersion()
        {
            return netVersion;
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
        bool onSignal
        (
            int aSignal
        )
        {
            terminate();
            return true;
        }
};


