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
#include "../points3d.h"
#include "../object.h"


#include "../scene.h"   /*LOOP*/

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
        Point3d drawSize            = POINT_3D_0;   /* Visual draw saze at GL units*/
        Point3i size                = POINT_3I_0;   /* Dimention size */

        double defaultBindWeightMin = 0.0;          /* Default bind weight for new binds */
        double defaultBindWeightMax = 0.0;

        string id                   = getUuid();    /* Id of layer */
        string name                 = "";           /* Name of layer */
        bool pointsRecalc           = true;         /* Recalculate points for Neurons */

    public:

        NeuronList* neurons;                        /* List of neurons */
        Points3d* points;                           /* List of neurons points */

        double neuronDrawBox        = 0.1;          /* Neuron size in 3d space */
        double neuronDrawSize       = 3.0;          /* Neuron size in scerrn pixels */
        Point3d borderSize          = Point3d( 0.1, 0.1, 0.1 );



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



        Layer* neuronPointsCalc();



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



        Layer* setNeuronDrawBox
        (
            const double
        );



        Layer* setNeuronDrawSize
        (
            const double
        );



        Layer* setBorderSize
        (
            const Point3d&
        );



        Layer* setDrawSize
        (
            const Point3d&
        );



        Layer* setPointsRecalc
        (
            bool a
        );
};
