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
#include "shoggoth_db.h"
#include "layer.h"


class Limb;
//class Layer;



typedef real NeuronWeight;
const size_t NEURON_WEIGHT_SIZE = sizeof( NeuronWeight );


class Nerve: public Result
{
    private:

        /* Array of weights, it contins weight value */
        real*       weights         = NULL;
        /* Array of control values for max count 0 || 1 */
        real*       controls        = NULL;
        /* County of weights */
        int         weightsCount    = 0;

        /*
            Массив ссылок на веса.
            Каждая строка `parent` содержит все возможные индексы в `weights`
            для всех возможных своих 'child' нейронов.

                в строках перечень родителей
                в колонках для каждого родителя все возможные связи с потомками

            В случае если связи нет храним -1.

            for bindsCount = 9
            * ---------------------> parents
            | 0:p0c0 1:p1c0 2:p2c0
            | 3:p0c1 4:p1c1 5:p2c1
            | 6:p0c2 7:p1c2 8:p2c2
            |
            children

        */
        int*        binds           = NULL;
        /* Count of binds */
        int         bindsCount      = 0;

        real*       deltaWeights    = NULL;

        /* Size of weight window for SOME_TO_SOME */
        Point3i     size            = POINT_3I_0;

        /* Minimum weight for fill method */
        real        minWeight         = 0;
        /* Maximum weight for fill method */
        real        maxWeight         = 0;

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
        BindType,   /* Bind of nerve */
        Point3i     /* Window size */
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
        BindType,       /* Bind of nerve */
        Point3i         /* Window size */
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
        Set dimentions size
    */
    Nerve* setSize
    (
        const Point3i& = POINT_3I_0
    );



    /*
        Set dimentions size from params
    */
    Nerve* setSize
    (
        ParamList*
    );



    /*
        Return 3d layer size at neurons for each axis
    */
    Point3i getSize()
    {
        return size;
    }


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
        real = 1.0,
        /* Default MaxWeight */
        real = -1.0
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
    int getBindsCount()
    {
        return bindsCount;
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
    real* getWeights()
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
        real aValue
    )
    {
        weights[ aIndex ] = aValue;
        return this;
    }



    /*
        Set control value by index
    */
    Nerve* setControl
    (
        /* Index of weight */
        int aIndex,
        /* Value for bind */
        real aValue
    )
    {
        if( controls != NULL) controls[ aIndex ] = aValue;
        return this;
    }



    /*
        Return weight by index
    */
    real getDeltaWeight
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
        real aValue
    )
    {
        deltaWeights[ aIndex ] = aValue;
        return this;
    }



    /*
        Return weight index by bind index
    */
    int getWeightIndexByBind
    (
        /* Bind index */
        int a
    )
    {
        return binds[ a ];
    }




    /*
        Return weight by index
    */
    real getWeight
    (
        /* Index in weights */
        int a
    )
    {
        return weights[ a ];
    }



    /*
        Return weight by index
    */
    real getControl
    (
        /* Index in controls */
        int a
    )
    {
        return controls == NULL ? 1.0 : controls[ a ];
    }



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
        Return minimal weight
    */
    real getMinWeight()
    {
        return minWeight;
    }



    /*
        Set minimal weight
    */
    Nerve* setMinWeight
    (
        real aValue
    )
    {
        minWeight = aValue;
        return this;
    }




    /*
        Return maximal weight
    */
    real getMaxWeight()
    {
        return maxWeight;
    }




    /*
        Return maximal weight
    */
    real calcSumWeights()
    {
        real result = 0;
        for( int i = 0; i < weightsCount; i++ )
        {
            result += weights[ i ];
        }
        return result;
    }



    /*
        Set maximal weight
    */
    Nerve* setMaxWeight
    (
        real aValue
    )
    {
        maxWeight = aValue;
        return this;
    }



    /*
        Reflect child neuron index to parent neuron position
    */
    Point3i calcParentPosByChildIndex
    (
        /* Child index */
        int
    );



    /*
        Reflect paremt neuron index to child neuron position
    */
    Point3i calcChildPosByParentIndex
    (
        /* Child index */
        int
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



    /*
        Return index of weight by parent neuron index  and child neuron
        index
    */
    int getWeightIndex
    (
        int aParentIndex,
        int aChildIndex
    )
    {
        auto bindIndex = aParentIndex + parent -> getCount() * aChildIndex;
        return
        bindIndex >= 0 && bindIndex < bindsCount
        ? binds[ bindIndex ]
        : -1;
    }



    /*
        Calculate nerv weights
    */
    Nerve* calcWeights
    (
        real
    );



    /*
        Return parent index by bind index
    */
    int calcParentIndexByBind
    (
        /* Bind index */
        int a
    )
    {
        return a % parent -> getCount();
    }



    /*
        Return child index by bind index
    */
    int calcChildIndexByBind
    (
        /* Bind index */
        int a
    )
    {
        return a / parent -> getCount();
    }



    /*
        Allocate memomry buffer for weights and binds arrays
    */
    Nerve* allocate()
    {
        auto newCount = 0;

        int cFrom   = parent -> getCount();
        int cTo     = child -> getCount();

        /* Calculate new buffer size */
        switch( nerveType )
        {
            case ALL_TO_ALL:
                newCount = cFrom * cTo;
            break;
            case ONE_TO_ONE:
                newCount = max( cFrom, cTo );
            break;
            case SOME_TO_SOME:
                newCount = size.x * size.y * size.z;
                if( newCount == 0 )
                {
                    setResult( "size-window-is-undefined" );
                }
            break;
        }

        /* Calculate count of the binds */
        int newBindsCount = cFrom * cTo;

        purge();

        weightsCount = newCount;
        bindsCount = newBindsCount;

        /* Create buffer */
        weights = new real[ weightsCount ];

        /* Create delta buffer */
        deltaWeights = new real[ weightsCount ];

        /* Create binds array */
        binds = new int[  bindsCount ];

        /* For bind type max */
        if( bindType == BT_MAX )
        {
            /* Create control signal buffer */
            controls = new real[ weightsCount ];
        }

        getLog()
        -> trace( "Memory allocated" )
        -> prm( "Wieghts count", weightsCount )
        -> prm( "Controls count", weightsCount )
        -> prm( "Binds count", bindsCount );

        return this;
    }



    /*
        Return nerve id
    */
    string buildId()
    {
        return
        getParent() -> getId()
        + "-"
        + bindTypeToString( bindType )
        + "-"
        + getChild() -> getId();
    }
};
