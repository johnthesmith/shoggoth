#include "nerve_list.h"



/*
    Create new nerve list
*/
NerveList* NerveList::create()
{
    auto result = new NerveList();
    return result;
}



/*
    Self destroy nerve list
*/
void NerveList::destroy()
{
    delete this;
}



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

    Heap::remove
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



/*
    Find and return list of the nerves
    by parent and child layers
*/
NerveList* NerveList::selectByLayers
(
    Layer* aParent,
    Layer* aChild,
    NerveList* aFoundedNerves
)
{
    loop
    (
        [ &aFoundedNerves, &aParent, &aChild ]
        (
            void* aItem
        )
        {
            auto nerve = ( Nerve* ) aItem;
            if
            (
                nerve -> getParent() == aParent &&
                nerve -> getChild() == aChild
            )
            {
                aFoundedNerves -> push( nerve );
            }
            return false;
        }
    );
    return this;
}



/*
    Clear all destroy all layers
*/
NerveList* NerveList::clear()
{
    int c = getCount();
    for( int i = 0; i < c; i++ )
    {
        Nerve* nerve = ( Nerve* ) getByIndex( i );
        nerve -> destroy();
    }
    resize( 0 );
    return this;
}



/*
    Compare layer structure
*/
bool NerveList::compare
(
    NerveList* a
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
                result = a -> getByNerve( (Nerve*) p ) != NULL;
                return !result;
            }
        );
    }
    return result;
}




/*
    Return nerve by nerve
*/
Nerve* NerveList::getByNerve
(
    Nerve* aNerve
)
{
    Nerve* result = NULL;
    loop
    (
        [ &aNerve, &result ]
        ( void* p )
        {
            auto iNerve = (Nerve*) p;
            result
            = aNerve -> getParent() -> getId() == iNerve -> getParent() -> getId()
            && aNerve -> getChild() -> getId() == iNerve -> getChild() -> getId()
            && aNerve -> getNerveType() == iNerve -> getNerveType()
            && aNerve -> getBindType() == iNerve -> getBindType() ? iNerve : NULL;
            return result != NULL;
        }
    );
    return result;
}



/*
    Copy list of nerves in to this from argument
*/
NerveList* NerveList::copyStructureFrom
(
    NerveList*  /* Source */
)
{
//...    TODO
    return this;
}
