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
    mutex           stateMutex;


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



    /*
        Change state for layer
    */
    inline bool setCalcState
    (
        CalcState a
    )
    {
        /* Lock state for layer */
        unique_lock <mutex> lck( stateMutex );

        if
        (
            ( calcState == WAIT && a == WAIT )
            || ( calcState == WAIT && a != WAIT )
            || ( calcState == CALCULATING && a == CALCULATED )
            || ( calcState == CALCULATED && a == WAIT )
        )
        {
            calcState = a;
            return true;
        }
        else
        {
            return false;
        }
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
        Return children vector
    */
    inline vector <Nerve*> getChildrenVector()
    {
        return children;
    }



    inline bool isInput()
    {
        return parents.size() == 0;
    }



    inline bool isOutput()
    {
        return children.size() == 0;
    }



    /*
        Return true if all parents of the layer has a calculated
    */
    inline bool isParentsCalculated()
    {
        for( Nerve* nerve:parents )
        {
            auto parent = (LayerProcessor* )(nerve -> getParent());
            if( parent != this && parent -> getCalcState() != CALCULATED )
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



    /*
        Finish backward calculation and run
        all children calcualteion
    */
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
                struct DataStruct
                {
                    LayerProcessor* layer;
                    ThreadManager* manager;
                };

                auto child = (LayerProcessor* )( nerve -> getChild() );
                if
                (
                    child != this
                    && child -> isParentsCalculated()
                    && child -> setCalcState( CALCULATING )
                )
                {
                    auto data = DataStruct{ child, threadManager };
                    threadManager
                    -> add( child -> getId() )
                    -> run
                    (
                        []
                        ( void* data )
                        {
                            auto [ child, manager ] = *(DataStruct*) data;
                            child
                            -> calcValues()
                            -> forwardCalcComplete( manager );
                        },
                        &data,
                        sizeof( data )
                    );
                }
            }
        }
        return this;
    }



    /*
        Finish backward calculation and run
        all children calcualteion
    */
    inline LayerProcessor* backwardCalcComplete
    (
        ThreadManager* threadManager
    )
    {
        /* Set status */
        if( this -> setCalcState( CALCULATED ))
        {
            for( Nerve* nerve:parents )
            {
                struct DataStruct
                {
                    LayerProcessor* layer;
                    ThreadManager* manager;
                };

                auto parent = (LayerProcessor* )(nerve -> getParent());
                if
                (
                    parent != this
                    && parent -> isChildrenCalculated()
                    && parent -> setCalcState( CALCULATING )
                )
                {
                    auto data = DataStruct{ parent, threadManager };
                    threadManager
                    -> add( parent -> getId() )
                    -> run
                    (
                        []
                        ( void* data )
                        {
                            auto [ parent, manager ] = *(DataStruct*) data;
                            parent
                            -> calcErrors()
                            -> backwardCalcComplete( manager );
                        },
                        &data,
                        sizeof( data )
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
                    nerve -> getWeightIndex( aParentIndex, childIndex );
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



    /*
        Calculate neurons in the layer using threads
    */
    LayerProcessor* dump()
    {
        cout
        << getId() << ":"
        << calcState << ":"
        << calcSumValue() << ":"
        << calcRmsError() << "\n";
        return this;
    }

};
