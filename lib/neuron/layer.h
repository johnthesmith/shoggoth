/*
    Layer

    This is a container for neurons with 3D adressing.
    Contains the NeuronList object.

    The layer creates new neurons or removes them when resized.
    Layer has reflection at 3D world like Object and it can be moved, rotated etc.
*/

#pragma once

#include <string>
#include <vector>

#include "../log.h"
#include "../point3.h"
#include "../point3i.h"
#include "../object.h"

#include "neuron_list.h"

using namespace std;



struct Neuron;



class Layer : public Object
{
    private:

        Log& log;

        Point3d drawSize = POINT_3D_I;

        virtual Neuron* newNeuron();

    public:

        NeuronList neurons;                /* List of neurons */
        Point3i size = Point3i( 0, 0, 0 );   /* Dimention size */


        /*
            Constructor
        */
        Layer
        (
            Log&
        );



        /*
            Destructor
        */
        ~Layer();



        /*
            Return the log object
        */
        Log& getLog();


        /*
            Add new neuron and return it
        */
        Layer& addNeuron
        (
            Neuron&
        );



        /*
        */
        int indexByPos
        (
            const Point3i&
        );



        Neuron& neuronByPos
        (
            const Point3i&
        );



        /*
            Set dimations size
        */
        Layer& setSize
        (
            const Point3i& = POINT_3I_0
        );



        Layer& connectTo
        (
            Layer&
        );



        Layer& disconnectFrom
        (
            Layer&
        );



        Layer& noiseFor
        (
            Layer&,
            double,  /* Minimum noise */
            double   /* Maximum noise */
        );
};
