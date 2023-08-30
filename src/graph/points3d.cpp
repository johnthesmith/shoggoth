#include <iostream>
#include <cstring>



/*
    Graphics libraries
*/
#include "points3d.h"

using namespace std;



/*
    Return count elements in Points3d
*/
int Points3d::getCount()
{
    return count;
}



/*
    Add Points3d from argument to this Points3d
*/
Points3d* Points3d::add
(
    Points3d* a
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
        sourceSize * sizeof( Point3d )
    );

    return this;
}




/*
    Return index by Point3d*
*/
int Points3d::indexBy
(
    Point3d* a
)
{
    intptr_t first = ( intptr_t ) items;
    intptr_t find = ( intptr_t ) a;
    intptr_t delta = find - first;
    int result = ( find - first ) / sizeof( Point3d );

    return result >= 0 && result < count ? result : -1;
}




/*
    Return point by index
*/
Point3d* Points3d::getByIndex
(
    int a
)
{
    if( a >= 0 && a < count )
    {
        return &items[ a ];
    }
    else
    {
        return NULL;
    }
}



/*
    Set value by index
*/
Points3d* Points3d::setByIndex
(
    int aIndex,
    Point3d& aPoint
)
{
    if( aIndex >= 0 && aIndex < count )
    {
        items[ aIndex ] = aPoint;
    }
    return this;
}



/*
    Resize
*/
Points3d* Points3d::resize
(
    int a
)
{
    /* Get current size */
    auto currentSize = count;

    if( a != currentSize )
    {
        /* Create new element */
        Point3d* newItems = a == 0 ? NULL : new Point3d[ a ];

        if( items != NULL && newItems != NULL )
        {
            /* Copy */
            memcpy
            (
                newItems,           /* Destination */
                items,              /* Source */
                min( currentSize, a ) * sizeof( Point3d )
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



///*
//    Calulate bounder box
//*/
//Box3d& Points3d::calcBox()
//{
//    Point3d pointMin = POINT_3D_MAX_INF;
//    Point3d pointMax = POINT_3D_MIN_INF;
//
//    auto box = Box3d();
//    for( int i = 0; i < count; i++ )
//    {
//        auto p = getByIndex( i );
///*
//        pointMin.min( p );
//        pointMax.max( p );
//*/
//    }
//    return box;
//}
