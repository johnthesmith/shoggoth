/*
    Shoggoth neural net
    Its contains mail neural logic, create and connect neurons Layers through Nerves.
    Emplements main methods for work with net
*/

#pragma once



#include <string>

#include "../lib/application.h"
#include "../lib/result.h"
#include "../json/param_list_file.h"
#include "../graph/scene.h"
#include "../lib/rpc_client.h"

#include "layer_list.h"
#include "nerve_list.h"



/*
    Lambda function for nerve weights loop
*/
typedef
function
<
    bool
    (
        int,        /* Index of weight in a nerve */
        Neuron*,    /* Neuron in tne parent layer */
        Neuron*,    /* Neuron in the child layer*/
        Nerve*      /* Nerve object */
    )
>
IndexWeightLambda;



class Net: public Result
{
    private:

        /* Objects */

        Application*    application     = NULL;     /* The log object */
        NerveList*      nerves          = NULL;     /* List of nerves*/
        LayerList*      layers          = NULL;     /* The layer list object */

        /* Events */
        string          supt            = "****";   /* Roles of the Net in SUPT */
        ParamListFile*  actions         = NULL;     /* Structure for resolve [actions] = f( supt, event ) */
        ParamList*      tasks           = NULL;     /* List of participants tasks */

        /* Calculation state */
        CalcDirection calcDirection     = CALC_FORWARD; /* Calculation direction */
        int calcLayerIndex              = 0;        /* Current lsyer for calculation */

        /* Settings */

        string          id              = "";       /* Net id */
        int             threadCount     = 1;        /* Count of threads */

        double  learningSpeed           = 0.001;    /* 0.0 - learning disable, max 0.1 recomended */
        double  wakeupWeight            = 0.0001;   /* 0.0 - zero weight dos not wakeup, max 0.0001 recomended */
        double  errorNormalize          = 0.0;      /* 0.0 - full error transfer,  1.0 - full dependency from sum weight of layer */
        double  sensivity               = 10;       /* Sensivity of neuronet [ 0; +oo], Set to sensivity of each layer */
        bool    learningMode            = true;     /* True for backweard calculation in learning */

        string  storagePath             = "net";

        LayerList* forwardList          = NULL;     /* Layer list with forward calculation sequence */
        LayerList* backwardList         = NULL;     /* Layer list with backward calculation sequence */

        long int    lastLoopMoment      = 0;        /* */
        long int    loopTimeoutMcs      = 1000000;

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
        Net
        (
            Application*    /* Application object */
        );



        /*
            Destructor
        */
        ~Net();



        /*
            Static constructor
        */
        static Net* create
        (
            Application*
        );



        /*
            Destructor
        */
        void destroy();



        /*
            Clear all layers
        */
        Net* clear();



        /*
            Create new nerve
        */
        Nerve* createNerve
        (
            string,         /* Nerve id */
            Layer*,         /* Layer source */
            Layer*,         /* Layer destination */
            NerveType,
            BindType,
            double,
            double
        );



        /*
            Delete layer by Id
        */
        Net* deleteNerve
        (
            string          /* Id of layer */
        );



        /*
            Return layer by Id
        */
        Layer* getLayerById
        (
            string /* Id of layer */
        );



        /*
            Request actual data layer form server
        */
        Net* writeLayersToServer();



        /*
            Build layers calculation sequence list
        */
        Net* precalc();



        /*
            Calculate all enabled layers
        */
        Net* calc();



        /*
            Return application object
        */
        Application* getApplication();




        /*
            Return log object
        */
        Log* getLog();




        Net* switchShowLayer();



        /*
            Set learning mode
        */
        Net* setLearningMode
        (
            bool /* Value */
        );



        /*
            Get learning mode
        */
        bool getLearningMode();



        /*
            Switch learning mode true/false
        */
        Net* switchLearningMode();



        /*
            Return layer list
        */
        LayerList* getLayers();



        /*
            Set learning speed
        */
        Net* setLearningSpeed
        (
            double
        );



        /*
            Set wakeup weight
        */
        Net* setWakeupWeight
        (
            double
        );



        /*
            Set error normalize
        */
        Net* setErrorNormalize
        (
            double
        );



        /*
            Get learning speed
        */
        double getLearningSpeed();



        /*
            Get wakeup weight k
        */
        double getWakeupWeight();



        /*
            Get error normalize
        */
        double getErrorNormalize();



        Net* setStoragePath
        (
            const string
        );



        string getStoragePath();


        /*
            Return list ov nerves
        */
        NerveList* getNerves();



        Net* setId
        (
            string
        );


        /******************************************************************************
            Layers
        */


        /*
            Create new layer
        */
        Layer* createLayer
        (
            string /* Name of layer */
        );



        /*
            Delete layer by Id
        */
        Net* deleteLayer
        (
            string  /* Id of layer */
        );



        /*
            Load layer structure from param list
            Layer may be resized.
        */
        Net* loadLayer
        (
            Layer*,
            ParamList*
        );



        /*
            Load layer from net.layers section
        */
        Layer* loadLayerFromConfig
        (
            string,
            ParamList*
        );



        /*
            Remove layers absent in the list
        */
        Net* purgeLayers
        (
            ParamList*
        );



        /*
            Read net configuration and reallocate net objects
        */
        Net* readNet();



        /*
            Set processors count for calculation
        */
        Net* setThreadCount
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
        Net* calcReset();



        /*
            Main event handler
        */
        Net* event
        (
            Event
        );



        /*
            Create roles strung of the process in SUPT format
            S - process works with server otherwice with local net
            U - process has a UI interface
            P - process uses as processor
            T - process uses as teacher
        */
        Net* buildSuptAndTasks();

        /* Dump sync info to log */
        Net* syncToLog
        (
            Layer* layer
        );




        bool isNextLoop();



        bool nerveWeightLoop
        (
            NeuronList*,
            IndexWeightLambda
        );



        Net* setCalcTick();



        bool getCalcTick();



        Net* setCalcDebug
        (
            bool
        );


        bool getCalcDebug();


        int getCalcLayerIndex();



        LayerList* getForwardList();
        LayerList* getBackwardList();
};

