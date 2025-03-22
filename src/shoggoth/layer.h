/*
    Layer

    Contains the NeuronList object.
    The layer creates new neurons or removes them when resized.



    Table of function using for the type of layers

        layers      FrontFuncIn BackFuncIn  BackFuncOut CalcError  CalcWeight Example
        --------------------------------------------------------------------------
        in          NULL        NULL        NULL        NONE       NONE         retina, bias, sample
        cortex      RELU        ONE         NULL        LEARNING   CALC         cortex
        result      RELU        ONE         NULL        LEARNING   CALC         result
        error       LINE        LINE        NULL        VALUE      NONE         error
        command     STEP        NULL        NULL        NONE       NONE         command
        errorTest   LINE        LINE        ZERO        VALUE      NONE         errorTest
*/


#pragma once

#include <string>

#include "../../../../lib/core/result.h"
#include "../../../../lib/core/log.h"
#include "../../../../lib/graph/point3i.h"      /* Size of layer */
#include "../../../../lib/json/param_list.h"    /* For read size from params */

#include "../../../../lib/core/chart_list.h"    /* TODO remove it*/
#include "../../../../lib/core/chart_data.h"
#include "../../../../lib/core/mon.h"

#include "func.h"
#include "shoggoth_consts.h"

using namespace std;


/*
    Predeclaration
*/

class Limb;



class Layer : public Result
{
    private:

        /*
            States
        */
        /* Limb object */
        Limb*           limb                    = NULL;
        /* Count of neurons */
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

        int             threadCount             = 0;

        /*
            Ticker
            Count tick between two tickerDrop calls
            Monotone incremental counter
        */
        long long int   tickCount   = -1;

        /*
            Statistics block
        */
        ChartData*      chartTick               = NULL;
        /* List of layer errors before layer uploaded by other participants */
        ChartData*      chartErrorsBeforeChange = NULL;
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

        /* Forward in function (activation) */
        NeuronFunc*     frontFunc   = &FUNC_NULL;
        /* Backward in function */
        NeuronFunc*     backFunc    = &FUNC_NULL;
        /* Learning function */
        NeuronFunc*     backFuncOut = &FUNC_NULL;



        /*
            Constructor
        */
        Layer
        (
            /* Limb object */
            Limb*,
            /* Layers id */
            string
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
        int getCount()
        {
            return count;
        }



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
        Point3i getSize()
        {
            return size;
        }



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



        /*
            Set the name of layer
        */
        Layer* setName
        (
            string a
        )
        {
           name = a;
           return this;
        }




        string getId()
        {
           return id;
        }




        /*
            Set the id of layer
        */
        Layer* setId
        (
            string a
        )
        {
           id = a;
           return this;
        }




        /*
            Return Name of ID if Name not exists
        */
        string getNameOrId()
        {
           return name == "" ? id : name;
        }



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



        /*
            Return true if neurons error changed in calcError method
        */
        bool getErrorChange()
        {
            return errorChange;
        }




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
            Return buffer and size of buffer of values
        */
        Layer* getValuesBuffer
        (
            char* &aBuffer, /* Buffer pointer */
            size_t &aSize   /* Size of buffer */
        )
        {
            aBuffer = ( char* )values;
            aSize = getValuesBufferSize();
            return this;
        }



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
            char* &aBuffer, /* Buffer pointer */
            size_t &aSize   /* Size of buffer */
        )
        {
            aBuffer = ( char* )errors;
            aSize = getValuesBufferSize();
            return this;
        }



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



        /**********************************************************************
            Neurons setters and getters
        */

        /*
            Set neuron value
        */
        Layer* setNeuronValue
        (
            /* Index of neuron */
            int aIndex,
            /* Value */
            double aValue
        )
        {
            if( values != NULL && aIndex >= 0 && aIndex < count )
            {
                values[ aIndex ] = aValue;
            }
            return this;
        }



        /*
            Return neuron vaalue or default value
        */
        double getNeuronValue
        (
            /* Index of neuron */
            int aIndex
        )
        {
            return
            values != NULL
            && aIndex >= 0
            && aIndex < count
            ? values[ aIndex ]
            : 0.0;
        };



        /*
            Set neuron error
        */
        Layer* setNeuronError
        (
            /* Index of neuron */
            int aIndex,
            /* Value */
            double aError
        )
        {
            if( errors != NULL && aIndex >= 0 && aIndex < count )
            {
                errors[ aIndex ] = aError;
            }
            return this;
        };



        /*
            Return neuron error or default value
        */
        double getNeuronError
        (
            /* Index of neuron */
            int aIndex
        )
        {
            return
            errors != NULL
            && aIndex >= 0
            && aIndex < count
            ? errors[ aIndex ]
            : 0.0;
        };



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
            Dump to log
        */
        Layer* dumpToLog();



        /*
            Dump to strout
        */
        Layer* dump();



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



        /*
            Drop tick count
        */
        Layer* writeErrorsBeforeChange();



        /*
            Return size of values buffer
        */
        size_t getValuesBufferSize()
        {
            return sizeof( double ) * count;
        }



        /*
            Set front in function for the layer
        */
        Layer* setFrontFunc
        (
            NeuronFunc* a
        )
        {
            frontFunc = a;
            return this;
        }



        auto setThreadCount
        (
            int a
        )
        {
            threadCount = a;
            return this;
        }



        int getThreadCount()
        {
            return threadCount;
        }


        NeuronFunc* getFrontFunc()
        {
            return frontFunc;
        }




        /*
            Set back in function for the layer
        */
        Layer* setBackFunc
        (
            NeuronFunc* a
        )
        {
            backFunc = a;
            return this;
        }




        NeuronFunc* getBackFunc()
        {
            return backFunc;
        }




        /*
            Set back out function for the layer
        */
        Layer* setBackFuncOut
        (
            NeuronFunc* a
        )
        {
            backFuncOut = a;
            return this;
        }




        NeuronFunc* getBackFuncOut()
        {
            return backFuncOut;
        }



        /*
            Set error calc flag for the layer.
            Layer will be calculating erors.
        */
        Layer* setErrorCalc
        (
            ErrorCalc a
        )
        {
            errorCalc = a;
            return this;
        }



        /*
            Return the layer calculation flag for the layer
        */
        ErrorCalc getErrorCalc()
        {
            return errorCalc;
        }



        /*
            Set weight calc flag for the layer.
            Layer will be calculating weights.
        */
        Layer* setWeightCalc
        (
            WeightCalc a
        )
        {
            weightCalc = a;
            return this;
        }



        /*
            Return the layer calculation flag for the layer
        */
        WeightCalc getWeightCalc()
        {
            return weightCalc;
        }



        ChartData* getChartTick()
        {
            return chartTick;
        }



        ChartData* getChartValues()
        {
            return chartValues;
        }



        ChartData* getChartErrors()
        {
            return chartErrors;
        }



        ChartData* getChartErrorsBeforeChange()
        {
            return chartErrorsBeforeChange;
        }

};






//
//TODO Добавить мутации создания удаления слоев
//
//Каждому слою можно прописать для какого слоя он может быть парентом и чайлдом примутации.
//Мутации могт касаться кортекса и биаса логичеси.
//Так же мутации для каждого слоя должны описывать правиал соединения.
