#include "neuron_list.h"
#include "neuron_null.h"



int NeuronList::getSize()
{
    return items.size();
}



/*
    Add exists neuron
*/
NeuronList& NeuronList::add
(
    Neuron& a /* Parent neuron */
)
{
    items.push_back( &a );
    return *this;
}



/*
    Return index by neuron
*/
int NeuronList::indexByNeuron
(
    Neuron& a
)
{
    int r = -1;
    int c = items.size();
    for( int i = 0; i < c && r != -1; i++ )
    {
        if( items[ i ] == &a )
        {
            r = i;
        }
    }
    return 0;
}




/*
    Return index by neuron
*/
Neuron* NeuronList::getByIndex
(
    int a
)
{
    if( a > 0 && a < items.size() )
    {
        return items[ a ];
    }
    else
    {
        return &NEURON_NULL;
    }
}




/*
    Return index by neuron
*/
NeuronList& NeuronList::setByIndex
(
    int aIndex,
    Neuron* aNeuron
)
{
    if( aIndex > 0 && aIndex < items.size() )
    {
        items[ aIndex ] = aNeuron;
    }
    return *this;
}




/*
    Return neuron by name
*/
Neuron& NeuronList::neuronByName
(
    const string a
)
{
    Neuron* result = &NEURON_NULL;
    int c = items.size();
    for( int i = 0; i < c && result == &NEURON_NULL; i++ )
    {
        if( items[ i ] -> getName() == a )
        {
            result = items[ i ];
        }
    }
    return *result;
}



/*
    Resize
    Warning!!! this method can not call directly.
*/
NeuronList& NeuronList::resize
(
    int a
)
{
    items.resize( a, &NEURON_NULL );
    return *this;
}
