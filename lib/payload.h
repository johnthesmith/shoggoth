#pragma once

/*
    Pyaload.
    Base for custom modules and controllers

    Overall functionality of users can be encapsulated in a child of this class.
*/

/* Local libraries */
#include "result.h"
#include "log.h"



using namespace std;



class Payload : public Result
{
    private:

        Log& log;   /* Log object*/

    public:

        /*
            Constructor
        */
        Payload
        (
            Log&
        );



        ~Payload();


        /*
            Creator
        */
        static Payload* create
        (
            Log&
        );



        /*
            Destructor
        */
        void destroy();



        /*
            Get scene value
        */
        Log& getLog();
};
