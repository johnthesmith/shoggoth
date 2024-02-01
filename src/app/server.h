#pragma once

/* Local libraries */
#include "../../../../lib/core/payload.h"
#include "../shoggoth/sync.h"

#include "shoggoth_application.h"
#include "../shoggoth/shoggoth_rpc_server.h"

#include "../shoggoth/limb/net.h"   /* TODO use limb_server*/


using namespace std;



class Server : public Payload
{
    private:

        /* SHoggoth */
        Net*                    net                 = NULL;
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
        void destroy();



        ShoggothApplication* getApplication() override;



        Server* help();



        /******************************************************************************
            Events
        */

        virtual void onLoop();

        virtual void onResume();

        virtual void onPause();

};
