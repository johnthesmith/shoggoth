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

#include "../../../../lib/core/log_manager.h"
#include "layer_list.h"



/*
    Lambda function for return parents neurons of child
*/
typedef
function
<
    bool
    (
        Layer*, /* Layer with parent neurons */
        int,    /* Neuron index*/
        Nerve*, /* Nerve */
        double, /* Weight of bind */
        int     /* return weight index */
    )
>
parentsLambda;



/*
    Lambda function for return children neurons for parent
*/
typedef
function
<
    bool
    (
        Layer*, /* return layer with children neurons */
        int,    /* return child neuron index*/
        Nerve*, /* return nerve */
        double, /* return weight of bind */
        int     /* return weight index */
    )
>
childrenLambda;




class Limb : public Result
{
    private:
        LogManager*     logManager  = NULL;

        LayerList*      layers      = NULL;
        NerveList*      nerves      = NULL;     /* List of nerves*/

        /*
            Synchronization states
        */
        /* Net synchronization mutex */
        recursive_mutex sync;
        /* Moment of reconfuguration */
        long long       lastUpdate = 0;
        /* Moment last chenged (insert delete layer and nerve)*/
        long long       lastChange = 0;
    public:

        /*
            Constructor
        */
        Limb
        (
            LogManager* /* Log object*/
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
            LogManager* /* The log object*/
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
            Return log manager object
        */
        LogManager* getLogManager();



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
            Limb*,  /* Destination */
            bool    /* Need structure synchronize if structuires not equals */
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


        /**********************************************************************
            Current age of the limbs config
        */

        /*
            Increamet age of limb
        */
        Limb* setLastUpdate
        (
            long long
        );


        long long getLastUpdate();



        /*
            Copy list of layers
        */
        Limb* copyStructureFrom
        (
            LayerList*
        );



        /*
            Create new layer for this limb and copy parameters from source layer.
            This method have to overriden at children Limbs.
        */
        virtual Layer* copyLayerFrom
        (
            Layer* /* Source layer */
        );



        long long getLastChange();


        /*
            Configuration postprocessing
        */
        virtual void onAfterReconfig
        (
            ParamList*
        );
};

