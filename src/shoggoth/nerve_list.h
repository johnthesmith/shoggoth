# pragma once

#include "../lib/log.h"

#include "layer.h"
#include "nerve.h"
#include "layer_list.h"



class LayerList;
class Layer;



class NerveList :  public Heap
{
    private:

        string id = "";

    public:



        /*
            Create new nerve list
        */
        static NerveList* create();



        /*
            Self destroy nerve list
        */
        void destroy();



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
            Return Nerve by id
        */
        Nerve* getById
        (
            string
        );



        /*
            Return Nerve index by id
        */
        int getIndexById
        (
            string
        );



        /*
            Remove by layer
        */
        NerveList* removeByLayer
        (
            Layer*
        );



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



        NerveList* readWeights();



        NerveList* writeWeights();



        NerveList* selectByLayers
        (
            Layer*, /* parent layer */
            Layer*, /* child layer */
            NerveList* /* Founded Nerves */
        );

};

