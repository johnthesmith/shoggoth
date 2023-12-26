#pragma once

/* Local libraries */
#include "../lib/application.h"
#include "../lib/sock_manager.h"


using namespace std;



enum Role
{
    ROLE_TEACHER,
    ROLE_PROCESSOR,
    ROLE_SERVER,    /* The component of Shogooth, whos  collect and synchronize layers and other participants */
    ROLE_UI
};



class ShoggothApplication : public Application
{
    private:

        bool                    configUpdated       = false;
        long int                lastConfigUpdate    = 0;
        long int                lastNetUpdate       = 0;
        SockManager*            sockManager         = NULL;

    public:

        ShoggothApplication
        (
            int,        /* cli argumends count */
            char**      /* cli arguments */
        );



        ~ShoggothApplication();



        /*
            Creator
        */
        static ShoggothApplication* create
        (
            int,        /* cli argumends count */
            char**      /* cli arguments */
        );



        /*
            Run application
        */
        ShoggothApplication* run();



        /*
            return application role by string
        */
        Role roleFromString
        (
            string
        );



        /*
            Return application role like string from enum
        */
        string roleToString
        (
            Role aRole
        );



        string getConfigFileName();



        ShoggothApplication* checkConfigUpdate();



        bool getConfigUpdated();


        SockManager* getSockManager();
};

