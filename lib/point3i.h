#pragma once

struct Point3i
{
    int x = 0;  /* x */
    int y = 0;  /* y */
    int z = 0;  /* z */

    Point3i
    (
        int = 0,
        int = 0,
        int = 0
    );
};


static Point3i VECTOR_3I_0 = Point3i( 0, 0, 0 );
