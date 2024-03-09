/*
    Server payload
    Create control and destroy the servers thread.
*/
#pragma once

/* Local libraries */
#include "../../../../lib/core/payload.h"

#include "shoggoth_application.h"
#include "../shoggoth/shoggoth_rpc_server.h"
#include "../shoggoth/limb/net.h"   /* TODO use limb_server*/


using namespace std;



class Server : public Payload
{
    private:

        /* SHoggoth */
        Net*                    net                 = NULL;
        Mon*                    mon                 = NULL;
        thread*                 serverThread        = NULL;
        ShoggothRpcServer*      srv                 = NULL;
        long int                lastConfigCheck     = 0;

    public:

        /*
            Constructor
        */
        Server
        (
            Net*
        );



        /*
            Destructor
        */
        ~Server();



        /*
            Creator
        */
        static Server* create
        (
            Net*
        );



        /*
            Destructor
        */
        void destroy() override;



        ShoggothApplication* getApplication() override;



        Server* help();



        /******************************************************************************
            Events
        */



        /*
            Server main loop event
        */
        virtual void onLoop() override;



        /*
            Server resume action
        */
        virtual void onResume() override;



        /*
            Server pause action
        */
        virtual void onPause() override;
};
