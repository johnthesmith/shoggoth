/*
    List of neurons layer for Shoggoth
*/

#pragma once



#include <string>

#include "../lib/heap.h"

#include "layer.h"



using namespace std;



class Layer;



class LayerList : public Heap
{
    public:

        /*
            Create the Layer list object
        */
        static LayerList* create();



        /*
            Destroy the Layer list object
        */
        void destroy();



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
        LayerList* add
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
            Read the layers value and error from io
        */
        LayerList* read();



        /*
            Write the layers value and error to the io
        */
        LayerList* write();

};
