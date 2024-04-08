/*
    Teacher payload
*/


#pragma once


/* Local libraries */

#include "../../../../lib/core/payload.h"
#include "../../../../lib/core/mon.h"
#include "../../../../lib/json/json.h"

#include "shoggoth_application.h"
#include "../shoggoth/limb/limb_teacher.h"



using namespace std;



class Teacher : public Payload
{
    private:

        LimbTeacher*    limb            = NULL;
        /* Monitor object */
        Mon*            mon             = NULL;

        /*
            Config
        */
        double          errorLimit      = 0;
        string          idErrorLayer    = "";
        string          mode            = "";
        ParamList*      batches         = NULL;

        long long       lastChange      = 0;

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



        Teacher* setMode
        (
            string
        );



        string getIdErrorLayer();



        ParamList* getBatches();



        Teacher* cmdValueToLayer
        (
            ParamList*
        );



        Teacher* cmdValuesToLayer
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
