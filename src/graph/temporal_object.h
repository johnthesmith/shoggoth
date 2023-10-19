#pragma once

/*
    The Object

    ---------*-------*------*-------> time
             begin   now    end
    Each components top, eye, target will be enterpolate
    from time function betweeb begin and end.
    Result will be load to Object eye, top and target components.
*/



#include "object.h"



class TemporalObject : public Object
{
    private:

        Point3d         eyeStart        = POINT_3D_0;
        Point3d         targetStart     = POINT_3D_0;
        Point3d         topStart        = POINT_3D_0;

        Point3d         eyeEnd          = POINT_3D_0;
        Point3d         targetEnd       = POINT_3D_0;
        Point3d         topEnd          = POINT_3D_0;

        long long       eyeTimeStart    = 0;
        long long       targetTimeStart = 0;
        long long       topTimeStart    = 0;

        long long       eyeTimeEnd      = 0;
        long long       targetTimeEnd   = 0;
        long long       topTimeEnd      = 0;

    public:

        /*
            Directive set eye attractor position
        */
        TemporalObject* setEyeEnd
        (
            const Point3d&,
            const long long
        );



        /*
            Directive set target attractor position
        */
        TemporalObject* setTargetEnd
        (
            const Point3d&,
            const long long
        );



        /*
            Directive set top attractor position
        */
        TemporalObject* setTopEnd
        (
            const Point3d&,
            const long long
        );



        /*
            Run time from now moment
        */
        TemporalObject* runTime();
};
