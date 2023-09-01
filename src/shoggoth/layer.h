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
#include "../lib/hid.h"

#include "../graph/point3.h"
#include "../graph/point3i.h"
#include "../graph/points3d.h"
#include "../graph/object.h"
#include "../graph/bitmap.h"

#include "../json/param_list.h"

#include "nerve_list.h"
#include "neuron_list.h"
#include "nerve_type.h"

#include "rpc_protocol.h"


using namespace std;



enum CalcStage
{
    CALC_UNKNOWN,
    CALC_NOT_START,
    CALC_START,
    CALC_COMPLETE
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
class Net;



class Layer : public Object
{
    private:

        /* States */
        Net*            net                     = NULL;         /* Net object */

        bool            loopParityValue         = false;        /* Loop parity value */
        bool            loopParityError         = false;        /* Loop parity error */
        bool            pointsRecalc            = false;        /* Recalculate points for Neurons */
        Point3d         drawSize                = POINT_3D_0;   /* Visual draw size at GL units*/

        bool            errorChange             = false;        /* True - method errorChange return true for any neuron, else false */
        bool            incomeChanged           = false;        /* True if preceptron chenged. Set in neuron->setValue*/
        Point3d         outerBox                = POINT_3D_0;   /* Outer box size recalculate in neuronPointsCalc */

        /* Settings */
        Point3i         size                    = POINT_3I_0;   /* Dimention size */
        string          id                      = "";           /* Id of layer */
        string          name                    = "";           /* Name of layer */

        bool            showNeurons             = true;         /* Show neurons */
        BindDrawMode    showBinds               = BDM_HIDDEN;   /* Bind draw mode */
        bool            showLayer               = true;         /* SHow layer cover */

        string          sourcePath              = "";           /* Source file for upload to neuron */
        long long int   lastSourceUpdate        = 0;

        string          storagePath             = "";

        bool            read                    = false;
        bool            write                   = false;
        bool            calc                    = false;
        bool            fromServer              = false;
        bool            toServer                = false;

        int             forward                 = -1;
        int             backward                = -1;

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
            Net*,
            string = "" /* id */
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
            Net*,
            string = ""
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



        Neuron* neuronByPos
        (
            const Point3i&
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
        Layer* neuronPointsCalc
        (
            bool
        );



        /*
            Clear values for all neurons
        */
        Layer* clearValues();



        /*
            Calc layer forward
        */
        Layer* calcValue
        (
            int,    /* calcFromNeuron */
            int     /* calcToNeuron */
        );




        /*
            Learning for layer (backward)
        */
        Layer* learning
        (
            double, /* errorNormalize */
            double, /* learningSpeed */
            double  /* wakeupWeight */
        );



        /***********************************************************************
            Setters and getters
        */



        /*
            Return the log object
        */
        Log* getLog();



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
            Set dimentions size
        */
        Layer* setSize
        (
            const Point3i& = POINT_3I_0
        );



        /*
            Set dimentions size
        */
        Layer* setSize
        (
            ParamList*
        );



        /*
            Set position
        */
        Layer* setPosition
        (
            ParamList*
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
            Return the Net object
        */
        Net* getNet();



        /*
            Set loop parity for error and value
        */
        Layer* setLoopParity
        (
            bool
        );



        /*
            Get loop parity error
        */
        bool getLoopParityError();



        /*
            Return neuron list
        */
        NeuronList* getNeurons();



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



        Layer* setShowBinds
        (
            BindDrawMode a
        );


        bool getShowLayer();


        double getError();
        double getValue();


        /*
            Set source path
        */
        Layer* setSourcePath
        (
            string
        );



        /*
            Return the source path
        */
        string getSourcePath();



        /*
            Set bitmap to neurons
        */
        Layer* setBitmap
        (
            Bitmap*
        );



        /*
            Load bitmap from source file name and set it
        */
        Layer* loadSource();

        Layer* applyUuid
        (
            Hid
        );

        Layer* applyImage();



        /*
            Return layer file name
        */
        string getStorageValueName();



        /*
            return the storage path
        */
        string getStoragePath();



        /*
            Set storage path
        */
        Layer* setStoragePath
        (
            string
        );



        /*
            Return source file name
        */
        string getSourceFilename();



        /*
            Return layer path
        */
        string getLayerPath();



        /*
            Return outer box size
        */
        Point3d& getOuterBox();



        /*
            Load value and error from storage file
        */
        Layer* loadValue();



        /*
            Save value and error to storage file
        */
        Layer* saveValue();



        /*
            Send value and error to remote server
        */
        Layer* writeToServer
        (
            string,         /* server ip address */
            int,            /* server port number */
            int,            /* Processor number */
            int,            /* Processor count */
            string,         /* Client ID*/
            CalcDirection   /* Direction of calculation */
        );



        /*
            Return read role
        */
        Layer* readFromServer
        (
            string,         /* ip address */
            int             /* port number */
        );



        /*
            Return read role
        */
        bool getRead();



        /*
            Set read role
        */
        Layer* setRead
        (
            bool /* value */
        );



        /*
            Return write role
        */
        bool getWrite();


        /*
            Set write role
        */
        Layer* setWrite
        (
            bool /* value */
        );



        /*
            Return calc role
        */
        bool getCalc();



        /*
            Return calc role
        */
        Layer* setCalc
        (
            bool /* value */
        );




        /*
            Return send role
        */
        bool getFromServer();



        /*
            Return send role
        */
        Layer* setFromServer
        (
            bool /* value */
        );



        /*
            Return send role
        */
        bool getToServer();



        /*
            Return send role
        */
        Layer* setToServer
        (
            bool /* value */
        );



        /*
            Calculate start neuron for processors operations
        */
        int calcNeuronFrom
        (
            int,    /* Number of processor */
            int     /* Count of processours */
        );



        /*
            Caluculate end of neurons for processors operations
        */
        int calcNeuronTo
        (
            int,    /* Number of processor */
            int     /* Count of processours */
        );




        /************************************************************
            Layer calculateion service
        */



        /*
            Reset calculate states
        */
        Layer* calcReset();



        Layer* calcStartForward();



        Layer* calcStartBackward();



        Layer* calcCompleteForward();



        Layer* calcCompleteBackward();



        CalcStage getForwardStage
        (
            int
        );



        CalcStage getBackwardStage
        (
            int
        );

};


