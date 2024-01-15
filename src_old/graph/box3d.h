#pragma once



#include "point3.h"



struct Box3d
{
    Point3d center  = POINT_3D_0;
    Point3d radius    = POINT_3D_0;



    /*
        Constructor
    */
    Box3d
    (
        const Point3d& = POINT_3D_0,
        const Point3d& = POINT_3D_0
    );



    /*
        Set border from center and radius
    */
    Box3d& setCenterRadius
    (
        const Point3d&,
        const Point3d&
    );




    Box3d& setCorners
    (
        Point3d&,
        Point3d&
    );



    const Box3d& calcCorners
    (
        Point3d&,    /* Left bottom */
        Point3d&,    /* Right bottom */
        Point3d&,    /* Right top */
        Point3d&     /* Left top */
    ) const;
};



static Box3d BOX_3D_0 = Box3d();
static Box3d BOX_3D_I = Box3d( POINT_3D_0, POINT_3D_I );
