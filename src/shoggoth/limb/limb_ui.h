/*
    Shoggoth limb for ui
    Contains methods for drawing net in UI interface.
    2023
*/



#pragma once



#include "../../../../../lib/graph/scene.h"
#include "../../../../../lib/graph/point3.h"
#include "../../../../../lib/graph/rgba.h"

#include "../limb.h"
#include "net.h"
#include "layer_ui_const.h"


class NeuronUi;
class LayerUi;
class NeuronListUi;


/*
    Lambda function for nerve weights loop
*/
typedef function
<
    bool
    (
        int,        /* Index of weight in a nerve */
        NeuronUi*,    /* Neuron in tne parent layer */
        NeuronUi*,    /* Neuron in the child layer*/
        Nerve*      /* Nerve object */
    )
>
IndexWeightLambda;



class LimbUi : public Limb
{
    private:

        Net*            net                 = NULL;
        NeuronListUi*   selected            = NULL;         /* List of selected neurons */

        NeuronDrawMode  neuronDrawMode      = NDM_VALUE;    /* Draw mode for neurons */
        BindDrawMode    showBinds           = BDM_HIDDEN;

        int             neuronDrawSize      = 4;

        Rgba            interfaceColorDark  = Rgba( 0.9, 0.9, 1.0, 0.05 );
        Rgba            interfaceColor      = Rgba( 0.9, 0.9, 1.0, 0.7 );

        Rgba            colorBindValueM     = Rgba( 0.0, 0.5, 1.0, 0.5 );
        Rgba            colorBindValueZ     = Rgba( 1.0, 1.0, 1.0, 0.01 );
        Rgba            colorBindValueP     = Rgba( 1.0, 0.5, 0.0, 0.5 );

        Rgba            colorBindErrorM     = Rgba( 0.0, 1.0, 1.0, 0.5 );
        Rgba            colorBindErrorZ     = Rgba( 1.0, 1.0, 1.0, 0.01 );
        Rgba            colorBindErrorP     = Rgba( 1.0, 0.0, 0.0, 0.5 );

        Rgba            colorValueZ         = Rgba( 0.0, 0.5, 1.0, 1.0 );
        Rgba            colorValueP         = Rgba( 1.0, 0.5, 0.0, 1.0 );

        Rgba            colorErrorM         = Rgba( 0.0, 1.0, 1.0, 1.0 );
        Rgba            colorErrorZ         = Rgba( 1.0, 1.0, 1.0, 0.3 );
        Rgba            colorErrorP         = Rgba( 1.0, 0.0, 0.0, 1.0 );

        double          cursorRadius        = 10.0;
    public:
        /*
            Constructor
        */
        LimbUi
        (
            Net*
        );



        /*
            Destructor
        */
        ~LimbUi();



        /*
            Create
        */
        static LimbUi* create
        (
            Net*
        );



        /*
            Draw neuron chart
        */
        LimbUi* drawNeuronChart
        (
            Scene*,
            NeuronListUi*
        );



        /*
            draw selected neurons
        */
        LimbUi* drawSelectedNeurons
        (
            Scene*
        );



        /*
            Set neuron draw mode
        */
        LimbUi* setNeuronDrawMode
        (
            NeuronDrawMode
        );



        /*
            Set neuron draw mode
        */
        NeuronDrawMode getNeuronDrawMode();



        /*
            Calculate screen points for layer
        */
        LimbUi* neuronPointsScreenCalc
        (
            Scene*, /* Scene object */
            LayerUi*
        );



        /*
            Draw layers
        */
        LimbUi* drawLayers
        (
            Scene*,     /* Scene object */
            bool        /* Recalculate the screen position */
        );



        /*
            Draw layer
        */
        LimbUi* drawLayer
        (
            Scene*,     /* Scene object */
            LayerUi*,   /* Drawne Layer object*/
            bool        /* Recalculate the screen position */
        );



        /*
            Draw nerves
        */
        LimbUi* drawNerves
        (
            Scene* /* Scene object */
        );



        /*
            Draw nerve
        */
        LimbUi* drawNerve
        (
            Scene*,
            Nerve*
        );



        /*
            Return neurons by screen rectangle
        */
        LimbUi* getNeuronsByScreenRect
        (
            NeuronListUi*,  /* Returned list */
            Point3d&,       /* top left */
            Point3d&        /* botom right */
        );



        /*
            Return list of selected neurons
        */
        NeuronListUi* getSelectedNeurons();



        /*
            Select neurons by screen rectangle
        */
        LimbUi* selectNeuronsByRect
        (
            Point3d,    /* Top left corner */
            Point3d     /* Bottom right corner */
        );



        Rgba getBindErrorColor
        (
            const double
        );



        Rgba getBindValueColor
        (
            const double
        );



        Rgba getErrorColor
        (
            const double
        );


        LimbUi* switchShowBinds();



        Rgba getArrowColorByType
        (
            BindType
        );



        /*
            Return neuron by screen position
        */
        LimbUi* getNeuronsByScreenPos
        (
            NeuronListUi*,
            const Point3d&
        );



        NeuronUi* getSelectedFirst();



        /*
            Set list of selected neurons
        */
        LimbUi* setSelected
        (
            NeuronUi*
        );



        LimbUi* setSelected
        (
            Scene*
        );



        /*
            Add new selected neurons
        */
        LimbUi* addSelectedByCursor
        (
            Scene*  /* Scene object */
        );



        /*
            Remove selected neurons
        */
        LimbUi* removeSelectedByCursor
        (
            Scene*  /* Scene object */
        );



        /*
            Return screen radius
        */
        double getCursorRadius();



        /*
            Set screen radius
        */
        LimbUi* setCursorRadius
        (
            double /* Pixel radius */
        );



        /*
            Return average weights for selected neurons
        */
        double avgWeightBySelect();



        bool nerveWeightLoop
        (
            NeuronListUi*,
            IndexWeightLambda
        );



        /*
        */
        LimbUi* switchShowLayer();



        /*
            Create new layer for this limb and copy parameters from source layer.
            This method is an overridden method of the Limb class.
        */
        Layer* copyLayerFrom
        (
            Layer* aFromLayer
        );



        /*
            Return net object
        */
        Net* getNet();
};
