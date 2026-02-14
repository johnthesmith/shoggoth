/*
    The Brain application definitions
    Called from Brain.h
*/

#pragma once

/* Local libraries */
/* Load base shoggoth application class */
#include "../shoggoth_application.h"



using namespace std;



class BrainApplication : public ShoggothApplication
{
    private:
    public:

        BrainApplication
        (
            int,        /* cli argumends count */
            char**      /* cli arguments */
        );



        /*
            Creator
        */
        static BrainApplication* create
        (
            int,        /* cli argumends count */
            char**      /* cli arguments */
        );



        /*
            Run application
        */
        BrainApplication* run();



        /*
            Main loop of Brain
        */
        BrainApplication* loop();

};
