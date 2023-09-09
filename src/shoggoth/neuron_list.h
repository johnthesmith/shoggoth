#pragma once

#include <string>
#include <functional>   /* for lyambda */

#include "../lib/heap.h"



using namespace std;



class Neuron;



class NeuronList : public Heap
{
    public:
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
};
