#pragma once

/* Local libraries */
#include "../graph/scene_payload.h"
#include "../graph/camera.h"
#include "../shoggoth/layer.h"
#include "../shoggoth/net.h"
#include "../json/json.h"

#include "shoggoth_application.h"


using namespace std;



class Teacher : public Payload
{
    private:

        Net*        net             = NULL;
        Json*       batch           = NULL;
        long int    lastBatchUpdate = 0;

    public:

        /*
            Constructor
        */
        Teacher
        (
            ShoggothApplication*
        );



        /*
            Destructor
        */
        ~Teacher();



        /*
            Creator
        */
        static Teacher* create
        (
            ShoggothApplication*
        );



        /*
            Destructor
        */
        void destroy();



        ShoggothApplication* getApplication();



        /******************************************************************************
            Payload methods
        */


        /*
            Run payload
        */
        void onLoop
        (
            bool&,
            bool&,
            unsigned int&,
            bool&
        ) override;



        Teacher* task();
};
