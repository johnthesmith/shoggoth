#pragma once

#include <functional>   /* for lyambda */

#include "rect2d.h"
#include "scene.h"
#include "rgba.h"

class Chart2d
{
    private:

        Rect2d& rect = RECT_2D_0;   /* Chart rect */

        double  xMin = -1.0;        /* Begin x axis */
        double  xMax = 1.0;
        double  yMin = -1.0;
        double  yMax = 1.0;

        Rgba backColor = Rgba( 0.4, 0.7, 1.0, 0.7 );
        Rgba lineColor = Rgba( 0.1, 0.2, 0.4, 0.7 );
        double lineWeight = 1.0;

    public:

        Chart2d();

        static Chart2d* create();
        void destroy();



        Chart2d* drawBack   /* Draw method */
        (
            Scene*
        );



        Chart2d* draw   /* Draw method */
        (
            Scene*,
            function <double ( double, double )>,    /* Lambda calculate function */
            double
        );



        Chart2d* drawX  /* Draw X value */
        (
            Scene*,
            double
        );



        Chart2d* drawY  /* Draw Y value */
        (
            Scene*,
            double
        );



        Chart2d* setCenterSize
        (
            const Point2d&,
            const Point2d&
        );



        Point2d chartToScreen
        (
            const Point2d& a
        );



        Chart2d* setBackColor
        (
            Rgba&
        );



        Chart2d* setLineColor
        (
            Rgba&
        );



        Chart2d* setXMin
        (
            double
        );



        Chart2d* setXMax
        (
            double
        );



        Chart2d* setYMin
        (
            double
        );



        Chart2d* setYMax
        (
            double
        );



        Chart2d* setLineWeight
        (
            double
        );
};
