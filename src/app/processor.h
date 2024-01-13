#pragma once

/* Local libraries */
#include "../lib/payload.h"
#include "../shoggoth/sync.h"
#include "../shoggoth/limb/limb_processor.h"

#include "shoggoth_application.h"

#include "../shoggoth/limb/net.h"


using namespace std;



class Processor : public Payload
{
    private:

        LimbProcessor*          limb            = NULL;
        long int                lastConfigCheck = 0;

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


        LimbProcessor* getLimb();
};
