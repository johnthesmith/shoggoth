#pragma once

#include <string>

using namespace std;



struct Matrix4;
struct Point3;




struct Point4
{
    double x = 0;
    double y = 0;
    double z = 0;
    double w = 0;


    /*
      Constructor
    */
    Point4
    (
        double = 0.0, /* x */
        double = 0.0, /* y */
        double = 0.0, /* z */
        double = 0.0  /* w */
    );



    friend Point4 operator+( const Point4&, const Point4& );
    friend Point4 operator-( const Point4&, const Point4& );
    /* Dot product */
    friend double operator*( const Point4&, const Point4& );
    /* Scale vector */
    friend Point4 operator*( const Point4&, const double );
    /* Multiplex matrix to vector */
    friend Point4 operator*( const Matrix4&, const Point4& );


    /*
        Set vector coordinates
    */
    Point4& set
    (
        double = 0.0, /* x */
        double = 0.0, /* y */
        double = 0.0, /* z */
        double = 0.0  /* w */
    );



    /*
        Set Point4 components from Point4
    */
    Point4& set
    (
        Point4&
    );



    /*
        Set Point4 components from Point3 and 4 compoent
    */
    Point4& set
    (
        Point3&,
        double aW = 0
    );



    /*
        Set X component
    */
    Point4& setX
    (
        double = 0
    );



    /*
        Set Y component
    */
    Point4& setY
    (
        double = 0
    );



    /*
        Set Z component
    */
    Point4& setZ
    (
        double = 0
    );



    /*
        Set W component
    */
    Point4& setW
    (
        double = 0
    );



    /*
        Negative vector
    */
    Point4& negative();



    /*
        Add Point4 to Point4
    */
    Point4& add
    (
        Point4&
    );



    /*
        Add components to Point4
    */
    Point4& add
    (
        double, /* x */
        double, /* y */
        double, /* z */
        double  /* w */
    );



    /*
        Sub
    */
    Point4& sub
    (
        Point4&
    );



    /*
        Sub current vector from argument
        and put result to this
    */
    Point4& subFrom
    (
        Point4&
    );



    /*
        Scale
    */
    Point4& scale
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
    Point4& norm();



    /*
        Distance between this Point4
    */
    double dist
    (
        Point4&
    );



    /*
        Cross vproduct
    */
    Point4& cross
    (
        Point4&
    );



    /*
        Dot product this Point4 and argument Point4
    */
    double dot
    (
        const Point4&
    );



    /*
        Dot product this Point4 and argument Matrix4
    */
    Point4& dot
    (
        const Matrix4&
    );



    /*
        Compare vectors
    */
    double cmp
    (
        Point4&
    );



    /*
        Convert Point4 to string
    */
    string toString();
    Point4& toConsole();

};


/*
    Define static constatns
*/
static Point4 VECTOR_4D_0 = Point4( 0.0, 0.0, 0.0, 0.0 );
static Point4 VECTOR_4D_X = Point4( 1.0, 0.0, 0.0, 0.0 );
static Point4 VECTOR_4D_Y = Point4( 0.0, 1.0, 0.0, 0.0 );
static Point4 VECTOR_4D_Z = Point4( 0.0, 0.0, 1.0, 0.0 );
static Point4 VECTOR_4D_W = Point4( 0.0, 0.0, 0.0, 1.0 );
