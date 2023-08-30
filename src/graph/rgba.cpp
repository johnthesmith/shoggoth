#include <cmath>

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
  Constructor
*/
Rgba::Rgba
(
    const Rgba& a
)
{
    red     = a.red;
    green   = a.green;
    blue    = a.blue;
    alpha   = a.alpha;
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



/*
    Get grey
*/
double Rgba::getGray()
{
    return ( red + green + blue ) / 3.0;
}



/*
    Linear interpolation
*/
Rgba& Rgba::itpLin
(
    const Rgba& a,
    const double aTime
)
{
    red     += ( a.red - red ) * aTime;
    green   += ( a.green - green ) * aTime;
    blue    += ( a.blue - blue ) * aTime;
    alpha   += ( a.alpha - alpha ) * aTime;
    return *this;
}



/*
    Tangens interpolation
*/
Rgba& Rgba::itpSgm
(
    const Rgba& a,
    const double aTime
)
{
    itpLin( a, 1.0 / ( 1.0 + pow( M_E, ( -aTime + 0.5 ) * 10 ) ));
    return *this;
}

