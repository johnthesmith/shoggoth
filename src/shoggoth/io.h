/*
    Shoggoth input ooutput system
    IO may works with:
       shoggoth server
        files
*/

#pragma once



#include "../../../../lib/core/application.h"
#include "../../../../lib/core/result.h"
#include "../../../../lib/sock/rpc_client.h"

#include "limb/net.h"
#include "shoggoth_consts.h"


using namespace std;



/*
    Io class
*/
class Io: public Result
{
    private:

        Net*        net         = NULL;
        ParamList*  answer      = NULL;
        ParamList*  request     = NULL;

        /* For true IO is an owner of the Answer and must destroy it */
        bool        answerOwner = false;


        /*
            Event handlers
        */
        void handleBeforeCall
        (
            RpcClient*
        );

        void handleAfterCall
        (
            RpcClient*
        );

    public:

        Io
        (
            Net*,               /* Net object */
            ParamList* = NULL   /* External answer */
        );



        ~Io();



        /*
            Create the Io
        */
        static Io* create
        (
            Net*,               /* Net object */
            ParamList* = NULL   /* External answer */
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



        /*
            Read net structore
        */
        Io* fileReadNet();



        /*
        */
        Io* fileWriteValues();



        Io* fileReadValues();


        /*
            Write layers errors to file
        */
        Io* fileWriteErrors();



        /*
            Read layer errors from file
        */
        Io* fileReadErrors();



        /*
            Write nerve weights to file
        */
        Io* fileWriteWeights();



        /*
            Read nerve weights from file
        */
        Io* fileReadWeights();



        /*
            Call input output operation
        */
        Io* disconnect();



        /******************************************************************************
            Shoggoth Rpc Client methods
        */



        /*
            Clone net from id and version
        */
        Io* cloneNet
        (
            string, /* Net id */
            string, /* Net version */
            int,    /* Parent generation */
            bool    /* True for mutation */
        );



        /*
            Switch net from id and version
        */
        Io* switchNet
        (
            string, /* Net id */
            string  /* Net version */
        );




        /*
            Return server net mode
        */
        Io* getNetMode
        (
            NetMode &
        );



        /*
            Set server net mode
        */
        Io* setNetMode
        (
            NetMode,
            ParamList*
        );
};

