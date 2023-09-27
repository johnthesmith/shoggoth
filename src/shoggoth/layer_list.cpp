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
LayerList* LayerList::readValues
(
    ParamList* aTasks
)
{
    loop
    (
        [ &aTasks ]
        ( void* aLayer )
        {
            auto iLayer = ( Layer* ) aLayer;
            if
            (
                aTasks == NULL ||
                aTasks
                -> isIntersect
                (
                    iLayer
                    -> getActions()
                    -> getObject( actionToString( READ_VALUES ))
                )
            )
            {
                iLayer -> readValues();
            }
            return false;
        }
    );
    return this;
}



/*
    Write to server
*/
LayerList*  LayerList::writeValues
(
    ParamList* aTasks
)
{
    loop
    (
        [ &aTasks ]
        ( void* aLayer )
        {
            auto iLayer = ( Layer* ) aLayer;

            if
            (
                aTasks == NULL ||
                aTasks -> isIntersect
                (
                    iLayer
                    -> getActions()
                    -> getObject( actionToString( WRITE_VALUES ))
                )
            )
            {
                iLayer -> writeValues();
            }
            return false;
        }
    );
    return this;
}




/*
    Read from server
*/
LayerList* LayerList::readErrors
(
    ParamList* aTasks
)
{
    loop
    (
        [ &aTasks ]
        ( void* aLayer )
        {
            auto iLayer = ( Layer* ) aLayer;
            if
            (
                aTasks == NULL ||
                aTasks
                -> isIntersect
                (
                    iLayer
                    -> getActions()
                    -> getObject( actionToString( READ_ERRORS ))
                )
            )
            {
                iLayer -> readErrors();
            }
            return false;
        }
    );
    return this;
}



/*
    Write to server
*/
LayerList*  LayerList::writeErrors
(
    ParamList* aTasks
)
{
    loop
    (
        [ &aTasks ]
        ( void* aLayer )
        {
            auto iLayer = ( Layer* ) aLayer;
            if
            (
                aTasks == NULL ||
                aTasks
                -> isIntersect
                (
                    iLayer
                    -> getActions()
                    -> getObject( actionToString( WRITE_ERRORS ))
                )
            )
            {
                iLayer -> writeErrors();
            }
            return false;
        }
    );
    return this;
}

