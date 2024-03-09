#include <iostream>
#include <cstring>

#include "layer_list.h"
#include "limb.h"



using namespace std;


/*
    Constructor
*/
LayerList::LayerList
(
    Limb* aLimb
)
{
    limb = aLimb;
}



/*
    Create the Layer list object
*/
LayerList* LayerList::create
(
    Limb* aLimb
)
{
    return new LayerList( aLimb );
}



/*
    Destroy the Layer list object
*/
void LayerList::destroy()
{
    delete this;
}



/*
    Return log
*/
Log* LayerList::getLog()
{
    return limb -> getLog();
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
    Add Layer from argument in to this list
*/
LayerList* LayerList::push
(
    Layer* a
)
{
    Heap::push((void*) a );
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
    Clear all destroy all layers
*/
LayerList* LayerList::clear()
{
    int c = getCount();
    for( int i = 0; i < c; i++ )
    {
        Layer* layer = ( Layer* ) getByIndex( i );
        layer -> destroy();
    }
    resize( 0 );
    return this;
}




/*
    Compare layer structure
*/
bool LayerList::compare
(
    LayerList* a
)
{
    bool result = getCount() == a -> getCount();
    if( result )
    {
        loop
        (
            [ &a, &result ]
            ( void* p )
            {
                auto iLayer = (Layer*) p;
                auto jLayer = a -> getById( iLayer -> getId() );
                result = iLayer -> compare( jLayer );
                return !result;
            }
        );
    }
    return result;
}



/*
    Copy values of equal layers
*/
LayerList* LayerList::copyValuesFrom
(
    LayerList* aSource
)
{
    aSource -> loop
    (
        [ this ]
        ( void* p )
        {
            auto fromLayer = (Layer*) p;
            auto toLayer = getById( fromLayer -> getId() );
            if( toLayer != NULL )
            {
                toLayer -> copyValuesFrom( fromLayer );
            }
            return false;
        }
    );
    return this;
}



/*
    Copy errors of equal layers
*/
LayerList* LayerList::copyErrorsFrom
(
    LayerList* aSource
)
{
    aSource -> loop
    (
        [ this ]
        ( void* p )
        {
            auto fromLayer = (Layer*) p;
            auto toLayer = getById( fromLayer -> getId() );
            if( toLayer != NULL )
            {
                toLayer -> copyErrorsFrom( fromLayer );
            }
            return false;
        }
    );
    return this;
}




/*
    Dump limb information to log
*/
LayerList* LayerList::dump
(
    string aComment
)
{
    getLog() -> begin( "Layer list dump" ) -> prm( "coment", aComment );
    loop
    (
        [ this ]
        ( void* p )
        {
            auto iLayer = (Layer*) p;
            getLog()
            -> trace()
            -> prm( "id", iLayer -> getId() )
            -> prm( "address", (void*) iLayer );
            return false;
        }
    );
    getLog() -> end();
    return this;
}
