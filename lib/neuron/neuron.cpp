#include "neuron.h"



using namespace std;



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
    if( parents.indexByNeuron( aParent ) < 0 )
    {
        parents.add( aParent );
    }

    /* Set bind from parent to this */
    if( aParent.children.indexByNeuron( *this ) < 0 )
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
    if( children.indexByNeuron( aChildren ) < 0 )
    {
        children.add( aChildren );
        weights.push_back( weight );
    }

    if( aChildren.parents.indexByNeuron( *this ) < 0)
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



