/*
    This file of Evolution Shoggoth network
    Main application loop.
    Contains the Net object.

    Authors:
        still@itserv.ru
        igor_ptx@gmail.com
*/

#pragma once



/* Local libraries */
#include "../../../../../lib/core/payload_engine.h"
#include "../../shoggoth/limb/net.h"



using namespace std;



class EvolutionPayload : public PayloadEngine
{
    private:

        /* Neural net object */
        Net*        net             = NULL;

        /* State */
        long int    lastConfigCheck = 0;

    public:

        /*
            Constructor
        */
        EvolutionPayload
        (
            /* Application object */
            EvolutionApplication*,
            /* Payload id */
            string,
            /* Net id */
            string,
            /* Net version */
            string
        );



        /*
            Destructor
        */
        ~EvolutionPayload();



        /*
            Creator
        */
        static EvolutionPayload* create
        (
            /* Application object */
            EvolutionApplication*,
            /* Payload id */
            string,
            /* Net id */
            string,
            /* Net version */
            string
        );



        /*
            Destructor
        */
        void destroy() override;



        /*
            Return application object
        */
        EvolutionApplication* getApplication() override;



        /*
            Help method
        */
        EvolutionPayload* help();



        /******************************************************************************
            Payload events
        */



        /*
            Main loop event
        */
        virtual void onEngineLoop
        (
            const bool,
            const bool
        ) override;
};
