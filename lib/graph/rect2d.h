#pragma once

#include "point2d.h"



struct Rect2d
{
    Point2d center  = POINT_2D_0;
    Point2d size    = POINT_2D_0;



    Rect2d& setCenterSize
    (
        const Point2d&,
        const Point2d&
    );



    Rect2d& setCorners
    (
        Point2d&,
        Point2d&
    );
};



static Rect2d RECT_2D_0 = Rect2d();
