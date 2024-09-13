/*
    The Processor application definitions
    Called from processor.h
*/

#pragma once

/* Local libraries */
/* Load base shoggoth application class */
#include "../shoggoth_application.h"



using namespace std;



class ProcessorApplication : public ShoggothApplication
{
    private:
    public:

        ProcessorApplication
        (
            int,        /* cli argumends count */
            char**      /* cli arguments */
        );



        /*
            Creator
        */
        static ProcessorApplication* create
        (
            int,        /* cli argumends count */
            char**      /* cli arguments */
        );



        /*
            Run application
        */
        ProcessorApplication* run();



        /*
            Main loop of Processor
        */
        ProcessorApplication* loop();

};
