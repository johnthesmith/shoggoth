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
