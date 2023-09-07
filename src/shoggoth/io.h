/*
    Shoggoth input ooutput system
    IO may works with:
        shoggoth server
        files
*/

#pragma once



#include "../lib/application.h"
#include "../lib/log.h"
#include "../lib/result.h"

#include "net.h"
#include "rpc_protocol.h"



using namespace std;



/*
    Io class
*/
class Io: public Result
{
    private:

        Net*        net     = NULL;
        ParamList*  answer  = NULL;
        ParamList*  request = NULL;

    public:

        Io
        (
            Net* /* Net object */
        );



        ~Io();



        /*
            Create the Io
        */
        static Io* create
        (
            Net* /* Net object */
        );



        /*
            Self destroy the neuron
        */
        void destroy();



        /*
            Call input output operation
        */
        Io* call
        (
            Command
        );


        /**************************************************************************
            Setters and getters
        */



        /*
            Return the Application object
        */
        Application* getApplication();



        /*
            Return log
        */
        Log* getLog();



        /*
            Return net
        */
        Net* getNet();



        /*
            Return answer parametes
        */
        ParamList* getAnswer();



        /*
            Return request parametes
        */
        ParamList* getRequest();



        /******************************************************************************
            File operation section
        */



        Io* fileReadNerve();
        Io* fileWriteNerve();
        Io* fileReadLayer();
        Io* fileWriteLayer();
};


