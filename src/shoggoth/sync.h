# pragma once

/*
    Object for layer synchronization.
    The object contains records with attributes:
        layer - ID Layer
        client - ID Client
        forward - Forward calculation compleet flag
        backward - Backwart calculation compleet flag

    JSON example
    {
        [
            {
                "layer":"retina",
                "client":"main",
                "forward":true,
                "backward":false
            },
            {
                "layer":"cortex,
                "client":"main",
                "forward":false,
                "backward":false
            },
        ]
    }
*/



#include <iostream>
#include <cstring>



#include "../../../../lib/core/result.h"
#include "../../../../lib/core/log.h"
#include "../../../../lib/json/param_list.h"



class Sync : public ParamList
{
    private:

        Log*        log         = NULL;

    public:

        /*
            Constructor
        */
        Sync
        (
            Log*    /* Log object*/
        );



        /*
            Destructor
        */
        ~Sync();



        /*
            Create
        */
        static Sync* create
        (
            Log*       /* The log object*/
        );



        /*
            Destroy
        */
        void destroy();



        /*
            Return log object
        */
        Log* getLog();



        /*
            Fill object from Net config
        */
        Sync* fill
        (
            ParamList*  /* Source ParamList */
        );



        /*
            Set paramlist for client
        */
        Sync* set
        (
            ParamList*  /* Source ParamList */
        );




        /*
            Return sync by client
        */
        Sync* getByClient
        (
            string,     /* Client ID */
            ParamList*  /* Destination ParamList */
        );




        /*
            return true if net is complete for front and back flags
        */
        bool isComplete
        (
            string = "" /* Layer id. Empty for all layer */
        );



        bool isForward
        (
            string = "" /* Layer id. Empty for all layer */
        );



        bool isBackward
        (
            string = "" /* Layer id. Empty for all layer */
        );


        /*
            Set front flag
        */
        Sync* setForward
        (
            string, /* IDClient */
            string  /* IDLayer */
        );



        /*
            Return forward flag
        */
        bool getForward
        (
            string, /* IDClient */
            string  /* IDLayer */
        );



        /*
            Set back flag
        */
        Sync* setBackward
        (
            string, /* IDClient */
            string  /* IDLayer */
        );



        /*
            Return backward flag
        */
        bool getBackward
        (
            string, /* IDClient */
            string  /* IDLayer */
        );



        /*
            Return paramlist by layer and client or null if not exists
        */
        ParamList* getByClientAndLayer
        (
            string,  /* Id Client */
            string  /* Id Layer */
        );



        /*
            Collect all clients id
        */
        Sync* collectClientsAndLayers
        (
            ParamList*,
            ParamList*
        );



        /*
            Dump sync structore to log
        */
        Sync* toLog
        (
            int = -1 /* Layer index */
        );
};
