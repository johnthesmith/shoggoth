#include "neuron_list.h"
#include "neuron_null.h"


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
Neuron& NeuronList::neuronByIndex
(
    int a
)
{
    Neuron* result = &NEURON_NULL;
    int c = items.size();
    if( a > 0 && a < c )
    {
        result = items[ a ];
    }
    return *result;
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
