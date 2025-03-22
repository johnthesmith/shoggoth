/*
    Shoggoth nerve
    Bind between two neuron layers.
*/

# pragma once


#include "../../../../lib/core/result.h"
#include "../../../../lib/core/log_manager.h"
#include "../../../../lib/core/mon.h"
#include "../../../../lib/core/rnd.h"


#include "../../../../lib/core/chart_list.h"


#include "shoggoth_consts.h"
#include "layer.h"


class Limb;
//class Layer;



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
        BindType    bindType        = BT_ADD;
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
        template <typename Func>
        Nerve* allocate
        (
            /*
                Callback function
                void
                    Nerve*
            */
            Func aOnAllocate
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
            /* Random seed */
            Rnd*,
            /* Default MinWeight */
            double = 1.0,
            /* Default MaxWeight */
            double = -1.0
        );


        /*
            Return BindType
        */
        BindType getBindType()
        {
            return bindType;
        };




        /*
            Return NerveType
        */
        NerveType getNerveType()
        {
            return nerveType;
        }




        /*
            Return parent layer
        */
        Layer* getParent()
        {
            return parent;
        }



        /*
            Return child layer
        */
        Layer* getChild()
        {
            return child;
        }



        /*
            Return count of weights
        */
        int getWeightsCount()
        {
            return weightsCount;
        }



        /*
            Return weights pointer
        */
        double* getWeights()
        {
            return weights;
        }



        /*
            Set weight by index
        */
        Nerve* setWeight
        (
            /* Index of weight */
            int aIndex,
            /* Value for bind */
            double aValue
        )
        {
            weights[ aIndex ] = aValue;
            return this;
        }



        /*
            Return weight by index
        */
        double getDeltaWeight
        (
            int aIndex
        )
        {
            return deltaWeights[ aIndex ];
        }



        /*
            Set weight by index
        */
        Nerve* setDeltaWeight
        (
            /* Index of weight */
            int aIndex,
            /* Value for bind */
            double aValue
        )
        {
            deltaWeights[ aIndex ] = aValue;
            return this;
        }



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
            /* Index */
            int
        );



        /*
            Return weight by index
        */
        double getWeight
        (
            /* Index in weights */
            int aIndex
        )
        {
            return weights[ aIndex ];
        }



        /*
            Return the weights from and to indexes by child index
        */
        Nerve* getWeightsRangeByChildIndex
        (
            /* index of neuron */
            int,
            /* index of weights begin for neurn */
            int&,
            /* index of weights eend for neuron */
            int&
        );



        /*
            Return the weights from and to indexes by parent index
        */
        Nerve* getWeightsRangeByParentIndex
        (
            /* index of parent neuron */
            int aIndex,
            /* index of weights begin for neurn */
            int &aFrom,
            /* index of weights eend for neuron */
            int &aTo,
            /* step for shift between from and to */
            int &aStep
        );



        /*
            Load nerve weights from buffer
        */
        Nerve* readFromBuffer
        (
            /* buffer */
            char *,
            /* size of buffer */
            size_t
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
        double getMinWeight()
        {
            return minWeight;
        }



        /*
            Set minimal weight
        */
        Nerve* setMinWeight
        (
            double aValue
        )
        {
            minWeight = aValue;
            return this;
        }




        /*
            Return maximal weight
        */
        double getMaxWeight()
        {
            return maxWeight;
        }



        /*
            Set maximal weight
        */
        Nerve* setMaxWeight
        (
            double aValue
        )
        {
            maxWeight = aValue;
            return this;
        }




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
            /* Path */
            string
        );



        /*
            Write Weigts to file
        */
        Nerve* saveWeight
        (
            /* Path */
            string
        );



        /*
            Extract children weithds in to buffer by neuron index
        */
        Nerve* extractChildWeightsBuffer
        (
            /* Neuron index */
            int,
            /* Bufer */
            char*&,
            /* Size of buffer */
            size_t&
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



        /*
            Dump to log
        */
        Nerve* dumpToLog();



        /*
            Dump to log
        */
        Nerve* dumpToMon
        (
            Mon*,
            ChartList*
        );



        /*
            Calculate nerve id like
                parent_id-(bind,nerve)-child_id
        */
        string calcId();
};



/******************************************************************************
    Template emplementations
*/


/*
    Allocate memomry buffer for weights
*/
template <typename Func>
Nerve* Nerve::allocate
(
    /*
        Callback function
        void
            Nerve*
    */
    Func aOnAllocate
)
{
    auto newCount = 0;

    int cFrom   = parent -> getCount();
    int cTo     = child -> getCount();

    /* Calculate new buffer size */
    switch( nerveType )
    {
        case ALL_TO_ALL:
        {
            newCount = cFrom * cTo;
        }
        break;
        case ONE_TO_ONE:
            newCount = max( cFrom, cTo );
        break;
    }

    if( weightsCount != newCount )
    {
        /*
            Count has been changed,
            and the weights array must be reallocated
        */
        purge();

        weightsCount = newCount;

        /* Create buffer */
        weights = new double[ weightsCount ];

        /* Create delta buffer */
        deltaWeights = new double[ weightsCount ];

        getLog()
        -> trace( "Memory allocated" )
        -> prm( "Binds count", weightsCount );

        aOnAllocate( this );
    }

    return this;
}
