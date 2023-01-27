/*
    Camera
*/

#include <iostream>

#include "camera.h"
#include "matrix.h"



/*
    Set angle of view in radians
*/
Camera& Camera::setAngleView
(
    const double a
)
{
    viewAngle = a;
    return *this;
}



/*
    Return angle of view in radians
*/
double Camera::getAngleView()
{
    return viewAngle;
}
