#include "point3i.h"


Point3i::Point3i
(
    int aX,
    int aY,
    int aZ
)
{
    x = aX;
    y = aY;
    z = aZ;
}



/*
    Minus Operator r = a - b
*/
Point3i operator-
(
    const Point3i& a, /* First operand */
    const int b       /* Second operand */
)
{
    return Point3i
    (
        a.x - b,
        a.y - b,
        a.z - b
    );
}



/*
    Minus Operator r = a - b
*/
Point3i operator-
(
    const Point3i& a,    /* First operand */
    const Point3i& b     /* Second operand */
)
{
    return Point3i
    (
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    );
}



