/*
    List of neurons layer
*/
#pragma once


#include <string>

#include "layer.h"
#include "../heap.h"



using namespace std;



class Layer;



struct LayerList : public Heap
{
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
        Delete layer by index
    */
    LayerList* deleteLayer
    (
        int /* Index of the layer */
    );



    /*
        Return layer by name
    */
    LayerList* byName
    (
        string
    );



    /*
        Return layer by id
    */
    LayerList* byId
    (
        string
    );
};
