/*
    Camera

    Class has:
    - eye - point of view
    - target - point for gaze direction
    - top - normalized vector for top directio

    Each point and vector can blocked
*/

#include "camera.h"



/*
    Directive set eye position
*/
Camera& Camera::setEye
(
    const Point3& a
)
{
    eye = a;
    return *this;
}



/*
    Directive set eye position
*/
Camera& Camera::moveEye
(
    const Point3& a
)
{
    if( !eyeLock )
    {
        eye = a;
        norm();
    }
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
    const Point3& a
)
{
    top = a;
    return *this;
}



/*
    Move top vector with normalize
*/
Camera& Camera::moveTop
(
    const Point3& a
)
{
    if( !topLock )
    {
        top = a;
        norm();
    }
    return *this;
}



/*
    Set eye position
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
    const Point3& a
)
{
    target = a;
    return *this;
}



/*
    Move camera target with normalize
*/
Camera& Camera::moveTarget
(
    const Point3& a
)
{
    if( !targetLock )
    {
        target = a;
        norm();
    }
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
Camera& Camera::norm()
{
    auto gaze = ( eye - target ).norm(); /* Camera gase direction */
    auto left = gaze % top;
    top = gaze % left;
    return *this;
}



/*
    Set top normal vector
*/
Camera& Camera::place
(
    const Point3& aEye,
    const Point3& aTarget,
    const Point3& aTop
)
{
    if( !eyeLock )
    {
        eye = aEye;
    }

    if( !targetLock )
    {
        target = aTarget;
    }

    if( !topLock )
    {
        top = aTop;
    }

    norm();
    return *this;
}



/*
    Shift camera at 3d
*/
Camera& Camera::shift
(
    const Point3& a
)
{
    place
    (
        eye + a,
        target + a,
        top
    );
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

