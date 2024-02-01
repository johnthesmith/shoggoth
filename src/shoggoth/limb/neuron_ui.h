/*
    Neuron object for shoggoth
*/

#pragma once

#include "../../../../../lib/graph/point3.h"
#include "../layer.h"


using namespace std;



/*
    Predeclaration
*/

class LayerUi;




class NeuronUi
{
    private:

        LayerUi*    layer = NULL;   /* Neuron layer object */
        int         index = -1;     /* Neurons index in layer */

    public:

        /*
            Constructor
        */
        NeuronUi
        (
            LayerUi*,
            int
        );



        /*
            Create the Layer list object
        */
        static NeuronUi* create
        (
            LayerUi*,
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
        LayerUi* getLayer();



        /*
            Set neuron screen position
        */
        NeuronUi* setValue
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
        NeuronUi* setError
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
        NeuronUi* setWorld
        (
            Point3d&
        );




        /*
            Set neuron screen position
        */
        NeuronUi* setScreen
        (
            Point3d&
        );
};
