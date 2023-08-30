#include <string>
#include <iostream>
#include <tgmath.h>

#include "../lib/math.h"

#include "point3.h"
#include "point4d.h"
#include "matrix.h"




/*
    4d matrix

       | A     | B     | C     | D
    ---|----------------------------
     x | ax  0 | bx  1 | cx  2 | dx  3
     y | ay  4 | by  5 | cy  6 | dy  7
     z | az  8 | bz  9 | cz 10 | dz 11
     t | aw 12 | bw 13 | cw 14 | dw 15
*/


Matrix4::Matrix4
(
    double ax, double bx, double cx, double dx,
    double ay, double by, double cy, double dy,
    double az, double bz, double cz, double dz,
    double aw, double bw, double cw, double dw
)
{
    m[ M_AX ] = ax; m[ M_BX ] = bx; m[ M_CX ] = cx; m[ M_DX ] = dx;
    m[ M_AY ] = ay; m[ M_BY ] = by; m[ M_CY ] = cy; m[ M_DY ] = dy;
    m[ M_AZ ] = az; m[ M_BZ ] = bz; m[ M_CZ ] = cz; m[ M_DZ ] = dz;
    m[ M_AW ] = aw; m[ M_BW ] = bw; m[ M_CW ] = cw; m[ M_DW ] = dw;
}




Matrix4& Matrix4::identity()
{
    from( MATRIX_4D_I );

    return *this;
}




Matrix4& Matrix4::from
(
    Matrix4& a
)
{
    std::copy( std::begin( m ), std::end( m ), std::begin( a.m ) );

    return *this;
}



/*
    Create shift matrix from shift vector
*/
Matrix4& Matrix4::shift
(
    const Point4d& a
)
{
    m[ M_AX ] = 1.0; m[ M_BX ] = 0.0; m[ M_CX ] = 0.0; m[ M_DX ] = 0.0;
    m[ M_AY ] = 0.0; m[ M_BY ] = 1.0; m[ M_CY ] = 0.0; m[ M_DY ] = 0.0;
    m[ M_AZ ] = 0.0; m[ M_BZ ] = 0.0; m[ M_CZ ] = 1.0; m[ M_DZ ] = 0.0;
    m[ M_AW ] = a.x; m[ M_BW ] = a.y; m[ M_CW ] = a.z; m[ M_DW ] = 1.0;
    return *this;
}



/*
    Create shift matrix from shift vector
*/
Matrix4& Matrix4::scale
(
    const Point4d& a
)
{
    m[ M_AX ] = a.x; m[ M_BX ] = 0.0; m[ M_CX ] = 0.0; m[ M_DX ] = 0.0;
    m[ M_AY ] = 0.0; m[ M_BY ] = a.y; m[ M_CY ] = 0.0; m[ M_DY ] = 0.0;
    m[ M_AZ ] = 0.0; m[ M_BZ ] = 0.0; m[ M_CZ ] = a.z; m[ M_DZ ] = 0.0;
    m[ M_AW ] = 0.0; m[ M_BW ] = 0.0; m[ M_CW ] = 0.0; m[ M_DW ] = 1.0;

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
    m[ M_AX ] = a;   m[ M_BX ] = 0.0; m[ M_CX ] = 0.0; m[ M_DX ] = 0.0;
    m[ M_AY ] = 0.0; m[ M_BY ] = a;   m[ M_CY ] = 0.0; m[ M_DY ] = 0.0;
    m[ M_AZ ] = 0.0; m[ M_BZ ] = 0.0; m[ M_CZ ] = a;   m[ M_DZ ] = 0.0;
    m[ M_AW ] = 0.0; m[ M_BW ] = 0.0; m[ M_CW ] = 0.0; m[ M_DW ] = 1.0;

    return *this;
}




Matrix4& Matrix4::rotate
(
    const Point4d& aBase,
    const float aAngle
)
{
    float cosA = cosf( aAngle );
    float sinA = sinf( aAngle );
    float cosA1 = 1 - cosA;

    m[ M_AX ] = cosA + cosA1 * aBase.x * aBase.x;
    m[ M_AY ] = cosA1 * aBase.x * aBase.y - sinA * aBase.z;
    m[ M_AZ ] = cosA1 * aBase.x * aBase.z + sinA * aBase.y;
    m[ M_AW ] = 0;

    m[ M_BX ] = cosA1 * aBase.y * aBase.x + sinA * aBase.z;
    m[ M_BY ] = cosA + cosA1 * aBase.y * aBase.y;
    m[ M_BZ ] = cosA1 * aBase.y * aBase.z - sinA * aBase.x;
    m[ M_BW ] = 0;

    m[ M_CX ] = cosA1 * aBase.z * aBase.x - sinA * aBase.y;
    m[ M_CY ] = cosA1 * aBase.z * aBase.y + sinA * aBase.x;
    m[ M_CZ ] = cosA + cosA1 * aBase.z * aBase.z;
    m[ M_CW ] = 0;

    m[ M_DX ] = 0;
    m[ M_DY ] = 0;
    m[ M_DZ ] = 0;
    m[ M_DW ] = 1;

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

    m[ M_AX ] = cosX * cosY;
    m[ M_AY ] = cosX * sinY * sinZ - sinX * cosZ;
    m[ M_AZ ] = cosX * sinY * cosZ + sinX * sinZ;
    m[ M_AW ] = 0.0;

    m[ M_BX ] = sinX * cosY;
    m[ M_BY ] = sinX * sinY * sinZ + cosX * cosZ;
    m[ M_BZ ] = sinX * sinY * cosZ - cosX * sinZ;
    m[ M_BW ] = 0.0;

    m[ M_CX ] = -sinY;
    m[ M_CY ] = cosY * sinZ;
    m[ M_CZ ] = cosY * cosZ;
    m[ M_CW ] = 0.0;

    m[ M_DX ] = 0;
    m[ M_DY ] = 0;
    m[ M_DZ ] = 0;
    m[ M_DW ] = 1;

    return *this;
}



Matrix4& Matrix4::look
(
    const Point3d& aGaze,    /* Normailized gaze direction */
    const Point3d& aTop      /* Top vector */
)
{
    auto vx = Point3d( aGaze ).cross( aTop );
    auto vy = Point3d( vx ).cross( aGaze ).toPoint4d( 0 );
    auto vz = Point3d( aGaze ).negative().toPoint4d( 0 );

    m[ M_AX ] = vx.x; m[ M_BX ] = vy.x; m[ M_CX ] = vz.x; m[ M_DX ] = 0.0;
    m[ M_AY ] = vx.y; m[ M_BY ] = vy.y; m[ M_CY ] = vz.y; m[ M_DY ] = 0.0;
    m[ M_AZ ] = vx.z; m[ M_BZ ] = vy.x; m[ M_CZ ] = vz.z; m[ M_DZ ] = 0.0;
    m[ M_AW ] = 0.0;  m[ M_BW ] = 0.0;  m[ M_CW ] = 0.0;  m[ M_DW ] = 1.0;

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
//	l1.x = 2.0 / (r-l);
//	l1.y = l1.z = l1.w = 0.0;
//
//	l2.y = 2.0 / (t-b);
//	l2.x = l2.z = l2.w = 0.0;
//
//	l3.z = -2.0 / (f-n);
//	l3.x = l3.y = l3.w = 0.0;
//
//	l4.x = -( r+l ) / ( r-l );
//	l4.y = -( t+b ) / ( t-b );
//    l4.z = -( f+n )/( f-n );
//    l4.w = 1.0;

    return *this;
}



Matrix4& Matrix4::dot
(
    Matrix4& a,
    Matrix4& b
)
{
//    l1.x = a.l1.x * b.l1.x + a.l1.y * b.l2.x + a.l1.z * b.l3.x + a.l1.w * b.l4.x;
//    l1.y = a.l1.x * b.l1.y + a.l1.y * b.l2.y + a.l1.z * b.l3.y + a.l1.w * b.l4.y;
//    l1.z = a.l1.x * b.l1.z + a.l1.y * b.l2.z + a.l1.z * b.l3.z + a.l1.w * b.l4.z;
//    l1.w = a.l1.x * b.l1.w + a.l1.y * b.l2.w + a.l1.z * b.l3.w + a.l1.w * b.l4.w;
//
//    l2.x = a.l2.x * b.l1.x + a.l2.y * b.l2.x + a.l2.z * b.l3.x + a.l2.w * b.l4.x;
//    l2.y = a.l2.x * b.l1.y + a.l2.y * b.l2.y + a.l2.z * b.l3.y + a.l2.w * b.l4.y;
//    l2.z = a.l2.x * b.l1.z + a.l2.y * b.l2.z + a.l2.z * b.l3.z + a.l2.w * b.l4.z;
//    l2.w = a.l2.x * b.l1.w + a.l2.y * b.l2.w + a.l2.z * b.l3.w + a.l2.w * b.l4.w;
//
//    l3.x = a.l3.x * b.l1.x + a.l3.y * b.l2.x + a.l3.z * b.l3.x + a.l3.w * b.l4.x;
//    l3.y = a.l3.x * b.l1.y + a.l3.y * b.l2.y + a.l3.z * b.l3.y + a.l3.w * b.l4.y;
//    l3.z = a.l3.x * b.l1.z + a.l3.y * b.l2.z + a.l3.z * b.l3.z + a.l3.w * b.l4.z;
//    l3.w = a.l3.x * b.l1.w + a.l3.y * b.l2.w + a.l3.z * b.l3.w + a.l3.w * b.l4.w;
//
//    l4.x = a.l4.x * b.l1.x + a.l4.y * b.l2.x + a.l4.z * b.l3.x + a.l4.w * b.l4.x;
//    l4.y = a.l4.x * b.l1.y + a.l4.y * b.l2.y + a.l4.z * b.l3.y + a.l4.w * b.l4.y;
//    l4.z = a.l4.x * b.l1.z + a.l4.y * b.l2.z + a.l4.z * b.l3.z + a.l4.w * b.l4.z;
//    l4.w = a.l4.x * b.l1.w + a.l4.y * b.l2.w + a.l4.z * b.l3.w + a.l4.w * b.l4.w;

    return *this;
}



string Matrix4::toString()
{
    return "";
//    l1.toString() + "\n" +
//    l2.toString() + "\n" +
//    l3.toString() + "\n" +
//    l4.toString();
}



/*
    https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluPerspective.xml
*/
Matrix4& Matrix4::perspective
(
    double aAngle,
    double aRatio,
    double aNear,
    double aFar
)
{
    double f = cosf( aAngle * 0.5 ) / sinf( aAngle * 0.5);

    m[ M_AX ] = f/aRatio;
    m[ M_BX ] = 0.0;
    m[ M_CX ] = 0.0;
    m[ M_DX ] = 0.0;

    m[ M_AY ] = 0.0;
    m[ M_BY ] = f;
    m[ M_CY ] = 0.0;
    m[ M_DY ] = 0.0;

    m[ M_AZ ] = 0.0;
    m[ M_BZ ] = 0.0;
    m[ M_CZ ] = (aFar + aNear) / ( aNear - aFar );
    m[ M_DZ ] = ( 2.0 * aFar * aNear ) / ( aNear - aFar );

    m[ M_AW ] = 0.0;
    m[ M_BW ] = 0.0;
    m[ M_CW ] = -1.0;
    m[ M_DW ] = 0.0;

   return *this;
}
