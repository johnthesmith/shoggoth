#include <iostream>
#include <cstring>
#include "heap.h"

using namespace std;

/*
    [Heap] ---> [ void*; void* ; void* ; .... ]
*/


/*
    Return count elements in heap
*/
int Heap::getCount()
{
    return count;
}



/*
    Add heap from argument to this heap
*/
Heap* Heap::add
(
    Heap* a
)
{
    auto currentSize = count;
    auto sourceSize = a -> count;
    auto newSize = currentSize + sourceSize;

    resize( newSize );

    memcpy
    (
        &items[ currentSize ],
        a -> items,
        sourceSize * sizeof( void* )
    );

    return this;
}




/*
    Return index by neuron
*/
int Heap::indexBy
(
    void* a
)
{
    intptr_t first = ( intptr_t ) items;
    intptr_t find = ( intptr_t ) a;
    intptr_t delta = find - first;
    int result = ( find - first ) / sizeof( void* );

    return result >= 0 && result < count ? result : -1;
}




/*
    Return index by neuron
*/
void* Heap::getByIndex
(
    int a
)
{
    if( a >= 0 && a < count )
    {
        return items[ a ];
    }
    else
    {
        return NULL;
    }
}




/*
    Set value by index
*/
Heap* Heap::setByIndex
(
    int aIndex,
    void* aNeuron
)
{
    if( aIndex >= 0 && aIndex < count )
    {
        items[ aIndex ] = aNeuron;
    }
    return this;
}



/*
    Resize
*/
Heap* Heap::resize
(
    int a
)
{
    /* Get current size */
    auto currentSize = count;

    if( a != currentSize )
    {
        /* Create new element */
        void** newItems = a == 0 ? NULL : new void*[ a ];

        if( items != NULL && newItems != NULL )
        {
            /* Copy */
            memcpy
            (
                newItems,
                items,
                min( currentSize, a ) * sizeof( void* )
            );
        }

        /* Delete old items */
        if( items != NULL )
        {
            delete [] items;
        }

        /* Set items to new items */
        items = newItems;
        /* Sct count */
        count = a;
    }
    return this;
}




/*
    Resize
*/
Heap* Heap::expand
(
    int a
)
{
    resize( count + a );
    return this;
}
