#pragma once

/*
    The Camera

    Class is describe the camera for payload.
    The camera can be applied to the Scene and changed the scenes's matrixes.
*/

#include <math.h>

#include "temporal_object.h"
#include "point3.h"

class Camera : public TemporalObject
{
    private:

        double          viewAngle       = M_PI * 0.5;
        double          sensivity       = 1.0;

    public:


        static Camera* create();



        void destroy();


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
};
