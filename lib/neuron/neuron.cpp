#include "neuron.h"


using namespace std;

/*
            ---->----
       [w] /         \
  [p]( P )[c]    [p]( C )[c]
        \        /
         ----<---
*/



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
    NeuronList* aList,  /* Child neuron list */
    double      aWeight /* Weight */
)
{
    /* Add childrens */
    children.add( aList );
    /* Add weights */
//    weights.resize( weights.size() + aList.getCount(), aWeight );
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
    parents.add( &aList );
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




/******************************************************************************
    Setters and getters
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



Neuron& Neuron::setValue
(
    double a
)
{
    value = a;
    return *this;
}



double Neuron::getValue()
{
    return value;
}



