/*
    This file of Teacher Shoggoth network
    Main application loop.
    Contains the Net object.

    Authors:
        still@itserv.ru
        igor_ptx@gmail.com
*/

#pragma once


/* Local libraries */
#include "../../../../../lib/json/json.h"
#include "../../../../../lib/core/payload.h"
#include "../../shoggoth/limb/net.h"

/* Local libraries */
#include "../../shoggoth/limb/limb_teacher.h"



using namespace std;



class TeacherPayload : public Payload
{
    private:

        /* Neural net object */
        Net*            net             = NULL;
        LimbTeacher*    limb            = NULL;

        long long       lastChange      = 0;


    public:

        /*
            Constructor
        */
        TeacherPayload
        (
            TeacherApplication*,  /* Application object */
            string,                 /* Net id */
            string                  /* Net version */
        );



        /*
            Destructor
        */
        ~TeacherPayload();



        /*
            Creator
        */
        static TeacherPayload* create
        (
            TeacherApplication*,
            string aNetId,
            string aNetVersion
        );



        /*
            Destructor
        */
        void destroy() override;



        /*
            Return application object
        */
        TeacherApplication* getApplication() override;



        /*
            Help method
        */
        TeacherPayload* help();



        /******************************************************************************
            Payload events
        */



        /*
            Main teacher loop event
        */
        virtual void onLoop() override;


        /******************************************************************************
            Getters and setters
        */

        /*
            Return current error limit from Net config
        */
        double getErrorLimit();



        /*
            Return error layer id from application config
            Teacher uses this layer for for controlling
        */
        string getIdErrorLayer();



        /*
            Return name of mode for batches from application config
        */
        string getMode();



        /*
            Return enabled status
        */
        bool getEnabled();



        ParamList* getBatches();


        /******************************************************************************
            Private methods
        */

        private:

        /*
            Processing of the loop
        */
        TeacherPayload* processing();


        /*
            Batch put value to layer
        */
        TeacherPayload* cmdValueToLayer
        (
            ParamList*
        );



        TeacherPayload* cmdValuesToLayer
        (
            ParamList*
        );



        TeacherPayload* cmdImageToLayer
        (
            ParamList*
        );



        TeacherPayload* cmdFolderToLayer
        (
            ParamList*
        );
};
