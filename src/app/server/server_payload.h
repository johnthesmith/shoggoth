/*
    Server payload
    Create control and destroy the servers thread.
*/
#pragma once

/* Local libraries */
#include "../../../../../lib/core/payload_engine.h"

#include "../../shoggoth/shoggoth_rpc_server.h"


using namespace std;



class Net;

class ServerPayload : public PayloadEngine
{
    private:

        /* Shoggoth */
        Net*                    net                 = NULL;
        Mon*                    mon                 = NULL;
        thread*                 serverThread        = NULL;
        ShoggothRpcServer*      srv                 = NULL;
        long int                lastConfigCheck     = 0;

    public:

        /*
            Constructor
        */
        ServerPayload
        (
            ShoggothApplication*,
            std::string
        );



        /*
            Destructor
        */
        ~ServerPayload();



        /*
            Creator
        */
        static ServerPayload* create
        (
            ShoggothApplication* aApp,
            std::string aId
        )
        {
            return new ServerPayload( aApp, aId );
        }



        /*
            Destructor
        */
        void destroy() override
        {
            delete this;
        }




        ServerPayload* help();



        /******************************************************************************
            Events
        */



        /*
            Server main loop event
        */
        virtual void onEngineLoop( bool ) override;



        /*
            Server resume action
        */
        virtual void onStartAfter() override;



        /*
            Server pause action
        */
        virtual void onStopBefore() override;
};
