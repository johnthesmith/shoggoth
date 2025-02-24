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
    Reset layer calcilated
*/
CalcRecord* CalcRecord::resetCalculated()
{
    calculated = false;
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

CalcTable::CalcTable()
{
}



/*
    Create calc table
*/
CalcTable* CalcTable::create()
{
    return new CalcTable();
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



CalcTable* CalcTable::addLayer
(
    Layer* aLayer
)
{
    table.push_back( (CalcRecord){false, aLayer} );
    return this;
}




CalcTable* CalcTable::reset()
{
    for( auto& item : table )
    {
        item.resetCalculated();
    }
    return this;
}



/*
    Return true if all parents of the layer has a calculated flag
*/
bool CalcTable::isParentsCalculated
(
    Layer* aLayer,
    NerveList* aNerveList
)
{
    bool result = true;

    auto parents = LayerList::create( NULL );
    aNerveList -> getParentsByLayer( aLayer, parents );

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
    Layer* aLayer,
    NerveList* aNerveList
)
{
    bool result = true;
    auto children = LayerList::create( NULL );
    aNerveList -> getChildrenByLayer( aLayer, children );

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
    Sort for forward calculation (neuron values)
*/
CalcTable* CalcTable::sortForward
(
    NerveList* aNerveList
)
{
    vector <CalcRecord> sorted;

    /*
        Drop calculated flags
        This flag is using for sorting.
    */
    reset();

    /* Sort */
    loop
    (
        [ &aNerveList, &sorted ]
        ( CalcTable* table, Layer* layer, bool &terminate )
        {
            /* Exclude calculation for layers without parents */
            if
            (
                ( !aNerveList -> parentsExists( layer )) ||
                ( table -> isParentsCalculated( layer, aNerveList ))
            )
            {
                sorted.push_back( (CalcRecord){false, layer} );
                return true;
            }
            else
            {
                return false;
            }
        }
    );

    /* Refresh the table */
    table = sorted;

    return this;
}



/*
    Sort for forward calculation (neuron values)
*/
CalcTable* CalcTable::sortBackward
(
    NerveList* aNerveList
)
{
    vector <CalcRecord> sorted;

    /*
        Drop calculated flags
        This flag is using for sorting.
    */
    reset();

    /* Sort */
    loop
    (
        [ &aNerveList, &sorted ]
        ( CalcTable* table, Layer* layer, bool &terminate )
        {
            /* Exclude calculation for layers without parents */
            if
            (
                ( table -> isChildrenCalculated( layer, aNerveList ))
            )
            {
                sorted.push_back( (CalcRecord){false, layer} );
                return true;
            }
            else
            {
                return false;
            }
        }
    );

    /* Refresh the table */
    table = sorted;

    return this;
}




CalcTable* CalcTable::dump
(
    Log* aLog,
    string aHeader
)
{
    aLog -> begin( aHeader );
    for( auto record:table )
    {
        aLog -> info( record.layer -> getId());
    }
    aLog -> end();

    return this;
}




CalcTable* CalcTable::clear()
{
    table.clear();
    return this;
}
