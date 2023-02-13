#include <iostream>

#include "chart.h"
#include "../rnd.h"



Chart2d::Chart2d()
{
}



Chart2d* Chart2d::create()
{
    return new Chart2d();
}



void Chart2d::destroy()
{
    delete this;
}


/*
    y
    ^
    |
    |
    |   p2-------------p3
    |    |             |
    |    |      *      |
    |    |      center |
    |   p1-------------p4
    |
    *-----------------------------> x
*/

Chart2d* Chart2d::drawBack
(
    Scene* aScene
)
{
    Point2d p1 = rect.center - rect.size;
    Point2d p2 = Point2d( p1.x, rect.center.y + rect.size.y );
    Point2d p3 = rect.center + rect.size;
    Point2d p4 = Point2d( p3.x, p1.y);

    aScene -> begin( QUAD ).color( backColor );
    aScene -> vertex( p1 ).vertex( p2 ).vertex( p3 ).vertex( p4 );
    aScene -> end();

    aScene -> setLineWidth( 1 );
    aScene -> begin( LOOP ).color( lineColor );
    aScene -> vertex( p1 ).vertex( p2 ).vertex( p3 ).vertex( p4 );
    aScene -> end();

    return this;
}



Chart2d* Chart2d::draw
(
    Scene* aScene,
    function <double ( double )> aCalc
)
{
    double step = ( xMax - xMin ) / ( rect.size.x * 2 );

    aScene -> setLineWidth( 1 );

    aScene -> begin( LINES  ).color( lineColor );;
    double x = xMin;
    while( x < xMax )
    {
        double y = aCalc( x );
        Point2d p = chartToScreen( Point2d( x, y ));
        aScene -> vertex( p );
        x = x + step;
    }
    aScene -> end();


    return this;
}



Chart2d* Chart2d::setCenterSize
(
    const Point2d& aCenter,
    const Point2d& aSize
)
{
    rect.setCenterSize( aCenter, aSize );
    return this;
}



Point2d Chart2d::chartToScreen
(
    const Point2d& a
)
{
    double chartWidth   = xMax - xMin;
    double screenWidth  = rect.size.x * 2;
    double chartHeight  = yMax - yMin;
    double screenHeight = rect.size.y * 2;

    double nx = ( a.x - xMin ) / chartWidth;
    double ny = ( a.y - yMin ) / chartHeight;

    nx = nx < xMin ? xMin : ( nx > xMax ? xMax : nx );
    ny = ny < yMin ? yMin : ( ny > yMax ? yMax : ny );

    auto result = Point2d
    (
        nx * screenWidth + rect.center.x - rect.size.x,
        ny * screenHeight + rect.center.y - rect.size.y
    );

    return result;
}



Chart2d* Chart2d::setBackColor
(
    Rgba& a
)
{
    backColor = a;
    return this;
}



Chart2d* Chart2d::setLineColor
(
    Rgba& a
)
{
    lineColor = a;
    return this;
}



Chart2d* Chart2d::setXMin
(
    double a
)
{
    xMin = a;
    return this;
}



Chart2d* Chart2d::setXMax
(
    double a
)
{
    xMax = a;
    return this;
}



Chart2d* Chart2d::setYMin
(
    double a
)
{
    yMin = a;
    return this;
}



Chart2d* Chart2d::setYMax
(
    double a
)
{
    yMax = a;
    return this;
}
