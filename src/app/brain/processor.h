/*
    Processor payload for shoggoth
    It is ranning in the separete thread.
*/
#pragma once

/* Local libraries */
#include "../../../../../lib/core/payload_engine.h"


#include "../../shoggoth/limb/net.h"   /* TODO use limb_server*/


#include "brain_application.h"
#include "limb_processor.h"
#include "shoggoth_rpc_server.h"


using namespace std;



class Processor : public PayloadEngine
{
    private:

        /* Shoggoth */
        Mon*                    mon                 = NULL;
        Net*                    net                 = NULL;
        LimbProcessor*          limb                = NULL;

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
        void destroy() override;



        BrainApplication* getApplication() override;



        Processor* help();



        LimbProcessor* getLimb();


        /******************************************************************************
            Events
        */



        /*
            Shoggoth main loop event
        */
        virtual void onLoop() override;



        /*
            Shoggoth resume action
        */
        virtual void onStartBefore() override;



        /*
            Shoggoth pause action
        */
        virtual void onStopAfter() override;
};
