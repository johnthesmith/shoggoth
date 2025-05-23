/*
    List of neurons layer for Shoggoth
*/

#pragma once



#include <string>

#include "../../../../lib/core/heap.h"

#include "layer.h"
#include "shoggoth_consts.h"


using namespace std;



class Layer;
class Limb;



class LayerList : public Heap
{
    private:

        Limb* limb;   /* Limb object */

    public:


        /*
            Constructor
        */
        LayerList
        (
            Limb*
        );



        /*
            Create the Layer list object
        */
        static LayerList* create
        (
            Limb*
        );



        /*
            Destroy the Layer list object
        */
        void destroy();



        /*
            Return log
        */
        Log* getLog();



        /*
            Add Layers from argument list to this list
        */
        LayerList* add
        (
            LayerList*
        );




        /*
            Add one layer
        */
        LayerList* push
        (
            Layer*
        );



        /*
            Return index by Layer
        */
        int indexBy
        (
            Layer*
        );



        /*
            Return Layer by index
        */
        Layer* getByIndex
        (
            int
        );



        /*
            Return Layer by index
        */
        LayerList* setByIndex
        (
            int,
            Layer*
        );



        /*
            Resize
        */
        LayerList* resize
        (
            int
        );



        /*
            Create new layer
        */
        LayerList* newLayer();




        /*
            Return layer by name
        */
        LayerList* byName
        (
            string
        );



        /*
            Return layer index by id
        */
        int getIndexById
        (
            string
        );



        /*
            Return layer by id
        */
        Layer* getById
        (
            string
        );



        /*
            Clear all destroy all layers
        */
        LayerList* clear();



        /*
            Compare layer structure
        */
        bool compare
        (
            LayerList*
        );



        /*
            Copy values of equal layers
        */
        LayerList* copyValuesFrom
        (
            LayerList*  /* Source */
        );



        /*
            Copy errors of equal layers
        */
        LayerList* copyErrorsFrom
        (
            LayerList* /* Source */
        );



        /*
            Dump nerveList information to log
        */
        LayerList* dump
        (
            /* Comment */
            string = ""
        );



        /*
            Dump limb information to STD_OUT
        */
        LayerList* dumpCout
        (
            /* Comment */
            string = ""
        );


};
