#pragma once


/*
    Neuron net class.
    Contains list of neuron layers.
*/


#include <string>

#include "../lib/log.h"
#include "../json/json.h"
#include "../graph/scene.h"

#include "layer_list.h"




class Net
{
    private:

        Log* log                = NULL; /* The log object */

        Neuron* selected        = NULL; /* Selectrd neuron */
        double  sensivity       = 10;   /* Sensivity of neuronet [ 0; +oo], Set to sensivity of each layer */

        bool    loopParity      = true;
        bool    learningMode    = false;

    public:
        LayerList* layers   = NULL; /* The layer list object */

        /*
            Constructor
        */
        Net
        (
            Log*    /* Log object */
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
            Log*
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
            Return layer by Id
        */
        Layer* getLayerById
        (
            string /* Id of layer */
        );



        /*
            Calculate all enabled layers
        */
        Net* calc
        (
            Scene*
        );



        /*
            Return log object
        */
        Log* getLog();



        /*
            Return list of neurons in screen rect
        */
        Net* getNeuronsByScreenRect
        (
            NeuronList*,/* Neuron list */
            Point3d&,   /* Top left point */
            Point3d&    /* Bottom right point */
        );


        /*
            Return neuron by screen position
        */
        Net* getNeuronsByScreenPos
        (
            NeuronList*,/* Neuron list */
            const Point3d&
        );



        Net* setSelected
        (
            Neuron*
        );



        /*
            Select neurn by MOUSE position
        */
        Net* setSelected
        (
            Scene&
        );



        Neuron* getSelected();




        Net* switchShowBinds();
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
            Apply config from Json
        */
        Net* applyConfig
        (
            Json*   /* Config structure */
        );
};
