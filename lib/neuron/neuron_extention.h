#pragma once

#include "../point3.h"
#include <string>

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
    double  waitingValue    = 0.0;          /* Waiting value in tech mode */
};
