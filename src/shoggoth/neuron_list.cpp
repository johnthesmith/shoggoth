#include <iostream>
#include <cstring>

#include "neuron_list.h"
#include "neuron.h"



using namespace std;



/*
    Create and return list of neurons
*/
NeuronList* NeuronList::create()
{
    return new NeuronList();
}




/*
    Add neurons from argument list to this list
*/
NeuronList* NeuronList::add
(
    NeuronList* a
)
{
    Heap::add(( Heap* ) a );

    return this;
}




/*
    Return index by neuron
*/
int NeuronList::indexBy
(
    Neuron* a
)
{
    return Heap::indexBy(( void* ) a );
}




/*
    Return index by neuron
*/
Neuron* NeuronList::getByIndex
(
    int a
)
{
    return ( Neuron* )Heap::getByIndex( a );
}




/*
    Set value by index
*/
NeuronList* NeuronList::setByIndex
(
    int aIndex,
    Neuron* aNeuron
)
{
    Heap::setByIndex(aIndex, ( void* ) aNeuron );
    return this;
}



/*
    Resize
*/
NeuronList* NeuronList::resize
(
    int a
)
{
    Heap::resize( a );
    return this;
}



/*
    Loop with lyambda
*/
NeuronList* NeuronList::loop
(
    function <bool ( Neuron* )> callback,
    int aFrom,
    int aTo
)
{
    bool stop = false;
    auto c = aTo == 0 ? getCount() : aTo;
    auto items = getItems();

    for( int i = aFrom; i < c && !stop; i++ )
    {
        stop = callback(( Neuron* ) items[ i ] );
    }
    return this;
}
