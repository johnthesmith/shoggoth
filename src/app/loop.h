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



enum EVENDS =
{
    TICK_BEGIN,
    TICHER_RESULT,
    READ_NET,
    CALC_BEGIN,
    CALC_END,
    LEARNING_END
};



enum ACTIONS =
{
    READ_VALUES,
    WRITE_VALUES,
    READ_ERRORS,
    WRITE_ERRORS,
    READ_WEIGHTS,
    WRITE_WEIGHTS,
    CALC_INIT
};



class Loop : public Payload
{
    private:

        /* Neural net object */
        NetGraph*               net                 = NULL;

        /* Payloads object  of roles */
        Processor*              processor           = NULL;
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
        Loop* uiControll();
        bool serverControll();

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
