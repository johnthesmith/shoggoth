/*
    Layer for teacher

    Extends dimantion layer for teacheng
    Contrins bitmab operations and ets
*/


#pragma once

#include <string>

#include "../layer_dim.h"
#include "../../../../../lib/graph/bitmap.h"
#include "../../../../../lib/core/hid.h"



using namespace std;



class LayerTeacher : public LayerDim
{
    private:


    public:

        /*
            Constructor
        */
        LayerTeacher
        (
            Limb*,          /* Limb object */
            string = ""     /* id */
        );



        /*
            Internal destructor
            WARNING!!! Use LayerTeacher -> destroy() only
        */
        ~LayerTeacher();



        /*
            Creator
        */
        static LayerTeacher* create
        (
            Limb*,          /* Limb object */
            string = ""
        );



        /*
            Destructor
        */
        void destroy();


        /**********************************************************************
            Load valus
        */



        /*
            Set bitmap to neurons
        */
        LayerTeacher* bitmapToValue
        (
            Bitmap*
        );



        /*
            Load bitmap from source file name and set it
        */
        LayerTeacher* imageToValue
        (
            string,     /* File name */
            double,     /* Rotate */
            double,     /* ZoomMin */
            double,     /* ZoomMax */
            double,     /* Shift */
            Result*
        );




        LayerTeacher* applyUuid
        (
            Hid
        );



        /*
            Noise fill values of layer neurons
        */
        LayerTeacher* noiseValue
        (
            int,        /* Random seed */
            double,     /* Min value */
            double      /* Max value */
        );



        /*
            Noise fill values of layer neurons
        */
        LayerTeacher* fillValue
        (
            ParamList*
        );
};
