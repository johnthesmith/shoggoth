#pragma once

#include <string>
#include <functional>   /* for lyambda */

#include "../../../../../lib/core/heap.h"
#include "../layer.h"



using namespace std;



class NeuronUi;



class NeuronListUi : public Heap
{
    public:
        /*
            Create and return list of neurons
        */
        static NeuronListUi* create();



        /*
            Add neurons from argument list to this list
        */
        NeuronListUi* add
        (
            NeuronListUi*
        );



        /*
            Return index by neuron
        */
        int indexBy
        (
            NeuronUi*
        );



        /*
            Return index by layer and index at him
        */
        int indexBy
        (
            Layer*,
            int
        );



        /*
            Return neuron by index
        */
        NeuronUi* getByIndex
        (
            int
        );



        /*
            Return neuron by index
        */
        NeuronListUi* setByIndex
        (
            int,
            NeuronUi*
        );



        /*
            Resize
            Warning!!! this method can not call directly.
        */
        NeuronListUi* resize
        (
            int
        );



        /*
            Loop with lyambda
        */
        NeuronListUi* loop
        (
            function <bool ( NeuronUi* )>,
            int = 0,
            int = 0
        );



        /*
            Calculate average value
        */
        double calcAvgValue();



        /*
            Calculate average error
        */
        double calcAvgError();



        NeuronListUi* merge
        (
            NeuronListUi*
        );



        NeuronListUi* remove
        (
            NeuronListUi*
        );
};
