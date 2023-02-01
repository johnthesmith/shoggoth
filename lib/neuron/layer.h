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

#include "../utils.h"
#include "../log.h"
#include "../point3.h"
#include "../point3i.h"
#include "../object.h"


#include "../scene.h"   /* Layer draw */

#include "neuron_list.h"

using namespace std;



enum ConnectType
{
    ONE_TO_ONE, /* Connection type = */
    ALL_TO_ALL  /* Connection type x */
};



/*
    Predescription of neuron
*/
struct Neuron;



class Layer : public Object
{
    private:

        /* States */
        Log& log;                                   /* Log object */
        virtual Neuron* newNeuron();                /* Internal method for neuron creation */

        /* Settings */
        Point3d drawSize            = POINT_3D_I;   /* Visual draw saze at GL units*/

        double defaultBindWeightMin = 0.0;          /* Default bind weight for new binds */
        double defaultBindWeightMax = 0.0;

        string id                   = getUuid();    /* Id of layer */
        string name                 = "";           /* Name of layer */

    public:

        NeuronList* neurons;                        /* List of neurons */
        Point3i size                = POINT_3I_0;   /* Dimention size */



        /*
            Constructor
        */
        Layer
        (
            Log&
        );



        /*
            Internal destructor
        */
        ~Layer();



        /*
            Creator
        */
        static Layer* create
        (
            Log&
        );



        /*
            Destructor
        */
        void destroy();



        /*
            Add new neuron and return it
        */
        Layer* addNeuron
        (
            Neuron&
        );



        /*
            Return index by point 3i at layer
        */
        int indexByPos
        (
            const Point3i&
        );



        Neuron& neuronByPos
        (
            const Point3i&
        );



        Layer* connectTo
        (
            Layer*
        );



        Layer* disconnectFrom
        (
            Layer&
        );



        Layer* noiseFor
        (
            Layer&,
            double,  /* Minimum noise */
            double   /* Maximum noise */
        );



        /*
            Draw layer
        */
        Layer* draw
        (
            Scene& aScene   /* Scene object */
        );




        /***********************************************************************
            Setters and getters
        */



        /*
            Return the log object
        */
        Log& getLog();



        string getName();

        Layer* setName
        (
            string
        );

        string getId();


        string getNameOrId();


        /*
            Set dimations size
        */
        Layer* setSize
        (
            const Point3i& = POINT_3I_0
        );

};
