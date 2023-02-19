#pragma once

/*
    4d matrix

       | A     | B     | C     | D
    ---|----------------------------
     x | ax  0 | bx  1 | cx  2 | dx  3
     y | ay  4 | by  5 | cy  6 | dy  7
     z | az  8 | bz  9 | cz 10 | dz 11
     t | aw 12 | bw 13 | cw 14 | dw 15
*/


#define M_AX 0
#define M_AY 4
#define M_AZ 8
#define M_AW 12

#define M_BX 1
#define M_BY 5
#define M_BZ 9
#define M_BW 13

#define M_CX 2
#define M_CY 6
#define M_CZ 10
#define M_CW 14

#define M_DX 3
#define M_DY 7
#define M_DZ 11
#define M_DW 15


struct Point4d;


struct Matrix4
{
    double m[16];


    Matrix4
    (
        double = 0, double = 0, double = 0, double = 0,
        double = 0, double = 0, double = 0, double = 0,
        double = 0, double = 0, double = 0, double = 0,
        double = 0, double = 0, double = 0, double = 0
    );


    Matrix4& identity();

    Matrix4& from
    (
        Matrix4&
    );

    Matrix4& shift
    (
        const Point4d&
    );

    Matrix4& scale
    (
        const Point4d&
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
        const Point4d& aBase,    /* Base axis for rotation, must be norm */
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
        const Point3d&,
        const Point3d&
    );



    Matrix4& dot
    (
        Matrix4& a,
        Matrix4& b
    );



    string toString();



    Matrix4& perspective
    (
        double aAngle,  /* Vertical Angle in rad */
        double aRatio,
        double aNear,
        double aFar
    );


};


static Matrix4 MATRIX_4D_I = Matrix4
(
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0
);


static Matrix4 MATRIX_4D_0 = Matrix4
(
    0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0
);


