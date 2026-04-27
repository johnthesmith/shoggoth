/*
    This file of the Shoggoth client network
    Main application loop.

    Authors:
        still@itserv.ru
        igor_ptx@gmail.com
*/
#pragma once



/* Local libraries */
#include "../../../../../lib/core/payload_engine.h"

#include "../../shoggoth/net.h"

#include "../shoggoth_application.h"



using namespace std;



class ClientPayload : public PayloadEngine
{
    private:
        /* Neural net object */
        Mon*            mon                 = NULL;
        /* Neural net object */
        Net* net = NULL;
        /* Resolve hash by layer id for values*/
        std::unordered_map<std::string, uint64_t> oldValuesHashes;

    public:

        /*
            Constructor
        */
        ClientPayload
        (
            /* The application object */
            ShoggothApplication* aApp,
            /* Payload id */
            std::string aId
        )
        /* Call parent constructor */
        : PayloadEngine(( Application* ) aApp, aId )
        {
            /* Define mon file */
            auto monFile = aApp -> getMonPath( aId + ".json" );

            /* Begin log */
            aApp -> getLog() -> trace( "Client creating" )
            -> prm( "id", aId )
            -> prm( "log", monFile )
            ;

            /* Create teacher monitor */
            mon = Mon::create( monFile )
            -> setString( Path{ "start", "source" }, aId )
            -> startTimer( Path{ "start", "moment" })
            -> flush()
            ;

            net = aApp -> getNet();
        }



        /*
            Destructor
        */
        ~ClientPayload()
        {
            /* Destroy Processor monitor */
            mon -> destroy();
            /* Log report */
            getLog() -> trace( "Client destroyd" );
        }




        /*
            Creator
        */
        static ClientPayload* create
        (
            /* The application object */
            ShoggothApplication* aApp,
            /* Payload id */
            std::string aId
        )
        {
            return new ClientPayload( aApp, aId );
        }



        /*
            Destructor
        */
        void destroy() override
        {
            delete this;
        }



        /*
            Return application object
        */
        ShoggothApplication* getApplication() override
        {
            return ( ShoggothApplication* ) PayloadEngine::getApplication();
        }



        /******************************************************************************
            Setters and getters
        */

        /*
            Get application monitor object
        */
        Mon* getMon()
        {
            return mon;
        }



        /******************************************************************************
            Payload events
        */

        /*
            Main Client loop event
        */
        virtual void onEngineLoop( bool ) override;
};
