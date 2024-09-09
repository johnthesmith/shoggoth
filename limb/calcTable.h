/*
    The calculation table class for layers
*/
# pragma once

#include "net.h"


/*
    Calculation item
*/
class CalcItem
{
    private:
        /* The net object */
        Net*    net         = NULL,

        /* Layer for this item */
        Layer*  loop        = NULL,
        bool    calculated  = false
    public:
        /*
            Check the layer is calculates
            Return true when layer in this item is calculated
        */
        bool isCalculated();



        /*
            Return true if all parents layers by nerves is calculated
        */
        bool isParentCalculated();



        /*
            Return true if all children layers by nerves is calculated
        */
        bool isChildrenCalculated();



        /*
            Set calculated at true
        */
        CalcItem* calculated()
}



/*
    Calcualteion table
*/
class CalcTable
{
    public:
        static loop
        (
            Net*,
            function <bool ( CalcItem*, Item* )>
        );
}



