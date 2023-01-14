#pragma once

#include <string>
#include "point4.h"

using namespace std;


struct Point4;

struct Point3
{
    double x = 0;
    double y = 0;
    double z = 0;



    /*
      Constructor
    */
    Point3
    (
        double = 0.0, /* x */
        double = 0.0, /* y */
        double = 0.0  /* z */
    );



    /*
      Constructor
    */
    Point3
    (
        const Point3&
    );



    friend Point3 operator+
    (
        const Point3&,  /* First operand */
        const Point3&   /* Second operand */
    );



    friend Point3 operator-
    (
        const Point3&,  /* First operand */
        const Point3&   /* Second operand */
    );



    friend double operator*
    (
        const Point3&,  /* First operand */
        const Point3&   /* Second operand */
    );



    friend Point3 operator%
    (
        const Point3&,  /* First operand */
        const Point3&   /* Second operand */
    );



    /*
        Return vector
    */
    Point3& get();


    /*
        Set vector coordinates
    */
    Point3& set
    (
        double = 0.0, /* x */
        double = 0.0, /* y */
        double = 0.0  /* z */
    );



    /*
        Set point3 from point3
    */
    Point3& set
    (
        const Point3&
    );



    /*
        Set point3 from point4
    */
    Point3& set
    (
        Point4&
    );



    /*
        Set X component
    */
    Point3& setX
    (
        double = 0
    );



    /*
        Set Y component
    */
    Point3& setY
    (
        double = 0
    );



    /*
        Set Z component
    */
    Point3& setZ
    (
        double = 0
    );



    /*
        Conver to Point4 object
    */
    Point4 toPoint4
    (
        const double = 0
    );



    /*
        Negative vector
    */
    Point3& negative();



    /*
        Add Point3 to Point3
    */
    Point3& add
    (
        const Point3&
    );



    /*
        Add components to Point3
    */
    Point3& add
    (
        double, /* x */
        double, /* y */
        double  /* z */
    );




    /*
        Sub
    */
    Point3& sub
    (
        const Point3&
    );



    /*
        Sub current vector from argument
        and put result to this
    */
    Point3& subFrom
    (
        Point3&
    );



    /*
        Scale
    */
    Point3& scale
    (
        double  /* Scalar */
    );



    /*
        Magnitude
    */
    double magn();



    /*
        Normalize
    */
    Point3& norm();



    /*
        Distance between this Point3
    */
    double dist
    (
        const Point3&
    );



    /*
        Cross vproduct
    */
    Point3& cross
    (
        const Point3&
    );



    /*
        Dot product
    */
    double dot
    (
        Point3&
    );



    /*
        Compare vectors
    */
    double cmp
    (
        Point3&
    );



    /*
        Convert Point3 to string
    */
    string toString() const;
};



/*
    Define static constatns
*/
static Point3 VECTOR_3D_0 = Point3( 0.0, 0.0, 0.0 );
static Point3 VECTOR_3D_I = Point3( 1.0, 1.0, 1.0 );
static Point3 VECTOR_3D_X = Point3( 1.0, 0.0, 0.0 );
static Point3 VECTOR_3D_Y = Point3( 0.0, 1.0, 0.0 );
static Point3 VECTOR_3D_Z = Point3( 0.0, 0.0, 1.0 );
