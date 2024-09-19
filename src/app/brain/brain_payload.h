/*
    This file of Brain Shoggoth network

    Authors:
        still@itserv.ru
        igor_ptx@gmail.com
*/

#pragma once

/* Local core libraries */
#include "../../../../../lib/json/json.h"
#include "../../../../../lib/core/payload_engine.h"

/* Shoggoth libraries */
#include "../../shoggoth/limb/net.h"

/* Brain libraries */
#include "brain_application.h"
#include "limb_processor.h"
#include "server.h"
#include "processor.h"



using namespace std;



class BrainPayload : public PayloadEngine
{
    private:

        /* Brains limb */
        Net*            net             = NULL;

        /* Processes */
        Server*         server          = NULL;
        Processor*      processor       = NULL;

    public:

        /*
            Constructor
        */
        BrainPayload
        (
            BrainApplication*,
            /* Net id */
            string,
            /* Net version */
            string
        );



        /*
            Destructor
        */
        ~BrainPayload();



        /*
            Creator
        */
        static BrainPayload* create
        (
            BrainApplication*,
            string,                 /* Net id */
            string                  /* Net version */
        );



        /*
            Destructor
        */
        void destroy() override;



        /*
            Return application object
        */
        BrainApplication* getApplication() override;



        /*
            Help method
        */
        BrainPayload* help();


        /*
            Return the processors net object
        */
        Limb* getNet();



        /*
            Return the limb object of processor
        */
        LimbProcessor* getLimb();


        /****************************************************************************
            Events
        */


        /*
            Run net calculateion
        */
        virtual void onEngineLoop
        (
            /* Config updated */
            const bool,
            /* Disable */
            const bool
        ) override;
};

