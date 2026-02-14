/*
    The Debugger application definitions
*/

#pragma once

/* Local libraries */
/* Load base shoggoth application class */
#include "../shoggoth_application.h"
#include "../../../../../lib/core/terminal.h"


using namespace std;


class DebuggerPayload;


class DebuggerApplication : public ShoggothApplication
{
    private:
        Terminal* terminal = NULL;
        DebuggerPayload* payload = NULL;
    public:

        DebuggerApplication
        (
            int,        /* cli argumends count */
            char**      /* cli arguments */
        );



        ~DebuggerApplication();


        /*
            Creator
        */
        static DebuggerApplication* create
        (
            int,        /* cli argumends count */
            char**      /* cli arguments */
        );



        /*
            Run application
        */
        DebuggerApplication* run();



        /*
            Main loop of Debugger
        */
        DebuggerApplication* loop();



        /*
            on signale event handler
        */
        bool onSignal
        (
            /* Signal */
            int
        );



        Terminal* getTerminal();
};
