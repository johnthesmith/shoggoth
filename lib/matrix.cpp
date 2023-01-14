#include <string>
#include <iostream>
#include <tgmath.h>
#include "math.h"
#include "point3.h"
#include "point4.h"
#include "matrix.h"



/*
    4d matrix

       |  x   y   z   w
    ---|-----------------
    l1 | ax, ay, az, aw
    l2 | bx, by, bz, bw
    l3 | cx, cy, cz, cw
    l4 | dx, dy, dz, dw
*/

Matrix4::Matrix4
(
    Point4 a1,
    Point4 a2,
    Point4 a3,
    Point4 a4
)
{
    l1 = a1;
    l2 = a2;
    l3 = a3;
    l4 = a4;
}



Matrix4& Matrix4::identity()
{
    l1.x = 1.0;
    l1.y = 0.0;
    l1.z = 0.0;
    l1.w = 0.0;

    l2.x = 0.0;
    l2.y = 1.0;
    l2.z = 0.0;
    l2.w = 0.0;

    l3.x = 0.0;
    l3.y = 0.0;
    l3.z = 1.0;
    l3.w = 0.0;

    l4.x = 0.0;
    l4.y = 0.0;
    l4.z = 0.0;
    l4.w = 1.0;

    return *this;
}




Matrix4& Matrix4::from
(
    Matrix4& a
)
{
    l1.x = a.l1.x;
    l1.y = a.l1.y;
    l1.z = a.l1.z;
    l1.w = a.l1.w;

    l2.x = a.l2.x;
    l2.y = a.l2.y;
    l2.z = a.l2.z;
    l2.w = a.l2.w;

    l3.x = a.l3.x;
    l3.y = a.l3.y;
    l3.z = a.l3.z;
    l3.w = a.l3.w;

    l4.x = a.l4.x;
    l4.y = a.l4.y;
    l4.z = a.l4.z;
    l4.w = a.l4.w;

    return *this;
}



/*
    Create shift matrix from shift vector
*/
Matrix4& Matrix4::shift
(
    const Point4& a
)
{
    l1.x = 1.0;
    l1.y = 0.0;
    l1.z = 0.0;
    l1.w = a.x;

    l2.x = 0.0;
    l2.y = 1.0;
    l2.z = 0.0;
    l2.w = a.y;

    l3.x = 0.0;
    l3.y = 0.0;
    l3.z = 1.0;
    l3.w = a.z;

    l4.x = 0.0;
    l4.y = 0.0;
    l4.z = 0.0;
    l4.w = 1;

    return *this;
}



/*
    Create shift matrix from shift vector
*/
Matrix4& Matrix4::scale
(
    const Point4& a
)
{
    l1.x = a.x;
    l1.y = 0.0;
    l1.z = 0.0;
    l1.w = 0.0;

    l2.x = 0.0;
    l2.y = a.y;
    l2.z = 0.0;
    l2.w = 0.0;

    l3.x = 0.0;
    l3.y = 0.0;
    l3.z = a.z;
    l3.w = 0.0;

    l4.x = 0.0;
    l4.y = 0.0;
    l4.z = 0.0;
    l4.w = 1.0;

    return *this;
}



/*
    Create shift matrix from shift vector
*/
Matrix4& Matrix4::scale
(
    const double a
)
{
    l1.x = a;
    l1.y = 0.0;
    l1.z = 0.0;
    l1.w = 0.0;

    l2.x = 0.0;
    l2.y = a;
    l2.z = 0.0;
    l2.w = 0.0;

    l3.x = 0.0;
    l3.y = 0.0;
    l3.z = a;
    l3.w = 0.0;

    l4.x = 0.0;
    l4.y = 0.0;
    l4.z = 0.0;
    l4.w = 1.0;

    return *this;
}




Matrix4& Matrix4::rotate
(
    const Point4& aBase,
    const float aAngle
)
{
    float cosA = cosf( aAngle );
    float sinA = sinf( aAngle );
    float cosA1 = 1 - cosA;

    l1.x = cosA + cosA1 * aBase.x * aBase.x;
    l1.y = cosA1 * aBase.x * aBase.y - sinA * aBase.z;
    l1.z = cosA1 * aBase.x * aBase.z + sinA * aBase.y;
    l1.w = 0;

    l2.x = cosA1 * aBase.y * aBase.x + sinA * aBase.z;
    l2.y = cosA + cosA1 * aBase.y * aBase.y;
    l2.z = cosA1 * aBase.y * aBase.z - sinA * aBase.x;
    l2.w = 0;

    l3.x = cosA1 * aBase.z * aBase.x - sinA * aBase.y;
    l3.y = cosA1 * aBase.z * aBase.y + sinA * aBase.x;
    l3.z = cosA + cosA1 * aBase.z * aBase.z;
    l3.x = 0;

    l4.x = 0;
    l4.y = 0;
    l4.z = 0;
    l4.w = 1;

    return *this;
}



Matrix4& Matrix4::rotate
(
    const float z,
    const float y,
    const float x
)
{
    auto sinX = sinf( x );
    auto cosX = cosf( x );
    auto sinY = sinf( y );
    auto cosY = cosf( y );
    auto sinZ = sinf( z );
    auto cosZ = cosf( z );

    l1.x = cosX * cosY;
    l1.y = cosX * sinY * sinZ - sinX * cosZ;
    l1.z = cosX * sinY * cosZ + sinX * sinZ;
    l1.w = 0.0;

    l2.x = sinX * cosY;
    l2.y = sinX * sinY * sinZ + cosX * cosZ;
    l2.z = sinX * sinY * cosZ - cosX * sinZ;
    l2.w = 0.0;

    l3.x = -sinY;
    l3.y = cosY * sinZ;
    l3.z = cosY * cosZ;
    l4.w = 0.0;

    l4.x = 0;
    l4.y = 0;
    l4.z = 0;
    l4.w = 1;

    return *this;
}



Matrix4& Matrix4::look
(
    const Point3& aGaze,    /* Normailized gaze direction */
    const Point3& aTop      /* Top vector */
)
{
    auto vx = Point3( aGaze ).cross( aTop );
    auto vy = Point3( vx ).cross( aGaze ).toPoint4( 0 );
    auto vz = Point3( aGaze ).negative().toPoint4( 0 );

    auto vx4 = vx.toPoint4( 0 );

    l1.set( vx4 );
    l2.set( vy );
    l3.set( vz );
    l4.set( VECTOR_4D_W );

    return *this;
}



Matrix4& Matrix4::ortho
(
    double l,
    double r,
    double b,
    double t,
    double n,
    double f
)
{
	l1.x = 2.0 / (r-l);
	l1.y = l1.z = l1.w = 0.0;

	l2.y = 2.0 / (t-b);
	l2.x = l2.z = l2.w = 0.0;

	l3.z = -2.0 / (f-n);
	l3.x = l3.y = l3.w = 0.0;

	l4.x = -( r+l ) / ( r-l );
	l4.y = -( t+b ) / ( t-b );
    l4.z = -( f+n )/( f-n );
    l4.w = 1.0;

    return *this;
}



Matrix4& Matrix4::dot
(
    Matrix4& a,
    Matrix4& b
)
{
    l1.x = a.l1.x * b.l1.x + a.l1.y * b.l2.x + a.l1.z * b.l3.x + a.l1.w * b.l4.x;
    l1.y = a.l1.x * b.l1.y + a.l1.y * b.l2.y + a.l1.z * b.l3.y + a.l1.w * b.l4.y;
    l1.z = a.l1.x * b.l1.z + a.l1.y * b.l2.z + a.l1.z * b.l3.z + a.l1.w * b.l4.z;
    l1.w = a.l1.x * b.l1.w + a.l1.y * b.l2.w + a.l1.z * b.l3.w + a.l1.w * b.l4.w;

    l2.x = a.l2.x * b.l1.x + a.l2.y * b.l2.x + a.l2.z * b.l3.x + a.l2.w * b.l4.x;
    l2.y = a.l2.x * b.l1.y + a.l2.y * b.l2.y + a.l2.z * b.l3.y + a.l2.w * b.l4.y;
    l2.z = a.l2.x * b.l1.z + a.l2.y * b.l2.z + a.l2.z * b.l3.z + a.l2.w * b.l4.z;
    l2.w = a.l2.x * b.l1.w + a.l2.y * b.l2.w + a.l2.z * b.l3.w + a.l2.w * b.l4.w;

    l3.x = a.l3.x * b.l1.x + a.l3.y * b.l2.x + a.l3.z * b.l3.x + a.l3.w * b.l4.x;
    l3.y = a.l3.x * b.l1.y + a.l3.y * b.l2.y + a.l3.z * b.l3.y + a.l3.w * b.l4.y;
    l3.z = a.l3.x * b.l1.z + a.l3.y * b.l2.z + a.l3.z * b.l3.z + a.l3.w * b.l4.z;
    l3.w = a.l3.x * b.l1.w + a.l3.y * b.l2.w + a.l3.z * b.l3.w + a.l3.w * b.l4.w;

    l4.x = a.l4.x * b.l1.x + a.l4.y * b.l2.x + a.l4.z * b.l3.x + a.l4.w * b.l4.x;
    l4.y = a.l4.x * b.l1.y + a.l4.y * b.l2.y + a.l4.z * b.l3.y + a.l4.w * b.l4.y;
    l4.z = a.l4.x * b.l1.z + a.l4.y * b.l2.z + a.l4.z * b.l3.z + a.l4.w * b.l4.z;
    l4.w = a.l4.x * b.l1.w + a.l4.y * b.l2.w + a.l4.z * b.l3.w + a.l4.w * b.l4.w;

    return *this;
}



string Matrix4::toString()
{
    return
    l1.toString() + "\n" +
    l2.toString() + "\n" +
    l3.toString() + "\n" +
    l4.toString();
}
