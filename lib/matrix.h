#pragma once

/*
    4d matrix

       |  x   y   z   w
    ---|-----------------
    l1 | ax, ay, az, aw
    l2 | bx, by, bz, bw
    l3 | cx, cy, cz, cw
    l4 | dx, dy, dz, dw
*/


struct Point4;


struct Matrix4
{
    Point4 l1;
    Point4 l2;
    Point4 l3;
    Point4 l4;

    Matrix4
    (
        Point4 = Point4(),
        Point4 = Point4(),
        Point4 = Point4(),
        Point4 = Point4()
    );

    Matrix4& identity();

    Matrix4& from
    (
        Matrix4&
    );

    Matrix4& shift
    (
        const Point4&
    );

    Matrix4& scale
    (
        const Point4&
    );

    Matrix4& scale
    (
        const double
    );

    /*
        Rotate around Point axis at angle
        https://vlab.fandom.com/ru/wiki/Матрица_поворота
    */
    Matrix4& rotate
    (
        const Point4& aBase,    /* Base axis for rotation, must be norm */
        const float aAngle      /* Angle of rotation */
    );

    /*
        Build rotate matrix around three axis
    */
    Matrix4& rotate
    (
        const float z = 0,  /* Angle around x axis */
        const float y = 0,  /* Angle around y axis */
        const float x = 0   /* Angle around z axis */
    );


    Matrix4& ortho
    (
        double, /* Left ege */
        double, /* Right ege */
        double, /* Bottom ege */
        double, /* Top ege */
        double, /* Near ege */
        double  /* Far eage */
    );



    Matrix4& look
    (
        const Point3&,
        const Point3&
    );



    Matrix4& dot
    (
        Matrix4& a,
        Matrix4& b
    );



    string toString();


};
