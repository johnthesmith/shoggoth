# pragma once

/*
    Limb - part of Shoggoth
    Using for processor, server, teacher, ui and other.
    Contain private layers for thread protected operations.
*/



#include <iostream>
#include <cstring>
#include <functional> /* for function */
#include <mutex>    /* For net sinchronization */

#include "../../../../lib/core/log.h"
#include "neuron.h"
#include "nerve_list.h"
#include "layer_list.h"



/*
    Lambda function for nerve weights loop
*/
typedef function
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



class Limb : public Result
{
    private:
        Log*        log         = NULL;

        LayerList*  layers      = NULL;
        NerveList*  nerves      = NULL;     /* List of nerves*/

        /*
            Synchronization states
        */
        /* Net synchronization mutex */
        mutex       sync;

    public:

        /*
            Constructor
        */
        Limb
        (
            Log* /* Log object*/
        );



        /*
            Destructor
        */
        ~Limb();



        /*
            Create
        */
        static Limb* create
        (
            Log* /* The log object*/
        );



        /*
            Destroy
        */
        void destroy();



        /*
            Return log object
        */
        Log* getLog();



        /*
            Create new layer
        */
        Layer* createLayer
        (
            string /* Id of layer */
        );



        /*
            Delete layer by Id
        */
        Limb* deleteLayer
        (
            string  /* Id of layer */
        );


        /*
            Create new nerve
        */
        Nerve* createNerve
        (
            Layer*,         /* Layer source */
            Layer*,         /* Layer destination */
            NerveType,
            BindType
        );



        /*
            Delete layer by Id
        */
        Limb* deleteNerve
        (
            Nerve* /* Nerve opbject */
        );



        /**********************************************************************
            Layers swap operation
        */



        /*
            Copy limb property
        */
        Limb* copyTo
        (
            Limb*,          /* Destination */
            bool = false    /* Need structure synchronize if structuires not equals */
        );



        /**********************************************************************
            Setters and getters
        */



        /*
            Return LayerList
        */
        LayerList* getLayerList();



        /*
            Return list of nerves
        */
        NerveList* getNerveList();



        bool nerveWeightLoop
        (
            NeuronList*,
            IndexWeightLambda
        );



        /**********************************************************************
            Net synchronization
        */



        /*
            Lock net for operations with layers
        */
        Limb* lock();



        /*
            Unlock net after lock method
        */
        Limb* unlock();



        /*
            Loop for each parents neuron of this neuron
        */
        Limb* parentsLoop
        (
            Layer*          aLayer,     /* Layer */
            int             aIndex,     /* Neuron index */
            parentsLambda   aCallback   /* Callback method */
        );



        /*
            Loop for each child of i neuron for Layer
        */
        Limb* childrenLoop
        (
            Layer*          aLayer,     /* Layer */
            int             aIndex,     /* Neuron index */
            childrenLambda  aCallback   /* Callback method */
        );
};

