#pragma once

/*
    Pyaload.
    Base for custom modules and controllers

    Overall functionality of users can be encapsulated in a child of this class.
*/

/* Vanilla librarits */
#include <thread>

/* Local libraries */
#include "result.h"
#include "application.h"




using namespace std;



enum ThreadState
{
    THREAD_STATE_WAIT_PAUSE,
    THREAD_STATE_PAUSE,
    THREAD_STATE_WORK
};



class Payload : public Result
{
    private:

        /* Application object*/
        Application*    application     = NULL;
        thread*         threadObject    = NULL;
        bool            terminated      = false;

        /* States */
        unsigned int    loopTimeoutMcs  = 0;
        bool            idling          = true;
        string          id              = "";
        ThreadState     state           = THREAD_STATE_PAUSE;

        /*
            Internal loop emplimentation
            This method calls a user onLoop
        */
        Payload* internalLoop();

        /*
            Set paused confirmation
        */
        Payload* setPaused
        (
            bool
        );


    public:

        /*
            Constructor
        */
        Payload
        (
            Application*
        );



        ~Payload();


        /*
            Creator
        */
        static Payload* create
        (
            Application*
        );



        /*
            Destructor
        */
        void destroy();



        /*
            Get scene value
        */
        Log* getLog();



        /*
            Run payload
        */
        Payload* run
        (
            bool = false    /* True for run like thread */
        );



        /*
            Main payload loop
        */
        Payload* loop
        (
            bool = false    /* True for run like thread */
        );



        /*
            Set terminate flag
        */
        Payload* terminate();



        /*
            Set idling mode
                true - pause will work
                false - pause will be skip
        */
        Payload* setIdling
        (
            bool    /* Value */
        );



        /*
            Set loop time out at microseconds
        */
        Payload* setLoopTimeoutMcs
        (
            unsigned int /* Value */
        );



        /*
            Get loop time out at microseconds
        */
        unsigned int getLoopTimeoutMcs();



        /******************************************************************************
            Events
        */

        /*
            User emplementaion
            This method must be overriden
        */
        virtual void onLoop();



        /*
            Payload loop before default even
        */
        virtual void onLoopBefore();



        /*
            Payload loop after default event
        */
        virtual void onLoopAfter();



        /*
            User emplementaion
            This method must be overriden
        */
        virtual void onRun();



        /*
            On pause event
        */
        virtual void onPause();



        /*
            On pause event when process paused
        */
        virtual void onPaused();



        /*
            On resume event
        */
        virtual void onResume();




        /******************************************************************************
            Setters and getters
        */

        /*
            Returen applicaiton pointer
        */
        virtual Application* getApplication();


        /*
            Set payload id
        */
        Payload* setId
        (
            string
        );



        /*
            Set order for pause
        */
        Payload* pause();



        /*
            Continue process after pause
        */
        Payload* resume();



        /*
            Wait pause
        */
        Payload* waitPause();

};
