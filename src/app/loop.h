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
#include "../shoggoth/shoggoth_rpc_server.h"

/* Payloads */
#include "ui.h"
#include "teacher.h"
#include "processor.h"



using namespace std;



class Loop : public Payload
{
    private:

        /* Neural net object */
        NetGraph*               net                 = NULL;

        /* Payloads object  of roles */
        bool                    processor           = false;
        Ui*                     ui                  = NULL;
        Scene*                  scene               = NULL;
        Teacher*                teacher             = NULL;
        ShoggothRpcServer*      server              = NULL;

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



        /*
            Control ui components after reconfiguration
        */
        Loop* uiControl();
        Loop* processorControl();
        Loop* teacherControl();
        bool serverControl();

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
