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
    Layer* layer   = NULL;        /* Neuron layer */

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
        Add all neurons from NeuronList to Children of this neuron
    */
    Neuron& addChildren
    (
        NeuronList*,    /* Child neuron list */
        double = 0      /* Weight */
    );




    /*
        Add all neurons from NeuronList to Partnts of this neuron
    */
    Neuron& addParents
    (
        NeuronList&    /* Parents neuron list */
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

