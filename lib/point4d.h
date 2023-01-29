#pragma once

#include <string>

using namespace std;



struct Matrix4;
struct Point3d;




struct Point4d
{
    double x = 0;
    double y = 0;
    double z = 0;
    double w = 0;


    /*
      Constructor
    */
    Point4d
    (
        double = 0.0, /* x */
        double = 0.0, /* y */
        double = 0.0, /* z */
        double = 0.0  /* w */
    );



    friend Point4d operator+( const Point4d&, const Point4d& );
    friend Point4d operator-( const Point4d&, const Point4d& );
    /* Dot product */
    friend double operator*( const Point4d&, const Point4d& );
    /* Scale vector */
    friend Point4d operator*( const Point4d&, const double );
    /* Multiplex matrix to vector */
    friend Point4d operator*( const Matrix4&, const Point4d& );


    /*
        Set vector coordinates
    */
    Point4d& set
    (
        double = 0.0, /* x */
        double = 0.0, /* y */
        double = 0.0, /* z */
        double = 0.0  /* w */
    );



    /*
        Set Point4d components from Point4d
    */
    Point4d& set
    (
        Point4d&
    );



    /*
        Set Point4d components from Point3d and 4 compoent
    */
    Point4d& set
    (
        Point3d&,
        double aW = 0
    );



    /*
        Set X component
    */
    Point4d& setX
    (
        double = 0
    );



    /*
        Set Y component
    */
    Point4d& setY
    (
        double = 0
    );



    /*
        Set Z component
    */
    Point4d& setZ
    (
        double = 0
    );



    /*
        Set W component
    */
    Point4d& setW
    (
        double = 0
    );



    /*
        Negative vector
    */
    Point4d& negative();



    /*
        Add Point4d to Point4d
    */
    Point4d& add
    (
        Point4d&
    );



    /*
        Add components to Point4d
    */
    Point4d& add
    (
        double, /* x */
        double, /* y */
        double, /* z */
        double  /* w */
    );



    /*
        Sub
    */
    Point4d& sub
    (
        Point4d&
    );



    /*
        Sub current vector from argument
        and put result to this
    */
    Point4d& subFrom
    (
        Point4d&
    );



    /*
        Scale
    */
    Point4d& scale
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
    Point4d& norm();



    /*
        Distance between this Point4d
    */
    double dist
    (
        Point4d&
    );



    /*
        Cross vproduct
    */
    Point4d& cross
    (
        Point4d&
    );



    /*
        Dot product this Point4d and argument Point4d
    */
    double dot
    (
        const Point4d&
    );



    /*
        Dot product this Point4d and argument Matrix4
    */
    Point4d& dot
    (
        const Matrix4&
    );



    /*
        Compare vectors
    */
    double cmp
    (
        Point4d&
    );



    /*
        Convert Point4d to string
    */
    string toString();
    Point4d& toConsole();

};


/*
    Define static constatns
*/
static Point4d POINT_4D_0 = Point4d( 0.0, 0.0, 0.0, 0.0 );
static Point4d POINT_4D_X = Point4d( 1.0, 0.0, 0.0, 0.0 );
static Point4d POINT_4D_Y = Point4d( 0.0, 1.0, 0.0, 0.0 );
static Point4d POINT_4D_Z = Point4d( 0.0, 0.0, 1.0, 0.0 );
static Point4d POINT_4D_W = Point4d( 0.0, 0.0, 0.0, 1.0 );
