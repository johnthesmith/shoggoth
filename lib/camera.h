#pragma once

#include <math.h>

#include "result.h"
#include "point3.h"

class Camera : Result
{
    private:

        /* Camera settings */
        Point3          eye             = VECTOR_3D_0;  /* Set on begin of coordinates */
        Point3          target          = VECTOR_3D_Z;  /* Look at far */
        Point3          top             = VECTOR_3D_Y;  /* And top on top */

        double          near            = 0.0;
        double          far             = 1.0;
        double          viewAngle       = M_PI * 0.5;

        bool            eyeLock         = false;    /* Lock eye position */
        bool            topLock         = false;    /* Lock top direction */
        bool            targetLock      = false;    /* Lock target position */
        bool            gazeLock        = true;     /* Lock distance between eye and target*/

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



        /*
            Set far clipping
        */
        Camera& setFar
        (
            double
        );



        /*
            Return far clipping
        */
        double getFar();



        /*
            Set near clipping
        */
        Camera& setNear
        (
            double
        );



        /*
            Return near clipping
        */
        double getNear();



        /*
             Set angle of view in radians
        */
        Camera& setAngleView
        (
            double  /* */
        );



        /*
             Return angle of view in radians
        */
        double getAngleView();
};
