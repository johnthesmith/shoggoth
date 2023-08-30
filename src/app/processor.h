#pragma once

/* Local libraries */
#include "../lib/payload.h"
#include "../shoggoth/net.h"
#include "../shoggoth/sync.h"

#include "shoggoth_application.h"


using namespace std;



class Processor : public Payload
{
    private:

        Net*                    net                 = NULL;
        long int                lastConfigCheck     = 0;

    public:

        /*
            Constructor
        */
        Processor
        (
            ShoggothApplication*
        );



        /*
            Destructor
        */
        ~Processor();



        /*
            Creator
        */
        static Processor* create
        (
            ShoggothApplication*
        );



        /*
            Destructor
        */
        void destroy();



        ShoggothApplication* getApplication() override;



        Processor* help();


        /******************************************************************************
        */


        virtual void onActivate();



        virtual void onLoop
        (
            bool&,
            bool&,
            unsigned int&,
            bool&
        );



        /*
            Read sync object from server
        */
        Processor* getSyncFromServer();
};
