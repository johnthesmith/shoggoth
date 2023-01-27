#pragma once

/*
    The Object

    Class is describe the Object for payload.
    The Object can be applied to the Scene and changed the scenes's matrixes.
*/

#include <math.h>

#include "result.h"
#include "point3.h"

class Object : public Result
{
    private:

       /* Object settings */
        Point3          eye             = VECTOR_3D_I;  /* Set on begin of coordinates */
        Point3          target          = VECTOR_3D_0;  /* Look at far */
        Point3          top             = VECTOR_3D_Y;  /* And top on top */

        bool            eyeLock         = false;    /* Lock eye position */
        bool            topLock         = false;    /* Lock top direction */
        bool            targetLock      = false;    /* Lock target position */
        bool            gazeLock        = true;     /* Lock distance between eye and target*/

        double          sensivity       = 1.0;

    public:

        /*
            Directive set eye position
        */
        Object& setEye
        (
            const Point3&
        );



        /*
            Directive set eye position
        */
        Object& moveEye
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
        Object& setTop
        (
            const Point3&
        );



        /*
            Move top vector with normalize
        */
        Object& moveTop
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
        Object& setTarget
        (
            const Point3&
        );



        /*
            Move Object target with normalize
        */
        Object& moveTarget
        (
            const Point3&
        );


        /*
            Seteye position
        */
        Point3& getTarget();



        /*
            Normalize Object vectors and position
        */
        Object& norm();



        /*
        Set top normal vector
        */
        Object& place
        (
            const Point3& = VECTOR_3D_X,    /* Eye */
            const Point3& = VECTOR_3D_0,    /* Target */
            const Point3& = VECTOR_3D_Y     /* Top */
        );



        /*
            Shift eye and target at 3d
        */
        Object& shift
        (
            const Point3&
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
        Point3 getFront();



        /*
            Return Back direction
        */
        Point3 getBack();



        /*
            Return the Right direction
        */
        Point3 getRight();



        /*
            Return the Gaze vector
        */
        Point3 getGaze();




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
            const Point3& aBase,    /* Base */
            const double aAngleRad  /* Rotation angle at radians */
        );



        /*
            Rotate top around vector
        */
        Object& rotateTop
        (
            const Point3& aBase,    /* Base */
            const double aAngleRad  /* Rotation angle at radians */
        );



        /*
            Rotate target around vector
        */
        Object& rotateTarget
        (
            const Point3& aBase,    /* Base */
            const double aAngleRad  /* Rotation angle at radians */
        );
};
