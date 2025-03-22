/*
    Layer calculation table
*/
# pragma once

#include "../../shoggoth/layer.h"
#include "../../shoggoth/nerve_list.h"
#include "../../../../../lib/core/log.h"



/*
    Predeclaration
*/
class CalcTable;




/*
    Record of calculation table
*/
class CalcRecord
{
    public:

        bool calculated = false;
        Layer* layer    = NULL;



        /*
            Confirm layer calcilated
        */
        CalcRecord* setCalculated()
        {
            calculated = true;
            return this;
        }




        /*
            Reset layer calcilated
        */
        CalcRecord* resetCalculated()
        {
            calculated = false;
            return this;
        }



        /*
            Return the calculated flag
        */
        inline bool getCalculated()
        {
            return calculated;
        };




        /*
            Set layer for record
        */
        CalcRecord* setLayer
        (
            Layer* aLayer
        )
        {
            layer = aLayer;
            calculated = false;
            return this;
        }



        /*
            Return the layer
        */
        Layer* getLayer()
        {
            return layer;
        }

};



/*
    Calculation table
*/
class CalcTable
{
    private:

        vector <CalcRecord> table;

    public:

        CalcTable();



        /*
            Create calc table
        */
        static CalcTable* create();



        /*
            Destroy calc table
        */
        void destroy();



        CalcTable* addLayer
        (
            Layer*
        );



        CalcTable* reset();



        /*
            Loop for all notcalculated layers
            Fills a CalcRecord vector with data from alimb and calls
            aCallback for each unprocessed layer until all layers are processed.
        */
        template <typename Func>
        CalcTable* loop
        (
            /*
                Callback function
                bool ( CalcTable*, Layer*, bool& )
            */
            Func aCallback
        )
        {
            /* Count of layer calculated */
            int calculatedCount = 0;
            auto layersCount = table.size();
            auto terminate = false;

            /* Main loop */
            do
            {
                calculatedCount = 0;
                for( int i = 0; i < layersCount && !terminate; i++ )
                {
                    if
                    (
                        ! table[ i ].getCalculated() &&
                        aCallback( this, table[ i ].getLayer(), terminate )
                    )
                    {
                        table[ i ].setCalculated();
                        calculatedCount ++;
                    }
                }
            }
            while( calculatedCount > 0 && !terminate );
            return this;
        }



        /*
            Return true if all parents of the layer has a calculated flag
        */
        bool isParentsCalculated
        (
            Layer*,
            NerveList*
        );



        /*
            Return true if all children of the layer has a calculated flag
        */
        bool isChildrenCalculated
        (
            Layer*,
            NerveList*
        );



        /*
            Return true if layer is calculated
        */
        bool isCalculated
        (
            Layer* aLayer
        );



        CalcTable* sortForward
        (
            NerveList*
        );



        CalcTable* sortBackward
        (
            NerveList*
        );




        CalcTable* dump
        (
            Log*,
            /* Dump header */
            string
        );



        CalcTable* clear();
};
