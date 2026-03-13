/*
    Its contain the Main neuralnet mathematics
*/
# pragma once


/*
    Limb - processor
*/

#include <cmath>

#include "../../../../../lib/core/mon.h"
#include "../../../../../lib/core/thread_manager.h"


#include "../../shoggoth/limb.h"
#include "../../shoggoth/net.h"
#include "layer_processor.h"


class Payload;


class LimbProcessor : public Limb
{
private:
    /* External Net object */
    Net*            net                 = NULL;
    /* Master payload */
    Payload*        payload             = NULL;
    /* Monitor object */
    Mon*            mon                 = NULL;
    /* Chart list object */
    ChartList*      weightsChart        = NULL;
    /* Forward and backward thread manager */
    ThreadManager*  threadManager       = NULL;
    /* Weight thread manager */
    ThreadManager*  threadManagerWeight = NULL;

    /*
        Calculation state
    */
    /* Terminated status, stop the calculation and dump works */
    bool            terminated      = false;

    long long int fpsStart = 0;
    int fpsTick = 0;

    /*
        Settings
    */

    /*
        Learning layer id
        Learning will start if this layer contains nonzero sum of value
    */
    string learningLayerId          = "learning";
    /* 0.0 - learning disable, max 0.1 recomended */
    real  learningSpeed             = 0.001;
    /* Each tickWrite from tick all Weights will be writen to file */
//    int     tickWrite               = 10;
    /* Each tickChart from ешсл Chartss will be writen to mon */
    int     tickChart               = 1000;

    /* Fps */
    ChartData* fps = NULL;
public:

    /*
        Constructor
    */
    LimbProcessor
    (
        /* Master payload */
        Payload*,
        /* Net limb object*/
        Net*
    );



    /*
        Destructor
    */
    ~LimbProcessor();



    /*
        Create
    */
    inline static LimbProcessor* create
    (
        /* Master payload */
        Payload* aPyaload,
        /* The net object*/
        Net* aNet
    )
    {
        return new LimbProcessor( aPyaload, aNet );
    }




    /**********************************************************************
        ...
    */



    /*
        Set terminated signal
    */
    LimbProcessor* setTerminated
    (
        bool a
    )
    {
        terminated = a;
        return this;
    }




    /*
        Calculate all enabled layers
    */
    LimbProcessor* calc();



    /*
        The nerve list controlling
    */
    LimbProcessor* nerveControl();



    /*
        Create new layer for this limb and copy parameters from source layer.
        This method have to overriden at children Limbs.
    */
    LayerProcessor* copyLayerFrom
    (
        LayerProcessor* aLayerFrom
    );


    /**********************************************************************
        Setters and getters
    */


    /*
        Get learning speed
    */
    inline real getLearningSpeed()
    {
        return learningSpeed;
    }



    /*
        Set learning speed
    */
    inline LimbProcessor* setLearningSpeed
    (
        real a
    )
    {
        learningSpeed = a;
        return this;
    }





    /*
        Get learning layer id
    */
    inline string getLearningLayerId()
    {
        return learningLayerId;
    }



    /*
        Set learning speed
    */
    inline LimbProcessor* setLearningLayerId
    (
        /* Value */
        string a
    )
    {
        learningLayerId = a;
        return this;
    }



    /*
        Get tick fo write weight
    */
    int getTickWrite();



    /*
        Set tick for write weights
    */
    LimbProcessor* setTickWrite
    (
        int /* Value */
    );



    /*
        Set tick for write charts
    */
    LimbProcessor* setTickChart
    (
        int /* Value */
    );




    /*
        Calculate neuron parent weights
    */
    LimbProcessor* neuronCalcWeight
    (
        /* Layer for calculation */
        Layer*,
        /* Neuron index of layer */
        int
    );



    /*
        Create processor lock file and waiting it removing
    */
    LimbProcessor* calcDebugWait
    (
        CalcStage aStage
    );



    /*
        Write debug monitoring
    */
    LimbProcessor* calcDebugDump
    (
        CalcStage aStage
    );



    LimbProcessor* weightsRead
    (
        /* Path folder for layers */
        string
    );



    LimbProcessor* weightsWrite
    (
        /* Path folder for layers */
        string
    );



    /*
        Dump layers
    */
    LimbProcessor* dumptypeLayerProcessing
    (
        /* file name */
        string,
        /* List of layers id*/
        ParamList*,
        /* Dataview ( how ) */
        Dataview,
        /* Data ( what ) */
        Data,
        /* Tick */
        int
    );



    /*
        Reset calc state
    */
    inline LimbProcessor* resetCalcState()
    {
        getLayerList() -> loop
        (
            []
            ( void* item )
            {
                (( LayerProcessor* ) item) -> setCalcState( WAIT );
                return false;
            }
        );
        return this;
    }



    /*
        Create new layer in this limb
    */
    inline Layer* createLayer
    (
        string aLayerId
    ) override
    {
        return ( Layer* ) LayerProcessor::create( ( Limb* ) this, aLayerId );
    }



    /*
        Dump layers
    */
    virtual LimbProcessor* dump() override;



    /*
        Return payload for limb
    */
    inline Payload* getPayload()
    {
        return payload;
    }

};

