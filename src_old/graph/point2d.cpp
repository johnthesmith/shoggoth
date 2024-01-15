#include <string>
#include <iostream>
#include <tgmath.h>


#include "../lib/math.h"
#include "point2d.h"



using namespace std;



/*
  Constructor
*/
Point2d::Point2d
(
    double ax,
    double ay
)
{
    x = ax;
    y = ay;
}



/*
  Constructor
*/
Point2d::Point2d
(
    const Point2d& a
)
{
    x = a.x;
    y = a.y;
}



/*
    Plus Operator r = a + b
*/
Point2d operator+
(
    const Point2d& a,    /* First operand */
    const Point2d& b     /* Second operand */
)
{
    return Point2d
    (
        a.x + b.x,
        a.y + b.y
    );
}



/*
    Minus Operator r = a - b
*/
Point2d operator-
(
    const Point2d& a,    /* First operand */
    const Point2d& b     /* Second operand */
)
{
    return Point2d
    (
        a.x - b.x,
        a.y - b.y
    );
}



/*
    Operator dot product r = a * b
*/
double operator^
(
    const Point2d& a,    /* First operand */
    const Point2d& b     /* Second operand */
)
{
    return a.x * b.x + a.y * b.y;
}



/*
    Mult two vectords r = a[xy] * b[xy]
*/
Point2d operator*
(
    const Point2d& a,    /* First operand */
    const Point2d& b     /* Second operand */
)
{
    return Point2d
    (
        a.x * b.x,
        a.y * b.y
    );
}



/*
    Operator scale r = a[xy] * b
*/
Point2d operator*
(
    const Point2d& a,    /* First operand */
    double b            /* Scalar operand */
)
{
    return Point2d
    (
        a.x * b,
        a.y * b
    );
}



/*
    Operator div r = a / b
*/
Point2d operator/
(
    const Point2d& a,   /* First operand */
    const Point2d& b    /* Second operand */
)
{
    return Point2d
    (
        abs( a.x - b.x ) < EPSILON_D ? 0 : a.x / b.x,
        abs( a.y - b.y ) < EPSILON_D ? 0 : a.y / b.y
    );
}




/*
    Operator equal for points
*/
bool operator==
(
    const Point2d& a,    /* First operand */
    const Point2d& b     /* Second operand */
)
{
    return
    abs( a.x - b.x ) <= EPSILON_D ||
    abs( a.y - b.y ) <= EPSILON_D;
}




/*
    Operator equal for points
*/
bool operator!=
(
    const Point2d& a,    /* First operand */
    const Point2d& b     /* Second operand */
)
{
    return
    abs( a.x - b.x ) > EPSILON_D ||
    abs( a.y - b.y ) > EPSILON_D;
}



/*
    Set vector coordinates
*/
Point2d& Point2d::get()
{
    return *this;
}



/*
    Set vector coordinates
*/
Point2d& Point2d::set
(
    double ax,
    double ay
)
{
    x = ax;
    y = ay;
    return *this;
}



/*
    Set point2 from point2
*/
Point2d& Point2d::set
(
    const Point2d& a
)
{
    x = a.x;
    y = a.y;
    return *this;
}



/*
    Set X component
*/
Point2d& Point2d::setX
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
Point2d& Point2d::setY
(
    double a
)
{
    y = a;
    return *this;
}



/*
    Negative vector
*/
Point2d& Point2d::negative()
{
    x = -x;
    y = -y;
    return *this;
}



/*
    Add
*/
Point2d& Point2d::add
(
    const Point2d& a
)
{
    x += a.x;
    y += a.y;
    return *this;
}



/*
    Add
*/
Point2d& Point2d::add
(
    double ax = 0.0,
    double ay = 0.0
)
{
    x += ax;
    y += ay;
    return *this;
}



/*
    Sub argument
*/
Point2d& Point2d::sub
(
    const Point2d& a
)
{
    x -= a.x;
    y -= a.y;
    return *this;
}



/*
    Sub current vector from argument
    and put result to this
*/
Point2d& Point2d::subFrom
(
    Point2d& a
)
{
    x = a.x - x;
    y = a.y - y;
    return *this;
}



/*
    Scale
*/
Point2d& Point2d::scale
(
    double a    /* Scalar */
)
{
    x *= a;
    y *= a;
    return *this;
}



/*
    Magnitude
*/
double Point2d::magn()
{
    return sqrt
    (
        x * x +
        y * y
    );
}



/*
    Normalize
*/
Point2d& Point2d::norm()
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
    Distance between this Point2d
*/
double Point2d::dist
(
    const Point2d& a
)
{
    auto dx = a.x - x;
    auto dy = a.y - y;
    return sqrt( dx * dx + dy * dy );
}



/*
    Dot product
*/
double Point2d::dot
(
    Point2d& a
)
{
    return
    x * a.x +
    y * a.y;
}



/*
    Compare vectors
*/
double Point2d::cmp
(
    Point2d& a
)
{
    return
    abs( x - a.x ) < EPSILON_D &&
    abs( y - a.y ) < EPSILON_D
    ;
}



/*
    Convert Point2d to string
*/
string Point2d::toString() const
{
    return
    "[" + to_string( x ) + "]" +
    "[" + to_string( y ) + "]"
    ;
}
