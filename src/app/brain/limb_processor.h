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

#include "calc_table.h"


class LimbProcessor;


/*
    Task structure
*/
struct LayerCalcTask
{
    /* Limb calculation */
    LimbProcessor*  limb        = nullptr;
    /* Layer for calculation */
    Layer*          layer       = nullptr;
    /* From neuron */
    int             neuronFrom  = 0;
    /* To nauron */
    int             neuronTo    = 0;
    /* With data calculation type */
    Data            data       = DATA_UNKNOWN;
};







class LimbProcessor : public Limb
{
    private:
        /* External Net object */
        Net*            net             = NULL;
        /* Monitor object */
        Mon*            mon             = NULL;
        /* Chart list object */
        ChartList*  weightsChart        = NULL;
        /* Create thread manager */
        ThreadManager* threadManager    = NULL;

        CalcTable*      calcTableValues = NULL;
        CalcTable*      calcTableErrors = NULL;

        /*
            Calculation state
        */
        /* Automatic learning mode, indicates learning stage */
        bool            learning        = false;
        /* Terminated status, stop the calculation and dump works */
        bool            terminated      = false;

        /*
            Settings
        */
        /* 0.0 - learning disable, max 0.1 recomended */
        double  learningSpeed           = 0.001;
        /* 0.0 - zero weight does not wakeup, max 0.0001 recomended */
        double  minWeight               = 0.0001;
        /* Maxumum weight */
        double  maxWeight               = 1000;
        /* Maxumum error, have to less then maxWeight */
        double  maxError                = 100;

        /* Count of threads */
        int     maxThreadCount          = 1;
        int     minNeuronPerThread      = 0;

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
            Set processors count for calculation
        */
        LimbProcessor* setMaxThreadCount
        (
            int
        );



        /*
            Return processors count
        */
        int getMaxThreadCount();



        LimbProcessor* setMinNeuronPerThread
        (
            int a
        );



        int getMinNeuronPerThread();




        /*
            Calculate all enabled layers
        */
        LimbProcessor* calc();



        /*
            The nerve list controlling
        */
        LimbProcessor* nerveControl();



        /*
            Build layers calculation sequence list
        */
        LimbProcessor* precalc();



        /**********************************************************************
            Setters and getters
        */


        /*
            Set learning mode
        */
        LimbProcessor* setLearningMode
        (
            bool /* Value */
        );



        /*
            Get learning mode
        */
        bool getLearningMode();




        /*
            Get learning speed
        */
        double getLearningSpeed();



        /*
            Set learning speed
        */
        LimbProcessor* setLearningSpeed
        (
            double /* Value */
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
            Get wakeup weight k
        */
        double getMinWeight();



        /*
            Set wakeup koeff
        */
        LimbProcessor* setMinWeight
        (
            double /* Value */
        );



        /*
            Get max weight
        */
        double getMaxWeight();



        /*
            Set max weight
        */
        LimbProcessor* setMaxWeight
        (
            double /* Value */
        );



        /*
            Get max eror
        */
        double getMaxError();



        /*
            Set max error
        */
        LimbProcessor* setMaxError
        (
            double /* Value */
        );



        int getCalcLayerIndex();



        /*
            Calculate neuron of layer
        */
        LimbProcessor* neuronCalcValue
        (
            Layer*, /* Layer for calculation */
            int     /* Neuron index of layer */
        );



        /*
            Calculate neuron error
        */
        LimbProcessor* neuronCalcError
        (
            Layer*, /* Layer for calculation */
            int     /* Neuron index of layer */
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
            Method return count of thread using following property
                maxThreadCount
                minNeuronPerThread

        */
        LimbProcessor* calcThreadCount();



        /*
            Calculate neurons in the layer
                values
                errors
                weights
        */
        LimbProcessor* layerCalc
        (
            Layer*, /* Layer for calculation */
            Data
        );



        /*
            Calculate start neuron for processors operations
        */
        int calcNeuronFrom
        (
            Layer* aLayer,
            int aThreadCount,   /* Thread count */
            int aThreadNumber   /* Thread number */
        )
        {
            return floor
            (
                (double) aLayer -> getCount() * (double) aThreadNumber / (double) aThreadCount
            );
        }




        /*
            Caluculate end of neurons for processors operations
        */
        int calcNeuronTo
        (
            Layer*  aLayer,
            /* Thread count */
            int     aThreadCount,
            /* Thread number */
            int     aThreadNumber
        )
        {
            return calcNeuronFrom( aLayer, aThreadCount, aThreadNumber + 1 );
        }



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
            Stop the thread manager
        */
        LimbProcessor* down();



        /*
            Stop the thread manager
        */
        LimbProcessor* up();



        LimbProcessor* buildCalcTables();



        LimbProcessor* calcDebugDump
        (
            CalcStage aStage
        );
};
