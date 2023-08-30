#pragma once

#include <string>


using namespace std;


struct Point2d
{
    double x = 0;
    double y = 0;



    /*
      Constructor
    */
    Point2d
    (
        double = 0.0, /* x */
        double = 0.0  /* y */
    );



    /*
      Constructor
    */
    Point2d
    (
        const Point2d&
    );



    friend Point2d operator+
    (
        const Point2d&,  /* First operand */
        const Point2d&   /* Second operand */
    );



    friend Point2d operator-
    (
        const Point2d&,  /* First operand */
        const Point2d&   /* Second operand */
    );



    friend double operator^
    (
        const Point2d&,  /* First operand */
        const Point2d&   /* Second operand */
    );



    friend Point2d operator*
    (
        const Point2d&,  /* First operand */
        const Point2d&   /* Second operand */
    );



    friend Point2d operator*
    (
        const Point2d&,  /* First operand */
        double /* Scalar operand */
    );



    friend Point2d operator/ (
        const Point2d&,  /* First operand */
        const Point2d&   /* Second operand */
    );



    friend Point2d operator%
    (
        const Point2d&,  /* First operand */
        const Point2d&   /* Second operand */
    );



    friend bool operator==
    (
        const Point2d&,  /* First operand */
        const Point2d&   /* Second operand */
    );



    friend bool operator!=
    (
        const Point2d&,  /* First operand */
        const Point2d&   /* Second operand */
    );



    /*
        Return vector
    */
    Point2d& get();


    /*
        Set vector coordinates
    */
    Point2d& set
    (
        double = 0.0, /* x */
        double = 0.0 /* y */
    );



    /*
        Set point3 from point3
    */
    Point2d& set
    (
        const Point2d&
    );



    /*
        Set X component
    */
    Point2d& setX
    (
        double = 0
    );



    /*
        Set Y component
    */
    Point2d& setY
    (
        double = 0
    );



    /*
        Negative vector
    */
    Point2d& negative();



    /*
        Add Point2d to Point2d
    */
    Point2d& add
    (
        const Point2d&
    );



    /*
        Add components to Point2d
    */
    Point2d& add
    (
        double, /* x */
        double  /* y */
    );




    /*
        Sub
    */
    Point2d& sub
    (
        const Point2d&
    );



    /*
        Sub current vector from argument
        and put result to this
    */
    Point2d& subFrom
    (
        Point2d&
    );



    /*
        Scale
    */
    Point2d& scale
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
    Point2d& norm();



    /*
        Distance between this Point2d
    */
    double dist
    (
        const Point2d&
    );



    /*
        Dot product
    */
    double dot
    (
        Point2d&
    );



    /*
        Compare vectors
    */
    double cmp
    (
        Point2d&
    );



    /*
        Convert Point2d to string
    */
    string toString() const;
};



/*
    Define static constatns
*/
static Point2d POINT_2D_0 = Point2d( 0.0, 0.0 );
static Point2d POINT_2D_I = Point2d( 1.0, 1.0 );

static Point2d POINT_2D_X = Point2d( 1.0, 0.0 );
static Point2d POINT_2D_Y = Point2d( 0.0, 1.0 );
static Point2d POINT_2D_XY = Point2d( 1.0, 1.0 );

static Point2d POINT_2D_X_05 = Point2d( 0.5, 0.0 );
static Point2d POINT_2D_Y_05 = Point2d( 0.0, 0.5 );
