/*
    This file of Debugger Shoggoth network
    Main application loop.
    Contains the Net object.

    Authors:
        still@itserv.ru
        igor_ptx@gmail.com
*/
#pragma once



/* Local libraries */
#include "../../../../../lib/json/json.h"
#include "../../../../../lib/core/payload_engine.h"
#include "../../shoggoth/net.h"



/* Local libraries */



using namespace std;



class DebuggerPayload : public PayloadEngine
{
    private:
        /* Neural net object */
        Net*            net             = NULL;
    public:

        /*
            Constructor
        */
        DebuggerPayload
        (
            DebuggerApplication*,    /* Application object */
            string,                 /* Payload Id */
            string,                 /* Net id */
            string                  /* Net version */
        );



        /*
            Destructor
        */
        ~DebuggerPayload();



        /*
            Creator
        */
        static DebuggerPayload* create
        (
            DebuggerApplication*,
            string,
            string,
            string
        );



        /*
            Destructor
        */
        void destroy() override;



        /*
            Return application object
        */
        DebuggerApplication* getApplication() override;



        /*
            Help method
        */
        DebuggerPayload* help();



        /******************************************************************************
            Payload events
        */



        /*
            Main debugger loop event
        */
        virtual void onEngineLoop
        (
            const bool,
            const bool
        ) override;
};




