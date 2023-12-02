#pragma once

/* Local libraries */
#include "../lib/payload.h"
#include "../shoggoth/net.h"
#include "../shoggoth/sync.h"

#include "shoggoth_application.h"
#include "../shoggoth/shoggoth_rpc_server.h"


using namespace std;



class Server : public Payload
{
    private:

        Net*                    net                 = NULL;
        thread*                 server              = NULL;

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
        */


        virtual void onActivate();

        virtual void onRun();
};
