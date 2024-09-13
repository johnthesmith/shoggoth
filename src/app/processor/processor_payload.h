/*
    This file of Processor Shoggoth network

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

/* Processor libraries */
#include "processor_application.h"
#include "limb_processor.h"
#include "server.h"





using namespace std;



class ProcessorPayload : public PayloadEngine
{
    private:

        /* Processors limb */
        Net*            net             = NULL;
        LimbProcessor*  limb            = NULL;

        /* Processes */
        Server*         server          = NULL;
        Processor*      processor       = NULL;

    public:

        /*
            Constructor
        */
        ProcessorPayload
        (
            ProcessorApplication*,
            /* Net id */
            string,
            /* Net version */
            string
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
            ProcessorApplication*,
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
        ProcessorApplication* getApplication() override;



        /*
            Help method
        */
        ProcessorPayload* help();




        /****************************************************************************
            Events
        */


        /*
            Run net calculateion
        */
        virtual void onEngineLoop() override;
