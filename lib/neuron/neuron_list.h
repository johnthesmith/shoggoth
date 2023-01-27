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
    Neuron& neuronByIndex
    (
        int
    );
};
