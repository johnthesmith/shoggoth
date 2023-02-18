#pragma once

#include <cstring> /* NULL */



struct Neuron;



class Bind
{
    private:

        Neuron* parent  = NULL; /* Parent neuroin */
        Neuron* child   = NULL;  /* Child neuron */
        double weight   = 0.0;  /* Bind weight */

    public:

        /*
            set bind weight
        */
        Bind* setWeight
        (
            double
        );


        /*
            Return weight bind
        */
        double getWeight();



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

