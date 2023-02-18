/*
    Neurons can not be created directly. You must use Layer.setSize();
*/

#pragma once

#include <string>

#include "../graph/point3.h"
#include "../graph/point3i.h"

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
    double  value;                      /* Neuron value */
    /* Neuron error, difference between value and extends.waitingValue,
    calculating in calcValue */
    double  error               = 0;
    bool    loopParity          = 0;    /* Loop parity, look at Net.loopParity */


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
    Layer* getLayer();



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
        Set error for neuron
    */
    Neuron& setError
    (
        const double
    );



    double getError();



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



    /*
        Return screen point
    */
    Point3d& getScreenPoint();



    /*
        Set waiting value for neuron in learning mode
    */
    Neuron* setWaitingValue
    (
        const double
    );



    /*
        Return a waiting value for neuron in learning mode
    */
    double getWaitingValue();



    /*
        Create extention for neuron
    */
    Neuron* createExtention();



    /*
        Destroy extention for neuron
    */
    Neuron* destroyExtention();



    /*
        Calculate value
    */
    Neuron* calcValue
    (
        bool    /* Loop parity */
    );


    /*
        Calculate neurons error
    */
    Neuron* calcError
    (
        bool& aChange   /* True for error will be change */
    );



    /*
        Set waiting value for neuron in learning mode
    */
    Neuron* setLearningValue
    (
        const double
    );



    /*
        Return a waiting value for neuron in learning mode
    */
    double getLearningValue();

};
