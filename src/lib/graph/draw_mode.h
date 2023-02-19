#pragma once



/*
    Draw modes
*/
enum DrawMode
{
    POINT,      /* p1 p2 p3 ... */
    LINE,       /* p1-p2 p3-p4 ... */
    LINES,      /* p1-p2-p3-... */
    LOOP,       /* p1-p2-p3-...-[p1] */
    POLYGON,    /* p1-p2-p3-...-[p1] */
    QUAD,       /* p1-p2-p3-p4 p5-p6-p7-p8 ... */
    QUADS,      /* p1-p2-p3-p4-p5-p6-p7-p8 ... */
    TRIANGLE,   /* p1-p2-p3 p4-p5-p6 ... */
    TRIANGLES,
    FAN
};



enum PolygonMode
{
    POLYGON_POINT,
    POLYGON_LINE,
    POLYGON_FILL
};
