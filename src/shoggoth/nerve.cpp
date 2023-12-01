#include "nerve.h"

#include "../lib/rnd.h"
#include "../lib/rpc_client.h"

#include "net.h"
#include "layer.h"
#include "io.h"

/*
    Constructor
*/
Nerve::Nerve
(
    Net*        aNet,       /* Net object*/
    string      aId,        /* id */
    Layer*      aParent,    /* Parent layer */
    Layer*      aChild,     /* Child layer */
    NerveType   aNerveType, /* Type of nerve */
    BindType    aBindType,  /* Bind of nerve */
    double      aMinWeight, /* MinWeight */
    double      aMaxWeight  /* MaxWeight */
)
{
    /* Set properties */
    net         = aNet;
    id          = aId;
    parent      = aParent;
    child       = aChild;
    nerveType   = aNerveType;
    bindType    = aBindType;

    /* Connect */
    getLog()
    -> begin( "Layer connecting" )
    -> prm( "from", parent -> getNameOrId() )
    -> prm( "to", child -> getNameOrId() )
    -> lineEnd();

    int cFrom   = parent -> getNeuronsCount();
    int cTo     = child -> getNeuronsCount();

    switch( nerveType )
    {
        case ALL_TO_ALL:
        {
            weightsCount = cFrom * cTo;
        }
        break;
        case ONE_TO_ONE:
            weightsCount = max( cFrom, cTo );
        break;
    }

    /* Create bind list */
    weights = new double[ weightsCount ];

    for( int i = 0; i < weightsCount; i++ )
    {
        weights[ i ] = Rnd::get( aMinWeight, aMaxWeight );
    }

    getLog()
    -> trace( "" )
    -> prm( "Binds count", weightsCount )
    -> end();
}



/*
    Destructor
*/
Nerve::~Nerve()
{
    delete [] weights;
}



/*
    Create
*/
Nerve* Nerve::create
(
    Net*        aNet,       /* Net object*/
    string      aId,        /* id */
    Layer*      aParent,    /* Parent layer */
    Layer*      aChild,     /* Child layer */
    NerveType   aNerveType, /* Type of nerve */
    BindType    aBindType,  /* Bind of nerve */
    double      aMinWeight, /* MinWeight */
    double      aMaxWeight  /* MaxWeight */
)
{
    return new Nerve
    (
        aNet,
        aId,
        aParent,
        aChild,
        aNerveType,
        aBindType,
        aMinWeight,
        aMaxWeight
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
    return net -> getLog();
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
    Return id
*/
string Nerve::getId()
{
    return id;
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



BindType Nerve::bindTypeFromString
(
    string a
)
{
    if( a == "VALUE" )              return BT_VALUE;
    if( a == "SAMPLE" )             return BT_SAMPLE;
    if( a == "COMMAND" )            return BT_COMMAND;
    if( a == "ERROR_TO_VALUE" )     return BT_ERROR_TO_VALUE;
    return BT_VALUE;
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
    Return the parent neuron iundex by index of weight array
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
            iParent = aIndex % parent -> getNeuronsCount();
        break;
        case ONE_TO_ONE:
            double cp = parent -> getNeuronsCount();
            double cc = child -> getNeuronsCount();
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
            iChild = aIndex / parent -> getNeuronsCount();
        break;
        case ONE_TO_ONE:
            double cp = parent -> getNeuronsCount();
            double cc = child -> getNeuronsCount();
            double m = max( cp, cc );
            iChild = ( int ) ( cc / m * aIndex );
        break;
    }
    return iChild;
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
            aFrom = aIndex * parent-> getNeuronsCount();
            aTo = aFrom + parent-> getNeuronsCount();
        break;
        case ONE_TO_ONE:
            double cp = parent -> getNeuronsCount();
            double cc = child -> getNeuronsCount();
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
            aTo = weightsCount - parent-> getNeuronsCount() + aIndex + 1;
            aStep = parent -> getNeuronsCount();
        break;
        case ONE_TO_ONE:
            double cp = parent -> getNeuronsCount();
            double cc = child -> getNeuronsCount();
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
    Read nerve weights array from Io
*/
Nerve* Nerve::readWeights()
{
    auto io = Io::create( net );

    io
    -> getRequest()
    -> setString( "idFrom", parent -> getId())
    -> setString( "idTo", child -> getId());

    if( io -> call( CMD_READ_WEIGHTS ) -> isOk() )
    {
        char* buffer = NULL;
        size_t size = 0;
        io -> getAnswer() -> getData( "data", buffer, size );
        if( buffer != NULL && size > 0 )
        {
            readFromBuffer( buffer, size );
        }
    }

    io -> destroy();

    return this;
}



/*
    Write the nerve weight array to io
*/
Nerve* Nerve::writeWeights()
{
    getLog()
    -> begin( "Write weights" )
    -> prm( "parent", parent -> getId())
    -> prm( "child", child -> getId());

    auto io = Io::create( net );

    io
    -> getRequest()
    -> setString( "idFrom", parent -> getId())
    -> setString( "idTo", child -> getId())
    -> setData
    (
        "data",
        ( char* )weights,
        sizeof(double) * weightsCount
    );

    io
    -> call( CMD_WRITE_WEIGHTS )
    -> destroy();

    getLog() -> end();

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
