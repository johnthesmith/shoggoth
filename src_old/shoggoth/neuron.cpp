#include "neuron.h"
#include "layer.h"



using namespace std;



/*
    Constructor
*/
Neuron::Neuron
(
    Layer* aLayer,
    int aIndex
)
{
    layer = aLayer;
    index = aIndex;
}




/*
    Create the Layer list object
*/
Neuron* Neuron::create
(
    Layer* aLayer,
    int aIndex
)
{
    return new Neuron( aLayer, aIndex );
}



/*
    Destroy the Layer list object
*/
void Neuron::destroy()
{
    delete this;
}



/*
    Return index
*/
int Neuron::getIndex()
{
    return index;
}



/*
    Return Layer
*/
Layer* Neuron::getLayer()
{
    return layer;
}



/*
    Return neuron screen position
*/
Point3d Neuron::getScreen()
{
    return layer -> getNeuronScreen( index );
}




/*
    Return neuron world position
*/
Point3d Neuron::getWorld()
{
    return layer -> getNeuronWorld( index );
}




/*
    Set neuron value
*/
Neuron* Neuron::setValue
(
    double aValue
)
{
    layer -> setNeuronValue( index, aValue );
    return this;
}



/*
    Return neuron value
*/
double Neuron::getValue()
{
    return layer -> getNeuronValue( index );
}



/*
    Set neuron error
*/
Neuron* Neuron::setError
(
    double aValue
)
{
    layer -> setNeuronError( index, aValue );
    return this;
}



/*
    Return neuron error
*/
double Neuron::getError()
{
    return layer -> getNeuronError( index );
}



/*
    Set neuron world position
*/
Neuron* Neuron::setWorld
(
    Point3d& aValue
)
{
    layer -> setNeuronWorld( index, aValue );
    return this;
}



/*
    Set neuron screen position
*/
Neuron* Neuron::setScreen
(
    Point3d& aValue
)
{
    layer -> setNeuronScreen( index, aValue );
    return this;
}
