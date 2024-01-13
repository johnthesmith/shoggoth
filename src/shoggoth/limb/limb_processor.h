# pragma once

/*
    Limb - processor
*/



#include "../limb.h"
#include "net.h"



class LimbProcessor : public Limb
{
    private:

        LayerList*      forwardList     = NULL;     /* Layer list with forward calculation sequence */
        LayerList*      backwardList    = NULL;     /* Layer list with backward calculation sequence */

        /* Calculation state */
        CalcDirection   calcDirection   = CALC_FORWARD; /* Calculation direction */
        int             calcLayerIndex  = 0;        /* Current lsyer for calculation */

        int             threadCount     = 1;        /* Count of threads */

        /* Settings */
        double  learningSpeed           = 0.001;    /* 0.0 - learning disable, max 0.1 recomended */
        double  wakeupWeight            = 0.0001;   /* 0.0 - zero weight dos not wakeup, max 0.0001 recomended */
        double  errorNormalize          = 0.0;      /* 0.0 - full error transfer,  1.0 - full dependency from sum weight of layer */
        double  sensivity               = 10;       /* Sensivity of neuronet [ 0; +oo], Set to sensivity of each layer */
        bool    learningMode            = true;     /* True for backweard calculation in learning */

        /*
            Calculation debug mode
            With calcDebug == true each calculation loop will be
            stoped till calcTick is not true.
        */
        bool        calcDebug           = false;
        bool        calcTick            = false;

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
            Set error normalize
        */
        LimbProcessor* setErrorNormalize
        (
            double
        );



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
            Get wakeup weight k
        */
        double getWakeupWeight();



        /*
            Set wakeup koeff
        */
        LimbProcessor* setWakeupWeight
        (
            double /* Value */
        );



        /*
            Get error normalize
        */
        double getErrorNormalize();



        LimbProcessor* setCalcDebug
        (
            bool
        );


        bool getCalcDebug();


        int getCalcLayerIndex();



        /*
            Calculate neuron of layer
        */
        LimbProsessor* neuronCalcValue
        (
            Layer*, /* Layer for calculation */
            int     /* Neuron index of layer */
        );



        /*
            Calculate neuron
        */
        LimbProcessor* neuronLearning
        (
            Layer*, /* Layer for calculation */
            int,    /* Neuron index of layer */
            double, /* */
            double, /* k for weight changing */
            double  /* */
        );
};
