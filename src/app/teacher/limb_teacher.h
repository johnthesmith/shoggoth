# pragma once

/*
    Limb - teacher
    Neuron net object for teacher.
*/



#include "../../shoggoth/limb.h"
#include "../../shoggoth/net.h"
#include "layer_teacher.h"


class Payload;


class LimbTeacher : public Limb
{
    private:

        /* Net object */
        Net*            net     = NULL;
        /* Master payload */
        Payload*        payload = NULL;

    public:

        /*
            Constructor
        */
        LimbTeacher
        (
            /* Master payload */
            Payload*,
            /* Net limb object*/
            Net*
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
            /* Master payload */
            Payload* aPyaload,
            /* The net object*/
            Net* aNet
        )
        {
            return new LimbTeacher( aPyaload, aNet );
        }



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
