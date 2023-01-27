#pragma once

#include "neuron.h"



/*
    Neuron in 3d
*/
struct Neuron3 : public Neuron
{
    Point3 point;

    /*
        Draw neuron
    */
    virtual Neuron3& draw();
};
