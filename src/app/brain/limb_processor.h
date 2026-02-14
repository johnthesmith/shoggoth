/*
    Its contain the Main neuralnet mathematics
*/
# pragma once


/*
    Limb - processor
*/

#include <cmath>

#include "../../../../../lib/core/mon.h"
#include "../../../../../lib/core/thread_manager.h"


#include "../../shoggoth/limb.h"
#include "../../shoggoth/limb/net.h"
#include "layer_processor.h"


class LimbProcessor;



class LimbProcessor : public Limb
{
private:
    /* External Net object */
    Net*            net             = NULL;
    /* Monitor object */
    Mon*            mon             = NULL;
    /* Chart list object */
    ChartList*      weightsChart    = NULL;

    ThreadManager*  threadManager   = NULL;

    /*
        Calculation state
    */
    /* Terminated status, stop the calculation and dump works */
    bool            terminated      = false;

    /*
        Settings
    */
    /* 0.0 - learning disable, max 0.1 recomended */
    real  learningSpeed           = 0.001;
    /* 0.0 - zero weight does not wakeup, max 0.0001 recomended */
    real  minWeight               = 0.0001;
    /* Maxumum weight */
    real  maxWeight               = 1000;
    /* Maxumum error, have to less then maxWeight */
    real  maxError                = 100;

    /* Each tickWrite from tick all Weights will be writen to file */
    int     tickWrite               = 10;
    /* Each tickChart from ешсл Chartss will be writen to mon */
    int     tickChart               = 1000;
    /* Configuration objtct for neurnos dump */
    ParamList* dumpConf             = NULL;

    /* Fps */
    ChartData* fps = NULL;
public:

    /*
        Constructor
    */
    LimbProcessor
    (
        Net*    /* Net limb object*/
    );



    /*
        Destructor
    */
    ~LimbProcessor();



    /*
        Create
    */
    static LimbProcessor* create
    (
        Net* /* The net object*/
    );



    /**********************************************************************
        ...
    */



    /*
        Set terminated signal
    */
    LimbProcessor* setTerminated
    (
        bool
    );



    /*
        Calculate all enabled layers
    */
    LimbProcessor* calc();



    /*
        The nerve list controlling
    */
    LimbProcessor* nerveControl();



    /*
        Create new layer for this limb and copy parameters from source layer.
        This method have to overriden at children Limbs.
    */
    LayerProcessor* copyLayerFrom
    (
        LayerProcessor* aLayerFrom
    );


    /**********************************************************************
        Setters and getters
    */


    /*
        Get learning speed
    */
    real getLearningSpeed();



    /*
        Set learning speed
    */
    LimbProcessor* setLearningSpeed
    (
        real /* Value */
    );



    /*
        Get tick fo write weight
    */
    int getTickWrite();



    /*
        Set tick for write weights
    */
    LimbProcessor* setTickWrite
    (
        int /* Value */
    );



    /*
        Set tick for write charts
    */
    LimbProcessor* setTickChart
    (
        int /* Value */
    );



    /*
        Get max eror
    */
    real getMaxError();



    /*
        Set max error
    */
    LimbProcessor* setMaxError
    (
        real /* Value */
    );




    /*
        Calculate neuron parent weights
    */
    LimbProcessor* neuronCalcWeight
    (
        Layer*, /* Layer for calculation */
        int     /* Neuron index of layer */
    );



    /*
        Copy configuration from argument
    */
    LimbProcessor* setDumpConf
    (
        ParamList*
    );



    /*
        Create processor lock file and waiting it removing
    */
    LimbProcessor* calcDebugWait
    (
        CalcStage aStage
    );



    /*
        Write debug monitoring
    */
    LimbProcessor* calcDebugDump
    (
        CalcStage aStage
    );



    LimbProcessor* weightsRead
    (
        /* Path folder for layers */
        string
    );



    LimbProcessor* weightsWrite
    (
        /* Path folder for layers */
        string
    );



    /*
        Dump layers
    */
    LimbProcessor* dumptypeLayerProcessing
    (
        /* file name */
        string,
        /* List of layers id*/
        ParamList*,
        /* Dataview ( how ) */
        Dataview,
        /* Data ( what ) */
        Data,
        /* Tick */
        int
    );



    /*
        Reset calc state
    */
    inline LimbProcessor* resetCalcState()
    {
        getLayerList() -> loop
        (
            []
            ( void* item )
            {
                (( LayerProcessor* ) item) -> setCalcState( WAIT );
                return false;
            }
        );
        return this;
    }
};
