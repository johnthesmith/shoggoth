#pragma once


/*
    Neuron net class.
    Contains list of neuron layers.
*/



#include <string>
#include "../log.h"
#include "../scene.h"
#include "layer_list.h"




class Net
{
    private:

        Log* log            = NULL; /* The log object */
        LayerList* layers   = NULL; /* The layer list object */

        Neuron* selected    = NULL; /* Selectrd neuron */
    public:

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
            Draw all enabled layers
        */
        Net* draw
        (
            Scene*,
            bool        /* Recalculate screen neurons position request */
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
            Point3d&
        );



        Net* setSelected
        (
            Neuron*
        );



        Neuron* getSelected();
};
