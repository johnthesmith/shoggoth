#pragma once

#include <string>
#include "log.h"
#include "point3.h"
#include "point4.h"
#include "matrix.h"


class LogPoints
{
    public:
        static void write( Log&, const Point3&, const string = "" );
        static void write( Log&, const Point4&, const string = "" );
        static void write( Log&, const Matrix4&, const string = "" );
};
