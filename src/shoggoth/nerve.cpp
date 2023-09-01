#include "../lib/rnd.h"
#include "nerve.h"
#include "layer.h"

/*
    Constructor
*/
Nerve::Nerve
(
    Log*        aLog,       /* Log object*/
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
    log         = aLog;
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

    int cFrom   = parent -> getNeurons() -> getCount();
    int cTo     = child -> getNeurons() -> getCount();

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
    Log*        aLog,       /* Log object*/
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
        aLog,
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
    return log;
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
    Return the parent neurons by index of weight array
*/
Neuron* Nerve::getParentByWeightIndex
(
    int aIndex
)
{
    int iParent = 0;

    switch( nerveType )
    {
        case ALL_TO_ALL:
            iParent = aIndex % parent -> getNeurons() -> getCount();
        break;
        case ONE_TO_ONE:
            double cp = parent -> getNeurons() -> getCount();
            double cc = child -> getNeurons() -> getCount();
            double m = max( cp, cc );
            iParent = ( int ) ( cp  / m * aIndex );
        break;
    }

    return parent -> getNeurons() -> getByIndex( iParent );
}



/*
    Return the child neurons by index of weight array
*/
Neuron* Nerve::getChildByWeightIndex
(
    int aIndex
)
{
    int iChild = 0;

    switch( nerveType )
    {
        case ALL_TO_ALL:
            iChild = aIndex / parent -> getNeurons() -> getCount();
        break;
        case ONE_TO_ONE:
            double cp = parent -> getNeurons() -> getCount();
            double cc = child -> getNeurons() -> getCount();
            double m = max( cp, cc );
            iChild = ( int ) ( cc / m * aIndex );
        break;
    }
    return child -> getNeurons() -> getByIndex( iChild );
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
            aFrom = aIndex * parent-> getNeurons() -> getCount();
            aTo = aFrom + parent-> getNeurons() -> getCount();
        break;
        case ONE_TO_ONE:
            double cp = parent -> getNeurons() -> getCount();
            double cc = child -> getNeurons() -> getCount();
            double m = max( cp, cc );
            aFrom = (int) ceil( aIndex * m / cc );
            aTo = (int) ceil(( aIndex + 1 ) * m / cc );
        break;
    }
    return this;
}



Nerve* Nerve::loadFromBuffer
(
    char * aBuffer,
    int aSize
)
{
    if( aSize == weightsCout * sizeof( double ) )
    {
        memcpy( weights, aBuffer, aSize );
    }
    return this;
}



/*
    Request nerwes weight from server
*/
Nerve* Nerve::readFromServer
(
    string aHost,       /* server ip address */
    int aPort           /* server port number */
)
{
    if( neurons -> isOk() )
    {
        auto rpc = RpcClient::create( getLog(), aHost, aPort );
        rpc
        -> getRequest()
        -> setString( "from", parent -> getId() )
        -> setString( "to", child -> getId() );

        if( rpc -> call( CMD_READ_NERVE ) -> isOk() )
        {
            char* buffer = NULL;
            size_t size = 0;
            rpc -> getAnswer() -> getData( "data", buffer, size );
            if( buffer != NULL && size > 0 )
            {
                loadFromBuffer( buffer, size );
            }
        }
        rpc -> destroy();
    }

    return this;
}
