#include "box3d.h"



/*
    Constructor
*/
Box3d::Box3d
(
    const Point3d& aCenter,
    const Point3d& aRadius
)
{
    center = aCenter;
    radius = aRadius;
}




/*
    Define box with center and radius
*/
Box3d& Box3d::setCenterRadius
(
    const Point3d& aCenter,
    const Point3d& aRadius
)
{
    center = aCenter;
    radius = aRadius;
    return *this;
}




/*
    y
    ^         o-------------*
    |        /             /|
    |      aTopLeftFront  / |
    |      *-------------o  |
    |      |       *     |  *aBottomRightBack
    |      |       center| /
    |      |             |/
    |  ^z  o-------------o
    | /
    |/
    *-----------------------> x


*/
Box3d& Box3d::setCorners
(
    Point3d& aTopLeftFront,
    Point3d& aBottomRightBack
)
{
    radius.set
    (
        ( aBottomRightBack.x - aTopLeftFront.x ) * 0.5,
        ( aTopLeftFront.y - aBottomRightBack.y ) * 0.5,
        ( aTopLeftFront.z - aBottomRightBack.z ) * 0.5
    );

    center.set
    (
        aTopLeftFront.x + radius.x,
        aTopLeftFront.y - radius.y,
        aTopLeftFront.z + radius.z
    );

    return *this;
}




/*
    Calc corner points at CW
*/

const Box3d& Box3d::calcCorners
(
    Point3d& lb,    /* Left bottom */
    Point3d& rb,    /* Right bottom */
    Point3d& rt,    /* Right top */
    Point3d& lt     /* Left top */
) const
{
    lb = center - radius;
    rt = center + radius;
    lt.x = lb.x;
    lt.y = rt.y;
    rb.x = rt.x;
    rb.y = lb.y;

    return *this;
}
