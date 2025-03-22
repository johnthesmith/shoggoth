# pragma once

#include "../../../../lib/core/log_manager.h"

#include "layer.h"
#include "nerve.h"
#include "layer_list.h"



class LayerList;
class Layer;



class NerveList :  public Heap
{
    private:

        LogManager* logManager = NULL;
        string      id  = "";

    public:


        NerveList
        (
            LogManager*
        );



        /*
            Create new nerve list
        */
        static NerveList* create
        (
            LogManager*
        );



        /*
            Self destroy nerve list
        */
        void destroy();



        /*
            Return log object
        */
        Log* getLog();



        /*
            Add Nerves from argument list to this list
        */
        NerveList* add
        (
            NerveList*
        );




        /*
            Add one Nerve
        */
        NerveList* add
        (
            Nerve*
        );



        /*
            Return index by Nerve
        */
        int indexBy
        (
            Nerve*
        );



        /*
            Return Nerve by index
        */
        Nerve* getByIndex
        (
            int
        );



        /*
            Return Nerve by index
        */
        NerveList* setByIndex
        (
            int,
            Nerve*
        );



        /*
            Resize
        */
        NerveList* resize
        (
            int
        );



        /*
            Create new Nerve
        */
        NerveList* newNerve();



        /*
            Return Nerve by argumets or NILL
        */
        Nerve* find
        (
            string,  /* Parent layer */
            string,  /* Child layer */
            BindType /* Type of bind between nerves */
        );



        /*
            Remove by layer
        */
        NerveList* removeByLayer
        (
            Layer*
        );



        /*
            Return true for layer parents exists
        */
        bool parentsExists
        (
            /* Layer object */
            Layer*
        );



        /*
            Return true for layer children exists
        */
        bool childrenExists
        (
            /* Layer object */
            Layer*
        );



        /*
            Return parents layers by child layer from nerves
        */
        NerveList* getParentsByLayer
        (
            Layer*,     /* Layer for parents search */
            LayerList*  /* Result list of parents */
        );



        NerveList* getChildrenByLayer
        (
            Layer*,     /* Layer for parents search */
            LayerList*  /* Result list of parents */
        );



        NerveList* selectByLayers
        (
            Layer*, /* parent layer */
            Layer*, /* child layer */
            NerveList* /* Founded Nerves */
        );



        /*
            Clear all destroy all nerves
        */
        NerveList* clear();



        /*
            Compare layer structure
        */
        bool compare
        (
            NerveList*
        );



        /*
            Return nerve by nerve
        */
        Nerve* getByNerve
        (
            Nerve*
        );



        /*
            Copy list of nerves in to this from argument
        */
        NerveList* copyStructureFrom
        (
            NerveList*,  /* Source */
            LayerList*
        );



        /*
            Dump information to log
        */
        NerveList* dump
        (
            string aComment = ""
        );



        /*
            Allocate nerves weights
        */
        template <typename Func>
        NerveList* weightsAllocate
        (
            Func aOnAllocate
        )
        {
            loop
            (
                [ &aOnAllocate ]( void* p )
                {
                    (( Nerve*) p ) -> allocate( aOnAllocate );
                    return false;
                }
            );
            return this;
        }

};

