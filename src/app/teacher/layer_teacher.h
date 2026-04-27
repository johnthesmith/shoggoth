/*
    Layer for teacher

    Extends dimantion layer for teacheng
    Contrins bitmab operations and ets
*/


#pragma once

#include <string>

#include "../../shoggoth/layer.h"
#include "../../../../../lib/graph/bitmap.h"
#include "../../../../../lib/core/hid.h"



using namespace std;



class LayerTeacher : public Layer
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
            real,     /* Rotate */
            real,     /* ZoomMin */
            real,     /* ZoomMax */
            real,     /* Shift */
            Result*,
            Rnd*
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
            /* Random seed */
            Rnd*,
            /* Min value */
            real,
            /* Max value */
            real
        );
};
