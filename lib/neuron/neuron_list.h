#pragma once

#include <string>
#include <vector>

#include "../point3.h"
#include "../point3i.h"
#include "../object.h"

using namespace std;



class Neuron;



struct NeuronList
{
    vector<Neuron*> items;



    /*
        Return Neurons list count elements
    */
    int getSize();



    /*
        Add exists neuron
    */
    NeuronList& add
    (
        Neuron&        /* Parent neuron */
    );



    /*
        Return neuron by name
    */
    Neuron& neuronByName
    (
        const string
    );



    /*
        Return index by neuron
    */
    int indexByNeuron
    (
        Neuron&
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
    NeuronList& setByIndex
    (
        int,
        Neuron*
    );



    /*
        Resize
        Warning!!! this method can not call directly.
    */
    NeuronList& resize
    (
        int
    );
};
