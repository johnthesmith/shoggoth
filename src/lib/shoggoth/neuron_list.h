#pragma once

#include <string>
#include <vector>
#include <functional>   /* for lyambda */

#include "../heap.h"

#include "../graph/point3.h"
#include "../graph/point3i.h"
#include "../graph/object.h"

using namespace std;



class Neuron;



struct NeuronList : public Heap
{
    /*
        Create and return list of neurons
    */
    static NeuronList* create();



    /*
        Add neurons from argument list to this list
    */
    NeuronList* add
    (
        NeuronList*
    );



    /*
        Return index by neuron
    */
    int indexBy
    (
        Neuron*
    );



    /*
        Return neuron by index
    */
    Neuron* getByIndex
    (
        int
    );



    /*
        Return neuron by index
    */
    NeuronList* setByIndex
    (
        int,
        Neuron*
    );



    /*
        Resize
        Warning!!! this method can not call directly.
    */
    NeuronList* resize
    (
        int
    );



    /*
        Loop with lyambda
    */
    NeuronList* loop
    (
        function <bool ( Neuron* )>
    );
};
