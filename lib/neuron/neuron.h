#pragma once

#include <string>
#include <vector>

#include "../point3.h"
#include "../point3i.h"

#include "layer.h"

using namespace std;


class Neuron;



/*
    Neuron structure
*/
struct Neuron
{
    Layer layer;

    vector<Neuron> children;    /* List of children */
    vector<double> weights;     /* Binds weights for children */
    vector<Neuron> parents;     /* List of parents */

    double value;



    /*
        this -> parent
    */
    Neuron& addParent
    (
        Neuron&,    /* Parent neuron */
        double = 0  /* Weight */
    );



    /*
        child -> this
    */
    Neuron& addChild
    (
        Neuron&,    /* Child neuron */
        double = 0  /* Weight */
    );



    /*
        Return Name of neuron
    */
    virtual string getName();

    /*
        Draw neuron
    */
    virtual Neuron& draw();
};



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
