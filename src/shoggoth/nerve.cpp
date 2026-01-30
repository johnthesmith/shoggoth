#include <cmath>
#include <cstring> /* для std::memset */

#include "nerve.h"

#include "../../../../lib/sock/rpc_client.h"
#include "../../../../lib/graph/point3.h"

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
    BindType    aBindType,      /* Bind of nerve */
    Point3i     aWindowSize
)
{
    /* Set properties */
    logManager  = aLogManager;
    parent      = aParent;
    child       = aChild;
    nerveType   = aNerveType;
    bindType    = aBindType;
    size        = aWindowSize;

    /* Connect */
    getLog()
    -> trace( "Nerve created" )
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
    BindType    aBindType,  /* Bind of nerve */
    Point3i     aWindowSize
)
{
    return new Nerve
    (
        aLogManager,
        aParent,
        aChild,
        aNerveType,
        aBindType,
        aWindowSize
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
    Set dimations size
*/
Nerve* Nerve::setSize
(
    const Point3i& a
)
{
    size = a;
    return this;
}



/*
    Set dimentions size from ParamList object
*/
Nerve* Nerve::setSize
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



/*
    purge memomry buffer for weights
*/
Nerve* Nerve::purge()
{
    /* Purge previous weights buffer if exists */
    if( weights != NULL )
    {
        /* Purge weights */
        delete [] weights;
        weights = NULL;
        delete [] deltaWeights;
        deltaWeights = NULL;
        weightsCount = 0;
        /* Purge binds */
        delete [] binds;
        binds = NULL;
        bindsCount = 0;
    }

    /* Purge controls signal if exists */
    if( controls != NULL )
    {
        delete [] controls;
        controls = NULL;
    }
    return this;
}



/*
    Fill weights
*/
Nerve* Nerve::fill
(
    /* Rnd seed */
    Rnd*    aSeed,
    /* MinWeight */
    real  aMinWeight,
    /* MaxWeight */
    real  aMaxWeight
)
{
    /* Fill weights */
    if( aMinWeight > aMaxWeight )
    {
        aMinWeight = minWeight;
        aMaxWeight = maxWeight;
    }

    if( weights != NULL )
    {
        for( int i = 0; i < weightsCount; i++ )
        {
            weights[ i ]
            = aSeed == NULL
            ? aMinWeight
            : aSeed -> get( aMinWeight, aMaxWeight );
            deltaWeights[ i ] = 0.0;
        }
    }

    /* Fill binds */
    auto cp = parent -> getCount();
    auto cc = child -> getCount();

    switch( nerveType )
    {
        case ALL_TO_ALL:
        {
            /*
                Заполняем каждую связь прямо ссылаясб на массив весов
                те каждая связь уникальна и обладает своим весом.
            */
            for( int i = 0; i < bindsCount; i ++ )
            {
                binds[ i ] = i;
            }
        }
        break;

        case ONE_TO_ONE:
        {
            /* Reset index of weights */
            for( int i = 0; i < bindsCount; i ++ )
            {
                binds[ i ] = -1;
            }

            auto parentSize = parent -> getSize();
            auto childSize = child -> getSize();

            if( cp > cc )
            {
                /* Для случая если парентов больше чем потомков */
                for( int ip = 0; ip < cp; ip ++ )
                {
                    auto ic = calcChildPosByParentIndex( ip ).toIndex( childSize );
                    binds[ ip + ic * cp ] = ip;
                }
            }
            else
            {
                /* Для случая если потомков больше чем потомков или столько же */
                for( int ic = 0; ic < cc; ic ++ )
                {
                    int ip = calcParentPosByChildIndex( ic ).toIndex( parentSize );
                    binds[ ip + ic * cp ] = ic;
                }
            }
        }
        break;

        case SOME_TO_SOME:
        {
            /* Reset index of weights */
            for( int i = 0; i < bindsCount; i ++ )
            {
                binds[ i ] = -1;
            }

            auto parentSize = getParent() -> getSize();
            /* Обходим потомков */
            for( int ic = 0; ic < cc; ic ++ )
            {
                /*
                    Для каждого потомка находим отражение в родителях
                    нормированием, при этом нейрон будет центром матрицы
                    весов
                */
                auto parentCenter = calcParentPosByChildIndex( ic );

                /* Обходим матрицу весов и находим родителей для каждого веса */
                for( int iw = 0; iw < weightsCount; iw ++ )
                {
                    /* Calc parent pos by weight index */
                    auto p = parentCenter + Point3i::byIndex( iw, size ) - size / 2;

                    if
                    (
                        p.x > -1 && p.x < parentSize.x &&
                        p.y > -1 && p.y < parentSize.y &&
                        p.z > -1 && p.z < parentSize.z
                    )
                    {
                        /* В указатели на веса записываем текущий индекс связи */
                        binds[ p.toIndex( parentSize ) + ic * cp ] = iw;
                    }
                }
            }
        }
        break;
    }

    return this;
}



/*
    Reflect child neuron index to parent neuron position

    P         C
    6         3

    *         *
    *
    O----<----O
    *
    *         *
    *

*/

Point3i Nerve::calcParentPosByChildIndex(int aChildIndex)
{
    auto childPos = getChild()->calcPosByIndex(aChildIndex);
    auto childSize = getChild()->getSize();
    auto parentSize = getParent()->getSize();

    Point3i parentPos;
    parentPos.x = (childPos.x * parentSize.x) / childSize.x;
    parentPos.y = (childPos.y * parentSize.y) / childSize.y;
    parentPos.z = (childPos.z * parentSize.z) / childSize.z;
    return parentPos;
}




/*
    Reflect parent neuron index to child neuron position

    P         C
    6         3

    *         *
    *
    O---->----O
    *
    *         *
    *

*/
Point3i Nerve::calcChildPosByParentIndex( int aParentIndex )
{
    auto parentPos = getParent()->calcPosByIndex(aParentIndex);
    auto parentSize = getParent()->getSize();
    auto childSize = getChild()->getSize();

    Point3i childPos;
    childPos.x = (parentPos.x * childSize.x) / parentSize.x;
    childPos.y = (parentPos.y * childSize.y) / parentSize.y;
    childPos.z = (parentPos.z * childSize.z) / parentSize.z;

    return childPos;
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
    if( aSize == weightsCount * sizeof( real ) )
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
        aSize = sizeof( real ) * weightsCount;
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
            if( size != weightsCount * sizeof( real ) )
            {
                setResult( "WeightFileSizeChanged" )
                -> getDetails()
                -> setString( "file", file )
                -> setInt( "fileSize", size )
                -> setInt( "weightSize", weightsCount * sizeof( real ) )
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
                        /* Fill deltaweight with 0 */
                        std::memset( deltaWeights, 0, size );

                        getLog()
                        -> trace( "Weights loaded" )
                        -> prm( "file", file )
                        -> prm( "from", getChild() -> getId() )
                        -> prm( "to", getParent() -> getId() )
                        ;
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
            auto writed = fwrite( weights, weightsCount * sizeof( real ), 1, h );
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
    int from = 0, to = 0;
// TODO
// Закоментировано так как метод возможно не нужен используется только
// для возврата буффера по кнкретному нейрону для отправки на UI
//  getWeightsRangeByChildIndex( aNeuronIndex, from, to );
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
    int from = 0, to = 0, step = 0;
// TODO
// Закоментировано так как метод возможно не нужен используется только
// для возврата буффера по кнкретному нейрону для отправки на UI
//    getWeightsRangeByParentIndex( aNeuronIndex, from, to, step );

    int count = ceil(( to - from ) / (real) step);
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



/*
    Calculate nerve id like
        parent_id-(bind,nerve)-child_id
*/
string Nerve::calcId()
{
    return
    getParent() -> getId()
    + "-("
    + bindTypeToString( getBindType() )
    + ","
    + nerveTypeToString( getNerveType())
    + ")-"
    + getChild() -> getId();
}
