#include <iostream> /* memcpy */
#include <cstring>

#include "../rnd.h"

#include "bind_list.h"



using namespace std;



/*
    Add BindList from argument to this BindList
*/
BindList* BindList::add
(
    BindList* a
)
{
    Heap::add( ( Heap* ) a );
    return this;
}



/*
    Return index by Bind*
*/
int BindList::indexBy
(
    Bind* a
)
{
    return Heap::indexBy( (void*) a );
}




/*
    Return index by neuron
*/
Bind* BindList::getByIndex
(
    int a
)
{
    return ( Bind* ) Heap::getByIndex( a );
}



/*
    Set value by index
*/
BindList* BindList::setByIndex
(
    int aIndex,
    Bind* aBind
)
{
    Heap::setByIndex( aIndex, (void*) aBind );
    return this;
}



/*
    Resize
*/
BindList* BindList::resize
(
    int a
)
{
    Heap::resize( a );
    return this;
}



/*
    Resize
*/
BindList* BindList::expand
(
    int a
)
{
    Heap::expand( a );
    return this;
}



/*
    Loop with lyambda
*/
BindList* BindList::loop
(
    function <bool ( Bind* )> callback
)
{
    bool stop = false;
    for( int i = 0; i < count && !stop; i++ )
    {
        stop = callback(( Bind*) items[ i ] );
    }
    return this;
}
