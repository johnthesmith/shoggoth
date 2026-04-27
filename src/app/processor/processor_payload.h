/*
    Processor payload for shoggoth
    It is ranning in the separete thread.
*/
#pragma once


#include <filesystem>


/* Local libraries */
#include "../../../../../lib/core/payload_engine.h"
#include "../../shoggoth/net.h"
#include "../shoggoth_application.h"
#include "limb_processor.h"


using namespace std;



class ProcessorPayload : public PayloadEngine
{
    private:

        /* Shoggoth */
        Mon*                    mon                 = NULL;
        Net*                    net                 = NULL;
        LimbProcessor*          limb                = NULL;

    public:

        /*
            Constructor
        */
        ProcessorPayload
        (
            ShoggothApplication* aApp,
            /* Payload id */
            std::string aId
        )
        /* Call parent constructor */
        : PayloadEngine
        (
            (Application*) aApp,
            aId
        )
        {
            getLog() -> trace( "Processor creating" );
            net = aApp -> getNet();

            /* Create Processor monitor */
            mon = Mon::create( net -> getApplication() -> getMonPath( aId + ".json" ))
            -> setString( Path{ "start", "source" }, aId )
            -> startTimer( Path{ "start", "moment" })
            -> now( Path{ "start", "time" } )
            -> setString( Path{ "start", "pwd" }, std::filesystem::current_path() )
            ;

            /* Create the limb */
            limb = LimbProcessor::create( this );
        }





        /*
            Destructor
        */
        ~ProcessorPayload()
        {
            waitStop();

            /* Destroy limb */
            limb -> destroy();
            /* Destroy Processor monitor */
            mon -> destroy();
            /* Log report */
            getLog() -> trace( "Processor destroyd" );
        }



        /*
            Creator
        */
        static ProcessorPayload* create
        (
            ShoggothApplication* aApp,
            /* Payload id */
            std::string aId
        )
        {
            return new ProcessorPayload( aApp, aId );
        }




        /*
            Destructor
        */
        void destroy() override
        {
            delete this;
        }




        ShoggothApplication* getApplication() override;



        LimbProcessor* getLimb();


        /******************************************************************************
            Events
        */



        /*
            Shoggoth main loop event
        */
        virtual void onEngineLoop
        (
            bool
        ) override;



        /*
            Shoggoth resume action
        */
        virtual void onStartBefore() override;



        /*
        */
        virtual void onStopBefore() override;



        /*
            Shoggoth pause action
        */
        virtual void onStopAfter() override;
};
