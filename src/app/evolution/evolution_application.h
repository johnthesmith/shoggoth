#pragma once

/* Local libraries */
#include "../shoggoth_application.h"


using namespace std;



class EvolutionApplication : public ShoggothApplication
{
    private:
    public:

        EvolutionApplication
        (
            int,        /* cli argumends count */
            char**      /* cli arguments */
        );



        /*
            Creator
        */
        static EvolutionApplication* create
        (
            int,        /* cli argumends count */
            char**      /* cli arguments */
        );



        /*
            Run application
        */
        EvolutionApplication* run();



        /*
            Main loop of evolution
        */
        EvolutionApplication* loop();

};

