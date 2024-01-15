#pragma once

#include <string>

#include "../lib/log.h"

#include "point3.h"
#include "point4d.h"
#include "matrix.h"


class LogPoints
{
    public:
        static void write( Log*, const Point3i&, const string = "" );
        static void write( Log*, const Point3d&, const string = "" );
        static void write( Log*, const Point4d&, const string = "" );
        static void write( Log*, const Matrix4&, const string = "" );
};
