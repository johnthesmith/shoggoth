# pragma once

/*
    Limb - part of Shoggoth
    Using for processor, server, teacher, ui and other.
    Contain private layers for thread protected operations.
*/



#include <iostream>
#include <cstring>
#include <mutex>    /* For net sinchronization */

#include "../../../../lib/core/log_manager.h"
#include "layer_list.h"
#include "nerve_list.h"


///*
//    Lambda function for return parents neurons of child
//*/
//typedef
//function
//<
//    bool
//    (
//        Layer*, /* Layer with parent neurons */
//        int,    /* Neuron index*/
//        Nerve*, /* Nerve */
//        double, /* Weight of bind */
//        int     /* return weight index */
//    )
//>
//parentsLambda;
//
//
//
///*
//    Lambda function for return children neurons for parent
//*/
//typedef
//function
//<
//    bool
//    (
//        Layer*, /* return layer with children neurons */
//        int,    /* return child neuron index*/
//        Nerve*, /* return nerve */
//        double, /* return weight of bind */
//        int     /* return weight index */
//    )
//>
//childrenLambda;




class Limb : public Result
{
    private:

        LogManager*     logManager  = NULL;

        LayerList*      layers      = NULL;
        NerveList*      nerves      = NULL;     /* List of nerves*/

        /*
            Synchronization states
        */
        /* Moment of reconfuguration */
        long long       lastUpdate = 0;
        /* Moment last chenged (insert delete layer and nerve)*/
        long long       lastChangeStructure = 0;
        /* Moment last values changed */
        long long       lastChangeValues = 0;

    public:

        /*
            Constructor
        */
        Limb
        (
            LogManager*
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
            bool,   /* Need structure synchronize if structuires not equals */
            bool,   /* Skip synchronization if this was locked */
            bool    /* Skip synchronization if limb was locked */
        );



        /**********************************************************************
            Setters and getters
        */



        /*
            Return LayerList
        */
        LayerList* getLayerList()
        {
            return layers;
        }



        /*
            Return list of nerves
        */
        NerveList* getNerveList()
        {
            return nerves;
        }




        /*
            Loop for each parents neuron of this neuron
        */
        template <typename Func> Limb* parentsLoop
        (
            Layer*          aLayer,     /* Layer */
            int             aIndex,     /* Neuron index */
            BindType        aBindType,
            Func            aCallback
        )
        {
            /* Loop by nerves */
            getNerveList() -> loop
            (
                [ &aLayer, &aCallback, &aIndex, &aBindType ]
                ( void* aNerve )
                {
                    auto iNerve = ( Nerve* ) aNerve;

                    if
                    (
                        iNerve -> getChild() == aLayer &&
                        (
                            aBindType == BT_ALL ||
                            iNerve -> getBindType() == aBindType
                        )
                    )
                    {
                        int from = 0;
                        int to = 0;
                        iNerve -> getWeightsRangeByChildIndex
                        (
                            aIndex, from, to
                        );
                        /* Loop by weights */
                        for( int i = from; i < to;  i++ )
                        {
                            aCallback
                            (
                                iNerve -> getParent(),
                                iNerve -> getParentByWeightIndex( i ),
                                iNerve,
                                iNerve -> getWeight( i ),
                                i
                            );
                        }
                    }
                    return false;
                }
            );
            return this;
        };



        /*
            Loop for each child of i neuron for Layer
        */
        template <typename Func> Limb* childrenLoop
        (
            Layer*          aLayer,
            int             aIndex,
            BindType        aBindType,
            Func            aCallback
        )
        {
            /* Loop by nerves */
            getNerveList() -> loop
            (
                [ &aLayer, &aCallback, &aIndex, &aBindType ]
                ( void* aNerve )
                {
                    auto iNerve = ( Nerve* ) aNerve;
                    if
                    (
                        iNerve -> getParent() == aLayer &&
                        (
                            aBindType == BT_ALL ||
                            iNerve -> getBindType() == aBindType
                        )
                    )
                    {
                        int from = 0;
                        int to = 0;
                        int step = 0;

                        iNerve -> getWeightsRangeByParentIndex
                        (
                            aIndex, from, to, step
                        );

                        /* Loop by weights */
                        for( int i = from; i < to;  i += step )
                        {
                            aCallback
                            (
                                iNerve -> getChild(),
                                iNerve -> getChildByWeightIndex( i ),
                                iNerve,
                                iNerve -> getWeight( i ),
                                i
                            );
                        }
                    }
                    return false;
                }
            );
            return this;
        };


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



        /*
            Configuration postprocessing
        */
        virtual void onAfterReconfig
        (
            ParamList*
        );



        /*
            Check layer parents existing by type
        */
        bool layerParentsExists
        (
            Layer*, /* layer */
            BindType /* Type of bind */
        );



        Limb* dumpValue
        (
            ofstream&,
            Data&,
            double,
            Layer*,
            int
        );



        /*
            Dump weights from neuron of the layer
        */
        Limb* dump
        (
            /* Store path */
            string,
            /* The layer */
            Layer*,
            /* Neuron Index in the layer */
            Point3i,
            /* Type parent or child */
            Direction,
            /* Data type */
            Data,
            /* Data view*/
            Dataview,
            /* Tick number */
            long long int
        );



        /*
            Dump weights from neuron of the layer
        */
        Limb* dump
        (
            /* Store path */
            string,
            /* Data type */
            CalcStage,
            /* The layer */
            Layer*,
            /* Data type */
            Data,
            /* Data view*/
            Dataview,
            /* Tick number */
            long long int,
            /* Colored */
            bool = true
        );



        /*
            On cahnge event for limb
        */
        Limb* onChangeValues();



        /*
            Return last moment of change structure of the limb
        */
        long long int getLastChangeStructure()
        {
            return lastChangeStructure;
        }



        /*
            Return last moment change values
        */
        long long getLastChangeValues()
        {
            return lastChangeValues;
        }
};
