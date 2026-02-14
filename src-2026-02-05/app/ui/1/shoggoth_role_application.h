#pragma once



/* Local libraries */
#include "shoggoth_application.h"


using namespace std;



enum Role
{
    ROLE_TEACHER,
    ROLE_PROCESSOR,
    ROLE_SERVER,    /* The component of Shogooth, whos  collect and synchronize layers and other participants */
    ROLE_UI
};



class ShoggothRoleApplication : public ShoggothApplication
{
    private:
    public:

        ShoggothRoleApplication
        (
            int,        /* cli argumends count */
            char**      /* cli arguments */
        );



        /*
            Creator
        */
        static ShoggothRoleApplication* create
        (
            int,        /* cli argumends count */
            char**      /* cli arguments */
        );




        /*
            Run application
        */
        ShoggothRoleApplication* run();



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
};
