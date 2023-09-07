#include <iostream>
#include <cstring>

#include "layer_list.h"



using namespace std;




/*
    Create the Layer list object
*/
LayerList* LayerList::create()
{
    return new LayerList();
}



/*
    Destroy the Layer list object
*/
void LayerList::destroy()
{
    delete this;
}



/*
    Add Layers from argument list to this list
*/
LayerList* LayerList::add
(
    LayerList* a
)
{
    Heap::add(( Heap* ) a );

    return this;
}



/*
    Return index by Layer
*/
int LayerList::indexBy
(
    Layer* a
)
{
    return Heap::indexBy(( void* ) a );
}




/*
    Return index by Layer
*/
Layer* LayerList::getByIndex
(
    int a
)
{
    return ( Layer* )Heap::getByIndex( a );
}




/*
    Set value by index
*/
LayerList* LayerList::setByIndex
(
    int aIndex,
    Layer* aLayer
)
{
    Heap::setByIndex(aIndex, ( void* ) aLayer );
    return this;
}



/*
    Resize
*/
LayerList* LayerList::resize
(
    int a
)
{
    Heap::resize( a );
    return this;
}




/*
    Return layer index by id
*/
int LayerList::getIndexById
(
    string  a
)
{
    int result = -1;
    int c = getCount();
    for( int i = 0; i < c && result == -1; i++ )
    {
        if( getByIndex( i ) -> getId() == a )
        {
            result = i;
        }
    }
    return result;
}



/*
    Return layer by id
*/
Layer* LayerList::getById
(
    string  a
)
{
    auto index = getIndexById( a );
    return index > -1 ? getByIndex( index ) : NULL;
}



/*
    Read from server
*/
LayerList* LayerList::read()
{
    loop
    (
        []
        ( void* aLayer )
        {
            (( Layer* ) aLayer ) -> read();
            return false;
        }
    );
    return this;
}



/*
    Write to server
*/
LayerList*  LayerList::write()
{
    loop
    (
        []
        ( void* aLayer )
        {
            (( Layer* ) aLayer ) -> write();
            return false;
        }
    );
    return this;
}
