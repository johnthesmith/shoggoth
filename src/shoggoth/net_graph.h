#pragma once


#include "../graph/scene.h"
#include "../graph/point3.h"
#include "../graph/rgba.h"



#include "net.h"
#include "neuron_list.h"



enum NeuronDrawMode
{
    NDM_VALUE,  /* Show values for neuron */
    NDM_ERROR   /* Show errors for neuron */
};



class NetGraph : public Net
{
    private:

        int selected                        = -1;       /* Index of selectrd neuron */

        NeuronList*     selected            = NULL;         /* List of selected neurons */
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

    public:
        /*
            Constructor
        */
        NetGraph
        (
            Application*    /* Application object */
        );



        /*
            Destructor
        */
        ~NetGraph();



        /*
            Create
        */
        static NetGraph* create
        (
            Application*
        );




        /*
            Draw learning mode indicator
        */
        NetGraph* drawLearningMode
        (
            Scene*
        );



        /*
            Draw neuron chart
        */
        NetGraph* drawNeuronChart
        (
            Scene*,
            Neuron*
        );



        /*
            draw selected neurons
        */
        NetGraph* drawSelectedNeurons
        (
            Scene*
        );



        /*
            Set neuron draw mode
        */
        NetGraph* setNeuronDrawMode
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
        NetGraph* neuronPointsScreenCalc
        (
            Scene*, /* Scene object */
            Layer*
        );



        /*
            Draw layers
        */
        NetGraph* drawLayers
        (
            Scene*, /* Scene object */
            bool    /* Recalculate the screen position */
        );



        /*
            Draw layer
        */
        NetGraph* drawLayer
        (
            Scene*, /* Scene object */
            Layer*, /* Drawne Layer object*/
            bool    /* Recalculate the screen position */
        );



        /*
            Draw nerves
        */
        NetGraph* drawNerves
        (
            Scene* /* Scene object */
        );



        /*
            Draw nerve
        */
        NetGraph* drawNerve
        (
            Scene*,
            Nerve*
        );



        /*
            Return neurons by screen rectangle
        */
        NetGraph* getNeuronsByScreenRect
        (
            NeuronList*,    /* Returned list */
            Point3d&,       /* top left */
            Point3d&        /* botom right */
        );



        /*
            Return list of selected neurons
        */
        NeuronList* getSelectedNeurons();



        /*
            Select neurons by screen rectangle
        */
        NetGraph* selectNeuronsByRect
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


        NetGraph* switchShowBinds();



        Rgba getArrowColorByType
        (
            BindType
        );



        /*
            Return neuron by screen position
        */
        vector<int> getNeuronsByScreenPos
        (
            const Point3d&
        );



//        Net* setSelected
//        (
//            
//        );



// Neuron* getSelected();
};
