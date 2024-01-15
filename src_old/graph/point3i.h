#pragma once



/*
    Three components integar point
*/
struct Point3i
{
    int x = 0;  /* x */
    int y = 0;  /* y */
    int z = 0;  /* z */



    /*
        Constructor
    */
    Point3i
    (
        int = 0,
        int = 0,
        int = 0
    );



    friend Point3i operator -
    (
        const Point3i&,  /* First operand */
        const Point3i&   /* Second operand */
    );



    friend Point3i operator -
    (
        const Point3i&,  /* First operand */
        const int        /* Second operand */
    );


    friend bool operator ==
    (
        const Point3i&,  /* First operand */
        const Point3i&   /* Second operand */
    );

    friend bool operator !=
    (
        const Point3i&,  /* First operand */
        const Point3i&   /* Second operand */
    );
};



/*
    Point declarations
*/
static Point3i POINT_3I_0 = Point3i( 0, 0, 0 );
static Point3i POINT_3I_I = Point3i( 1, 1, 1 );
static Point3i POINT_3I_X = Point3i( 1, 0, 0 );
static Point3i POINT_3I_Y = Point3i( 0, 1, 0 );
static Point3i POINT_3I_Z = Point3i( 0, 0, 1 );
