#include <cmath>

#include "nerve.h"

#include "../../../../lib/core/rnd_obj.h"
#include "../../../../lib/sock/rpc_client.h"

#include "limb.h"
#include "layer.h"
#include "io.h"



/*
    Constructor
*/
Nerve::Nerve
(
    LogManager* aLogManager,    /* The log object */
    Layer*      aParent,        /* Parent layer */
    Layer*      aChild,         /* Child layer */
    NerveType   aNerveType,     /* Type of nerve */
    BindType    aBindType       /* Bind of nerve */
)
{
    /* Set properties */
    logManager  = aLogManager;
    parent      = aParent;
    child       = aChild;
    nerveType   = aNerveType;
    bindType    = aBindType;

    /* Connect */
    getLog()
    -> trace( "Layer connecting" )
    -> prm( "from", parent -> getNameOrId() )
    -> prm( "to", child -> getNameOrId() )
    -> lineEnd();
}



/*
    Destructor
*/
Nerve::~Nerve()
{
    purge();
}



/*
    Create new nerve with details arguments
*/
Nerve* Nerve::create
(
    LogManager* aLogManager,/* The log manager object*/
    Layer*      aParent,    /* Parent layer */
    Layer*      aChild,     /* Child layer */
    NerveType   aNerveType, /* Type of nerve */
    BindType    aBindType   /* Bind of nerve */
)
{
    return new Nerve
    (
        aLogManager,
        aParent,
        aChild,
        aNerveType,
        aBindType
    );
}



/*
    Destroy
*/
void Nerve::destroy()
{
    delete this;
}



/*
    Return log object
*/
Log* Nerve::getLog()
{
    return logManager -> getLog();
}



/*
    Allocate memomry buffer for weights
*/
Nerve* Nerve::allocate
(
    function <void ( Nerve* )> aOnAllocate
)
{
    auto newCount = 0;

    int cFrom   = parent -> getCount();
    int cTo     = child -> getCount();

    /* Calculate new buffer size */
    switch( nerveType )
    {
        case ALL_TO_ALL:
        {
            newCount = cFrom * cTo;
        }
        break;
        case ONE_TO_ONE:
            newCount = max( cFrom, cTo );
        break;
    }

    if( weightsCount != newCount )
    {
        /*
            Count has been changed,
            and the weights array must be reallocated
        */
        purge();

        weightsCount = newCount;

        /* Create buffer */
        weights = new double[ weightsCount ];

        /* Create delta buffer */
        deltaWeights = new double[ weightsCount ];

        getLog()
        -> trace( "Memory allocated" )
        -> prm( "Binds count", weightsCount );

        aOnAllocate( this );
    }

    return this;
}




/*
    purge memomry buffer for weights
*/
Nerve* Nerve::purge()
{
    /* Purge previous weights buffer if exists */
    if( weights != NULL )
    {
        delete [] weights;
        delete [] deltaWeights;
    }
    return this;
}



/*
    Fill weights
*/
Nerve* Nerve::fill
(
    /* Rnd seed */
    RndObj* aSeed,
    /* MinWeight */
    double aMinWeight,
    /* MaxWeight */
    double aMaxWeight
)
{
    if( aMinWeight > aMaxWeight )
    {
        aMinWeight = minWeight;
        aMaxWeight = maxWeight;
    }

    if( weights != NULL )
    {
        for( int i = 0; i < weightsCount; i++ )
        {
            weights[ i ] = aSeed == NULL ? aMinWeight : aSeed -> get( aMinWeight, aMaxWeight );
            deltaWeights[ i ] = 0.0;
        }
    }
    return this;
}



/*
    Return BindType
*/
BindType Nerve::getBindType()
{
    return bindType;
}



/*
    Return NerveType
*/
NerveType Nerve::getNerveType()
{
    return nerveType;
}



/*
    Return parent layer
*/
Layer* Nerve::getParent()
{
    return parent;
}



/*
    Return child layer
*/
Layer* Nerve::getChild()
{
    return child;
}



/*
    Return count of weights
*/
int Nerve::getWeightsCount()
{
    return weightsCount;
}



NerveType Nerve::nerveTypeFromString
(
    string a
)
{
    if( a == "ONE_TO_ONE" ) return ONE_TO_ONE;
    if( a == "ALL_TO_ALL" ) return ALL_TO_ALL;
    return ALL_TO_ALL;
};



/*
    Return the parent neuron index by index of weight array
*/
int Nerve::getParentByWeightIndex
(
    int aIndex
)
{
    int iParent = -1;

    switch( nerveType )
    {
        case ALL_TO_ALL:
            /*
                Parent0--0--child0
                      \    /
                       2  1
                        \/
                        /\
                       /  \
                      /    \
                parent2--3--child2
            */
            iParent = aIndex % parent -> getCount();
        break;
        case ONE_TO_ONE:
            /*
                P0--0--c0

                p2--1--c2
            */
            double cp = parent -> getCount();
            double cc = child -> getCount();
            double m = max( cp, cc );
            iParent = ( int ) ( cp  / m * aIndex );
        break;
    }

    return iParent;
}



/*
    Return the child neuron index by index of weight array
*/
int Nerve::getChildByWeightIndex
(
    int aIndex
)
{
    int iChild = -1;

    switch( nerveType )
    {
        case ALL_TO_ALL:
            iChild = aIndex / parent -> getCount();
        break;
        case ONE_TO_ONE:
            double cp = parent -> getCount();
            double cc = child -> getCount();
            double m = max( cp, cc );
            iChild = ( int ) ( cc / m * aIndex );
        break;
    }
    return iChild;
}



/*
    Return weights pointer
*/
double* Nerve::getWeights()
{
    return weights;
}



/*
    Return weight by index
*/
double Nerve::getWeight
(
    int aIndex
)
{
    return weights[ aIndex ];
}



/*
    Set weight by index
*/
Nerve* Nerve::setWeight
(
    int aIndex,
    double aValue
)
{
    weights[ aIndex ] = aValue;
    return this;
}



/*
    Return weight by index
*/
double Nerve::getDeltaWeight
(
    int aIndex
)
{
    return deltaWeights[ aIndex ];
}



/*
    Set weight by index
*/
Nerve* Nerve::setDeltaWeight
(
    int aIndex,
    double aValue
)
{
    deltaWeights[ aIndex ] = aValue;
    return this;
}



/*
    Return the weights from and to indexes by child index
*/
Nerve* Nerve::getWeightsRangeByChildIndex
(
    int aIndex, /* index of neuron */
    int &aFrom, /* index of weights begin for neurn */
    int &aTo    /* index of weights eend for neuron */
)
{
    switch( nerveType )
    {
        case ALL_TO_ALL:
        {
            auto c = parent-> getCount();
            aFrom = aIndex * c;
            aTo = aFrom + c;
        }
        break;
        case ONE_TO_ONE:
            double cp = parent -> getCount();
            double cc = child -> getCount();
            double m = max( cp, cc );
            aFrom = (int) ceil( aIndex * m / cc );
            aTo = (int) ceil(( aIndex + 1 ) * m / cc );
        break;
    }
    return this;
}



/*
    Return the weights from and to indexes by parent index
*/
Nerve* Nerve::getWeightsRangeByParentIndex
(
    int aIndex, /* index of parent neuron */
    int &aFrom, /* index of weights begin for neurn */
    int &aTo,   /* index of weights eend for neuron */
    int &aStep  /* step for shift between from and to */
)
{
    switch( nerveType )
    {
        case ALL_TO_ALL:
            aFrom = aIndex;
            aTo = weightsCount - parent-> getCount() + aIndex + 1;
            aStep = parent -> getCount();
        break;
        case ONE_TO_ONE:
            double cp = parent -> getCount();
            double cc = child -> getCount();
            double m = max( cp, cc );
            aFrom = (int) ceil( aIndex * m / cp );
            aTo = (int) ceil(( aIndex + 1 ) * m / cp );
            aStep = 1;
        break;
    }
    return this;
}



/*
    Nerve IO
*/

Nerve* Nerve::readFromBuffer
(
    char* aBuffer,
    size_t aSize
)
{
    if( aSize == weightsCount * sizeof( double ) )
    {
        memcpy(( char* )weights, aBuffer, aSize );
    }
    else
    {
        setResult( "BufferSizeNotMatchWeightsCount" );
    }
    return this;
}



/*
    Put weights array from memory buffer with size
*/
Nerve* Nerve::writeToBuffer
(
    char*   &aBuffer,
    size_t  &aSize
)
{
    if( aBuffer == NULL )
    {
        aSize = sizeof( double ) * weightsCount;
        aBuffer = new char[ aSize ];
        memcpy( aBuffer, (char*) weights, aSize );
    }
    else
    {
        setResult( "BufferIsNotEmpty" );
    }
    return this;
}



/*
    Return weight index in nerve by neurons index
*/
int Nerve::getIndexByNeuronsIndex
(
    int aIndexNeuronParent,
    int aIndexNeuronChild
)
{
    int result = -1;

    switch( nerveType )
    {
        case ALL_TO_ALL:
        {
            int from, to = 0;
            getWeightsRangeByChildIndex( aIndexNeuronChild, from, to );
            result = from + aIndexNeuronParent;
        }
        break;
        case ONE_TO_ONE:
        {
            int fromChild, toChild = 0;
            int fromParent, toParent, step = 0;
            getWeightsRangeByParentIndex( aIndexNeuronParent, fromParent, toParent, step );
            getWeightsRangeByChildIndex( aIndexNeuronChild, fromChild, toChild );
            if( fromParent > fromChild && fromParent < toChild )
            {
                result = fromParent;
            }
            if( fromChild >= fromParent && fromChild < toParent )
            {
                result = fromChild;
            }
        }
        break;
    }
    return result;
}



/*
    Return minimal weight
*/
double Nerve::getMinWeight()
{
    return minWeight;
}



/*
    Set minimal weight
*/
Nerve* Nerve::setMinWeight
(
    double aValue
)
{
    minWeight = aValue;
    return this;
}



/*
    Return maximal weight
*/
double Nerve::getMaxWeight()
{
    return maxWeight;
}



/*
    Set maximal weight
*/
Nerve* Nerve::setMaxWeight
(
    double aValue
)
{
    maxWeight = aValue;
    return this;
}



/*
    Return weight storage file path
*/
string Nerve::getWeightFileName
(
    string aPath
)
{
    return
    aPath
    + "/"
    + getParent() -> getId()
    + "_"
    + bindTypeToString(bindType)
    + "_"
    + getChild() -> getId()
    + ".bin";
}



/*
    Read Weigts from file
*/
Nerve* Nerve::loadWeight
(
    string aPath
)
{
    auto file = getWeightFileName( aPath );
    auto path = getPath( file );

    if( !checkPath( path ) )
    {
        setResult( "WeightPathCreateError" )
        -> getDetails()
        -> setString( "file", file )
        -> setString( "path", path )
        ;
    }
    else
    {
        if( !fileExists( file ))
        {
            setResult( "WeightFileNotExists" )
            -> getDetails()
            -> setString( "file", file );
        }
        else
        {
            auto size = fileSize( file );
            if( size != weightsCount * sizeof( double ) )
            {
                setResult( "WeightFileSizeChanged" )
                -> getDetails()
                -> setString( "file", file )
                -> setInt( "fileSize", size )
                -> setInt( "weightSize", weightsCount * sizeof( double ) )
                ;
            }
            else
            {
                auto h = fopen( file.c_str(), "r" );
                if( !h )
                {
                    setResult( "LoadOpenStorageError" )
                    -> getDetails()
                    -> setString( "file", file );
                }
                else
                {
                    auto readed = fread( weights, size, 1, h );
                    if( readed != 1 )
                    {
                        setResult( "LoadError" )
                        -> getDetails()
                        -> setString( "file", file );
                    }
                    else
                    {
                        getLog() -> trace( "Weights loaded" ) -> prm( "file", file );
                    }
                    fclose( h );
                }
            }
        }
    }

    return this;
}




/*
    Write Weigts to file
*/
Nerve* Nerve::saveWeight
(
    string aPath
)
{
    auto file = getWeightFileName( aPath );
    auto path = getPath( file );

    if( !checkPath( path ) )
    {
        setResult( "WeightPathCreateError" )
        -> getDetails()
        -> setString( "file", file )
        -> setString( "path", path )
        ;
    }
    else
    {
        auto h = fopen( file.c_str(), "wb" );
        if( !h )
        {
            setResult( "SaveOpenStorageError" )
            -> getDetails()
            -> setString( "file", file );
        }
        else
        {
            auto writed = fwrite( weights, weightsCount * sizeof( double ), 1, h );
            if( writed != 1 )
            {
                setResult( "SaveError" )
                -> getDetails()
                -> setString( "file", file );
            }
            fclose( h );
        }
    }
    return this;
}



/*
    Extract parent weithds in to buffer by child neuron index
*/
Nerve* Nerve::extractParentsWeightsBuffer
(
    int     aNeuronIndex,
    char*   &aBuffer,
    size_t  &aSize
)
{
    int from, to = 0;
    getWeightsRangeByChildIndex( aNeuronIndex, from, to );
    aSize = ( to - from ) * NEURON_WEIGHT_SIZE;
    aBuffer = new char [ aSize ];
    memcpy( aBuffer, weights, aSize );
    return this;
}




/*
    Extract children weithds in to buffer by paraent neuron index
*/
Nerve* Nerve::extractChildWeightsBuffer
(
    int     aNeuronIndex,
    char*   &aBuffer,
    size_t  &aSize
)
{
    int from, to, step = 0;

    getWeightsRangeByParentIndex( aNeuronIndex, from, to, step );

    int count = ceil(( to - from ) / (double) step);
    aSize = count * NEURON_WEIGHT_SIZE;

    auto buffer = new NeuronWeight[ count ];
    for( int i = 0 ; i < count ; i++ )
    {
        buffer[ i ] = weights[ from + i * step ];
    }

    aBuffer = (char*) buffer;
    return this;
}


/*
    Dump to log
*/
Nerve* Nerve::dumpToLog()
{
    getLog()
    -> begin( "nerve" )
    -> prm( "from", getParent() -> getId() )
    -> prm( "to", getChild() -> getId() )
    ;
    for( int i = 0; i < weightsCount; i++ )
    {
        getLog()
        -> trace()
        -> value( i )
        -> text( " | weights: " )
        -> value( weights[ i ])
        -> text( " | delta:   " )
        -> value( abs( deltaWeights[ i ] ))
        ;
    }
    getLog() -> end();
    return this;
}




/*
    Dump to mon
*/
Nerve* Nerve::dumpToMon
(
    Mon* aMon,
    ChartList* aChartList
)
{
    for( int i = 0; i < weightsCount; i++ )
    {
        auto iChart = aChartList -> add
        (
            "weights"
            + to_string( i )
            + getParent() -> getId()
            + getChild() -> getId()
        );


        iChart -> createLast( abs( weights[ i ] ));
        aMon
        -> setString
        (
            Path{ getParent() -> getId(), getChild() -> getId(), to_string( i ) },
            iChart -> toString( 40 )
        );
    }

    return this;
}
