#pragma once

#include <cstddef>  /* for NULL */


using namespace std;



struct Heap
{
    void** items = NULL;
    int count = 0;


    /*
        Destructor
    */
    ~Heap();



    /*
        Create and return heap
    */
    static Heap* create();



    /*
        Destroy heap
    */
    void destroy();


    /*
        Return Neurons list count elements
    */
    virtual int getCount();



    /*
        Add neurons from argument list to this list
    */
    Heap* add
    (
        Heap*
    );



    /*
        Return index by neuron
    */
    int indexBy
    (
        void*
    );



    /*
        Return neuron by index
    */
    void* getByIndex
    (
        int
    );



    /*
        Return neuron by index
    */
    Heap* setByIndex
    (
        int,
        void*
    );



    /*
        Resize
    */
    Heap* resize
    (
        int
    );



    /*
        Expand the heap
    */
    Heap* expand
    (
        int /* Size of expanding */
    );



    /*
        Expand the heap
    */
    Heap* push
    (
        void*
    );



    /*
        Remove one element and resize object
    */
    void* remove
    (
        int a
    );

};
