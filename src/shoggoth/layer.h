/*
    Layer

    Contains the NeuronList object.
    The layer creates new neurons or removes them when resized.



    Table of function using for the type of layers

        layers  FrontFunc   BackFunc    CalcError  CalcWeight Example
        --------------------------------------------------------------------------
        in      NULL        NULL        NONE       NONE       retina, bias, sample
        cortex  RELU        ONE         LEARNING   CALC       cortex
        result  RELU        ONE         LEARNING   CALC       result
        error   LINE        LINE        VALUE      NONE       error
        command STEP        NULL        NONE       NONE       command
*/


#pragma once

#include <string>

#include "../../../../lib/core/result.h"
#include "../../../../lib/graph/point3i.h"      /* Size of layer */
#include "../../../../lib/json/param_list.h"    /* For read size from params */

#include "../../../../lib/core/chart_list.h"    /* TODO remove it*/
#include "../../../../lib/core/chart_data.h"
#include "../../../../lib/core/mon.h"

#include "func.h"
#include "nerve_list.h"
#include "shoggoth_consts.h"

using namespace std;



/*
    Predeclaration
*/

class Limb;
class Nerve;



class Layer : public Result
{
    private:

        /*
            States
        */
        /* Limb object */
        Limb*           limb                    = NULL;
        /* Count fo neurons */
        int             count                   = 0;
        /* Dimention size */
        Point3i         size                    = POINT_3I_0;

        bool            errorChange             = false;        /* True - method errorChange return true for any neuron, else false */
        bool            incomeChanged           = false;        /* True if preceptron chenged. Set in neuron->setValue*/

        /* Settings */
        string          id                      = "";           /* Id of layer */
        string          name                    = "";           /* Name of layer */

        string          storagePath             = "";

//        /* List of actions for layer */
//        ParamList*      actions                 = NULL;

        /*
            Plans of neurons data
        */

        double*         values                  = NULL;
        double*         errors                  = NULL;

        /*
            Layer settings
        */
        ErrorCalc       errorCalc               = EC_NONE;
        WeightCalc      weightCalc              = WC_NONE;

        /*
            Ticker
            Count tick between two tickerDrop calls
            Monotone incremental counter
        */
        long long int   tickCount               = -1;

        /*
            Statistics block
        */
        ChartData*      chartTick               = NULL;
        ChartData*      chartValues             = NULL;
        ChartData*      chartErrors             = NULL;


        /*
            Set neurons count and reallocate plans
            Do not use this method.
            Use the setSize
        */
        virtual Layer* setCount
        (
            const int = 0 /* New count */
        );

    public:

        /* Function activation */
        NeuronFunc*     frontFunc               = &FUNC_NULL;
        /* Learning function */
        NeuronFunc*     backFunc                = &FUNC_NULL;

        /*
            Constructor
        */
        Layer
        (
            Limb*,          /* Limb object */
            string = ""     /* id */
        );



        /*
            Internal destructor
            WARNING!!! Use Layer -> destroy() only
        */
        ~Layer();



        /*
            Creator
        */
        static Layer* create
        (
            Limb*,          /* Limb object */
            string = ""
        );



        /*
            Destructor
        */
        void destroy();




        /*
            Return count of neurons in layer
        */
        int getCount();



        /*
            Set dimentions size
        */
        Layer* setSize
        (
            const Point3i& = POINT_3I_0
        );



        /*
            Set dimentions size from params
        */
        Layer* setSize
        (
            ParamList*
        );



        /*
            Return 3d layer size at neurons for each axis
        */
        Point3i getSize();



        /*
            Return index by point 3i at layer
        */
        int indexByPos
        (
            const Point3i&
        );


        /**********************************************************************
            Data plans work
        */

        /*
            Clear values for all neurons
        */
        Layer* clearValues();



        /*
            Clear errors for all neurons
        */
        Layer* clearErrors();



        /***********************************************************************
            Setters and getters
        */


        Limb* getLimb();



        string getName();



        Layer* setName
        (
            string
        );



        string getId();



        /*
            Set the id of layer
        */
        Layer* setId
        (
            string
        );



        /*
            Return Name of ID if Name not exists
        */
        string getNameOrId();



        /*
            Resize values plan
        */
        Layer* valuesDestroy();



        /*
            Resize errors plan
        */
        Layer* errorsDestroy();



        /*
            Resize values plan
        */
        Layer* valuesCreate();



        /*
            Resize errors plan
        */
        Layer* errorsCreate();



        bool getErrorChange();



        /*
            Calculate sum of neurons error
        */
        double calcSumError();



        /*
            Calculate sum of neurons value
        */
        double calcSumValue();



        /*
            Calculate Root Main Square of neurons value
        */
        double calcRmsValue();



        /*
            Set source path
        */
        Layer* setSourcePath
        (
            string
        );



        /*
            Return the source path
        */
        string getSourcePath();



        /*
            Return layer file name
        */
        string getStorageValueName();



        /*
            return the storage path
        */
        string getStoragePath();



        /*
            Set storage path
        */
        Layer* setStoragePath
        (
            string
        );



        /*
            Return layer path
        */
        string getLayerPath();



        /*
            Set buffer and size of values
        */
        Layer* setValuesFromBuffer
        (
            char*,
            size_t
        );



        /*
            Return buffer and size of values
        */
        Layer* getValuesBuffer
        (
            char*&,  /* Buffer pointer */
            size_t&  /* Size of buffer */
        );



        /*
            Send errors to io
        */
        Layer* writeErrors();



        /*
            Read errors from io
        */
        Layer* readErrors();



        /*
            Return buffer and size of buffer of errors
        */
        Layer* getErrorsBuffer
        (
            char*&, /* Buffer pointer */
            size_t& /* Size of buffer */
        );



        Layer* errorsFromBuffer
        (
            char*,
            size_t
        );




        /*
            Return read role
        */
        bool getRoleRead();



        /*
            Set read role
        */
        Layer* setRoleRead
        (
            bool /* value */
        );




        /************************************************************
            Layer calculateion service
        */



        /**********************************************************************
            Neurons setters and getters
        */



        /*
            Set neuron value
        */
        Layer* setNeuronValue
        (
            int,            /* Index of neuron */
            double          /* Value */
        );



        /*
            Return neuron vaalue or default value
        */
        double getNeuronValue
        (
            int             /* Index of neuron */
        );



        /*
            Set neuron error
        */
        Layer* setNeuronError
        (
            int,            /* Index of neuron */
            double          /* Value */
        );



        /*
            Return neuron error or default value
        */
        double getNeuronError
        (
            int             /* Index of neuron */
        );






//        /*
//            Return event actions
//        */
//        ParamList* getActions();



        /*
            Return the log object
        */
        Log* getLog();



        virtual bool compare
        (
            Layer*
        );



        /*
            Move values data to this from the argument layer
        */
        Layer* copyValuesFrom
        (
            Layer* /* Source layer */
        );



        /*
            Move errors data to this from the argument layer
        */
        Layer* copyErrorsFrom
        (
            Layer* /* Source layer */
        );



        /*
            Return size of values buffer
        */
        size_t getValuesBufferSize();



//        /*
//            Return true if action exists in task for this layer
//        */
//        bool checkTask
//        (
//            Task,   /* Checking task */
//            Action  /* Checking action */
//        );
//


        /*
            Set front function for the layer
        */
        Layer* setFrontFunc
        (
            NeuronFunc*
        );



        NeuronFunc* getFrontFunc();



        /*
            Set back function for the layer
        */
        Layer* setBackFunc
        (
            NeuronFunc*
        );


        NeuronFunc* getBackFunc();



        /*
            Dump to log
        */
        Layer* dumpToLog();



        /*
            Dump to mon
        */
        Layer* dumpToMon
        (
            Mon*,
            Mon*,
            ChartList* achartList
        );



        /*
            Calculate statistics for layer
        */
        Layer* stat();



        /*
            Drop tick count
        */
        Layer* dropTickCount();



        ChartData* getChartTick();
        ChartData* getChartValues();
        ChartData* getChartErrors();



        /*
            Set error calc flag for the layer.
            Layer will be calculating erors.
        */
        Layer* setErrorCalc
        (
            ErrorCalc
        );



        /*
            Return the layer calculation flag for the layer
        */
        ErrorCalc getErrorCalc();



        /*
            Set weight calc flag for the layer.
            Layer will be calculating weights.
        */
        Layer* setWeightCalc
        (
            WeightCalc
        );



        /*
            Return the layer calculation flag for the layer
        */
        WeightCalc getWeightCalc();
};
