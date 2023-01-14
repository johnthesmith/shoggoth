#include <string>
#include <iostream>
#include <tgmath.h>
#include "math.h"
#include "point3.h"
#include "point4.h"



using namespace std;



/*
  Constructor
*/
Point3::Point3
(
    double ax,
    double ay,
    double az
)
{
    x = ax;
    y = ay;
    z = az;
}



/*
  Constructor
*/
Point3::Point3
(
    const Point3& a
)
{
    x = a.x;
    y = a.y;
    z = a.z;
}



/*
    Plus Operator r = a + b
*/
Point3 operator+
(
    const Point3& a,    /* First operand */
    const Point3& b     /* Second operand */
)
{
    return Point3
    (
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    );
}



/*
    Minus Operator r = a - b
*/
Point3 operator-
(
    const Point3& a,    /* First operand */
    const Point3& b     /* Second operand */
)
{
    return Point3
    (
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    );
}



/*
    Operator dot product r = a * b
*/
double operator*
(
    const Point3& a,    /* First operand */
    const Point3& b     /* Second operand */
)
{
    return
    a.x * b.x +
    a.y * b.y +
    a.z * b.z
    ;
}



/*
    Operator cross r = a x b
*/
Point3 operator%
(
    const Point3& a,    /* First operand */
    const Point3& b     /* Second operand */
)
{
    return Point3
    (
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}



/*
    Set vector coordinates
*/
Point3& Point3::get()
{
    return *this;
}



/*
    Set vector coordinates
*/
Point3& Point3::set
(
    double ax,
    double ay,
    double az
)
{
    x = ax;
    y = ay;
    z = az;
    return *this;
}



/*
    Set point3 from point3
*/
Point3& Point3::set
(
    const Point3& a
)
{
    x = a.x;
    y = a.y;
    z = a.z;
    return *this;
}



/*
    Set point3 from point4
*/
Point3& Point3::set
(
    Point4& a
)
{
    x = a.x;
    y = a.y;
    z = a.z;
    return *this;
}



/*
    Set X component
*/
Point3& Point3::setX
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
Point3& Point3::setY
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
Point3& Point3::setZ
(
    double a
)
{
    z = a;
    return *this;
}



/*
    Conver to Point4 object
*/
Point4 Point3::toPoint4
(
    const double w
)
{
    return Point4( x, y, z, w );
}



/*
    Negative vector
*/
Point3& Point3::negative()
{
    x = -x;
    y = -y;
    z = -z;
    return *this;
}



/*
    Add
*/
Point3& Point3::add
(
    const Point3& a
)
{
    x += a.x;
    y += a.y;
    z += a.z;
    return *this;
}



/*
    Add
*/
Point3& Point3::add
(
    double ax = 0.0,
    double ay = 0.0,
    double az = 0.0
)
{
    x += ax;
    y += ay;
    z += az;
    return *this;
}



/*
    Sub argument
*/
Point3& Point3::sub
(
    const Point3& a
)
{
    x -= a.x;
    y -= a.y;
    z -= a.z;
    return *this;
}



/*
    Sub current vector from argument
    and put result to this
*/
Point3& Point3::subFrom
(
    Point3& a
)
{
    x = a.x - x;
    y = a.y - y;
    z = a.z - z;
    return *this;
}



/*
    Scale
*/
Point3& Point3::scale
(
    double a    /* Scalar */
)
{
    x *= a;
    y *= a;
    z *= a;
    return *this;
}



/*
    Magnitude
*/
double Point3::magn()
{
    return sqrt
    (
        x * x +
        y * y +
        z * z
    );
}



/*
    Normalize
*/
Point3& Point3::norm()
{
    auto m = magn();
    if( m > EPSILON )
    {
        scale( 1 / m );
    }
    else
    {
        set();
    }
    return *this;
}



/*
    Distance between this Point3
*/
double Point3::dist
(
    const Point3& a
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
Point3& Point3::cross
(
    const Point3& a
)
{
    auto dx = y * a.z - z * a.y;
    auto dy = z * a.x - x * a.z;
    auto dz = x * a.y - y * a.x;

    x = dx;
    y = dy;
    z = dz;

    return *this;
}



/*
    Dot product
*/
double Point3::dot
(
    Point3& a
)
{
    return
    x * a.x +
    y * a.y +
    z * a.z;
}



/*
    Compare vectors
*/
double Point3::cmp
(
    Point3& a
)
{
    return
    abs( x - a.x ) < EPSILON &&
    abs( y - a.y ) < EPSILON &&
    abs( z - a.z ) < EPSILON
    ;
}



/*
    Convert Point3 to string
*/
string Point3::toString() const
{
    return
    "[" + to_string( x ) + "]" +
    "[" + to_string( y ) + "]" +
    "[" + to_string( z ) + "]"
    ;
}
