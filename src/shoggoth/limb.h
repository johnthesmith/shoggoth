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



class Limb : public Result
{
    private:
        /* Shoggot application */
        LogManager*     logManager      = nullptr;
        /* Payload id */
        std::string     payloadId       = "";
        /* Current net version */
        string          version         = "";

        /* List of layers in the limb */
        LayerList*      layers      = nullptr;
        /* List of nerves*/
        NerveList*      nerves      = nullptr;

        /*
            wSynchronization states
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
            /* Log manager object */
            LogManager*,
            /* Payload Id */
            std::string,
            /* version */
            string
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
            /* Log Manager object */
            LogManager* aLogManager,
            /* Master payload id */
            std::string aPayloadId,
            /* Version */
            string aVersion = ""
        )
        {
            return new Limb( aLogManager, aPayloadId, aVersion );
        }




        /* Destroy
        */
        void destroy()
        {
            delete this;
        }




        /*
            Return log object
        */
        inline Log* getLog()
        {
            return logManager -> getLog();
        }



        /*
            Create new layer
        */
        Layer* addLayer
        (
            string /* Id of layer */
        );



        /*
            Delete layer by Id
        */
        Limb* deleteLayer
        (
            /* Id of layer */
            string
        );


        /*
            Create new nerve
        */
        Nerve* createNerve
        (
            /* Layer source */
            Layer*,
            /* Layer destination */
            Layer*,
            NerveType,      /* */
            BindType,
            Point3i
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
            Return true if stracture was different and copied
        */
        bool copyTo
        (
            /* Destination */
            Limb*,
            /* Need structure synchronize if structuires not equals */
            bool //,
//            /* Skip synchronization if this was locked */
//            bool,
//            /* Skip synchronization if limb was locked */
//            bool
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
            /* Child layer */
            Layer*          aLayer,
            /* Child neuron index */
            int             aChildIndex,
            /* */
            BindType        aBindType,
            Func            aCallback
        )
        {
             /* Loop by nerves */
             getNerveList() -> loop
             (
                [ &aLayer, &aCallback, &aChildIndex, &aBindType ]
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
                        auto parentsCount = iNerve -> getParent() -> getCount();
                        /* Loop by weights */
                        for
                        (
                            int parentIndex = 0;
                            parentIndex < parentsCount;
                            parentIndex++
                        )
                        {
                            auto weightIndex = iNerve
                            -> getWeightIndex( parentIndex, aChildIndex );
                            if( weightIndex >= 0 )
                            {
                                aCallback
                                (
                                    iNerve -> getParent(),
                                    parentIndex,
                                    iNerve,
                                    iNerve -> getWeight( weightIndex ),
                                    weightIndex
                                );
                            }
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
            /* Parent index */
            int             aParentIndex,
            BindType        aBindType,
            Func            aCallback
        )
        {
            /* Loop by nerves */
            getNerveList() -> loop
            (
                [ &aLayer, &aCallback, &aParentIndex, &aBindType ]
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
                        auto childrenCount = iNerve -> getChild() -> getCount();
                        /* Loop by weights */
                        for
                        (
                            int childIndex = 0;
                            childIndex < childrenCount;
                            childIndex++
                        )
                        {
                            auto weightIndex = iNerve
                            -> getWeightIndex( aParentIndex, childIndex );

                            if( weightIndex >= 0 )
                            {
                                aCallback
                                (
                                    iNerve -> getChild(),
                                    childIndex,
                                    iNerve,
                                    iNerve -> getWeight( weightIndex ),
                                    weightIndex
                                );
                            }
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
        Layer* copyLayerFrom
        (
            /* Source layer */
            Layer*
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



        /*
            Dump value in to file
        */
        Limb* dumpValue
        (
            /* File stream */
            ofstream&,
            /**/
            Data&,
            real,
            Layer*,
            int
        );



        string getDumpFile
        (
            /* File name masque цшер %% */
            string,
            /* Number of tick */
            long long int,
            /* Stage */
            CalcStage,
            /* The layer */
            Layer*,
            /* Neuron Index in the layer */
            Layer*,
            /* Neuron Index in the layer */
            Point3i,
            /* Type parent or child */
            Direction,
            /* Data type */
            Data
        );



        /*
            Dump weights from neuron of the layer
        */
        Limb* dumpLayer
        (
            /* Store path */
            string,
            /* The layer */
            Layer*,
            /* Data type */
            Data,
            /* Data view*/
            Dataview,
            /* Tick */
            int,
            /* Colored */
            bool = true
        );



        /*
            Dump weights from neuron of the layer
        */
        Limb* dumpNeuron
        (
            /* File masque */
            string          aFile,
            /* The layer */
            Layer*          aLayer,
            /* Neuron Index in the layer */
            Point3i         aNeuronPos,
            /* Data type */
            Data            aData
        );



        /*
            Dump nerve information in to the file
        */
        Limb* dumpNerve
        (
            /* Store path */
            string,
            /* The layer */
            Nerve*,
            /* Data type */
            Data,
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



        /*
            Return net version
        */
        Limb* setVersion
        (
            string a
        )
        {
            version = a;
            return this;
        }



        /*
            Return net version
        */
        string getVersion()
        {
            return version;
        }


        /*
            Create new layer in this limb
        */
        inline virtual Layer* createLayer
        (
            string aLayerId
        )
        {
            return Layer::create( this, aLayerId );
        }




        /*
            Dump layers
        */
        virtual Limb* dump();





        /*
            Increamet age of limb
        */
        inline Limb* setLastUpdate
        (
            long long a
        )
        {
            lastUpdate = a;
            return this;
        }



        inline long long getLastUpdate()
        {
            return lastUpdate;
        }



        /*
            Retrun payload id
        */
        inline std::string getPayloadId()
        {
            return payloadId;
        }



        inline LogManager* getLogManager()
        {
            return logManager;
        }
};





