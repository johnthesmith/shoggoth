#include "rgba.h"



/*
  Constructor
*/
Rgba::Rgba
(
    float aRed,
    float aGreen,
    float aBlue,
    float aAlpha
)
{
    red     = aRed;
    green   = aGreen;
    blue    = aBlue;
    alpha   = aAlpha;
}



/*
    Set color components for exists object
*/
Rgba& Rgba::set
(
    float aRed,
    float aGreen,
    float aBlue,
    float aAlpha
)
{
    red     = aRed;
    green   = aGreen;
    blue    = aBlue;
    alpha   = aAlpha;

    return *this;
}



/*
    Set Red component
*/
Rgba& Rgba::setRed
(
    float a
)
{
    red = a;
    return *this;
}



/*
    Set Green component
*/
Rgba& Rgba::setGreen
(
    float a
)
{
    green = a;
    return *this;
}



/*
    Set Blue component
*/
Rgba& Rgba::setBlue
(
    float a
)
{
    blue = a;
    return *this;
}



/*
    Set Alpha component
*/
Rgba& Rgba::setAlpha
(
    float a
)
{
    alpha = a;
    return *this;
}
