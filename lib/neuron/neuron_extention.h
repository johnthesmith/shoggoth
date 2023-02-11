#pragma once

#include "../point3.h"
#include <string>

struct NeuronExtention
{
    Point3d world       = POINT_3D_0;
    Point3d screen      = POINT_3D_0;
    bool selected       = false;
    string id           = "";
};
