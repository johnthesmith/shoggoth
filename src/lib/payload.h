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



class Payload : public Result
{
    private:

        /* Application object*/
        Application*    application     = NULL;
        Log*            log             = NULL;
        thread*         threadObject    = NULL;
        bool            terminated      = false;
        string          id              = "";

        /*
            Internal loop emplimentation
            This method calls a user onLoop
        */
        Payload* internalLoop();

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


        /******************************************************************************
            Events
        */

        /*
            User emplementaion
            This method must be overriden
        */
        virtual void onLoop
        (
            bool&,
            bool&,
            unsigned int&,
            bool&
        );



        /*
            User emplementaion
            This method must be overriden
        */
        virtual void onRun();



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
};
