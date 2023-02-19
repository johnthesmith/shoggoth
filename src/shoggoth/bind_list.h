#pragma once

#include <cstring>

#include "../lib/heap.h"

#include "bind.h"
#include "neuron_list.h"



/*
    List of Bind
*/


using namespace std;



struct BindList : public Heap
{
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



    /*
        Loop with lyambda
    */
    BindList* loop
    (
        function <bool ( Bind* )>
    );
};
