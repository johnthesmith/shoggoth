#pragma once

/* Local libraries */
#include "../lib/payload.h"
#include "../shoggoth/net.h"
#include "../shoggoth/sync.h"

#include "shoggoth_application.h"


using namespace std;



class Processor : public Payload
{
    private:

        Net*                    net                 = NULL;
        thread*                 server              = NULL;

        long int                lastConfigCheck     = 0;

    public:

        /*
            Constructor
        */
        Processor
        (
            Net*
        );



        /*
            Destructor
        */
        ~Processor();



        /*
            Creator
        */
        static Processor* create
        (
            Net*
        );



        /*
            Destructor
        */
        void destroy();



        ShoggothApplication* getApplication() override;



        Processor* help();


        /****************************************************************************
            Events
        */


        /*
            Run net calculateion
        */
        virtual void onLoop();
};
