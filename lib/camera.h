#pragma once

/*
    The Camera

    Class is describe the camera for payload.
    The camera can be applied to the Scene and changed the scenes's matrixes.
*/

#include <math.h>

#include "result.h"
#include "point3.h"

class Camera : Result
{
    private:

       /* Camera settings */
        Point3          eye             = VECTOR_3D_I;  /* Set on begin of coordinates */
        Point3          target          = VECTOR_3D_0;  /* Look at far */
        Point3          top             = VECTOR_3D_Y;  /* And top on top */

        double          viewAngle       = M_PI * 0.5;

        bool            eyeLock         = false;    /* Lock eye position */
        bool            topLock         = false;    /* Lock top direction */
        bool            targetLock      = false;    /* Lock target position */
        bool            gazeLock        = true;     /* Lock distance between eye and target*/

        double          sensivity       = 1.0;

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
            const Point3& = VECTOR_3D_X,    /* Eye */
            const Point3& = VECTOR_3D_0,    /* Target */
            const Point3& = VECTOR_3D_Y     /* Top */
        );



        /*
            Shift eye and target at 3d
        */
        Camera& shift
        (
            const Point3&
        );



        /*
            Set eye lock
        */
        Camera& setEyeLock
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
        Camera& setTargetLock
        (
            const bool = false /* value */
        );



        /*
            Return far clipping
        */
        bool getTargetLock();



        /*
            Return near clipping
        */
        double getNear();



        /*
             Set angle of view in radians
        */
        Camera& setAngleView
        (
            const double /* value */
        );



        /*
             Return angle of view in radians
        */
        double getAngleView();



        /*
            Return Front direction
        */
        Point3 getFront();



        /*
            Return Back direction
        */
        Point3 getBack();



        /*
            Return the Right direction
        */
        Point3 getRight();




        Camera& setViewMatrixTo
        (
            Matrix4& a
        );


        
};
