/*
    Temporal Object
*/


#include "temporal_object.h"
#include "../lib/utils.h"



/*
    Directive set eye attractor position
*/
TemporalObject* TemporalObject::setEyeEnd
(
    const Point3d& aPoint,
    const long long aTime
)
{
    eyeStart = getEye();
    eyeEnd = aPoint;
    eyeTimeStart = now();
    eyeTimeEnd = eyeTimeStart + aTime;
    return this;
}



/*
    Directive set target attractor position
*/
TemporalObject* TemporalObject::setTargetEnd
(
    const Point3d& aPoint,
    const long long aTime
)
{
    targetStart = getTarget();
    targetEnd = aPoint;
    targetTimeStart = now();
    targetTimeEnd = targetTimeStart + aTime;
    return this;
}



/*
    Directive set top attractor position
*/
TemporalObject* TemporalObject::setTopEnd
(
    const Point3d& aPoint,
    const long long aTime
)
{
    topStart = getTop();
    topEnd = aPoint;
    topTimeStart = now();
    topTimeEnd = topTimeStart + aTime;
    return this;
}



/*
    Run time from now moment
*/
TemporalObject* TemporalObject::runTime()
{
    auto t =
    double ( now() - topTimeStart ) /
    double ( topTimeEnd - topTimeStart );

    t = t < 0 ? 0 : ( t > 1 ? 1 : t);

//    setEye( eyeStart.itpLin( eyeEnd, t ));
    return this;
}
