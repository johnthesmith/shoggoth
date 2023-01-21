#include <string>
#include <iostream>
#include <tgmath.h>
#include "math.h"
#include "point4.h"
#include "point3.h"
#include "matrix.h"



using namespace std;



/*
  Constructor
*/
Point4::Point4
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



Point4 operator+
(
    const Point4& a,
    const Point4& b
)
{
    return Point4
    (
        a.x + b.x,
        a.y + b.y,
        a.z + b.z,
        a.w + b.w
    );
}



Point4 operator-
(
    const Point4& a,
    const Point4& b
)
{
    return Point4
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
    const Point4& a,
    const Point4& b
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
Point4 operator*
(
    const Point4& a,
    double aScalar
)
{
    return Point4
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
Point4 operator*
(
    const Matrix4& aMatrix,
    const Point4& aPoint
)
{
    return Point4
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
Point4& Point4::set
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
Point4& Point4::set
(
    Point4& a
)
{
    x = a.x;
    y = a.y;
    z = a.z;
    w = a.w;
    return *this;
}



/*
    Set Point4 components from Point3 and 4 compoent
*/
Point4& Point4::set
(
    Point3& a,
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
Point4& Point4::setX
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
Point4& Point4::setY
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
Point4& Point4::setZ
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
Point4& Point4::setW
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
Point4& Point4::negative()
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
Point4& Point4::add
(
    Point4& a
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
Point4& Point4::add
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
Point4& Point4::sub
(
    Point4& a
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
Point4& Point4::subFrom
(
    Point4& a
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
Point4& Point4::scale
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
double Point4::magn()
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
Point4& Point4::norm()
{
    auto m = magn();
    if( m > EPSILON )
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
    Distance between this Point4
*/
double Point4::dist
(
    Point4& a
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
Point4& Point4::cross
(
    Point4& a
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
double Point4::dot
(
    const Point4& a
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
Point4& Point4::dot
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
double Point4::cmp
(
    Point4& a
)
{
    return
    abs( x - a.x ) < EPSILON &&
    abs( y - a.y ) < EPSILON &&
    abs( z - a.z ) < EPSILON &&
    abs( w - a.w ) < EPSILON
    ;
}



/*
    Convert Point4 to string
*/
string Point4::toString()
{
    return
    "[" + to_string( x ) + "]" +
    "[" + to_string( y ) + "]" +
    "[" + to_string( z ) + "]" +
    "[" + to_string( w ) + "]"
    ;
}



/*
    Convert Point4 to string
*/
Point4& Point4::toConsole()
{
    cout << toString() << "\n";
    return *this;
}
