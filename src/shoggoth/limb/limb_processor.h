/*
    Its contain the Main neuralnet mathematics
*/
# pragma once



/*
    Limb - processor
*/


#include "../../../../../lib/core/mon.h"

#include "../limb.h"
#include "net.h"



class LimbProcessor : public Limb
{
    private:
        /* Net object */
        Net*            net             = NULL;
        /* Monitor object */
        Mon*            mon             = NULL;
        /* Layer list with forward calculation sequence */
        LayerList*      forwardList     = NULL;
        /* Layer list with backward calculation sequence */
        LayerList*      backwardList    = NULL;

        /* Calculation state */
        CalcDirection   calcDirection   = CALC_FORWARD; /* Calculation direction */
        int             calcLayerIndex  = 0;        /* Current lsyer for calculation */
        long long int   tick            = 0;        /* Current tick */
        int             threadCount     = 1;        /* Count of threads */
        bool            learning        = false;    /* Automatic learning mode */

        /* Settings */
        double  learningSpeed           = 0.001;    /* 0.0 - learning disable, max 0.1 recomended */
        double  minWeight               = 0.0001;   /* 0.0 - zero weight does not wakeup, max 0.0001 recomended */
        double  maxWeight               = 1000;     /* Maxumum weight */
        double  maxError                = 100;      /* Maxumum error, have to less then maxWeight */

        int     tickWrite               = 10;       /* Each of finished calculation form tickWrite will be writen to file */

        ParamList* dumpWeightConf       = NULL;     /**/
        unsigned long long int  frame   = 0;

        /*
            Calculation debug mode
            With calcDebug == true each calculation loop will be
            stoped till calcTick is not true.
        */
        bool        calcDebug           = false;

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



        /* Dump sync info to log */
        LimbProcessor* syncToLog
        (
            Layer* layer
        );



        /*
            Return calc stage for layers with direction
        */
        CalcStage getCalcStage
        (
            CalcDirection CALC_ALL /* Direction */
        );



        /*
            Reset forward and backward counts for layers
        */
        LimbProcessor* calcReset();



        /*
            Set processors count for calculation
        */
        LimbProcessor* setThreadCount
        (
            int
        );



        /*
            Return processors count
        */
        int getThreadCount();



        /*
            Load parents layers and check forward calculation
            return true if all parents layers is forward calculated
            otherwise return false
        */
        bool preparedParents
        (
            Layer* /* Layer for parents check */
        );



        /*
            Load children layers and check backward calculation
            return true if all children layers is backward calculated
            otherwise return false
        */
        bool preparedChildren
        (
            Layer* /* Layer for children check */
        );


        LayerList* getForwardList();
        LayerList* getBackwardList();




        /*
            Calculate all enabled layers
        */
        LimbProcessor* calc();



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



        LimbProcessor* setCalcDebug
        (
            bool
        );


        bool getCalcDebug();



        int getCalcLayerIndex();



        /*
            Calculate neuron of layer
        */
        LimbProcessor* neuronCalcValue
        (
            Layer*, /* Layer for calculation */
            int,    /* Neuron index of layer */
            bool&
        );



        /*
            Calculate neuron
        */
        LimbProcessor* neuronLearning
        (
            Layer*, /* Layer for calculation */
            int     /* Neuron index of layer */
        );



        /*
            Calculate neurons in the layer
        */
        LimbProcessor* layerCalcValue
        (
            Layer*, /* Layer for calculation */
            int,    /* Current thread number */
            bool&
        );



        LimbProcessor* layerLearning
        (
            Layer*, /* Layer for calculation */
            int     /* Current thread number */
        );



        /*
            Calculate start neuron for processors operations
        */
        int calcNeuronFrom
        (
            Layer*,
            int
        );



        /*
            Caluculate end of neurons for processors operations
        */
        int calcNeuronTo
        (
            Layer*,
            int
        );
};
