#pragma once

#include "point2d.h"
#include "point3.h"


struct Rect2d
{
    Point2d center  = POINT_2D_0;
    Point2d size    = POINT_2D_0;



    Rect2d& setCenterSize
    (
        const Point2d&,
        const Point2d&
    );



    Rect2d& setCenterSize
    (
        const Point3d&,
        const Point2d&
    );



    Rect2d& setCorners
    (
        Point2d&,
        Point2d&
    );


    const Rect2d& calcCorners
    (
        Point2d&,    /* Left bottom */
        Point2d&,    /* Right bottom */
        Point2d&,    /* Right top */
        Point2d&     /* Left top */
    ) const;
};



static Rect2d RECT_2D_0 = Rect2d();
