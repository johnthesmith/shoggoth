#pragma once

#include <string>

#include "../graph/point3.h"



/*
    Neuron extention
    This exetention will be define when any of values will be set for neuron
*/

struct NeuronExtention
{
    Point3d world           = POINT_3D_0;
    Point3d screen          = POINT_3D_0;
    bool    selected        = false;        /* Neuron is selected */
    string  id              = "";           /* Neuron identifier */
};
