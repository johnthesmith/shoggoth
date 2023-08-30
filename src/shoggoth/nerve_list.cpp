#include "nerve_list.h"



/*
    Add Nerves from argument list to this list
*/
NerveList* NerveList::add
(
    NerveList* a
)
{
    Heap::add(( Heap* ) a );

    return this;
}



/*
    Return index by Nerve
*/
int NerveList::indexBy
(
    Nerve* a
)
{
    return Heap::indexBy(( void* ) a );
}




/*
    Return index by Nerve
*/
Nerve* NerveList::getByIndex
(
    int a
)
{
    return ( Nerve* )Heap::getByIndex( a );
}




/*
    Set value by index
*/
NerveList* NerveList::setByIndex
(
    int aIndex,
    Nerve* aNerve
)
{
    Heap::setByIndex(aIndex, ( void* ) aNerve );
    return this;
}



/*
    Resize
*/
NerveList* NerveList::resize
(
    int a
)
{
    Heap::resize( a );
    return this;
}




/*
    Return Nerve index by id
*/
int NerveList::getIndexById
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
    Return Nerve by id
*/
Nerve* NerveList::getById
(
    string  a
)
{
    auto index = getIndexById( a );
    return index > -1 ? getByIndex( index ) : NULL;
}



/*
    Remove by layer
*/
NerveList* NerveList::removeByLayer
(
    Layer* aLayer
)
{
    auto removed = NerveList::create();

    remove
    (
        [ &aLayer ]
        ( void* iNerve )
        {
            return
            ((Nerve*) iNerve) -> getParent() == aLayer ||
            ((Nerve*) iNerve) -> getChild() == aLayer;
        },
        removed
    );

    /* Destroy the nerves */
    removed -> loop
    (
        []
        ( void* iNerve )
        {
            ((Nerve*) iNerve) -> destroy();
            return false;
        }
    );

    removed -> destroy();
    return this;
}



/*
    Return parents layers by child layer from nerves
*/
NerveList* NerveList::getParentsByLayer
(
    Layer* aChild,      /* Layer for parents search */
    LayerList* aResult  /* Result list of parents */
)
{
    loop
    (
        [ &aChild, &aResult ]
        ( void* aNerve )
        {
            auto nerve = ( Nerve* ) aNerve;
            if( nerve -> getChild() == aChild )
            {
                aResult -> push( nerve -> getParent() );
            }
            return false;
        }
    );
    return this;
}



/*
    Return children layers by parent from nerves
*/
NerveList* NerveList::getChildrenByLayer
(
    Layer*      aParent,    /* Layer for parents search */
    LayerList*  aResult     /* Result list of parents */
)
{
    loop
    (
        [ &aParent, &aResult ]
        ( void* aNerve )
        {
            auto nerve = ( Nerve* ) aNerve;
            if( nerve -> getParent() == aParent )
            {
                aResult -> push( nerve -> getChild());
            }
            return false;
        }
    );
    return this;
}
