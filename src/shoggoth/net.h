/*
    Shoggoth neural net
*/

#pragma once

/*
    Neuron net class for Shogogth.
    Contains list of neuron layers.
    Emplements main methods for work with net
*/



#include <string>

#include "../lib/application.h"
#include "../lib/result.h"
#include "../json/param_list_file.h"
#include "../graph/scene.h"
#include "../lib/rpc_client.h"

#include "bind_type.h"
#include "layer_list.h"
#include "nerve_list.h"
#include "shoggoth_consts.h"




class Net: public Result
{
    private:

        /* State */

        Application*    application     = NULL;     /* The log object */
        NerveList*      nerves          = NULL;     /* List of nerves*/
        LayerList*      layers          = NULL;     /* The layer list object */

        /* Settings */

        string          id              = "";       /* Net id */

        double  learningSpeed           = 0.001;    /* 0.0 - learning disable, max 0.1 recomended */
        double  wakeupWeight            = 0.0001;   /* 0.0 - zero weight dos not wakeup, max 0.0001 recomended */
        double  errorNormalize          = 0.0;      /* 0.0 - full error transfer,  1.0 - full dependency from sum weight of layer */
        double  sensivity               = 10;       /* Sensivity of neuronet [ 0; +oo], Set to sensivity of each layer */
        bool    learningMode            = false;    /* True for backweard calculation in learning */

        string  storagePath             = "net";

        /* Calculation state */

        bool calcDirection              = false;    /* Calculation direction true - forward, false - backward */
        int calcLayerIndex              = 0;        /* Current lsyer for calculation */

        /* Events */
        string          supt            = "****";   /* Roles of the Net in SUPT */
        ParamListFile*  actions         = NULL;     /* Structure for resolve [actions] = f( supt, event ) */
        ParamList*      tasks           = NULL;     /* List of participants tasks */

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
        Net* buildSuptAndTasks
        (
            ParamList*
        );
};

