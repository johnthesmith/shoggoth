/*
    Neuron object for shoggoth
*/

#pragma once

#include "../graph/point3.h"


using namespace std;



/*
    Predeclaration
*/

class
Layer;




class Neuron
{
    private:

        Layer*  layer = NULL;   /* Neuron layer object */
        int     index = -1;     /* Neurons index in layer */

    public:

        /*
            Constructor
        */
        Neuron
        (
            Layer*,
            int
        );



        /*
            Create the Layer list object
        */
        static Neuron* create
        (
            Layer*,
            int
        );



        /*
            Destroy the Layer list object
        */
        void destroy();



        /*
            Return index
        */
        int getIndex();



        /*
            Return Layer
        */
        Layer* getLayer();



        /*
            Set neuron screen position
        */
        Neuron* setValue
        (
            double
        );



        /*
            Return neuron screen position
        */
        double getValue();



        /*
            Set neuron error
        */
        Neuron* setError
        (
            double
        );



        /*
            Return neuron error
        */
        double getError();




        /*
            Return neuron screen position
        */
        Point3d getScreen();




        /*
            Return neuron world position
        */
        Point3d getWorld();



        /*
            Set neuron world position
        */
        Neuron* setWorld
        (
            Point3d&
        );




        /*
            Set neuron screen position
        */
        Neuron* setScreen
        (
            Point3d&
        );
};
