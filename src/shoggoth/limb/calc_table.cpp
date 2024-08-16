#include "calc_table.h"




/*
    Confirm layer calcilated
*/
CalcRecord* CalcRecord::setCalculated()
{
    calculated = true;
    return this;
}



/*
    Return the calculated flag
*/
bool CalcRecord::getCalculated()
{
    return calculated;
}



/*
    Set layer for record
*/
CalcRecord* CalcRecord::setLayer
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
Layer* CalcRecord::getLayer()
{
    return layer;
}






/******************************************************************************
    Calculation table
*/

CalcTable::CalcTable
(
    /* The Net object */
    Net* aNet
)
:
    net( aNet ),
    table( aNet -> getLayerList() -> getCount() )
{
    auto layers = net -> getLayerList();
    /* Load Calc records */
    for( int i = 0; i < layers -> getCount(); i++ )
    {
        table[ i ].setLayer(( Layer* )( layers -> getByIndex( i )));
    }
}



/*
    Create calc table
*/
CalcTable* CalcTable::create
(
    /* The Net object */
    Net* aNet
)
{
    return new CalcTable( aNet );
}



/*
    Selfdestructor
*/
void CalcTable::destroy()
{
    delete this;
}



/*
    Return true if layer is calculated
*/
bool CalcTable::isCalculated
(
    Layer* aLayer
)
{
    bool result = false;
    for( int i = 0; i < table.size(); i++ )
    {
        if( table[ i ].getLayer() == aLayer )
        {
            result = table[ i ].getCalculated();
        }
    }
    return result;
}




/*
    Return true if all parents of the layer has a calculated flag
*/
bool CalcTable::isParentsCalculated
(
    Layer* aLayer
)
{
    bool result = true;

    /* Get nerve list */
    NerveList* nerveList = net -> getNerveList();

    auto parents = LayerList::create( net );
    nerveList -> getParentsByLayer( aLayer , parents );

    parents -> loop
    (
        [ this, &result ]
        ( void* item )
        {
            auto layer = ( Layer* ) item;
            result = result && isCalculated( layer );
            return false;
        }
    );

    parents -> destroy();

    return result;
}



/*
    Return true if all children of the layer has a calculated flag
*/
bool CalcTable::isChildrenCalculated
(
    Layer* aLayer
)
{
    bool result = true;

    /* Get nerve list */
    NerveList* nerveList = net -> getNerveList();

    auto children = LayerList::create( net );
    nerveList -> getChildrenByLayer( aLayer , children );

    children -> loop
    (
        [ this, &result ]
        ( void* item )
        {
            auto layer = ( Layer* ) item;
            result = result && isCalculated( layer );
            return false;
        }
    );

    children -> destroy();

    return result;
}







/*
    Loop for all notcalculated layers
    Fills a CalcRecord vector with data from aNet and calls
    aCallback for each unprocessed layer until all layers are processed.
*/
CalcTable* CalcTable::loop
(
    /* Callback function */
    CalcTableLoopCallback aCallback
)
{
    /* Count of layer calculated */
    int calculatedCount = 0;
    auto layers = net -> getLayerList();
    auto layersCount = layers -> getCount();

    /* Main loop */
    do
    {
        calculatedCount = 0;
        for( int i = 0; i < layersCount; i++ )
        {
            if
            (
                ! table[ i ].getCalculated() &&
                aCallback( this, table[ i ].getLayer() )
            )
            {
                table[ i ].setCalculated();
                calculatedCount ++;
            }
        }
    }
    while( calculatedCount > 0 );
    return this;
}
