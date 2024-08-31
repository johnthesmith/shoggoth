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
#include "../../../../../lib/core/payload.h"
#include "../../shoggoth/limb/net.h"



using namespace std;



class TeacherPayload : public Payload
{
    private:

        /* Neural net object */
        Net*        net             = NULL;

        /* State */
        long int    lastConfigCheck = 0;

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
            Private methods
        */

        private:

        /*
            Processing of the loop
        */
        TeacherPayload* processing();
};
