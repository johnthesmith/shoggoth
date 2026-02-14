/*
    Layer for Processor

    Extends dimantion layer for teacheng
    Contrins bitmab operations and ets
*/


#pragma once

#include <string>

#include "../../shoggoth/layer.h"



using namespace std;



class LayerProcessor : public Layer
{
    private:


    public:

        /*
            Constructor
        */
        LayerProcessor
        (
            Limb*,          /* Limb object */
            string = ""     /* id */
        );



        /*
            Internal destructor
            WARNING!!! Use LayerProcessor -> destroy() only
        */
        ~LayerProcessor();



        /*
            Creator
        */
        static LayerProcessor* create
        (
            Limb*,          /* Limb object */
            string = ""
        );



        /*
            Destructor
        */
        void destroy();


        /**********************************************************************
            Specific methods
        */
};
