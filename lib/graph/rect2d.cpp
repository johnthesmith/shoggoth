#include "rect2d.h"



Rect2d& Rect2d::setCenterSize
(
    const Point2d& aCenter,
    const Point2d& aSize
)
{
    center = aCenter;
    size = aSize;
    return *this;
}



Rect2d& Rect2d::setCenterSize
(
    const Point3d& aCenter,
    const Point2d& aSize
)
{
    center.x = aCenter.x;
    center.y = aCenter.y;
    size = aSize;
    return *this;
}



/*
    y
    ^
    |
    |    topLeft
    |    *-------------o
    |    |             |
    |    |      *      |
    |    |      center |
    |    o-------------*
    |                bottomRight
    |
    *-----------------------------> x
*/
Rect2d& Rect2d::setCorners
(
    Point2d& aTopLeft,
    Point2d& aBottomRight
)
{
    size.set
    (
        ( aBottomRight.x - aTopLeft.x ) * 0.5,
        ( aTopLeft.y - aBottomRight.y ) * 0.5
    );

    center.set
    (
        aTopLeft.x + size.x,
        aTopLeft.y - size.y
    );

    return *this;
}




/*
    Calc corner points at CW

    y
    ^
    |
    |    left top      right top
    |    *-------------*
    |    |             |
    |    |      *      |
    |    |      center |
    |    *-------------*
    |    left bottom   right bottom
    |
    *-----------------------------> x
*/

const Rect2d& Rect2d::calcCorners
(
    Point2d& lb,    /* Left bottom */
    Point2d& rb,    /* Right bottom */
    Point2d& rt,    /* Right top */
    Point2d& lt     /* Left top */
) const
{
    lb = center - size;
    rt = center + size;
    lt.x = lb.x;
    lt.y = rt.y;
    rb.x = rt.x;
    rb.y = lb.y;

    return *this;
}
