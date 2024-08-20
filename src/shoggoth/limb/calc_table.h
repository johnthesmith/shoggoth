/*
    Layer calculation table
*/
# pragma once

#include <functional>   /* for lyambda */

#include "./limb_processor.h"
#include "../layer.h"






/*
    Predeclaration
*/
class CalcTable;


/*
    Events callback
*/
typedef function <bool ( CalcTable*, Layer* )> CalcTableLoopCallback;




/*
    Record of calculation table
*/
class CalcRecord
{
    private:

        bool calculated = false;
        Layer* layer    = NULL;

    public:


        /*
            Confirm layer calcilated
        */
        CalcRecord* setCalculated();



        /*
            Return the calculated flag
        */
        bool getCalculated();



        /*
            Set layer for record
        */
        CalcRecord* setLayer
        (
            Layer*
        );



        /*
            Return the layer
        */
        Layer* getLayer();
};



/*
    Calculation table
*/
class CalcTable
{
    private:

        LimbProcessor* limb = NULL;
        vector <CalcRecord> table;

    public:

        CalcTable
        (
            /* The Net object */
            LimbProcessor*
        );



        /*
            Create calc table
        */
        static CalcTable* create
        (
            /* The Net object */
            LimbProcessor*
        );



        /*
            Destroy calc table
        */
        void destroy();



        /*
            Loop for all notcalculated layers
        */
        CalcTable* loop
        (
            /* Calback function */
            CalcTableLoopCallback
        );



        /*
            Return true if all parents of the layer has a calculated flag
        */
        bool isParentsCalculated
        (
            Layer*
        );



        /*
            Return true if all children of the layer has a calculated flag
        */
        bool isChildrenCalculated
        (
            Layer*
        );



        /*
            Return true if layer is calculated
        */
        bool isCalculated
        (
            Layer* aLayer
        );
};
