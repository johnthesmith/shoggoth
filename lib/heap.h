#pragma once



using namespace std;



struct Heap
{
    void** items = NULL;
    int count = 0;



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
};
