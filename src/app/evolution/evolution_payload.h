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

        /* Last tick of the net for test stage*/
        long long int lastNetTick   = 0;

        double      lastTestError = 0.0;

        /* Quantity successful tests */
        int         testSuccessCount = 0;


        EvolutionPayload* testStage
        (
            Layer*,
            ParamList*
        );

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



        /******************************************************************************
            Actions methods
        */

        /*
            Switch server net in to Learn mode
        */
        EvolutionPayload* netSwitchToLearn
        (
            ParamList* /* Reason */
        );



        /*
            Switch server net in to Learn mode
        */
        EvolutionPayload* netSwitchToTest
        (
            ParamList* /* Reason */
        );



        /*
            Mutate net
        */
        EvolutionPayload* commitNet
        (
            bool, /* Success */
            double,
            ParamList* /* Reason */
        );
};
