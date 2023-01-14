#pragma once

#include "result.h"
#include "point3.h"

class Camera : Result
{
    private:

        /* Camera settings */
        Point3          eye             = VECTOR_3D_I;
        Point3          top             = VECTOR_3D_Y;
        Point3          target          = VECTOR_3D_0;

        bool            eyeLock         = false;    /* Lock eye position */
        bool            topLock         = false;    /* Lock top direction */
        bool            targetLock      = false;    /* Lock target position */
        bool            distanceLock    = true;     /* Lock distance between eye and target*/

    public:

        /*
            Directive set eye position
        */
        Camera& setEye
        (
            const Point3&
        );



        /*
            Directive set eye position
        */
        Camera& moveEye
        (
            const Point3&
        );



        /*
            Return eye position
        */
        Point3& getEye();



        /*
            Set top normal vector
        */
        Camera& setTop
        (
            const Point3&
        );



        /*
            Move top vector with normalize
        */
        Camera& moveTop
        (
            const Point3&
        );



        /*
            Seteye position
        */
        Point3& getTop();



        /*
            Set eye position
        */
        Camera& setTarget
        (
            const Point3&
        );



        /*
            Move camera target with normalize
        */
        Camera& moveTarget
        (
            const Point3&
        );


        /*
            Seteye position
        */
        Point3& getTarget();



        /*
            Normalize camera vectors and position
        */
        Camera& norm();



        /*
        Set top normal vector
        */
        Camera& place
        (
            const Point3& aEye = VECTOR_3D_X,
            const Point3& aTarget = VECTOR_3D_0,
            const Point3& aTop  = VECTOR_3D_Y
        );



        /*
            Shift camera at 3d
        */
        Camera& shift
        (
            const Point3&
        );



        /*
            Shift camera at screen
        */
        Camera& shift
        (
            const double,
            const double
        );

};
