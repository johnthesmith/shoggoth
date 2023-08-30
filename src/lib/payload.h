#pragma once

/*
    Pyaload.
    Base for custom modules and controllers

    Overall functionality of users can be encapsulated in a child of this class.
*/

/* Local libraries */
#include "result.h"
#include "application.h"



using namespace std;



class Payload : public Result
{
    private:

        Application* application = NULL;   /* Application object*/

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


        virtual Application* getApplication();


        Payload* loop();


        virtual void onLoop
        (
            bool&,
            bool&,
            unsigned int&,
            bool&
        );
};
