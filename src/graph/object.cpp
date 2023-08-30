/*
    Object

    Class has:
    - eye - point of view
    - target - point for Front direction
    - top - normalized vector for top directio

    Each point and vector can blocked

                      O target         ^
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

#include "object.h"
#include "matrix.h"


/*
    Directive set eye position
*/
Object& Object::setEye
(
    const Point3d& a
)
{
    changed = !eye.cmp( a );
    eye = a;
    return *this;
}



/*
    Directive set eye position
*/
Object& Object::moveEye
(
    const Point3d& a
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
Point3d& Object::getEye()
{
    return eye;
}



/*
    Set top normal vector
*/
Object& Object::setTop
(
    const Point3d& a
)
{
    top = a;
    changed = true;
    return *this;
}



/*
    Move top vector with normalize
*/
Object& Object::moveTop
(
    const Point3d& a
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
Point3d& Object::getTop()
{
    return top;
}



/*
    Set eye position
*/
Object& Object::setTarget
(
    const Point3d& a
)
{
    target = a;
    changed = true;
    return *this;
}



/*
    Move Object target with normalize
*/
Object& Object::moveTarget
(
    const Point3d& a
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
Point3d& Object::getTarget()
{
    return target;
}



/*
    Normalize Object at 3d
    TODO Vectors Top and Front must not equal
*/
Object& Object::norm()
{
    auto z = getFront(); /* Object Front direction */
    auto x = (top % z).norm();
    setTop( (z % x).norm());
    return *this;
}



/*
    Set Object to new place
*/
Object& Object::place
(
    const Point3d& aEye,
    const Point3d& aTarget,
    const Point3d& aTop
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
Object& Object::setEyeLock
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
bool Object::getEyeLock()
{
    return eyeLock;
}



/*
    Set eye lock
*/
Object& Object::setTargetLock
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
bool Object::getTargetLock()
{
    return targetLock;
}



/*
    Return the Front direction
*/
Point3d Object::getFront()
{
    return ( target - eye ).norm();
}



/*
    Return the Back direction
*/
Point3d Object::getBack()
{
    return ( eye - target ).norm();
}



/*
    Return the Right direction
*/
Point3d Object::getRight()
{
    return (top % getFront()).norm();
}



/*
    Return the gase
*/
Point3d Object::getGaze()
{
    return target - eye;
}



/*
    Apply the Object to the scene
    https://vk.com/@bleenchiki-opengl-3
*/
Object& Object::setViewMatrixTo
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
    a.m[M_AW] = -( x ^ eye );   a.m[M_BW] = -( y ^ eye );  a.m[M_CW] = -( z ^ eye );  a.m[M_DW] = 1.0;

    return *this;
}



Object& Object::zoom
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
Object& Object::shift
(
    const Point3d& a
)
{
    Point3d s = a * sensivity;
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
Object& Object::rotateEye
(
    const Point3d& aBase,    /* Base */
    const double aAngleRad  /* Rotation angle at radians */
)
{
    setEye( ( eye - target ).rotate( aBase, aAngleRad ) + target );
    norm();
    return *this;
}



/*
    Rotate top around vector
*/
Object& Object::rotateTop
(
    const Point3d& aBase,    /* Base */
    const double aAngleRad  /* Rotation angle at radians */
)
{
    setTop( top.rotate( aBase, aAngleRad ));
    norm();
    return *this;
}



/*
    Rotate target around vector
*/
Object& Object::rotateTarget
(
    const Point3d& aBase,    /* Base */
    const double aAngleRad  /* Rotation angle at radians */
)
{
//    target - eye
    return *this;
}



bool Object::getChanged()
{
    return changed;
}



Object& Object::setChanged
(
    bool a
)
{
    changed = a;
    return *this;
}
