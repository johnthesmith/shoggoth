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
    Return index by layer and index at him
*/
int NeuronList::indexBy
(
    Layer* aLayer, /* Layer object */
    int    aIndex /* Index in the layer */
)
{
    bool stop = false;
    auto c = getCount();

    auto result = -1;

    for( int i = 0; i < c && !stop; i++ )
    {
        auto neuron = getByIndex( i );
        if
        (
            neuron -> getLayer() == aLayer &&
            neuron -> getIndex() == aIndex
        )
        {
            result = i;
            stop = true;
        }
    }
    return result;
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



/*
    Calculate average value
*/
double NeuronList::calcAvgValue()
{
    double sum = 0;
    loop
    (
        [ &sum ]
        ( Neuron* aNeuron )
        {
            sum += aNeuron -> getValue();
            return false;
        }
    );
    return sum / getCount();
}



/*
    Calculate average error
*/
double NeuronList::calcAvgError()
{
    double sum = 0;
    loop
    (
        [ &sum ]
        ( Neuron* aNeuron )
        {
            sum += aNeuron -> getError();
            return false;
        }
    );
    return sum / getCount();
}



/*
    Mearge two heaps
    Each elemento of Argument heap will add to This heap
    if it not exists in This
*/
NeuronList* NeuronList::merge
(
    NeuronList* a
)
{
    Heap::merge
    (
        ( Heap* ) a,
        [ this ]
        ( void* aItem )
        {
            auto item = ( Neuron* ) aItem;
            return indexBy
            (
                item -> getLayer(),
                item -> getIndex()
            ) < 0;
        }
    );
    return this;
}



/*
    From this remove all Neurons in Argument
*/
NeuronList* NeuronList::remove
(
    NeuronList* a
)
{
    Heap::remove
    (
        [ this, &a ]
        ( void* aItem )
        {
            auto item = ( Neuron* ) aItem;
            return a -> indexBy
            (
                item -> getLayer(),
                item -> getIndex()
            ) >= 0;
        }
    );
    return this;
}
