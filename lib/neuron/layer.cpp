#include "layer.h"
#include "neuron.h"




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
    setSize();
    log.trace( "Layer destroy" );
}



 /*
     Return the log object
 */
Log& Layer::getLog()
{
    return log;
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



Neuron* Layer::newNeuron()
{
    Neuron* result = new Neuron();
    result -> setLayer( this );
    return result;
}



/*
    Set dimations size
*/
Layer& Layer::setSize
(
    const Point3i& a
)
{
    auto currentSize = neurons.getSize();
    auto newSize = a.x * a.y * a.z;

    getLog()
    .begin( "Layer resize" )
    .prm( "from", currentSize )
    .prm( "to", newSize )
    .lineEnd();

    if( newSize > currentSize )
    {
        /* Size increase */
        neurons.resize( newSize );

        for( int i = currentSize; i < newSize; i++ )
        {
            neurons.setByIndex( i, newNeuron() );
        }
    }
    else
    {
        if( newSize < currentSize )
        {
            /* Size decrease */
            for( int i = newSize; i < currentSize; i++ )
            {
                auto n = neurons.getByIndex( i );
                if( !n -> isNull())
                {
                    delete n;
                }
            }

            neurons.resize( newSize );
        }
    }

    size = a;

    getLog().end();

    return *this;
}



/*
    Connect all neurons from current layer to argument
    (i) -> (j)
*/
Layer& Layer::connectTo
(
    Layer& a
)
{
    auto si = neurons.getSize();
    auto sj = a.neurons.getSize();

    for( int i = 0; i < si; i++ )
    {
        auto iNeuron = neurons.getByIndex( i );

        for( int j = 0; j < sj; i++ )
        {
            auto jNeuron = neurons.getByIndex( i );
            iNeuron -> addParent( *jNeuron );
        }
    }
    return *this;
}
