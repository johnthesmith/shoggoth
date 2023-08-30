/*
    Neurons can not be created directly. You must use Layer.setSize();
*/

#pragma once

#include <string>
#include <functional>   /* for lyambda */

#include "../graph/point3.h"
#include "../graph/point3i.h"

#include "neuron_extention.h"
#include "neuron_list.h"
#include "storage_neuron.h"



using namespace std;


class Nerve;
class Layer;



/*
    Neuron structure
*/
class Neuron
{
    private:
    public:

        Layer*              layer      = NULL;  /* Neuron layer */
        int                 index      = -1;    /* Index of neuron in layer */
        NeuronExtention*    extention  = NULL;  /* Neuron extention object */
        double  value;                          /* Neuron value */
        /*
            Neuron error, difference between value and extends.LearningValue,
            calculating in calcValue
        */
        double  error                   = 0;



        Neuron
        (
            Layer*, /* Main neurons layer object */
            int     /* Neuron index in main layer */
        );



        ~Neuron();



        /*
            Create the neuron
        */
        static Neuron* create
        (
            Layer*, /* Main neurons layer object */
            int     /* Neuron index in main layer */
        );



        /*
            Self destroy the neuron
        */
        void destroy();


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
        Neuron* calcValue();



        /*
            Calculate neurons error
        */
        Neuron* learning
        (
            double, /* Error normalize koeff */
            double, /* aLearningSpeed */
            double  /* aWakeupWeight */
        );


        /*
            Loop for each parents of this neuron
        */
        Neuron* parentsLoop
        (
            function <bool ( Neuron*, Nerve*, double )>
        );

};
