/*
    Camera

    Class has:
    - eye - point of view
    - target - point for Front direction
    - top - normalized vector for top directio

    Each point and vector can blocked

                      O target         o
                     /                /
                    /                /
           top o   /                /
               |  o front          / gase
               | /                /
               |/eye             /
    left o-----0-----o right    0
              /|
             / |
       back o  |
               o bottom

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
    Normalize camera at 3d
    TODO Vectors Top and Front must not equal
*/
Camera& Camera::norm()
{
    auto z = getFront(); /* Camera Front direction */
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
    Set far clipping
*/

/*
    Set eye lock
*/
Camera& Camera::setEyeLock
(
    const bool a /* value */
)
{
    eyeLock = a;
    return *this;
}



/*
    Return near clipping
*/
bool Camera::getEyeLock()
{
    return eyeLock;
}



/*
    Set eye lock
*/
Camera& Camera::setTargetLock
(
    const bool a /* value */
)
{
    targetLock = a;
    return *this;
}



/*
    Return target lock
*/
bool Camera::getTargetLock()
{
    return targetLock;
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
    Return the Front direction
*/
Point3 Camera::getFront()
{
    return ( target - eye ).norm();
}



/*
    Return the Back direction
*/
Point3 Camera::getBack()
{
    return ( eye - target ).norm();
}



/*
    Return the Right direction
*/
Point3 Camera::getRight()
{
    return (top % getFront()).norm();
}



/*
    Return the gase
*/
Point3 Camera::getGaze()
{
    return target - eye;
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
    auto z = getBack();
    auto x = (top % z).norm();
    auto y = (z % x).norm();

    a.m[M_AX] = x.x;            a.m[M_BX] = y.x;           a.m[M_CX] = z.x;           a.m[M_DX] = 0.0;
    a.m[M_AY] = x.y;            a.m[M_BY] = y.y;           a.m[M_CY] = z.y;           a.m[M_DY] = 0.0;
    a.m[M_AZ] = x.z;            a.m[M_BZ] = y.z;           a.m[M_CZ] = z.z;           a.m[M_DZ] = 0.0;
    a.m[M_AW] = -( x * eye );   a.m[M_BW] = -( y * eye );  a.m[M_CW] = -( z * eye );  a.m[M_DW] = 1.0;

    return *this;
}



Camera& Camera::zoom
(
    double a
)
{
    setEye( getTarget() - getGaze().scale( a ));
    return *this;
}



/*
    Shift eye and target at 3d
*/
Camera& Camera::shift
(
    const Point3& a
)
{
    Point3 s = a * sensivity;
    place
    (
        eye + s,
        target + s,
        top
    );
    return *this;
}



/*
    Rotate eye around vector
*/
Camera& Camera::rotateEye
(
    const Point3& aBase,    /* Base */
    const double aAngleRad  /* Rotation angle at radians */
)
{
    moveEye( ( eye - target ).rotate( aBase, aAngleRad ) + target );
    return *this;
}



/*
    Rotate top around vector
*/
Camera& Camera::rotateTop
(
    const Point3& aBase,    /* Base */
    const double aAngleRad  /* Rotation angle at radians */
)
{
    moveTop( top.rotate( aBase, aAngleRad ));
    return *this;
}



/*
    Rotate target around vector
*/
Camera& Camera::rotateTarget
(
    const Point3& aBase,    /* Base */
    const double aAngleRad  /* Rotation angle at radians */
)
{
//    target - eye
    return *this;
}


