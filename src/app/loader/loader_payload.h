/*
    This file of Loader Shoggoth network
    Load structure of the Shoggoth

    Authors:
        still@itserv.ru
        igor_ptx@gmail.com

    2026-03-18
*/
#pragma once



/* Local libraries */
#include "../../../../../lib/core/payload_engine.h"
#include "../../../../../lib/json/param_list.h"
#include "../../shoggoth/net.h"
#include "../shoggoth_application.h"



using namespace std;



class LoaderPayload : public PayloadEngine
{
    private:
        /* Neural net object */
        Mon*            mon                 = NULL;
        Net*            net                 = NULL;
    public:

        /*
            Constructor
        */
        LoaderPayload
        (
            /* Application object */
            ShoggothApplication*,
            /* Payload Id */
            string
        );



        /*
            Destructor
        */
        ~LoaderPayload();



        /*
            Creator
        */
        static LoaderPayload* create
        (
            ShoggothApplication* aApp,
            std::string aId
        )
        {
            return new LoaderPayload( aApp, aId );
        }



        /*
            Destructor
        */
        void destroy() override;



        /*
            Return application object
        */
        ShoggothApplication* getApplication() override;



        /******************************************************************************
            Payload events
        */



        /*
            Main Loader loop event
        */
        virtual void onEngineLoop
        (
            /* true if application config updated */
            const bool
        ) override;



        /******************************************************************************
            Getters and setters
        */

        /*
            Return enabled status
        */
        bool getEnabled();



        /******************************************************************************
            Private methods
        */

        private:


        /*
            Try to load net
        */
        bool readNetFromFile
        (
            ParamList* aAnswer
        );


};



