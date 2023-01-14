#include "camera.h"



/*
    Directive set eye position
*/
Camera& Camera::setEye
(
    const Point3* a
)
{
    return *this;
}



/*
    Return eye position
*/
Point3& Camera::getEye()
{
    return eye;
}



/*
    Set top normal vector
*/
Camera& Camera::setTop
(
    const Point3* a
)
{
    return *this;
}



/*
    Seteye position
*/
Point3& Camera::getTop()
{
    return top;
}



/*
    Set eye position
*/
Camera& Camera::setTarget
(
    const Point3* a
)
{
    return *this;
}



/*
    Seteye position
*/
Point3& Camera::getTarget()
{
    return target;
}



/*
    Shift camera at 3d
*/
Camera& Camera::shift
(
    const Point3* a
)
{
//    eye.add( a );
//    target.add( a );
    return *this;
}



/*
    Shift camera at screen
*/
Camera& Camera::shift
(
    const double ax,
    const double ay
)
{
    return *this;
}
