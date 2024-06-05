#pragma once

/* Core libraries */
#include "../../../../lib/core/payload.h"

/* Shogoth libraries */
#include "shoggoth_role_application.h"
#include "../shoggoth/limb/limb_processor.h"
#include "../shoggoth/limb/net.h"



using namespace std;



class Processor : public Payload
{
    private:

        /* Processors limb */
        LimbProcessor*  limb            = NULL;

    public:

        /*
            Constructor
        */
        Processor
        (
            Net*
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
            Net*
        );



        /*
            Destructor
        */
        void destroy() override;



        /****************************************************************************
            Setters and getters
        */



        /*
            Return application object
        */
        ShoggothRoleApplication* getApplication() override;



        /*
            Return l;imb object
        */
        LimbProcessor* getLimb();


        /****************************************************************************
            Events
        */


        /*
            Run net calculateion
        */
        virtual void onLoop() override;
};
