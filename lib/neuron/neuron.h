/*
    Neurons can not be created directly. You must use Layer.setSize();
*/

#pragma once

#include <string>
#include <vector>

#include "../point3.h"
#include "../point3i.h"

#include "neuron_list.h"
#include "bind_list.h"
#include "bind.h"



using namespace std;


class Layer;


/*
    Neuron structure
*/
struct Neuron
{
    Layer* layer   = NULL;  /* Neuron layer */
    int layerIndex = 0;

    BindList* parentBinds;        /* Binds for parents */
    BindList* childrenBinds;      /* Binds for children */

    double value;



    Neuron
    (
        Layer*,      /* Layer object */
        int         /* Index in layer */
    );



    ~Neuron();



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



    /**************************************************************************
        Setters and getters
    */


    /*
        Return neuron layer
    */
    Layer& getLayer();



    /*
        Return neuron layer index
    */
    int getLayerIndex();


    Neuron& setValue
    (
        double
    );



    double getValue();



    Point3d& getWorldPoint();
};

