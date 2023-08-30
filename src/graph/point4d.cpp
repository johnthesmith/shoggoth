#include <string>
#include <iostream>
#include <tgmath.h>

#include "../lib/math.h"

#include "point4d.h"
#include "point3.h"
#include "matrix.h"



using namespace std;



/*
  Constructor
*/
Point4d::Point4d
(
    double ax,
    double ay,
    double az,
    double aw
)
{
    x = ax;
    y = ay;
    z = az;
    w = aw;
}



Point4d operator+
(
    const Point4d& a,
    const Point4d& b
)
{
    return Point4d
    (
        a.x + b.x,
        a.y + b.y,
        a.z + b.z,
        a.w + b.w
    );
}



Point4d operator-
(
    const Point4d& a,
    const Point4d& b
)
{
    return Point4d
    (
        a.x - b.x,
        a.y - b.y,
        a.z - b.z,
        a.w - b.w
    );
}



/*
    Dot product
*/
double operator*
(
    const Point4d& a,
    const Point4d& b
)
{
    return
    a.x * b.x +
    a.y * b.y +
    a.z * b.z +
    a.w * b.w;
}



/*
    Scale vector
*/
Point4d operator*
(
    const Point4d& a,
    double aScalar
)
{
    return Point4d
    (
        a.x * aScalar,
        a.y * aScalar,
        a.z * aScalar,
        a.w * aScalar
    );
}



/*
    Dot product matrix and vector
    [a b c d ]   [ x ]   [ ax + by + cz + dw ]
    [e f g h ] * [ y ] = [ ex + fy + gz + hw ]
    [i j k l ]   [ z ]   [ ix + jy + kz + lw ]
    [m n o p ]   [ w ]   [ mx + ny + oz + pw ]
*/
Point4d operator*
(
    const Matrix4& aMatrix,
    const Point4d& aPoint
)
{
    return Point4d
    (
//    TODO
//        aMatrix.l1 * aPoint,
//        aMatrix.l2 * aPoint,
//        aMatrix.l3 * aPoint,
//        aMatrix.l4 * aPoint
    );
}



/*
    Set vector coordinates
*/
Point4d& Point4d::set
(
    double ax,
    double ay,
    double az,
    double aw
)
{
    x = ax;
    y = ay;
    z = az;
    w = aw;
    return *this;
}



/*
    Set vector from vector
*/
Point4d& Point4d::set
(
    Point4d& a
)
{
    x = a.x;
    y = a.y;
    z = a.z;
    w = a.w;
    return *this;
}



/*
    Set Point4d components from Point3d and 4 compoent
*/
Point4d& Point4d::set
(
    Point3d& a,
    double aW
)
{
    x = a.x;
    y = a.y;
    z = a.z;
    w = aW;
    return *this;
}



/*
    Set X component
*/
Point4d& Point4d::setX
(
    double a
)
{
    x = a;
    return *this;
}



/*
    Set Y component
*/
Point4d& Point4d::setY
(
    double a
)
{
    y = a;
    return *this;
}



/*
    Set Z component
*/
Point4d& Point4d::setZ
(
    double a
)
{
    z = a;
    return *this;
}



/*
    Set W component
*/
Point4d& Point4d::setW
(
    double a
)
{
    w = a;
    return *this;
}



/*
    Negative vector
*/
Point4d& Point4d::negative()
{
    x = -x;
    y = -y;
    z = -z;
    w = -z;
    return *this;
}



/*
    Add
*/
Point4d& Point4d::add
(
    Point4d& a
)
{
    x += a.x;
    y += a.y;
    z += a.z;
    w += a.w;
    return *this;
}



/*
    Add
*/
Point4d& Point4d::add
(
    double ax = 0.0,
    double ay = 0.0,
    double az = 0.0,
    double aw = 0.0
)
{
    x += ax;
    y += ay;
    z += az;
    w += aw;
    return *this;
}



/*
    Sub argument
*/
Point4d& Point4d::sub
(
    Point4d& a
)
{
    x -= a.x;
    y -= a.y;
    z -= a.z;
    w -= a.w;
    return *this;
}



/*
    Sub current vector from argument
    and put result to this
*/
Point4d& Point4d::subFrom
(
    Point4d& a
)
{
    x = a.x - x;
    y = a.y - y;
    z = a.z - z;
    w = a.w - w;
    return *this;
}



/*
    Scale
*/
Point4d& Point4d::scale
(
    double a    /* Scalar */
)
{
    x *= a;
    y *= a;
    z *= a;
    w *= a;
    return *this;
}



/*
    Magnitude
*/
double Point4d::magn()
{
    return sqrt
    (
        x * x +
        y * y +
        z * z +
        w * w
    );
}



/*
    Normalize
*/
Point4d& Point4d::norm()
{
    auto m = magn();
    if( m > EPSILON_D )
    {
        scale( m );
    }
    else
    {
        set();
    }
    return *this;
}



/*
    Distance between this Point4d
*/
double Point4d::dist
(
    Point4d& a
)
{
    auto dx = a.x - x;
    auto dy = a.y - y;
    auto dz = a.z - z;
    return sqrt( dx * dx + dy * dy + dz * dz );
}



/*
    Cross vproduct
*/
Point4d& Point4d::cross
(
    Point4d& a
)
{
    x = y * a.z - z * a.y;
    y = z * a.x - x * a.z;
    z = x * a.y - y * a.x;
    return *this;
}



/*
    Dot product
*/
double Point4d::dot
(
    const Point4d& a
)
{
    return
    x * a.x +
    y * a.y +
    z * a.z +
    w * a.w
    ;
}



/*
    Dot product matrix and vector
    [ x ]   [ a b c d ]   [ ax + by + cz + dw ]
    [ y ] * [ e f g h ] = [ ex + fy + gz + hw ]
    [ z ]   [ i j k l ]   [ ix + jy + kz + lw ]
    [ w ]   [ m n o p ]   [ mx + ny + oz + pw ]
*/
Point4d& Point4d::dot
(
    const Matrix4& a
)
{
// TODO
//    x = x * a.l1.x + y * a.l1.y + z * a.l1.z + w * a.l1.w;
//    y = x * a.l2.x + y * a.l2.y + z * a.l2.z + w * a.l2.w;
//    z = x * a.l3.x + y * a.l3.y + z * a.l3.z + w * a.l3.w;
//    w = x * a.l4.x + y * a.l4.y + z * a.l4.z + w * a.l4.w;
    return *this;
}



/*
    Compare vectors
*/
double Point4d::cmp
(
    Point4d& a
)
{
    return
    abs( x - a.x ) < EPSILON_D &&
    abs( y - a.y ) < EPSILON_D &&
    abs( z - a.z ) < EPSILON_D &&
    abs( w - a.w ) < EPSILON_D
    ;
}



/*
    Convert Point4d to string
*/
string Point4d::toString()
{
    return
    "[" + to_string( x ) + "]" +
    "[" + to_string( y ) + "]" +
    "[" + to_string( z ) + "]" +
    "[" + to_string( w ) + "]"
    ;
}



/*
    Convert Point4d to string
*/
Point4d& Point4d::toConsole()
{
    cout << toString() << "\n";
    return *this;
}
