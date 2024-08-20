#include "func.h"
#include "limb.h"

#include "../../../../lib/core/rnd.h"



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

    id = aId == "" ? Rnd::getUuid() : aId;

    /* Actions */
    actions         = ParamList::create();

    /* Statistics */
    chartValues      = ChartData::create();
    chartErrors      = ChartData::create();
    chartTick        = ChartData::create() -> setMaxCount( 1000 );

    /* Log */
    getLog() -> trace( "Create layer" ) -> prm( "id", id );
}



/*
    Internal destructor
*/
Layer::~Layer()
{
    /* Actions destroy */
    actions -> destroy();

    /* Statistics destroy */
    chartValues -> destroy();
    chartErrors -> destroy();
    chartTick -> destroy();

    /* Destroy neurons */
    setCount( 0 );

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





/******************************************************************************
    Data plans work
*/


/*
    Clear values for all neurons
*/
Layer* Layer::clearValues()
{
    getLimb() -> lock();
    memset( values, 0, getValuesBufferSize() );
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
    }
    getLimb() -> unlock();
    return this;
}



/*
    Return buffer and size of buffer of values
*/
Layer* Layer::getValuesBuffer
(
    char* &aBuffer, /* Buffer pointer */
    size_t &aSize   /* Size of buffer */
)
{
    aBuffer = ( char* )values;
    aSize = getValuesBufferSize();
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



/*
    Return buffer and size of buffer of errors
*/
Layer* Layer::getErrorsBuffer
(
    char* &aBuffer, /* Buffer pointer */
    size_t &aSize   /* Size of buffer */
)
{
    aBuffer = ( char* )errors;
    aSize = getValuesBufferSize();
    return this;
}



/***********************************************************************
    Setters and getters
*/



Limb* Layer::getLimb()
{
    return limb;
}


/*
    Return the log object
*/
Log* Layer::getLog()
{
    return limb -> getLog();
}



/*
    Set count of neurons
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



/*
    Return neurons count in layer
*/
int Layer::getCount()
{
    return count;
}



Layer* Layer::valuesCreate()
{
    getLimb() -> lock();
    /* Delete old plan */
    if( values != NULL )
    {
        delete [] values;
    }

    /*Create new plan */
    values = new double[ count ];
    getLimb() -> unlock();

    clearValues();

    return this;
}



Layer* Layer::errorsCreate()
{
    getLimb() -> lock();
    /* Delete old plan */
    if( errors != NULL )
    {
        delete [] errors;
    }

    /*Create new plan */
    errors = new double[ count ];
    getLimb() -> unlock();

    clearErrors();

    return this;
}



/*
    Return the name of layer
*/
string Layer::getName()
{
   return name;
}



/*
    Set the name of layer
*/
Layer* Layer::setName
(
    string a
)
{
   name = a;
   return this;
}



string Layer::getId()
{
   return id;
}



/*
    Set the id of layer
*/
Layer* Layer::setId
(
    string a
)
{
   id = a;
   return this;
}




string Layer::getNameOrId()
{
   return name == "" ? id : name;
}



/*
    Return true if neurons error changed in calcError method
*/
bool Layer::getErrorChange()
{
    return errorChange;
}



/*
    Return sum of error
*/
double Layer::calcSumError()
{
    double result = 0.0;

    for( int i = 0; i < count; i ++ )
    {
        result += abs( errors[ i ] );
    }

    return result;
}



/*
    Return sum of value
*/
double Layer::calcSumValue()
{
    double result = 0.0;

    getLimb() -> lock();
    for( int i = 0; i < count; i ++ )
    {
        result += values[ i ];
    }
    getLimb() -> unlock();

    return result;
}



/*
    Storage path
*/

Layer* Layer::setStoragePath
(
    string a
)
{
    storagePath = a;
    return this;
}



/*
*/
string Layer::getStoragePath()
{
    return storagePath;
}




string Layer::getLayerPath()
{
    return storagePath == "" ? "" : storagePath + "/" + getId();
}



/*
    Return layer file name
*/
string Layer::getStorageValueName()
{
    return storagePath == "" ? "" : ( getLayerPath() + "/value.bin" );
}



/**********************************************************************
    Neurons setters and getters
*/



/*
    Set neuron value
*/
Layer* Layer::setNeuronValue
(
    int aIndex,            /* Index of neuron */
    double aValue          /* Value */
)
{
    if( values != NULL )
    {
        if( aIndex >= 0 && aIndex < count )
        {
            values[ aIndex ] = aValue;
        }
        else
        {
            setResult( "IndexValueOutOfRangeForSet" );
        }
    }
    else
    {
        setResult( "ValueArrayNotDefinedForSet" );
    }

    return this;
}



/*
    Return neuron vaalue or default value
*/
double Layer::getNeuronValue
(
    int aIndex            /* Index of neuron */
)
{
    double result = 0.0;
    if( values != NULL )
    {
        if( aIndex >= 0 && aIndex < count )
        {
            result = values[ aIndex ];
        }
        else
        {
            setResult( "IndexValueOutOfRangeForGet" );
        }
    }
    else
    {
        setResult( "ValueArrayNotDefinedForGet" );
    }
    return result;
}



/*
    Set neuron error
*/
Layer* Layer::setNeuronError
(
    int aIndex,            /* Index of neuron */
    double aValue          /* Value */
)
{
    if( errors != NULL && aIndex >= 0 && aIndex < count )
    {
        errors[ aIndex ] = aValue;
    }
    else
    {
        setResult( "SetIndexValueOutOfRange" );
    }
    return this;
}



/*
    Return neuron error or default value
*/
double Layer::getNeuronError
(
    int aIndex  /* Index of neuron */
)
{
    double result = 0.0;

    if( errors != NULL && aIndex >= 0 && aIndex < count )
    {
        result = errors[ aIndex ];
    }
    else
    {
        setResult( "GettingIndexValueOutOfRange" );
    }

    return result;
}



/*
    Return event actions
*/
ParamList* Layer::getActions()
{
    return actions;
}



/******************************************************************************
    Fill layer
*/


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
    getId()         == aLayer -> getId() &&
    getCount()      == aLayer -> getCount() &&
    getName()       == aLayer -> getName() &&
    getFrontFunc()  == aLayer -> getFrontFunc() &&
    getBackFunc()   == aLayer -> getBackFunc() &&
    getErrorCalc()  == aLayer -> getErrorCalc() &&
    getWeightCalc() == aLayer -> getWeightCalc()
    ;
}




/*
    Return size of values buffer
*/
size_t Layer::getValuesBufferSize()
{
    return sizeof( double ) * count;
}



/*
    Return true if layer contains one of tasks
*/
bool Layer::checkTasks
(
    ParamList* aTasks,
    Action aAction
)
{
    return
    getActions() ->
    getObject( actionToString( aAction )) ->
    isIntersect( aTasks );
}



Layer* Layer::setFrontFunc
(
    NeuronFunc* a
)
{
    frontFunc = a;
    return this;
}



NeuronFunc* Layer::getFrontFunc()
{
    return frontFunc;
}



Layer* Layer::setBackFunc
(
    NeuronFunc* a
)
{
    backFunc = a;
    return this;
}



NeuronFunc* Layer::getBackFunc()
{
    return backFunc;
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
            iValuesChart -> toString( 40 )
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
    chartErrors -> createLast( calcSumError() );
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
        chartTick -> createLast( ( double ) tickCount );
    }
    /* Reset tick counter */
    tickCount = 0;

    return this;
}



ChartData* Layer::getChartTick()
{
    return chartTick;
}



ChartData* Layer::getChartValues()
{
    return chartValues;
}



ChartData* Layer::getChartErrors()
{
    return chartErrors;
}



/*
    Set error calc flag for the layer.
    Layer will be calculating erors.
*/
Layer* Layer::setErrorCalc
(
    ErrorCalc a
)
{
    errorCalc = a;
    return this;
}



/*
    Return the layer calculation flag for the layer
*/
ErrorCalc Layer::getErrorCalc()
{
    return errorCalc;
}



/*
    Set weight calc flag for the layer.
    Layer will be calculating weights.
*/
Layer* Layer::setWeightCalc
(
    WeightCalc a
)
{
    weightCalc = a;
    return this;
}



/*
    Return the layer calculation flag for the layer
*/
WeightCalc Layer::getWeightCalc()
{
    return weightCalc;
}
