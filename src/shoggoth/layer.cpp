#include <cmath>

#include "func.h"
#include "limb.h"



/*
    Constructor
*/
Layer::Layer
(
    Limb* aLimb,
    string aId
)
{
    limb = aLimb;
    id = aId;

    /* Statistics */
    chartValues             = ChartData::create();
    chartErrors             = ChartData::create();
    chartTick               = ChartData::create() -> setMaxCount( 1000 );
    chartErrorsBeforeChange = ChartData::create() -> setMaxCount( 1000 );

    /* Log */
    getLog() -> trace( "Create layer" ) -> prm( "id", id );
}



/*
    Internal destructor for Layer object
*/
Layer::~Layer()
{
    /* Statistics destroy */
    chartValues -> destroy();
    chartErrors -> destroy();
    chartTick -> destroy();
    chartErrorsBeforeChange -> destroy();

    /* Destroy neurons */
    setSize( POINT_3I_0 );

    /* Destroy the list of neurons */

    getLog()
    -> trace( "Layer destroy" )
    -> prm( "Name", getNameOrId() );
}



/*
    Creator
*/
Layer* Layer::create
(
    Limb* aLimb,
    string aId
)
{
    return new Layer( aLimb, aId );
}



/*
    Destructor
*/
void Layer::destroy()
{
    delete this;
}



/*
    Sets dimensions size
*/
Layer* Layer::setSize
(
    const Point3i& a
)
{
    setCount( a.x * a.y * a.z );
    size = a;
    limb -> onChangeValues();
    return this;
}



/*
    Sets dimensions size from ParamList object
*/
Layer* Layer::setSize
(
    ParamList* a
)
{
    auto jsonSize = a -> getObject( Path{ "size" });
    if( jsonSize != NULL )
    {
        setSize
        (
            Point3i
            (
                jsonSize -> getInt( 0 ),
                jsonSize -> getInt( 1 ),
                jsonSize -> getInt( 2 )
            )
        );
    }
    return this;
}



/******************************************************************************
    Methods for work with dataplans
*/


/*
    Clear values for all neurons
*/
Layer* Layer::clearValues()
{
    getLimb() -> lock();
    memset( values, 0, getValuesBufferSize() );
    getLimb() -> onChangeValues();
    getLimb() -> unlock();
    return this;
}



/*
    Clear errors for all neurons
*/
Layer* Layer::clearErrors()
{
    getLimb() -> lock();
    memset( errors, 0, getValuesBufferSize() );
    getLimb() -> onChangeValues();
    getLimb() -> unlock();
    return this;
}



/*
    Set values from buffer
*/
Layer* Layer::setValuesFromBuffer
(
    char* aBuffer,
    size_t  aSize
)
{
    getLimb() -> lock();
    if( aBuffer != NULL && aSize == getValuesBufferSize() )
    {
        memcpy( values, aBuffer, aSize );
        getLimb() -> onChangeValues();
    }
    getLimb() -> unlock();
    return this;
}



/*
    Return errors buffer
*/
Layer* Layer::errorsFromBuffer
(
    char* aBuffer,
    size_t aSize
)
{
    getLimb() -> lock();
    if( aBuffer != NULL && aSize == getValuesBufferSize() )
    {
        memcpy( errors, aBuffer, aSize );
    }
    getLimb() -> unlock();
    return this;
}



/***********************************************************************
    Setters and getters
*/



/*
    Sets count of neurons
    Do not use this method.
    Use the setSize

    Thes method reallocate memory fot plans
*/
Layer* Layer::setCount
(
    const int aCount
)
{
    getLimb() -> lock();
    if( aCount != count )
    {
        /* The size was changed */
        getLog()
        -> begin( "Layer resize" )
        -> prm( "id", getNameOrId() )
        -> prm( "from", count )
        -> prm( "to", aCount )
        -> lineEnd();

        /* Change count and size */
        count = aCount;

        /* Create default plans */
        valuesCreate();
        errorsCreate();

        getLog() -> end();
    }
    getLimb() -> unlock();

    return this;
}



Layer* Layer::valuesDestroy()
{
    getLimb() -> lock();
    /* Delete old plan */
    if( values != NULL )
    {
        delete [] values;
        values = NULL;
    }
    getLimb() -> unlock();
    return this;
}



Layer* Layer::errorsDestroy()
{
    getLimb() -> lock();
    /* Delete old plan */
    if( errors != NULL )
    {
        delete [] errors;
        errors = NULL;
    }
    getLimb() -> unlock();
    return this;
}



Layer* Layer::valuesCreate()
{
    getLimb() -> lock();
    valuesDestroy();

    /*Create new plan */
    if( count != 0 )
    {
        values = new real[ count ];
    }

    getLimb() -> unlock();

    clearValues();

    return this;
}



Layer* Layer::errorsCreate()
{
    getLimb() -> lock();
    errorsDestroy();

    /*Create new plan */
    if( count != 0 )
    {
        errors = new real[ count ];
    }

    getLimb() -> unlock();

    clearErrors();

    return this;
}



/*
    Return sum of error
*/
real Layer::calcRmsError()
{
    real result = 0.0;

    if( count > 0 )
    {
        getLimb() -> lock();
        for( int i = 0; i < count; i ++ )
        {
            result += errors[ i ] * errors[ i ];
        }
        getLimb() -> unlock();
        result = sqrt( result ) / count;
    }

    return result;
}



/*
    Return sum of value
*/
real Layer::calcSumValue()
{
    real result = 0.0;

    getLimb() -> lock();
    for( int i = 0; i < count; i ++ )
    {
        result += values[ i ];
    }
    getLimb() -> unlock();

    return result;
}



/*
    Calculate Root Main Square of neurons value
*/
real Layer::calcRmsValue()
{
    real result = 0.0;
    if( count > 0 )
    {
        getLimb() -> lock();
        for( int i = 0; i < count; i ++ )
        {
            result += values[ i ] * values[ i ];
        }
        getLimb() -> unlock();
        result = sqrt( result ) / count;
    }

    return result;
}



///*
//    Storage path
//*/
//
//Layer* Layer::setStoragePath
//(
//    string a
//)
//{
//    storagePath = a;
//    return this;
//}
//

//
///*
//*/
//string Layer::getStoragePath()
//{
//    return storagePath;
//}
//
//

//
//string Layer::getLayerPath()
//{
//    return storagePath == "" ? "" : storagePath + "/" + getId();
//}
//
//
//
///*
//    Return layer file name
//*/
//string Layer::getStorageValueName()
//{
//    return storagePath == "" ? "" : ( getLayerPath() + "/value.bin" );
//}



/*
    Move values data to this from the argument layer
*/
Layer* Layer::copyValuesFrom
(
    Layer* aLayer
)
{
    aLayer -> getLimb() -> lock();

    char* fromBuffer;
    size_t fromSize;
    aLayer -> getValuesBuffer( fromBuffer, fromSize );

    getLimb() -> lock();

    auto size = getValuesBufferSize();
    if
    (
        values != NULL &&
        fromBuffer != NULL &&
        size == fromSize
    )
    {
        memcpy( values, fromBuffer, size );
    }
    else
    {
        setCode( "LayersValuePlanNotEquals" );
    }

    getLimb() -> unlock();
    aLayer -> getLimb() -> unlock();

    return this;
}



/*
    Move values data to this from the argument layer
*/
Layer* Layer::copyErrorsFrom
(
    Layer* aLayer
)
{
    aLayer -> getLimb() -> lock();
    char* fromBuffer;
    size_t fromSize;
    aLayer -> getErrorsBuffer( fromBuffer, fromSize );

    getLimb() -> lock();
    auto size = getValuesBufferSize();
    if
    (
        errors != NULL &&
        fromBuffer != NULL &&
        size == fromSize
    )
    {
        memcpy( errors, fromBuffer, size );
    }
    else
    {
        setCode( "LayersErrorPlanNotEquals" );
    }

    getLimb() -> unlock();
    aLayer -> getLimb() -> unlock();

    return this;
}




/*
    Compare this layer and argument layer
*/
bool Layer::compare
(
    Layer* aLayer
)
{
    return
    getId()             == aLayer -> getId() &&
    getCount()          == aLayer -> getCount() &&
    getName()           == aLayer -> getName() &&
    getFrontFunc()      == aLayer -> getFrontFunc() &&
    getBackFunc()       == aLayer -> getBackFunc() &&
    getBackFuncOut()    == aLayer -> getBackFuncOut() &&
    getErrorCalc()      == aLayer -> getErrorCalc() &&
    getWeightCalc()     == aLayer -> getWeightCalc()
    ;
}



/*
    Return layer values like array
    id[ v1, v2, .... ]
*/
string Layer::getValuesString()
{
    vector <string> valuesString;
    for( int i = 0; i < count; i++ )
    {
        valuesString.push_back( toString( values[ i ] ));
    }
    return getId() + "[ "+ implode( valuesString, ", " ) +" ]";
}



/*
    Dump to log
*/
Layer* Layer::dumpToLog()
{
    getLog() -> begin() -> prm( "layer_id", getId() );
    for( int i = 0; i < count; i++ )
    {
        getLog()
        -> trace()
        -> value( i )
        -> text( " | value: " )
        -> value( values[ i ])
        -> text( " | error: " )
        -> value( errors[ i ]);
    }
    getLog() -> end();
    return this;
}



/*
    Dump to stdout
*/
Layer* Layer::dump()
{
    cout << "layer_id" << getId() << "\n";
    for( int i = 0; i < count; i++ )
    {
        cout
        << " index "
        << i
        << " | value: "
        << values[ i ]
        << " | error: "
        << errors[ i ]
        << "\n";
    }
    return this;
}



/*
    Dump to mon
*/
Layer* Layer::dumpToMon
(
    Mon* aMonValues,
    Mon* aMonErrors,
    ChartList* aChartList
)
{
    for( int i = 0; i < count; i++ )
    {
        auto iValuesChart = aChartList -> add
        (
            "values"
            + to_string( i )
            + getId()
        );
        auto iErrorsChart = aChartList -> add
        (
            "errors"
            + to_string( i )
            + getId()
        );

        iValuesChart -> createLast( values[ i ] );
        aMonValues -> setString
        (
            Path{ getId(), to_string( i ) },
            iValuesChart -> toString( 40 )
        );

        iErrorsChart -> createLast( abs( errors[ i ] ));
        aMonErrors -> setString
        (
            Path{ getId(), to_string( i ) },
            iErrorsChart -> toString( 40 )
        );
    }

    return this;
}



/*
    Calculate statistics for layer
*/
Layer* Layer::stat()
{
    /* Ink tick count */
    if( tickCount >= 0)
    {
        tickCount ++;
    }
    chartValues -> createLast( calcSumValue() );
    chartErrors -> createLast( calcRmsError() );
    return this;
}



/*
    Drop tick count
*/
Layer* Layer::dropTickCount()
{
    /* Push count in to stat chart */
    if( tickCount >= 0 )
    {
        chartTick -> createLast( ( real ) tickCount );
    }
    /* Reset tick counter */
    tickCount = 0;

    return this;
}



/*
    Write error before change
*/
Layer* Layer::writeErrorsBeforeChange()
{
    /* Push error in to the stat chart */
    if( tickCount >= 0 )
    {
        chartErrorsBeforeChange -> createLast( ( real ) calcRmsError() );
    }

    return this;
}




/*
    Returns the log object
*/
Log* Layer::getLog()
{
    return limb -> getLog();
}
