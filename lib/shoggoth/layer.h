/*
    Layer

    This is a container for neurons with 3D adressing.
    Contains the NeuronList object.

    The layer creates new neurons or removes them when resized.
    Layer has reflection at 3D world like Object and it can be moved, rotated etc.
*/

#pragma once

#include <string>

#include "../utils.h"
#include "../log.h"

#include "../graph/point3.h"
#include "../graph/point3i.h"
#include "../graph/points3d.h"
#include "../graph/object.h"
#include "../graph/scene.h"   /*LOOP*/

#include "neuron_list.h"

using namespace std;



enum NeuronDrawMode
{
    NDM_VALUE,
    NDM_ERROR
};



enum LayerType
{
    LT_RECEPTOR,
    LT_CORTEX,
    LT_RESULT
};



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

        /* Settings */
        Point3d drawSize            = POINT_3D_0;   /* Visual draw saze at GL units*/
        Point3i size                = POINT_3I_0;   /* Dimention size */

        double defaultBindWeightMin = 0.0;          /* Default bind weight for new binds */
        double defaultBindWeightMax = 0.0;

        string id                   = "";           /* Id of layer */
        string name                 = "";           /* Name of layer */
        bool pointsRecalc           = true;         /* Recalculate points for Neurons */

        bool showBox                = true;
        bool showNeurons            = true;
        bool showChildrenLink       = true;
        bool showParentsLink        = true;

        /* Internal method for neuron creation */
        virtual Neuron* newNeuron();

        LayerType       layerType       = LT_CORTEX;

        double          sensivity       = 10;

        bool            errorChange     = false;    /* True - method errorChange return true for any neuron, else false */
        bool            loopParity      = false;    /* Loop parity value */
        bool            incomeChanged   = false;    /* True if preceptron chenged. Set in neuron->setValue*/

        const Rgba      colorValue0     = Rgba( 0.0, 0.5, 1.0, 1.0 );
        const Rgba      colorValue1     = Rgba( 1.0, 0.5, 0.0, 1.0 );
        const Rgba      colorError0     = Rgba( 1.0, 0.0, 0.0, 0.0 );
        const Rgba      colorError1     = Rgba( 1.0, 0.0, 0.0, 1.0 );

        NeuronDrawMode  neuronDrawMode  = NDM_VALUE;

        Rgba            colorLayerTypeReceptor  = Rgba( 1.0, 1.0, 0.0, 0.1 );
        Rgba            colorLayerTypeCortex    = Rgba( 0.5, 0.7, 1.0, 0.1 );
        Rgba            colorLayerTypeResult    = Rgba( 1.0, 0.5, 1.0, 0.1 );
    public:

        NeuronList* neurons;                        /* List of neurons */

        double neuronDrawBox        = 0.1;          /* Neuron size in 3d space */
        double neuronDrawSize       = 6.0;          /* Neuron size in scerrn pixels */
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



        /*
            Recalculate world position
        */
        Layer* neuronPointsCalc();



        /*
            Recalculate screeen position
        */
        Layer* neuronPointsScreenCalc
        (
            Scene*  /* Scene */
        );



        /*
            Calc layer forward
        */
        Layer* calc
        (
            bool    /* Loop parity */
        );



        /*
            Calc layer error (backward)
        */
        Layer* calcError();



        /*
            Draw layer
        */
        Layer* draw
        (
            Scene* aScene,   /* Scene object */
            bool calcScreenPos
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




        bool getErrorChange();



        /******************************************************************************
            Setters and getters
        */


        /*
            Set sensivity of layer
        */
        Layer* setSensivity
        (
            const double
        );



        /*
            Get sensivity of layer
        */
        double getSensivity();



        /*
            Get loop parity
        */
        bool getLoopParity();



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



        Layer* setNeuronDrawMode
        (
            const NeuronDrawMode
        );



        NeuronDrawMode getNeuronDrawMode();



        Layer* setLayerType
        (
            const LayerType
        );



        LayerType getLayerType();
};
