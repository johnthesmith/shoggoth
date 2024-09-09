/*
    Layer for UI limb

    This is a container for neurons with 3D adressing.
    Layer has reflection at 3D world like Object and it can be moved, rotated etc.
*/


#pragma once

#include "../../../../../lib/graph/object.h"
#include "../layer.h"
#include "layer_ui_const.h"


using namespace std;



/*
    Predeclaration
*/

class LimbUi;
class NeuronListUi;



class LayerUi : public Layer
{
    private:

        /* Recalculate points for Neurons */
        bool            pointsRecalc            = false;
        /* 3D Object */
        Object*         object                  = NULL;
        /* Visual draw size at GL units*/
        Point3d         drawSize                = POINT_3D_0;
        /* Outer box size recalculate in neuronPointsCalc */
        Point3d         outerBox                = POINT_3D_0;

        /*
            Settings
        */
        /* Bind draw mode */
        BindDrawMode    showBinds               = BDM_HIDDEN;
        /* SHow layer cover */
        bool            showLayer               = true;
        /* Neuron size in 3d space */
        double          neuronDrawBox           = 0.1;
        /* Neuron size in scerrn pixels */
        double          neuronDrawSize          = 6.0;
        Point3d         borderSize              = POINT_3D_I * 0.1;

        /*
            Plans of neurons data
        */
        Point3d*        screen                  = NULL;
        Point3d*        world                   = NULL;
        bool*           selected                = NULL;

    public:

        /*
            Constructor
        */
        LayerUi
        (
            Limb*,        /* Limb object */
            string = ""     /* id */
        );



        /*
            Destructor
        */
        ~LayerUi();



        /*
            Creator
        */
        static LayerUi* create
        (
            Limb*,        /* Ui Limb object */
            string = ""
        );



        /**********************************************************************
            Data plans work
        */



        /*
            Clear screen points  for all neurons
        */
        LayerUi* clearScreen();



        /*
            Clear world points for all neurons
        */
        LayerUi* clearWorld();



        /*
            Clear world points for all neurons
        */
        LayerUi* clearSelected();



        /***********************************************************************
            Setters and getters
        */


        LimbUi* getLimb();



        /*
            Resize world position plan
        */
        LayerUi* worldCreate();


        /*
            Resize screen position plan
        */
        LayerUi* screenCreate();



        /*
            Resize selected plan
        */
        LayerUi* selectedCreate();



        /*
            Set position
        */
        LayerUi* setPosition
        (
            ParamList*
        );



        LayerUi* setNeuronDrawBox
        (
            const double
        );



        LayerUi* setNeuronDrawSize
        (
            const double
        );



        LayerUi* setBorderSize
        (
            const Point3d&
        );



        LayerUi* setDrawSize
        (
            const Point3d&
        );



        Point3d& getDrawSize();



        LayerUi* setDrawSize
        (
            ParamList*
        );



        LayerUi* setPointsRecalc
        (
            bool
        );



        /*
            Return list of neurons indexes in screen rect
        */
        LayerUi* getNeuronsByScreenRect
        (
            NeuronListUi*,
            Point3d&,       /* Top left point */
            Point3d&        /* Bottom right point */
        );



        /*
            Return list of neurons around the screen poistion
        */
        LayerUi* getNeuronsByScreenPos
        (
            NeuronListUi*,      /* List of neurons */
            const Point3d&,     /* Top left point */
            const int           /* Screen radius */
        );



        BindDrawMode getShowBinds();



        LayerUi* setShowBinds
        (
            BindDrawMode a
        );



        bool getShowLayer();



        /*
            Return outer box size
        */
        Point3d& getOuterBox();



        /**********************************************************************
            Neurons setters and getters
        */



        /*
            Set neuron world position
        */
        LayerUi* setNeuronWorld
        (
            int,            /* Index of neuron */
            Point3d&        /* Value */
        );



        /*
            Return neuron world position or default value
        */
        Point3d getNeuronWorld
        (
            int             /* Index of neuron */
        );



        /*
            Set neuron screen position
        */
        LayerUi* setNeuronScreen
        (
            int,            /* Index of neuron */
            Point3d&        /* Value */
        );



        /*
            Return neuron screen position or default value
        */
        Point3d getNeuronScreen
        (
            int             /* Index of neuron */
        );



        /*
            Set neuron selected
        */
        LayerUi* setNeuronSelected
        (
            int,            /* Index of neuron */
            bool            /* Value */
        );



        /*
            Return neuron selected or default value
        */
        Point3d getNeuronSelected
        (
            int             /* Index of neuron */
        );



        /**********************************************************************
            UI
        */

        LayerUi* switchShowLayer();


        /*
            Recalculate world position
        */
        LayerUi* neuronPointsCalc
        (
            bool
        );


        Object* getObject();



        /*
            Change the method scope for setCount
            This mthod must called by the setSize only
        */
        LayerUi* setSize
        (
            Point3i  = POINT_3I_0
        );
};
