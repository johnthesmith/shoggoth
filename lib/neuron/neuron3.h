#pragma once

#include "neuron.h"



/*
    Neuron in 3d
*/
struct Neuron3 : public Neuron
{
    Point3d point;

    /*
        Draw neuron
    */
    virtual Neuron3& draw();
};
