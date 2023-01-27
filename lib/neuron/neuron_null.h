#pragma once

#include <string>
#include <vector>

#include "../point3.h"
#include "../point3i.h"

#include "neuron.h"

using namespace std;



struct NeuronNull: public Neuron
{
    bool isNull();
};



static NeuronNull NEURON_NULL = NeuronNull();
