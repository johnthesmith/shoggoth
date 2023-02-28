#pragma once

#include <cstring> /* NULL */



struct Neuron;


/*
    Type of bind
*/
enum BindType
{
    BT_VALUE,       /* Spred the value of neuron and error on reverse move */
    BT_SAMPLE,      /* Spred the value from sample */
    BT_COMMAND      /* Spred the command value from sample */
};



class Bind
{
    private:

        Neuron*     parent  = NULL;     /* Parent neuroin */
        Neuron*     child   = NULL;     /* Child neuron */
        double      weight  = 0.0;      /* Bind weight */
        BindType    type    = BT_VALUE; /* Type of bind */

    public:

        /*
            set bind weight
        */
        Bind* setWeight
        (
            double  /* Value */
        );



        /*
            Return weight bind
        */
        double getWeight();



        /*
            set bind type
        */
        Bind* setType
        (
            BindType
        );



        /*
            Return type of bind
        */
        BindType getType();



        /*
            Set bind parent
        */
        Bind* setParent
        (
            Neuron*
        );




        /*
            Return parent neuron
        */
        Neuron* getParent();




        /*
            set bind child
        */
        Bind* setChild
        (
            Neuron* /* Child neuron */
        );



        /*
            Return child neuron
        */
        Neuron* getChild();

};

