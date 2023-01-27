#pragma once

#include <string>
#include <vector>

#include "../point3.h"
#include "../point3i.h"

#include "layer.h"
#include "neuron_list.h"

using namespace std;



/*
    Neuron structure
*/
struct Neuron
{
    Layer* layer = NULL;

    NeuronList children;    /* List of children */
    vector<double> weights;     /* Binds weights for children */
    NeuronList parents;     /* List of parents */


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
        Return null neuron
    */
    virtual bool isNull();



    /*
        Return Name of neuron
    */
    virtual string getName();



    /*
        Draw neuron
    */
    virtual Neuron& draw();
};

