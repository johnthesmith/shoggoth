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
    Predeclaration
*/
class Layer;
class Nerve;




/*
    Lambda function for return parents neurons of child
*/
typedef
function
<
    bool
    (
        Layer*, /* Layer with parent neurons */
        int,    /* Neuron index*/
        Nerve*, /* Nerve */
        double  /* Weight of bind */
    )
>
parensLambda;



/*
    Predescription of neuron
*/
class Net;



class Layer : public Object
{
    private:

        /* States */
        Net*            net                     = NULL;         /* Net object */

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

        int             forward                 = -1;
        int             backward                = -1;

        int             count                   = 0;    /* Count fo neurons */

        /*
            IO operations for layer
        */
        bool            readableValues          = false;
        bool            writableValues          = false;
        bool            readableErrors          = false;
        bool            writableErrors          = false;

        /*
            Plans of neurons data
        */

        double*         values                  = NULL;
        double*         errors                  = NULL;
        Point3d*        screen                  = NULL;
        Point3d*        world                   = NULL;
        bool*           selected                = NULL;

        /*
            Plans exists flags
        */
        bool            valuesExists            = false;
        bool            errorsExists            = false;
        bool            screenExists            = false;
        bool            worldExists             = false;        /* Worls points exists */
        bool            selectedExists          = false;

    public:

        double          neuronDrawBox           = 0.1;          /* Neuron size in 3d space */
        double          neuronDrawSize          = 6.0;          /* Neuron size in scerrn pixels */
        Point3d         borderSize              = POINT_3D_I * 0.1;



        /*
            Constructor
        */
        Layer
        (
            Net*,
            string = "",    /* id */
            bool = true,    /* valuesExists */
            bool = true,    /* errorsExists */
            bool = false,   /* screenExists */
            bool = false,   /* worldExists */
            bool = false    /* selectedExists */
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
            Net*,           /* Net object */
            string = "",
            bool = true,    /* valuesExists */
            bool = true,    /* errorsExists */
            bool = false,   /* screenExists */
            bool = false,   /* worldExists */
            bool = false    /* selectedExists */
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




        Layer* noiseFor
        (
            Layer&,
            double,  /* Minimum noise */
            double   /* Maximum noise */
        );




        /*
            Clear values for all neurons
        */
        Layer* clearValues();



        /*
            Clear errors for all neurons
        */
        Layer* clearErrors();



        /*
            Clear screen points  for all neurons
        */
        Layer* clearScreen();



        /*
            Clear world points for all neurons
        */
        Layer* clearWorld();



        /*
            Clear world points for all neurons
        */
        Layer* clearSelected();



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



        /*
            Return Name of ID if Name not exists
        */
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
            Return list of neurons indexes in screen rect
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
            NeuronList*,        /* List of neurons */
            const Point3d&      /* Top left point */
        );




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
            Send values to io
        */
        Layer* writeValues();



        /*
            Read values from io
        */
        Layer* readValues();



        /*
            Send errors to io
        */
        Layer* writeErrors();



        /*
            Read errors from io
        */
        Layer* readErrors();



        /*
            Return read role
        */
        bool getRoleRead();



        /*
            Set read role
        */
        Layer* setRoleRead
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



        /**********************************************************************
            Neurons setters and getters
        */



        /*
            Set neuron value
        */
        Layer* setNeuronValue
        (
            int,            /* Index of neuron */
            double          /* Value */
        );



        /*
            Return neuron vaalue or default value
        */
        double getNeuronValue
        (
            int             /* Index of neuron */
        );



        /*
            Set neuron error
        */
        Layer* setNeuronError
        (
            int,            /* Index of neuron */
            double          /* Value */
        );



        /*
            Return neuron error or default value
        */
        double getNeuronError
        (
            int             /* Index of neuron */
        );



        /*
            Set neuron world position
        */
        Layer* setNeuronWorld
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
        Layer* setNeuronScreen
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
        Layer* setNeuronSelected
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

        Layer* switchShowBinds();
        Layer* switchShowLayer();


        /*
            Recalculate world position
        */
        Layer* neuronPointsCalc
        (
            bool
        );




        /**********************************************************************
            Load valus
        */



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



        /**********************************************************************
            Calculations
        */

        /*
            Calculate neuron
        */
        Layer* neuronCalcValue
        (
            int /* Index of neuron */
        );



        /*
            Loop for each parents of this neuron
        */
        Layer* parentsLoop
        (
            int,    /* Index of neuron for loop by his parents */
            parensLambda callback
        );


        /**********************************************************************
        */


        /*
            Set readable flag for values in layer over io
        */
        bool getReadableValues();
        bool getWritableValues();
        bool getReadableErrors();
        bool getWritableErrors();

        Layer* setReadableValues
        (
            bool
        );

        Layer* setWritableValues
        (
            bool
        );

        Layer* setReadableErrors
        (
            bool
        );



        /*
        */
        Layer* setWritableErrors
        (
            bool
        );

};
