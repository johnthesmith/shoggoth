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
#include "../shoggoth/net.h"

/* Payloads */
#include "ui.h"
#include "teacher.h"
#include "processor.h"
#include "server.h"



using namespace std;



class Processor : public Payload
{
    private:

        /* Neural net object */
        Net*                    net                 = NULL;

        /* Payloads object  of roles */
        Processor*              processor           = NULL;
        Ui*                     ui                  = NULL;
        Server*                 server              = NULL;
        Teacher*                teacher             = NULL;

        /* State */
        long int                lastConfigCheck     = 0;
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



        ShoggothApplication* getApplication() override;



        Loop* help();


        /******************************************************************************
            Payload events
        */


        virtual void onActivate();



        virtual void onLoop
        (
            bool&,
            bool&,
            unsigned int&,
            bool&
        );
};
