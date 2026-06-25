# pragma once

/*
    Limb - part of Shoggoth
    Using for processor, server, teacher, ui and other.
    Contain private layers for thread protected operations.
*/


#include <atomic>
#include <iostream>
#include <cstring>
/* For net sinchronization */
#include <mutex>
/* For layers precalc lists */
#include <unordered_set>

#include "../../../../lib/core/log_manager.h"
#include "../../../../lib/core/rnd.h"
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
        LayerList*      layers          = nullptr;
        /* List of nerves*/
        NerveList*      nerves          = nullptr;
        /* Rnd object */
        Rnd*            rnd             = NULL;
        /* Using layers */
        std::unordered_set<std::string> allLayers;
        std::unordered_set<std::string> readValues;
        std::unordered_set<std::string> writeValues;
        std::unordered_set<std::string> readErrors;
        std::unordered_set<std::string> writeErrors;

        /*
            wSynchronization states
        */
        /* Moment of reconfuguration */
        long long       lastUpdate = 0;
        /* Moment last chenged (insert delete layer and nerve)*/
        long long       lastChangeStructure = 0;
        /* Moment last values changed */
        long long       lastChangeValues = 0;
        /* Learning tick */
        std::atomic <unsigned long long int> learningTick = 0;
        /* Hash map */
        std::map<std::string, uint64_t> hashValues = {};

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



        /*
            Destroy
        */
        void destroy()
        {
            delete this;
        }



        /*
            Apply config
        */
        Limb* applyConfig
        (
            /* Application config  */
            ParamList*  aAppConfig,
            /* Net config */
            ParamList*  aNetConfig
        );



        /*
            Load nerves from config
        */
        Limb* loadLayers
        (
            ParamList*
        );



        /*
            Load layer structure from param list
            Layer may be resized.
        */
        Limb* loadLayer
        (
            /* Layer object */
            Layer*,
            /* Layer configuration */
            ParamList*
        );



        /*
            Load nerves from config
        */
        Limb* loadNerves
        (
            ParamList*
        );



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
            Limb*
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
            lock();
            version = a;
            unlock();
            return this;
        }



        /*
            Return net version
        */
        string getVersion()
        {
            lock();
            auto v = version;
            unlock();
            return v;
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



        /*
            Return the tick of the net
        */
        unsigned long long int getLearningTick()
        {
            return learningTick.load( std::memory_order_relaxed );
        }



        /*
            Set the tick for the net
        */
        Limb* setLearningTick
        (
            /* Tick number */
            unsigned long long int a
        )
        {
            learningTick.store( a, std::memory_order_relaxed );
            return this;
        }



        /*
            Tick increment
        */
        Limb* incLearningTick()
        {
            learningTick.fetch_add( 1, std::memory_order_relaxed );
            return this;
        }



        /*
            Load selected weights to this limb from the limb argument
        */
        Limb* weightsFrom
        (
            /* Sorce limb */
            Limb*
        );



        Rnd* getRnd()
        {
            return rnd;
        }



        Limb* setRndSeed
        (
            long long int a
        )
        {
            rnd -> setSeed( a );
            return this;
        }



        /*
            Remove layers absent in the list
        */
        Limb* purgeLayers
        (
            ParamList*
        );



        Limb* collectLayersUsing
        (
            ParamList*
        );



        /*
            Recalculate layer hash and store it
        */
        Limb* calcLayerValuesHash
        (
            Layer* aLayer
        )
        {
            hashValues[ aLayer -> getId() ] = aLayer -> calcValuesHash();
            return this;
        }


        /*
            Return layer hash by layer id
        */
        Limb* setValuesHashByLayerId
        (
            /* Layer Id */
            std::string aLayerId,
            /* Layer values hash */
            uint64_t aHash
        )
        {
            hashValues[ aLayerId ] = aHash;
            return this;
        }



        /*
            Return layer hash by layer id
        */
        inline uint64_t getValuesHashByLayerId
        (
            /* Layer id */
            std::string a
        )
        {
            return hashValues.count( a ) ? hashValues[a] : 0;
        }



        /*
            Return list of layers
        */
        const std::unordered_set<std::string>& getAllLayers() 
        const
        {
            return allLayers;
        }



        /*
            Return list of read values layers
        */
        const std::unordered_set<std::string>& getReadValues() 
        const
        {
            return readValues;
        }


        /*
            Return list of write values layers
        */
        const std::unordered_set<std::string>& getWriteValues() 
        const
        {
            return writeValues;
        }


        /*
            Return list of read errors layers
        */
        const std::unordered_set<std::string>& getReadErrors() 
        const
        {
            return readErrors;
        }


        /*
            Return list of write errors layers
        */
        const std::unordered_set<std::string>& getWriteErrors() 
        const
        {
            return writeErrors;
        }


        /*
            Load values and errors from argument limb
        */
        valuesFrom
        valuesTo
        errorsFrom
        errorsTo

        // Надо загрущить в текущий лимб занчения и ошибик из другого лимба
        // Используем списки слоев которые должны быть загружены из текущего лимба
        //



};





