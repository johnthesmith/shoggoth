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
#include "../../../../../lib/core/payload_engine.h"

#include "../../shoggoth/net.h"

#include "../shoggoth_application.h"
#include "limb_teacher.h"



using namespace std;



class TeacherPayload : public PayloadEngine
{
    private:

        /* Neural net object */
        Net*            net             = NULL;
        LimbTeacher*    limb            = NULL;

        long long       lastChange      = 0;
        std::string     lastBatches     = "";

        long long       testId          = 0;

        int             currentIndexBatch   = -1;
        int             orderIndex          = 0;
        int             loopIndex           = 0;
        int             repeatIndex         = 0;

    public:

        /*
            Constructor
        */
        TeacherPayload
        (
            ShoggothApplication*,    /* Application object */
            string                  /* Payload Id */
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
            ShoggothApplication* aApp,
            std::string aId
        )
        {
            return new TeacherPayload( aApp, aId );
        }



        /*
            Destructor
        */
        void destroy() override;



        /*
            Return application object
        */
        ShoggothApplication* getApplication() override;



        LimbTeacher* getLimb();



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
        real getErrorLimit();



        /*
            Return error layer id from application config
            Teacher uses this layer for for controlling
        */
        string getIdErrorLayer();



        /*
            Return name of mode for batches
        */
        std::string requestCurrentBatches();



        /*
            Return enabled status
        */
        bool getEnabled();



        ParamList* getBatches();



        Param* nextBatchItem
        (
            ParamList*
        );


        /******************************************************************************
            Private methods
        */

        private:


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




