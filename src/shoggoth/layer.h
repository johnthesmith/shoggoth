/*
    Layer

    Contains the NeuronList object.
    The layer creates new neurons or removes them when resized.
*/


#pragma once

#include <string>

#include "../../../../lib/core/result.h"
#include "../../../../lib/graph/point3i.h"      /* Size of layer */

#include "nerve_list.h"


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

        int             forward                 = -1;
        int             backward                = -1;

        ParamList*      actions                 = NULL;         /* List of actions for layer */

        /*
            Plans of neurons data
        */

        double*         values                  = NULL;
        double*         errors                  = NULL;

    public:

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
            Set neurons count and reallocate plans
        */
        virtual Layer* setCount
        (
            const int = 0 /* New count */
        );



        /*
            Return count of neurons in layer
        */
        int getCount();




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



        /*
            Reset calculate states
        */
        Layer* calcReset();



        Layer* calcStartForward();



        Layer* calcStartBackward();



        Layer* calcCompleteForward();



        Layer* calcCompleteBackward();



        CalcStage getForwardStage
        (
            int
        );



        CalcStage getBackwardStage
        (
            int
        );



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






        /*
            Return event actions
        */
        ParamList* getActions();



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



        /*
            Return true if layer contains one of tasks
        */
        bool checkTasks
        (
            ParamList*,     /* Checking tasks */
            Action aAction  /* Checking action */
        );



};
