/*
    Camera
*/

#include <iostream>

#include "camera.h"
#include "matrix.h"



Camera* Camera::create()
{
    return new Camera();
}



void Camera::destroy()
{
    delete this;
}



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
