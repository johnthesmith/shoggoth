#pragma once

#include <string>
#include "point4d.h"

using namespace std;


struct Point4d;

struct Point3d
{
    double x = 0;
    double y = 0;
    double z = 0;



    /*
      Constructor
    */
    Point3d
    (
        double = 0.0, /* x */
        double = 0.0, /* y */
        double = 0.0  /* z */
    );



    /*
      Constructor
    */
    Point3d
    (
        const Point3d&
    );



    friend Point3d operator+
    (
        const Point3d&,  /* First operand */
        const Point3d&   /* Second operand */
    );



    friend Point3d operator-
    (
        const Point3d&,  /* First operand */
        const Point3d&   /* Second operand */
    );



    friend double operator*
    (
        const Point3d&,  /* First operand */
        const Point3d&   /* Second operand */
    );



    friend Point3d operator*
    (
        const Point3d&,  /* First operand */
        double /* Scalar operand */
    );



    friend Point3d operator%
    (
        const Point3d&,  /* First operand */
        const Point3d&   /* Second operand */
    );



    /*
        Return vector
    */
    Point3d& get();


    /*
        Set vector coordinates
    */
    Point3d& set
    (
        double = 0.0, /* x */
        double = 0.0, /* y */
        double = 0.0  /* z */
    );



    /*
        Set point3 from point3
    */
    Point3d& set
    (
        const Point3d&
    );



    /*
        Set point3 from point4
    */
    Point3d& set
    (
        Point4d&
    );



    /*
        Set X component
    */
    Point3d& setX
    (
        double = 0
    );



    /*
        Set Y component
    */
    Point3d& setY
    (
        double = 0
    );



    /*
        Set Z component
    */
    Point3d& setZ
    (
        double = 0
    );



    /*
        Conver to Point4d object
    */
    Point4d toPoint4d
    (
        const double = 0
    );



    /*
        Negative vector
    */
    Point3d& negative();



    /*
        Add Point3d to Point3d
    */
    Point3d& add
    (
        const Point3d&
    );



    /*
        Add components to Point3d
    */
    Point3d& add
    (
        double, /* x */
        double, /* y */
        double  /* z */
    );




    /*
        Sub
    */
    Point3d& sub
    (
        const Point3d&
    );



    /*
        Sub current vector from argument
        and put result to this
    */
    Point3d& subFrom
    (
        Point3d&
    );



    /*
        Scale
    */
    Point3d& scale
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
    Point3d& norm();


    /*
        Rotate current point around vector
    */
    Point3d& rotate
    (
        const Point3d,
        const double    /* Angle at radians */
    );


    /*
        Distance between this Point3d
    */
    double dist
    (
        const Point3d&
    );



    /*
        Cross vproduct
    */
    Point3d& cross
    (
        const Point3d&
    );



    /*
        Dot product
    */
    double dot
    (
        Point3d&
    );



    /*
        Compare vectors
    */
    double cmp
    (
        Point3d&
    );



    /*
        Convert Point3d to string
    */
    string toString() const;
};



/*
    Define static constatns
*/
static Point3d POINT_3D_0 = Point3d( 0.0, 0.0, 0.0 );
static Point3d POINT_3D_I = Point3d( 1.0, 1.0, 1.0 );
static Point3d POINT_3D_X = Point3d( 1.0, 0.0, 0.0 );
static Point3d POINT_3D_Y = Point3d( 0.0, 1.0, 0.0 );
static Point3d POINT_3D_Z = Point3d( 0.0, 0.0, 1.0 );
