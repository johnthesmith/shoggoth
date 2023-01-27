#pragma once

#include "neuron3.h"

/*
    Named neuron
*/
struct NeuronNamed : public Neuron3
{
    string name;

    /*
        Return Name of neuron
    */
    virtual string getName();
};
