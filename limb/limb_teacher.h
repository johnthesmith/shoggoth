# pragma once

/*
    Limb - teacher
    Neuron net object for teacher.
*/



#include "../limb.h"
#include "net.h"
#include "layer_teacher.h"



class LimbTeacher : public Limb
{
    private:

        /* Net object */
        Net*            net             = NULL;

    public:

        /*
            Constructor
        */
        LimbTeacher
        (
            Net*    /* Net limb object*/
        );



        /*
            Destructor
        */
        ~LimbTeacher();



        /*
            Create
        */
        static LimbTeacher* create
        (
            Net* /* The net object*/
        );



        /*
            Create a new layer for this limb and copy parameters from source layer.
            This method is an overridden method of the Limb class.
        */
        Layer* copyLayerFrom
        (
            Layer*  /* Source layer */
        );



        /*
            Return layer by id or NULL for not exists
        */
        LayerTeacher* getLayerById
        (
            string
        );



        /*
            Return net object
        */
        Net* getNet();

};
