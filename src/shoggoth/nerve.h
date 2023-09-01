# pragma once

#include "../lib/result.h"
#include "../lib/log.h"

#include "bind_type.h"
#include "nerve_type.h"
#include "neuron.h"


class Layer;

class Nerve : public Result
{
    private:

        double*     weights         = NULL;         /* Array of weights */
        int         weightsCount    = 0;            /* County of weights */

        Log*        log             = NULL;         /* The log object */

        string      id          = "";
        BindType    bindType    = BT_VALUE;
        NerveType   nerveType   = ALL_TO_ALL;

        Layer*      parent      = NULL;
        Layer*      child       = NULL;

    public:

        /*
            Constructor
        */
        Nerve
        (
            Log*,       /* Log object*/
            string,     /* id */
            Layer*,     /* Parent layer */
            Layer*,     /* Child layer */
            NerveType,  /* Type of nerve */
            BindType,   /* Bind of nerve */
            double,     /* MinWeight */
            double      /* MaxWeight */
        );



        /*
            Destructor
        */
        ~Nerve();



        /*
            Create
        */
        static Nerve* create
        (
            Log*,       /* Log object*/
            string,     /* id */
            Layer*,     /* Parent layer */
            Layer*,     /* Child layer */
            NerveType,  /* Type of nerve */
            BindType,   /* Bind of nerve */
            double,     /* MinWeight */
            double      /* MaxWeight */
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
            Return BindType
        */
        BindType getBindType();



        /*
            Return NerveType
        */
        NerveType getNerveType();



        /*
            Return id
        */
        string getId();



        /*
            Return parent layer
        */
        Layer* getParent();



        /*
            Return child layer
        */
        Layer* getChild();



        /*
            Return count of weights
        */
        int getWeightsCount();



        static BindType bindTypeFromString
        (
            string
        );



        static NerveType nerveTypeFromString
        (
            string
        );



        /*
            Return the parent neuron by index of weight array
        */
        Neuron* getParentByWeightIndex
        (
            int        /* Index */
        );



        /*
            Return the child neuron by index of weight array
        */
        Neuron* getChildByWeightIndex
        (
            int        /* Index */
        );



        /*
            Return weight by index
        */
        double getWeight
        (
            int /* Index in weights */
        );



        /*
            Return the weights from and to indexes by child index
        */
        Nerve* getWeightsRangeByChildIndex
        (
            int,   /* index of neuron */
            int&,  /* index of weights begin for neurn */
            int&   /* index of weights eend for neuron */
        );



        /*
            Load nerve weights from buffer
        */
        Nerve* loadFromBuffer
        (
            char *, /* buffer */
            int     /* size of buffer */
        );



        /*
            Request the nerve weights array from the server
        */
        Nerve* Nerve::readFromServer
        (
            string, /* server ip address */
            int     /* server port number */
        );
};
