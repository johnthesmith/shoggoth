#pragma once

#include <string>
#include <vector>

#include "../point3.h"
#include "../point3i.h"

using namespace std;


class Neuron;



struct Layer
{
    Point3i dim = Point3i( 0, 0, 0 );   /* Dimention size */



    vector<Neuron> children;    /* List of children */

    /*
        Add exists neuron
    */
    Layer& add
    (
        Neuron&,        /* Parent neuron */
        const Point3i&  /* Point */
    );



    /*
        Create new neuron and return it
    */
    Neuron& newNeuron();



    int indexByPos
    (
        const Point3i*
    );



    Neuron& neuronByPos
    (
        const Point3i&
    );



    Neuron& neuronByName
    (
        const string
    );



    int indexByNeuron
    (
        Neuron&
    );



    int neuronByIndex
    (
        Neuron&
    );


    Neuron& setDim
    (
        const Point3i&
    );



    /*
        Draw neurons
    */
    virtual Layer& draw();
};
