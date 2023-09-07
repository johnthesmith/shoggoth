#pragma once

#include <string>
#include <vector>
#include <functional>   /* for lyambda */

#include "../lib/heap.h"

#include "../graph/point3.h"
#include "../graph/point3i.h"
#include "../graph/object.h"

#include "storage_neuron.h"

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
        function <bool ( Neuron* )>,
        int = 0,
        int = 0
    );


    /*
        Storage utils
    */


    /*
        Return storage size for neuron list
    */
    int getStorageSize();



    /*
        Return true if storage file exists and has the actual size.
    */

    bool checkStorage
    (
        string /* Path */
    );



    NeuronList* createStorage
    (
        string /* Path */
    );



    /*
        Save neurons to memory buffer
        Buffer must be delete after using with deleteBuffer
    */
    NeuronList* saveToBuffer
    (
        int,
        int,
        char*&,
        size_t&
    );



    NeuronList* loadFromBuffer
    (
        const char*,
        const size_t
    );



    NeuronList* saveToStorage
    (
        string /* Path */
    );



    NeuronList* saveToStorage
    (
        string, /* Path */
        char*,
        size_t
    );



    NeuronList* loadFromStorage
    (
        string /* Path */
    );
};
