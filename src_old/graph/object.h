#pragma once

/*
    The Object

    Class is describe the Object for payload.
    The Object can be applied to the Scene and changed the scenes's matrixes.

    The class has a:
    - eye - point of view
    - target - point for Front direction
    - top - normalized vector for top directio

    Each point and vector can blocked

                      O target         ^
                     /                /
                    /                /
           top o   /                /
               |  c front          / gase
               | /                /
               |/                /
    left c-----0-----c right    0
              /|eye
             / |
       back c  |
               c bottom

*/

#include <math.h>

#include "../lib/result.h"
#include "point3.h"

class Object : public Result
{
    private:

        /* Object settings */
        Point3d         eye             = POINT_3D_I;  /* Position of object */
        Point3d         target          = POINT_3D_0;  /* Look at far */
        Point3d         top             = POINT_3D_Y;  /* And top */

        bool            changed         = true;

        bool            eyeLock         = false;    /* Lock eye position */
        bool            topLock         = false;    /* Lock top direction */
        bool            targetLock      = false;    /* Lock target position */
        bool            gazeLock        = true;     /* Lock distance between eye and target*/

        double          sensivity       = 1.0;

    public:

        /*
            Directive set eye position
        */
        Object* setEye
        (
            const Point3d&
        );



        /*
            Directive set eye position
        */
        Object& moveEye
        (
            const Point3d&
        );



        /*
            Return eye position
        */
        Point3d& getEye();



        /*
            Set top normal vector
        */
        Object* setTop
        (
            const Point3d&
        );



        /*
            Move top vector with normalize
        */
        Object& moveTop
        (
            const Point3d&
        );



        /*
            Seteye position
        */
        Point3d& getTop();



        /*
            Set eye position
        */
        Object& setTarget
        (
            const Point3d&
        );



        /*
            Move Object target with normalize
        */
        Object& moveTarget
        (
            const Point3d&
        );


        /*
            Seteye position
        */
        Point3d& getTarget();



        /*
            Normalize Object vectors and position
        */
        Object& norm();



        /*
        Set top normal vector
        */
        Object& place
        (
            const Point3d& = POINT_3D_X,    /* Eye */
            const Point3d& = POINT_3D_0,    /* Target */
            const Point3d& = POINT_3D_Y     /* Top */
        );



        /*
            Shift eye and target at 3d
        */
        Object& shift
        (
            const Point3d&
        );



        /*
            Set eye lock
        */
        Object& setEyeLock
        (
            const bool = false /* value */
        );



        /*
            Return far clipping
        */
        bool getEyeLock();



        /*
            Set eye lock
        */
        Object& setTargetLock
        (
            const bool = false /* value */
        );



        /*
            Return far clipping
        */
        bool getTargetLock();




        /*
            Return Front direction
        */
        Point3d getFront();



        /*
            Return Back direction
        */
        Point3d getBack();



        /*
            Return the Right direction
        */
        Point3d getRight();



        /*
            Return the Gaze vector
        */
        Point3d getGaze();




        Object& setViewMatrixTo
        (
            Matrix4& a
        );



        Object& zoom
        (
            double
        );




        /*
            Rotate eye around vector
        */
        Object& rotateEye
        (
            const Point3d& aBase,    /* Base */
            const double aAngleRad  /* Rotation angle at radians */
        );



        /*
            Rotate top around vector
        */
        Object& rotateTop
        (
            const Point3d& aBase,    /* Base */
            const double aAngleRad  /* Rotation angle at radians */
        );



        /*
            Rotate target around vector
        */
        Object& rotateTarget
        (
            const Point3d& aBase,    /* Base */
            const double aAngleRad  /* Rotation angle at radians */
        );



        bool getChanged();



        Object& setChanged
        (
            bool
        );



        /*
            Compare this object and argument object
        */
        bool compare
        (
            Object*
        );
};
