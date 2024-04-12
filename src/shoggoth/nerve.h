/*
    Shoggoth nerve
    Bind between two neuron layers.
*/

# pragma once

#include <functional>   /* for allocate lyambda */


#include "../../../../lib/core/result.h"
#include "../../../../lib/core/log_manager.h"

#include "shoggoth_consts.h"


class Layer;
class Limb;



typedef double NeuronWeight;
const size_t NEURON_WEIGHT_SIZE = sizeof( NeuronWeight );



class Nerve: public Result
{
    private:

        /* Array of weights */
        double*     weights         = NULL;
        double*     deltaWeights    = NULL;
        /* County of weights */
        int         weightsCount    = 0;

        /* Minimum weight for fill method */
        double      minWeight       = 0;
        /* Maximum weight for fill method */
        double      maxWeight       = 0;

        /* The log manger object */
        LogManager* logManager      = NULL;

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
            LogManager*,/* Log manager object*/
            Layer*,     /* Parent layer */
            Layer*,     /* Child layer */
            NerveType,  /* Type of nerve */
            BindType    /* Bind of nerve */
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
            LogManager*,    /* The log manager object*/
            Layer*,         /* Parent layer */
            Layer*,         /* Child layer */
            NerveType,      /* Type of nerve */
            BindType        /* Bind of nerve */
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
            Allocate memomry buffer for weights
        */
        Nerve* allocate
        (
            /* On Allocate callback */
            function <void ( Nerve* )>
        );



        /*
            Purge memomry buffer for weights
        */
        Nerve* purge();



        /*
            Fill weights
        */
        Nerve* fill
        (
            /*
                The default value MinWeight should be less then MaxWeight
                for using configuration properties
            */
            double = 1.0,    /* Default MinWeight */
            double = -1.0    /* Default MaxWeight */
        );


        /*
            Return BindType
        */
        BindType getBindType();



        /*
            Return NerveType
        */
        NerveType getNerveType();



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




        /*
            Return weights pointer
        */
        double getDeltaWeight
        (
            int
        );



        /*
            Set weight by index
        */
        Nerve* setDeltaWeight
        (
            int     /* Index of weight */,
            double  /* Value for bind */
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


        /*
            Return minimal weight
        */
        double getMinWeight();



        /*
            Set minimal weight
        */
        Nerve* setMinWeight
        (
            double aValue
        );



        /*
            Return maximal weight
        */
        double getMaxWeight();



        /*
            Set maximal weight
        */
        Nerve* setMaxWeight
        (
            double aValue
        );



        /*
            Return weight storage file path
        */
        string getWeightFileName
        (
            string aPath
        );



        /*
            Read Weigts from file
        */
        Nerve* loadWeight
        (
            string /* Path */
        );



        /*
            Write Weigts to file
        */
        Nerve* saveWeight
        (
            string /* Path */
        );



        /*
            Extract children weithds in to buffer by neuron index
        */
        Nerve* extractChildWeightsBuffer
        (
            int,        /* Neuron index */
            char*&,     /* Bufer */
            size_t&     /* Size of buffer */
        );



        /*
            Extract parent weithds in to buffer by child neuron index
        */
        Nerve* extractParentsWeightsBuffer
        (
            int     aNeuronIndex,
            char*   &aBuffer,
            size_t  &aSize
        );

};
