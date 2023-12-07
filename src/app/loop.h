/*
    This file is part of the Shoggoth neuronet project.
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
#include "../lib/payload.h"
#include "../shoggoth/net_graph.h"

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
        NetGraph*   net             = NULL;

        /* Payloads object  of roles */
        Ui*         ui              = NULL;
        Scene*      scene           = NULL;
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
            ShoggothApplication*
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
            ShoggothApplication*
        );



        /*
            Destructor
        */
        void destroy();



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
        virtual void onLoop
        (
            bool&,
            bool&,
            unsigned int&,
            bool&
        );
};
