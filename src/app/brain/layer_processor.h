/*
    Layer for Processor

    Extends layer for learning
*/



#pragma once

#include <string>
#include <mutex>

#include "../../shoggoth/layer.h"
#include "../../shoggoth/nerve.h"
#include "../../../../../lib/core/thread_manager.h"



using namespace std;



enum CalcState
{
    WAIT,
    CALCULATING,
    CALCULATED
};




/*
    Class specified methods for layer processing
*/
class LayerProcessor : public Layer
{
private:

    string          remoteProcessorHost;
    int             remoteProcessorPort;

    CalcState       calcState = WAIT;

    /* List of parents nerve for layer */
    vector <Nerve*> parents;
    /* List of children nerve for layer */
    vector <Nerve*> children;

public:

    /*
        Constructor
    */
    LayerProcessor
    (
        Limb*,          /* Limb object */
        string = ""     /* id */
    );



    /*
        Internal destructor
        WARNING!!! Use LayerProcessor -> destroy() only
    */
    ~LayerProcessor();



    /*
        Creator
    */
    static LayerProcessor* create
    (
        Limb*,          /* Limb object */
        string = ""
    );



    /*
        Destructor
    */
    void destroy();



    /**********************************************************************
        Specific methods
    */

    /*
        Calc layer in the thread
    */
    LayerProcessor* calc
    (
        Data,
        ThreadManager*
    );



    /*
        Calculate values neurons
    */
    LayerProcessor* calcValues();



    /*
        Calculate erros neurons
    */
    LayerProcessor* calcErrors();




    /**********************************************************************
        Works with parent an chilrens counters
    */



    inline bool setCalcState
    (
        CalcState a
    )
    {
        if
        (
            ( calcState == WAIT && a != WAIT )
            || ( calcState == CALCULATING && a == CALCULATED )
            || ( calcState == CALCULATED && a == WAIT )
        )
        {
            calcState = a;
            return true;
        }

        getLog()
        -> warning( "Strange state chage" )
        -> prm( "Layer", getId());

        return false;
    }



    inline CalcState getCalcState()
    {
        return calcState;
    }



    /**************************************************************************
        Parents and children control
    */

    /*
        Reset parents and children lists for this layer
    */
    inline LayerProcessor* clearParentsAndChildren()
    {
        parents = {};
        children = {};
        return this;
    }



    inline LayerProcessor* addChild
    (
        Nerve* a
    )
    {
        children.push_back( a );
        return this;
    }



    inline LayerProcessor* addParent
    (
        Nerve* a
    )
    {
        parents.push_back( a );
        return this;
    }



    /*
        Return true if all parents of the layer has a calculated
    */
    inline bool isParentsCalculated()
    {
        for( Nerve* nerve:parents )
        {
            auto parent = (LayerProcessor* )(nerve -> getParent());
            if( parent != this && parent -> getCalcState() == CALCULATED )
            {
                return false;
            }
        }
        return true;
    }



    /*
        Return true if all parents of the layer has a calculated
    */
    inline bool isChildrenCalculated()
    {
        for( Nerve* nerve:children )
        {
            auto child = (LayerProcessor* )(nerve -> getChild());
            if( child != this && child -> getCalcState() != CALCULATED )
            {
                return false;
            }
        }
        return true;
    }



    inline LayerProcessor* backwardCalcComplete
    (
        ThreadManager* threadManager
    )
    {
//        /* Set status */
//        if( this -> setCalcState( CALCULATED ))
//        {
//            for( Nerve* nerve:parents )
//            {
//                auto parent = (LayerProcessor* )(nerve -> getParent());
//                if( parent != this && parent -> isChildrenCalculated())
//                {
//                    parent -> calc( DATA_ERRORS );
//                }
//            }
//        }
        return this;
    }



    inline LayerProcessor* forwardCalcComplete
    (
        ThreadManager* threadManager
    )
    {
        /* Set status */
        if( this -> setCalcState( CALCULATED ))
        {
            for( Nerve* nerve:children )
            {
                auto child = (LayerProcessor* )(nerve -> getChild());
                if( child != this && child -> isParentsCalculated())
                {
                    threadManager
                    -> add( child -> getId() )
                    -> run
                    (
                        []
                        ( void* item )
                        {
                            auto child = (LayerProcessor*) item;
                            // child -> calc( DATA_VALUES );
                        }
                    );
                }
            }
        }
        return this;
    }



    /*
        Loop for each parents neuron of this neuron
    */
    template <typename Func> LayerProcessor* parentsLoop
    (
        /* Child neuron index */
        int             aChildIndex,
        BindType        aBindType,
        Func            aCallback
    )
    {
        for( Nerve* nerve:parents )
        {
            auto parent = (LayerProcessor* )(nerve -> getParent());
            if
            (
                aBindType == BT_ALL ||
                nerve -> getBindType() == aBindType
            )
            {
                auto parentsCount = parent -> getCount();
                /* Loop by weights */
                for
                (
                    int parentIndex = 0;
                    parentIndex < parentsCount;
                    parentIndex++
                )
                {
                    auto weightIndex =
                    nerve -> getWeightIndex( parentIndex, aChildIndex );
                    if( weightIndex >= 0 )
                    {
                        aCallback
                        (
                            parent,
                            parentIndex,
                            nerve,
                            nerve -> getWeight( weightIndex ),
                            weightIndex
                        );
                    }
                }
            }
        }
        return this;
    }



    /*
        Loop for each children neurons of this neuron
    */
    template <typename Func> LayerProcessor* childrenLoop
    (
        /* Child neuron index */
        int             aParentIndex,
        BindType        aBindType,
        Func            aCallback
    )
    {
        for( Nerve* nerve:children )
        {
            auto child = (LayerProcessor* )(nerve -> getChild());
            if
            (
                aBindType == BT_ALL ||
                nerve -> getBindType() == aBindType
            )
            {
                auto childrenCount = child -> getCount();
                /* Loop by weights */
                for
                (
                    int childIndex = 0;
                    childIndex < childrenCount;
                    childIndex++
                )
                {
                    auto weightIndex =
                    nerve -> getWeightIndex( childIndex, aParentIndex );
                    if( weightIndex >= 0 )
                    {
                        aCallback
                        (
                            child,
                            childIndex,
                            nerve,
                            nerve -> getWeight( weightIndex ),
                            weightIndex
                        );
                    }
                }
            }
        }
        return this;
    }
};
