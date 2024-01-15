#pragma once

/* Local libraries */
#include "../graph/scene_payload.h"
#include "../graph/camera.h"
#include "../shoggoth/layer.h"
#include "../shoggoth/limb/net.h"
#include "../json/json.h"

#include "shoggoth_application.h"



using namespace std;



class Teacher : public Payload
{
    private:

        long int    lastBatchUpdate = 0;

        /*
            Config
        */
        double      errorLimit      = 0;
        string      idErrorLayer    = "";
        ParamList*  batches         = NULL;

    public:

        /*
            Constructor
        */
        Teacher
        (
            Net*
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
            Net*
        );



        /*
            Destructor
        */
        void destroy();



        /*
            Return application object
        */
        ShoggothApplication* getApplication();



        /******************************************************************************
            Payload methods
        */



        /******************************************************************************
            Getters and setters
        */



        Teacher* setErrorLimit
        (
            double
        );



        double getErrorLimit();



        Teacher* setIdErrorLayer
        (
            string
        );



        string getIdErrorLayer();



        ParamList* getBatches();



        Teacher* cmdValueToLayer
        (
            ParamList*
        );



        Teacher* cmdImageToLayer
        (
            ParamList*
        );



        Teacher* cmdFolderToLayer
        (
            ParamList*
        );



        /******************************************************************************
            Events
        */



        /*
            Run teacher loop
        */
        void onLoop();
};
