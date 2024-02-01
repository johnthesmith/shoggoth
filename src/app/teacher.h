/*
    Teacher payload
*/


#pragma once


/* Local libraries */

#include "../../../../lib/core/payload.h"
#include "../../../../lib/json/json.h"

#include "shoggoth_application.h"
#include "../shoggoth/limb/limb_teacher.h"



using namespace std;



class Teacher : public Payload
{
    private:

        LimbTeacher*    limb            = NULL;

        /*
            State
        */
        long int        lastBatchUpdate = 0;

        /*
            Config
        */
        double          errorLimit      = 0;
        string          idErrorLayer    = "";
        ParamList*      batches         = NULL;

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
