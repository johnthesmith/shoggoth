#pragma once

#include <string>
#include <vector>

#include "../log.h"
#include "../point3.h"
#include "../point3i.h"
#include "../object.h"

#include "neuron_list.h"

using namespace std;



class Neuron;



class Layer : public Object
{
    private:

        Log& log;

    public:

        NeuronList neurons;                /* List of neurons */
        Point3i size = Point3i( 0, 0, 0 );   /* Dimention size */

        Layer
        (
            Log&
        );


        ~Layer();


        /*
            Add new neuron and return it
        */
        Layer& addNeuron
        (
            Neuron&
        );



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
            const Point3i&
        );
};
