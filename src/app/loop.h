/*
    This file is part of the Shoggoth neuronet project.
    Main application loop.
    Contains the Net object.

    Authors:
        still@itserv.ru
        igor_ptx@gmail.com
*/

#pragma once

/*
    Shoggoths main loop
    Launched from the shoggoth application
*/


/* Local libraries */
#include "../../../../lib/core/payload.h"
#include "../shoggoth/limb/net.h"

/* Payloads */
#include "ui.h"
#include "teacher.h"
#include "processor.h"
#include "server.h"



using namespace std;



class Loop : public Payload
{
    private:

        /* Neural net object */
        Net*        net             = NULL;

        /* Payloads object  of roles */
        Ui*         ui              = NULL;
        Teacher*    teacher         = NULL;
        Processor*  processor       = NULL;
        Server*     server          = NULL;

        /* State */
        long int    lastConfigCheck = 0;

    public:

        /*
            Constructor
        */
        Loop
        (
            ShoggothApplication*,   /* Application object */
            string,                 /* Net id */
            string                  /* Net version */
        );



        /*
            Destructor
        */
        ~Loop();



        /*
            Creator
        */
        static Loop* create
        (
            ShoggothApplication*,
            string aNetId,
            string aNetVersion
        );



        /*
            Destructor
        */
        void destroy() override;



        /*
            Return application object
        */
        ShoggothApplication* getApplication() override;



        /*
            Help method
        */
        Loop* help();



        /*
            Control ui components after reconfiguration
        */
        Loop* uiControl();
        Loop* processorControl();
        Loop* teacherControl();



        /******************************************************************************
            Payload events
        */


        /*
            Main loop event
        */
        virtual void onLoop() override;
};
