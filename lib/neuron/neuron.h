/*
    Neurons can not be created directly. You must use Layer.setSize();
*/

#pragma once

#include <string>
#include <vector>

#include "../point3.h"
#include "../point3i.h"



#include "neuron_list.h"



using namespace std;


class Layer;


/*
    Neuron structure
*/
struct Neuron
{
    Layer* layer = NULL;        /* Neuron layer */

    NeuronList children;        /* List of children */
    vector<double> weights;     /* Binds weights for children */
    NeuronList parents;         /* List of parents */


    double value;



    Neuron& setLayer
    (
        Layer*
    );



    /*
        Return neuron layer
    */
    Layer& getLayer();




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

