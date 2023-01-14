#pragma once

#include <string>
#include "log.h"
#include "point3.h"
#include "point4.h"
#include "matrix.h"


class LogPoints
{
    public:
        static void write( Log&, Point3&, string = "" );
        static void write( Log&, Point4&, string = "" );
        static void write( Log&, Matrix4&, string = "" );
};
