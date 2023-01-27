#include "layer.h"




Layer::Layer
(
    Log& aLog
):
    log( aLog )
{
    log.trace( "Create layer" );
}


Layer::~Layer()
{
    log.trace( "Layer destroy" );
}


/*
    Add new neuron and return it
*/
Layer& Layer::addNeuron
(
    Neuron& aNeuron
)
{
    neurons.add( aNeuron );
    return *this;
}





int Layer::indexByPos
(
    const Point3i& a
)
{
    return a.x + a.y * size.x + a.z * size.x * size.y;
}



/*
    Set dimations size
*/
Layer& Layer::setSize
(
    const Point3i& a
)
{
    size = a;
    return *this;
}




