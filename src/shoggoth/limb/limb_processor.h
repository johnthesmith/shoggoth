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

        /*
            Calculation state
        */

        /* Current tick */
        long long int   tick            = 0;
        /* Count of threads */
        int             threadCount     = 1;
        /* Automatic learning mode */
        bool            learning        = false;

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

        /* Each of finished calculation form tickWrite will be writen to file */
        int     tickWrite               = 10;

        ParamList* dumpWeightConf       = NULL;
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



        LayerList* getForwardList();
        LayerList* getBackwardList();




        /*
            Calculate all enabled layers
        */
        LimbProcessor* calc();



        /*
            The nerve list controlling
        */
        LimbProcessor* nerveControl
        (
            unsigned long long
        );



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
            Calculate neurons in the layer
        */
        LimbProcessor* layerCalcValue
        (
            Layer*, /* Layer for calculation */
            int,    /* Current thread number */
            bool&
        );



        LimbProcessor* layerCalcError
        (
            Layer*, /* Layer for calculation */
            int     /* Current thread number */
        );



        LimbProcessor* layerCalcWeight
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
