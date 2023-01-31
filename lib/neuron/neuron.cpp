#include "neuron.h"



using namespace std;

/*
            ---->----
       [w] /         \
  [p]( P )[c]    [p]( C )[c]
        \        /
         ----<---
*/


Neuron& Neuron::setLayer
(
    Layer* a
)
{
    layer = a;
    return *this;
}



Layer& Neuron::getLayer()
{
    return *layer;
}



/*
    this -> parent
*/
Neuron& Neuron::addParent
(
    Neuron& aParent,    /* Parent neuron */
    double weight       /* Weight */
)
{
    /* Set bind from this to parent */
//    if( parents.indexByNeuron( aParent ) < 0 )
    {
        parents.add( aParent );
    }

    /* Set bind from parent to this */
//    if( aParent.children.indexByNeuron( *this ) < 0 )
    {
        aParent.children.add( *this );
        aParent.weights.push_back( weight );
    }

    return *this;
}



/*
    child -> this
*/
Neuron& Neuron::addChild
(
    Neuron& aChildren,  /* Child neuron */
    double weight       /* Weight */
)
{
//    if( children.indexByNeuron( aChildren ) < 0 )
    {
        children.add( aChildren );
        weights.push_back( weight );
    }

//    if( aChildren.parents.indexByNeuron( *this ) < 0)
    {
        aChildren.parents.add( *this );
    }

    return *this;
}



/*
    Return null neuron
*/
bool Neuron::isNull()
{
    return false;
}



/*
    Add all neurons from NeuronList to Children of this neuron
*/
Neuron& Neuron::addChildren
(
    NeuronList& aList,  /* Child neuron list */
    double      aWeight /* Weight */
)
{
    /* Add childrens */
    children.add( aList );
    /* Add weights */
    weights.resize( weights.size() + aList.getSize(), aWeight );
    return *this;
}




/*
    Add all neurons from NeuronList to Partnts of this neuron
*/
Neuron& Neuron::addParents
(
    NeuronList& aList      /* Parents neuron list */
)
{
    /* Add parents */
    parents.add( aList );
    return *this;
}






/******************************************************************************
    Virtuals
*/



/*
    Return Name of neuron
*/
string Neuron::getName()
{
    return NULL;
}



/*
    Draw neuron
*/
Neuron& Neuron::draw()
{
    return *this;
}
