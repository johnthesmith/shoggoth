#pragma once

#include <cstddef>      /* for NULL */
#include <functional>   /* for lyambda */
#include "result.h"     /* for extends */



using namespace std;



class Heap : public Result
{
    private:

        void** items = NULL;
        int count = 0;

        Heap* setItems
        (
            void **
        );

    public:


        /*
            Constructor
        */
        Heap();



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
        virtual void destroy();



        /*
            Return Neurons list count elements
        */
        virtual int getCount();



        /*
            Return Neurons list count elements
        */
        virtual void** getItems();



        /*
            Add neurons from argument list to this list
        */
        Heap* add
        (
            Heap*
        );



        /*
            Return index by element
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
            int = 0
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
            int
        );


        /*
            Remove one element and resize object
        */
        void* remove
        (
            void*
        );



        /*
            Remove elements by lambda condition
        */
        Heap* remove
        (
            function <bool ( void* )>,
            Heap* = NULL                  /* Removind. Warning!!! Must be empty */
        );



        /*
            Remove all elements form  this,
            contains in the argument
        */
        virtual Heap* remove
        (
            Heap* aRemove
        );



        /*
            Remove some elememnts from start index
        */
        Heap* bite
        (
            int,    /* Strat index */
            int     /* Count for bite */
        );



        /*
            Loop with lyambda
        */
        Heap* loop
        (
            function <bool ( void* )>
        );



        /*
            Return first element or null if not exists
        */
        void* getFirst();



        /*
            Merge two heaps
            Each elemento of Argument heap will add to This heap
            if it not exists in This
        */
        virtual Heap* merge
        (
            Heap* /* Heap for merging */
        );



        /*
            Merge two heaps
            Each elemento of Argument heap will add to This heap
            if it not exists in This
        */
        virtual Heap* merge
        (
            Heap*, /* Heap for merging */
            function <bool ( void* )>
        );
};
