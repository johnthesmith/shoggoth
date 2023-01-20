/*
    Camera

    Class has:
    - eye - point of view
    - target - point for gaze direction
    - top - normalized vector for top directio

    Each point and vector can blocked
*/

#include <iostream>

#include "camera.h"
#include "matrix.h"


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
        setEye( a );
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
        setTop( a );
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
        setTarget( a );
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
    TODO VectorUP and Gase does not equal
*/
Camera& Camera::norm()
{
    auto z = getGaze(); /* Camera gase direction */
    auto x = (top % z).norm();
    setTop( (z % x).norm());
    return *this;
}



/*
    Set camera to new place
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
    Shift camera at 2d screen
    TODO
*/
Camera& Camera::shift
(
    const double ax,
    const double ay
)
{
    shift( Point3( ax, ay, 0 ));
    return *this;
}



/*
    Set far clipping
*/
Camera& Camera::setFar
(
    const double a
)
{
    far = a;
    return *this;
}



/*
    Return far clipping
*/
double Camera::getFar()
{
    return far;
}



/*
    Set near clipping
*/
Camera& Camera::setNear
(
    const double a
)
{
    near = a;
    return *this;
}



/*
    Return near clipping
*/
double Camera::getNear()
{
    return near;
}



/*
    Set angle of view in radians
*/
Camera& Camera::setAngleView
(
    const double a
)
{
    viewAngle = a;
    return *this;
}



/*
    Return angle of view in radians
*/
double Camera::getAngleView()
{
    return viewAngle;
}



/*
    Return the gaze direction
*/
Point3 Camera::getGaze()
{
    return ( target - eye ).norm();
}



/*
    Return the left direction
*/
Point3 Camera::getRight()
{
    return (top % getGaze()).norm();
}



/*
    Apply the camera to the scene
    https://vk.com/@bleenchiki-opengl-3
*/
Camera& Camera::setViewMatrixTo
(
    Matrix4& a
)
{
    auto z = getGaze().negative();
    auto x = (top % z).norm();
    auto y = (z % x).norm();

    a.l1.set( x.x, x.y, x.z, -( x * eye ));
    a.l2.set( y.x, y.y, y.z, -( y * eye ));
    a.l3.set( z.x, z.y, z.z, -( z * eye ));
    a.l4.set( VECTOR_4D_W );

    return *this;
}



