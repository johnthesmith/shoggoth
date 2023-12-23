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
#include "shoggoth_consts.h"


using namespace std;



enum BindDrawMode
{
    BDM_HIDDEN,
    BDM_WEIGHT,
    BDM_SIGNAL,
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
        double, /* Weight of bind */
        int     /* return weight index */
    )
>
parentsLambda;



/*
    Lambda function for return children neurons for parent
*/
typedef
function
<
    bool
    (
        Layer*, /* return layer with children neurons */
        int,    /* return child neuron index*/
        Nerve*, /* return nerve */
        double, /* return weight of bind */
        int     /* return weight index */
    )
>
childrenLambda;



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

        string          storagePath             = "";

        int             forward                 = -1;
        int             backward                = -1;

        int             count                   = 0;            /* Count fo neurons */
        int             cacheCount              = 0;            /* Count fo neurons in cach */

        ParamList*      actions                 = NULL;         /* List of actions for layer */

        /*
            Plans of neurons data
        */

        double*         valuesCache             = NULL;         /* Processors values layer  */
        double*         errorsCache             = NULL;         /* Processors errors layer */
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
        bool            valuesCacheExists       = false;
        bool            errorsCacheExists       = false;
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
            WARNING!!! Use Layer -> destroy() only
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
            double, /* wakeupWeight */
            int,
            int
        );



        /***********************************************************************
            Setters and getters
        */


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



        Layer* setDrawSize
        (
            ParamList*
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
            const Point3d&,     /* Top left point */
            const int           /* Screen radius */
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



        Layer* valuesFromBuffer
        (
            char*,
            size_t
        );



        /*
            Read values from io
        */
        Layer* readValues();



        /*
            Return buffer and size of buffer of values
        */
        Layer* getValuesBuffer
        (
            char*&,  /* Buffer pointer */
            size_t&  /* Size of buffer */
        );



        /*
            Send errors to io
        */
        Layer* writeErrors();



        /*
            Read errors from io
        */
        Layer* readErrors();



        /*
            Return buffer and size of buffer of errors
        */
        Layer* getErrorsBuffer
        (
            char*&, /* Buffer pointer */
            size_t& /* Size of buffer */
        );



        Layer* errorsFromBuffer
        (
            char*,
            size_t
        );




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
        Layer* bitmapToValue
        (
            Bitmap*
        );



        /*
            Load bitmap from source file name and set it
        */
        Layer* imageToValue
        (
            string  /* File name */
        );




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
            parentsLambda
        );



        /*
            Loop for each child of i neuron
        */
        Layer* childrenLoop
        (
            int,
            childrenLambda
        );



        /*
            Return event actions
        */
        ParamList* getActions();



        /*
            Noise fill values of layer neurons
        */
        Layer* noiseValue
        (
            int,        /* Random seed */
            double,     /* Min value */
            double      /* Max value */
        );



        /*
            Calculate neuron
        */
        Layer* neuronLearning
        (
            int,    /* Index of neuron */
            double, /* ErrorNormalize */
            double, /* LearningSpeed */
            double  /* WakeupWeight */
        );



        /* Move values and errors data to processor cache */
        Layer* dataToCache();



        /* Move values and errors from processor cache to data */
        Layer* cacheToData();


        /*
            Return the log object
        */
        Log* getLog();

};


