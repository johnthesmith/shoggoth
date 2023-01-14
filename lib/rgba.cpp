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



///*
//    Define static constatns
//*/
//static Rgba RGBA_BLACK    = Rgba( 0.0, 0.0, 0.0, 1.0 );
//static Rgba RGBA_WHITE    = Rgba( 1.0, 1.0, 1.0, 1.0 );
//static Rgba RGBA_RED      = Rgba( 1.0, 0.0, 0.0, 1.0 );
//static Rgba RGBA_GREEN    = Rgba( 0.0, 1.0, 0.0, 1.0 );
//static Rgba RGBA_BLUE     = Rgba( 0.0, 0.0, 1.0, 1.0 );
//static Rgba RGBA_YELLOW   = Rgba( 1.0, 1.0, 0.0, 1.0 );
//static Rgba RGBA_MAGENTA  = Rgba( 1.0, 0.0, 1.0, 1.0 );
//static Rgba RGBA_CYAN     = Rgba( 0.0, 1.0, 1.0, 1.0 );
