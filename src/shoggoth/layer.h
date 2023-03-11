/*
    Layer

    This is a container for neurons with 3D adressing.
    Contains the NeuronList object.

    The layer creates new neurons or removes them when resized.
    Layer has reflection at 3D world like Object and it can be moved, rotated etc.
*/

#pragma once

#include <string>

#include "../lib/utils.h"
#include "../lib/log.h"

#include "../graph/point3.h"
#include "../graph/point3i.h"
#include "../graph/points3d.h"
#include "../graph/object.h"

#include "neuron_list.h"
#include "bind.h"



using namespace std;




enum ConnectType
{
    ONE_TO_ONE, /* Connection type = */
    ALL_TO_ALL  /* Connection type x */
};



enum BindDrawMode
{
    BDM_HIDDEN,
    BDM_WEIGHT,
    BDM_TYPE
};



/*
    Predescription of neuron
*/
struct Neuron;



class Layer : public Object
{
    private:

        /* States */
        Log&            log;                                    /* Log object */
        bool            loopParityValue         = false;        /* Loop parity value */
        bool            loopParityError         = false;        /* Loop parity error */
        bool            pointsRecalc            = true;         /* Recalculate points for Neurons */
        Point3d         drawSize                = POINT_3D_0;   /* Visual draw size at GL units*/

        bool            errorChange             = false;        /* True - method errorChange return true for any neuron, else false */
        bool            incomeChanged           = false;        /* True if preceptron chenged. Set in neuron->setValue*/

        /* Settings */
        Point3i         size                    = POINT_3I_0;   /* Dimention size */
        string          id                      = "";           /* Id of layer */
        string          name                    = "";           /* Name of layer */

        bool            showNeurons             = true;         /* Show neurons */
        BindDrawMode    showBinds               = BDM_HIDDEN;   /* Bind draw mode */
        bool            showLayer               = true;         /* SHow layer cover */

        /* Internal method for neuron creation */
        virtual         Neuron* newNeuron();

    public:

        NeuronList*     neurons;                                /* List of neurons */

        double          neuronDrawBox           = 0.1;          /* Neuron size in 3d space */
        double          neuronDrawSize          = 6.0;          /* Neuron size in scerrn pixels */
        Point3d         borderSize              = POINT_3D_I * 0.1;

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
            Layer*,
            ConnectType,
            BindType,
            double = +0.1,
            double = -0.1
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
            Recalculate world position
        */
        Layer* neuronPointsCalc();




        /*
            Calc layer forward
        */
        Layer* calcValue
        (
            bool    /* Loop parity */
        );



        /*
            Calc layer error (backward)
        */
        Layer* calcError
        (
            bool    /* Loop parity */
        );



        /*
            Calc layer forward
        */
        Layer* learning();



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

        /*
            Set the id of layer
        */
        Layer* setId
        (
            string
        );


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




        bool getErrorChange();



        /******************************************************************************
            Setters and getters
        */


        /*
            Get loop parity error
        */
        bool getLoopParityError();



        /*
            Get loop parity value
        */
        bool getLoopParityValue();



        /*
            Return list of neurons in screen rect
        */
        Layer* getNeuronsByScreenRect
        (
            NeuronList*,
            Point3d&,       /* Top left point */
            Point3d&        /* Bottom right point */
        );



        /*
            Return list of neurons around the screen poistion
        */
        Layer* getNeuronsByScreenPos
        (
            NeuronList*,
            const Point3d&       /* Top left point */
        );


        Layer* switchShowBinds();
        Layer* switchShowLayer();


        Point3i getSize();



        /*
            Return count of neurons in layer
        */
        int getNeuronsCount();



        BindDrawMode getShowBinds();
        bool getShowLayer();
};
