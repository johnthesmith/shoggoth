#pragma once

#include "point3.h"



/*
    List of points3d
*/

using namespace std;



struct Points3d
{
    Point3d* items = NULL;
    int count = 0;



    /*
        Return Neurons list count elements
    */
    virtual int getCount();



    /*
        Add points from argument list to this list
    */
    Points3d* add
    (
        Points3d*
    );



    /*
        Return index by neuron
    */
    int indexBy
    (
        Point3d*
    );



    /*
        Return neuron by index
    */
    Point3d* getByIndex
    (
        int
    );



    /*
        Return neuron by index
    */
    Points3d* setByIndex
    (
        int,
        Point3d&
    );



    /*
        Resize
    */
    Points3d* resize
    (
        int
    );
};
