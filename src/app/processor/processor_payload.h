/*
    Processor payload for shoggoth
    It is ranning in the separete thread.
*/
#pragma once

/* Local libraries */
#include "../../../../../lib/core/payload_engine.h"

#include "../../shoggoth/net.h"

#include "../shoggoth_application.h"
#include "limb_processor.h"


using namespace std;



class ProcessorPayload : public PayloadEngine
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
        ProcessorPayload
        (
            ShoggothApplication*,
            /* Payload id */
            std::string
        );



        /*
            Destructor
        */
        ~ProcessorPayload();



        /*
            Creator
        */
        static ProcessorPayload* create
        (
            ShoggothApplication* aApp,
            /* Payload id */
            std::string aId
        )
        {
            return new ProcessorPayload( aApp, aId );
        }




        /*
            Destructor
        */
        void destroy() override;



        ShoggothApplication* getApplication() override;



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
        */
        virtual void onStopBefore() override;



        /*
            Shoggoth pause action
        */
        virtual void onStopAfter() override;
};
