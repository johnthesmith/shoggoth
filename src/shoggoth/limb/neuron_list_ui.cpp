#include <iostream>
#include <cstring>

#include "neuron_list_ui.h"
#include "neuron_ui.h"



using namespace std;



/*
    Create and return list of neurons
*/
NeuronListUi* NeuronListUi::create()
{
    return new NeuronListUi();
}




/*
    Add neurons from argument list to this list
*/
NeuronListUi* NeuronListUi::add
(
    NeuronListUi* a
)
{
    Heap::add(( Heap* ) a );

    return this;
}




/*
    Return index by neuron
*/
int NeuronListUi::indexBy
(
    NeuronUi* a
)
{
    return Heap::indexBy(( void* ) a );
}



/*
    Return index by layer and index at him
*/
int NeuronListUi::indexBy
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
            ( Layer* ) ( neuron -> getLayer()) == aLayer &&
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
NeuronUi* NeuronListUi::getByIndex
(
    int a
)
{
    return ( NeuronUi* )Heap::getByIndex( a );
}




/*
    Set value by index
*/
NeuronListUi* NeuronListUi::setByIndex
(
    int aIndex,
    NeuronUi* aNeuron
)
{
    Heap::setByIndex(aIndex, ( void* ) aNeuron );
    return this;
}



/*
    Resize
*/
NeuronListUi* NeuronListUi::resize
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
NeuronListUi* NeuronListUi::loop
(
    function <bool ( NeuronUi* )> callback,
    int aFrom,
    int aTo
)
{
    bool stop = false;
    auto c = aTo == 0 ? getCount() : aTo;
    auto items = getItems();

    for( int i = aFrom; i < c && !stop; i++ )
    {
        stop = callback(( NeuronUi* ) items[ i ] );
    }
    return this;
}



/*
    Calculate average value
*/
double NeuronListUi::calcAvgValue()
{
    double sum = 0;
    loop
    (
        [ &sum ]
        ( NeuronUi* aNeuron )
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
double NeuronListUi::calcAvgError()
{
    double sum = 0;
    loop
    (
        [ &sum ]
        ( NeuronUi* aNeuron )
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
NeuronListUi* NeuronListUi::merge
(
    NeuronListUi* a
)
{
    Heap::merge
    (
        ( Heap* ) a,
        [ this ]
        ( void* aItem )
        {
            auto item = ( NeuronUi* ) aItem;
            return indexBy
            (
                ( Layer* ) ( item -> getLayer()),
                item -> getIndex()
            ) < 0;
        }
    );
    return this;
}



/*
    From this remove all Neurons in Argument
*/
NeuronListUi* NeuronListUi::remove
(
    NeuronListUi* a
)
{
    Heap::remove
    (
        [ this, &a ]
        ( void* aItem )
        {
            auto item = ( NeuronUi* ) aItem;
            return a -> indexBy
            (
                ( Layer* ) ( item -> getLayer()),
                item -> getIndex()
            ) >= 0;
        }
    );
    return this;
}
