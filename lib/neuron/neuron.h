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
