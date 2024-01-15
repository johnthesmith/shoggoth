/*
    Shoggoth nerve
    Bind between two neuron layers.
*/

# pragma once

#include "../lib/result.h"
#include "../lib/log.h"



#include "shoggoth_consts.h"


class Layer;
class Limb;

class Nerve: public Result
{
    private:

        double*     weights         = NULL;         /* Array of weights */
        int         weightsCount    = 0;            /* County of weights */

        Limb*       limb            = NULL;         /* The limb object */

        string      id              = "";
        BindType    bindType        = BT_VALUE;
        NerveType   nerveType       = ALL_TO_ALL;

        Layer*      parent          = NULL;
        Layer*      child           = NULL;

    public:

        /*
            Constructor
        */
        Nerve
        (
            Limb*,      /* Limb object*/
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
            Limb*,      /* The limb object*/
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



        /*
            Return weights pointer
        */
        double* getWeights();



        /*
            Set weight by index
        */
        Nerve* setWeight
        (
            int     /* Index of weight */,
            double  /* Value for bind */
        );



        static BindType bindTypeFromString
        (
            string
        );



        static NerveType nerveTypeFromString
        (
            string
        );



        /*
            Return the parent neuron index by index of weight array
        */
        int getParentByWeightIndex
        (
            int        /* Index */
        );



        /*
            Return the child neuron index by index of weight array
        */
        int getChildByWeightIndex
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
            Return the weights from and to indexes by parent index
        */
        Nerve* getWeightsRangeByParentIndex
        (
            int aIndex, /* index of parent neuron */
            int &aFrom, /* index of weights begin for neurn */
            int &aTo,   /* index of weights eend for neuron */
            int &aStep  /* step for shift between from and to */
        );



        /*
            Load nerve weights from buffer
        */
        Nerve* readFromBuffer
        (
            char *, /* buffer */
            size_t  /* size of buffer */
        );



        /*
            Put weights array from memory buffer with size
        */
        Nerve* writeToBuffer
        (
            char*&, /* Buffer */
            size_t& /* Size */
        );



        /*
            Return weight index  in nerve by neurons index
        */
        int getIndexByNeuronsIndex
        (
            int,    /* index From */
            int     /* index To */
        );

};
