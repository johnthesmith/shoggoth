#pragma once

#include <cstring>
#include "bind.h"
#include "neuron_list.h"
#include "../heap.h"

/*
    List of Bind
*/


using namespace std;



struct BindList : public Heap
{
    int count = 0;



    /*
        Add points from argument list to this list
    */
    BindList* add
    (
        BindList*
    );



    /*
        Return index by neuron
    */
    int indexBy
    (
        Bind*
    );



    /*
        Return neuron by index
    */
    Bind* getByIndex
    (
        int
    );



    /*
        Return neuron by index
    */
    BindList* setByIndex
    (
        int,
        Bind*
    );



    /*
        Resize
    */
    BindList* resize
    (
        int
    );



    /*
        Resize
    */
    BindList* expand
    (
        int
    );
};
