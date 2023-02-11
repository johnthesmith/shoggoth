/*
    Neurons can not be created directly. You must use Layer.setSize();
*/

#pragma once

#include <string>
#include <vector>

#include "../point3.h"
#include "../point3i.h"

#include "neuron_list.h"
#include "neuron_extention.h"
#include "bind_list.h"
#include "bind.h"



using namespace std;


class Layer;


/*
    Neuron structure
*/
struct Neuron
{
    Layer* layer                = NULL; /* Neuron layer */
    NeuronExtention* extention  = NULL; /* Neuron extention object */
    BindList* parentBinds       = NULL; /* Binds for parents */
    BindList* childrenBinds     = NULL; /* Binds for children */
    double value;                       /* Neuron value */



    Neuron
    (
        Layer*  /* Layer object */
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



    /*
        Set world point
    */
    Neuron* setWorldPoint
    (
        Point3d&
    );

    Point3d& getWorldPoint();


    /*
        Set screen point
    */
    Neuron* setScreenPoint
    (
        Point3d&
    );

    Point3d& getScreenPoint();



    /*
        Create extention for neuron
    */
    Neuron* createExtention();



    /*
        Destroy extention for neuron
    */
    Neuron* destroyExtention();
};

