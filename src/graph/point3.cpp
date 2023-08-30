#include <string>
#include <iostream>
#include <tgmath.h>


#include "../lib/math.h"
#include "point3.h"
#include "point4d.h"



using namespace std;



/*
  Constructor
*/
Point3d::Point3d
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
Point3d::Point3d
(
    const Point3d& a
)
{
    x = a.x;
    y = a.y;
    z = a.z;
}



/*
    Plus Operator r = a + b
*/
Point3d operator+
(
    const Point3d& a,    /* First operand */
    const Point3d& b     /* Second operand */
)
{
    return Point3d
    (
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    );
}



/*
    Minus Operator r = a - b
*/
Point3d operator-
(
    const Point3d& a,    /* First operand */
    const Point3d& b     /* Second operand */
)
{
    return Point3d
    (
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    );
}



/*
    Operator dot product r = a * b
*/
double operator^
(
    const Point3d& a,    /* First operand */
    const Point3d& b     /* Second operand */
)
{
    return
    a.x * b.x +
    a.y * b.y +
    a.z * b.z
    ;
}



/*
    Operator div r = a / b
*/
Point3d operator/
(
    const Point3d& a,   /* First operand */
    const Point3d& b    /* Second operand */
)
{
    return Point3d
    (
        abs( a.x - b.x ) < EPSILON_D ? 0 : a.x / b.x,
        abs( a.y - b.y ) < EPSILON_D ? 0 : a.y / b.y,
        abs( a.z - b.z ) < EPSILON_D ? 0 : a.z / b.z
    );
}



/*
    Operator div r = a / b
*/
Point3d operator/
(
    const Point3d& a,   /* First operand */
    const Point3i& b    /* Second operand */
)
{
    return Point3d
    (
        b.x == 0 ? 0 : a.x / b.x,
        b.y == 0 ? 0 : a.y / b.y,
        b.z == 0 ? 0 : a.z / b.z
    );
}



/*
    Operator scale r = a * b
*/
Point3d operator*
(
    const Point3d& a,    /* First operand */
    double b            /* Scalar operand */
)
{
    return Point3d
    (
        a.x * b,
        a.y * b,
        a.z * b
    );
}



/*
    Operator cross r = a x b
*/
Point3d operator%
(
    const Point3d& a,    /* First operand */
    const Point3d& b     /* Second operand */
)
{
    return Point3d
    (
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}



/*
    Operator equal for points
*/
bool operator==
(
    const Point3d& a,    /* First operand */
    const Point3d& b     /* Second operand */
)
{
    return
    abs( a.x - b.x ) <= EPSILON_D &&
    abs( a.y - b.y ) <= EPSILON_D &&
    abs( a.z - b.z ) <= EPSILON_D;
}




/*
    Operator equal for points
*/
bool operator!=
(
    const Point3d& a,    /* First operand */
    const Point3d& b     /* Second operand */
)
{
    return
    abs( a.x - b.x ) > EPSILON_D ||
    abs( a.y - b.y ) > EPSILON_D ||
    abs( a.z - b.z ) > EPSILON_D;
}



/*
    Cross vproduct
*/
Point3d& Point3d::cross
(
    const Point3d& a
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
    Set vector coordinates
*/
Point3d& Point3d::get()
{
    return *this;
}



/*
    Set vector coordinates
*/
Point3d& Point3d::set
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
Point3d& Point3d::set
(
    const Point3d& a
)
{
    x = a.x;
    y = a.y;
    z = a.z;
    return *this;
}



/*
    Set point3 from Point4dd
*/
Point3d& Point3d::set
(
    Point4d& a
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
Point3d& Point3d::setX
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
Point3d& Point3d::setY
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
Point3d& Point3d::setZ
(
    double a
)
{
    z = a;
    return *this;
}



/*
    Conver to Point4d object
*/
Point4d Point3d::toPoint4d
(
    const double w
)
{
    return Point4d( x, y, z, w );
}



/*
    Negative vector
*/
Point3d& Point3d::negative()
{
    x = -x;
    y = -y;
    z = -z;
    return *this;
}



/*
    Add
*/
Point3d& Point3d::add
(
    const Point3d& a
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
Point3d& Point3d::add
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
Point3d& Point3d::sub
(
    const Point3d& a
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
Point3d& Point3d::subFrom
(
    Point3d& a
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
Point3d& Point3d::scale
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
double Point3d::magn()
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
Point3d& Point3d::norm()
{
    auto m = magn();
    if( m > EPSILON_D )
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
    Rotate current point around vector

     Point
     o
     |  o Result
     |a/
     |/
     O---------> aBase
*/
Point3d& Point3d::rotate
(
    const Point3d aBase,
    const double aAngleRad
)
{
    auto sn = sinf( aAngleRad );
    auto cs = cosf( aAngleRad );

    auto dz = aBase * ( *this ^ aBase );
    auto dx = *this - dz;
    auto dy = dx % aBase;

    set( dx.scale( cs ) + dy.scale( sn ) + dz );
    return *this;
}



/*
    Distance between this Point3d
*/
double Point3d::dist
(
    const Point3d& a
) const
{
    auto dx = a.x - x;
    auto dy = a.y - y;
    auto dz = a.z - z;
    return sqrt( dx * dx + dy * dy + dz * dz );
}



/*
    Distance between this Point3d in XY surface
*/
double Point3d::distXY
(
    const Point3d& a
)
{
    auto dx = a.x - x;
    auto dy = a.y - y;
    return sqrt( dx * dx + dy * dy );
}



/*
    Dot product
*/
double Point3d::dot
(
    Point3d& a
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
double Point3d::cmp
(
    const Point3d& a
)
{
    return
    abs( x - a.x ) < EPSILON_D &&
    abs( y - a.y ) < EPSILON_D &&
    abs( z - a.z ) < EPSILON_D
    ;
}



/*
    Set min point to this
*/
Point3d& Point3d::min
(
    const Point3d& a
)
{
    x = std::min( x, a.x );
    y = std::min( y, a.y );
    z = std::min( z, a.z );
    return *this;
}



/*
    Set max point to this
*/
Point3d& Point3d::max
(
    const Point3d& a
)
{
    x = std::max( x, a.x );
    y = std::max( y, a.y );
    z = std::max( z, a.z );
    return *this;
}




/*
    Convert Point3d to string
*/
string Point3d::toString() const
{
    return
    "[" + to_string( x ) + "]" +
    "[" + to_string( y ) + "]" +
    "[" + to_string( z ) + "]"
    ;
}



/*
    Test this point at rect
*/
bool Point3d::testRectXY
(
    Point3d&    aTopLeft,       /* Top left */
    Point3d     aBottomRight    /* Bottom right */
)
{
    return
    x >= std::min( aTopLeft.x, aBottomRight.x ) &&
    x <= std::max( aBottomRight.x, aTopLeft.x ) &&
    y >= std::min( aTopLeft.y, aBottomRight.y ) &&
    y <= std::max( aBottomRight.y, aTopLeft.y );
}

