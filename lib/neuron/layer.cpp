#include "layer.h"
#include "../log_points.h"
#include "neuron.h"
#include "../point3.h"



/*
    Constructor
*/
Layer::Layer
(
    Log& aLog
):
    log( aLog )
{
    log.trace( "Create layer" );
    /* Create list of neurons */
    neurons = new NeuronList();
}



/*
    Internal destructor
*/
Layer::~Layer()
{
    /* Destroy neurons */
    setSize();

    /* Destroy list of neurons */
    delete neurons;

    getLog()
    .trace( "Layer destroy" )
    .prm( "Name", getNameOrId() );
}



/*
    Creator
*/
Layer* Layer::create
(
    Log& aLog
)
{
    return new Layer( aLog );
}



/*
    Destructor
*/
void Layer::destroy()
{
    delete this;
}



/*
    Add new neuron and return it
*/
Layer* Layer::addNeuron
(
    Neuron& aNeuron
)
{
    neurons -> add( aNeuron );
    return this;
}



int Layer::indexByPos
(
    const Point3i& a
)
{
    return a.x + a.y * size.x + a.z * size.x * size.y;
}



Neuron* Layer::newNeuron()
{
    Neuron* result = new Neuron();
    result -> setLayer( this );
    return result;
}



/*
    Connect all neurons from current layer to argument
    (i) -> (j)
*/
Layer* Layer::connectTo
(
    Layer* a
)
{
    getLog()
    .begin( "Layer connecting" )
    .prm( "from", getNameOrId() )
    .prm( "to", a -> getNameOrId() )
    .lineEnd();

    /* Create children binds */
    auto si = neurons -> getSize();
    for( int i = 0; i < si; i++ )
    {
        auto iNeuron = neurons -> getByIndex( i );
        iNeuron -> addChildren( *a -> neurons, 0 ); /* TODO rnd min max */
    }

    /* Create parnet binds */
    auto sj = a -> neurons -> getSize();
    for( int j = 0; j < sj; j++ )
    {
        auto jNeuron = a -> neurons -> getByIndex( j );
        jNeuron -> addParents( *neurons );
    }

    getLog().end();

    return this;
}



Layer* Layer::draw
(
    Scene& aScene
)
{
    /* Draw center point */
    glPointSize( 4 );
    aScene
    .begin( POINT )
    .color( Rgba( RGBA_WHITE ))
    .vertex( getTarget())
    .end();

    /* Calculate box */
    auto box = Point3d
    (
        drawSize.x == 0 ? ( size.x - 1 ) * neuronDrawSize : drawSize.x,
        drawSize.y == 0 ? ( size.y - 1 ) * neuronDrawSize : drawSize.y,
        drawSize.z == 0 ? ( size.z - 1 ) * neuronDrawSize : drawSize.z
    );

    auto step = box / ( size - POINT_3I_I );


    Point3d ege = box * -0.5 + getTarget();
    Point3d p = ege;


    aScene.color( Rgba( 1.0, 0.7, 0.0, 0.5 ));
glPointSize( 6 );

    aScene.begin( POINT );
    for( int z = 0; z < size.z; z++ )
    {
        for( int y = 0; y < size.y; y++ )
        {
            for( int x = 0; x < size.x; x++ )
            {
                aScene.vertex( p );
                p.x += step.x;
            }
            p.y += step.y;
            p.x = ege.x;
        }
        p.z += step.z;
        p.y = ege.y;
    }
    aScene.end();

    auto outerBox = Point3d().set( box ).scale( 0.5 ).add( borderSize );

    aScene
    .polygonMode( POLYGON_LINE )
    .begin( QUAD )
    .color( Rgba( 0.5, 0.7, 1.0, 0.1 ) )
    .sendQube( getTarget(), outerBox )
    .end()
    .polygonMode( POLYGON_FILL )
    .begin( QUAD )
    .color( Rgba( 0.5, 0.7, 1.0, 0.1 ) )
    .sendQube( getTarget(), outerBox )
    .end();

    return this;
}



//    drawSize
//
//
//
//    .polygonMode( POLYGON_FILL )
//
//    .begin( QUAD )
//    .color( c )
//    .sendQube( getTarget() )
//    .end()
//
//    ;
//


/***********************************************************************
    Setters and getters
*/

/*
    Return the log object
*/
Log& Layer::getLog()
{
    return log;
}



/*
    Set dimations size
*/
Layer* Layer::setSize
(
    const Point3i& a
)
{
    auto currentSize = neurons -> getSize();
    auto newSize = a.x * a.y * a.z;

    getLog()
    .begin( "Layer resize" )
    .prm( "id", getNameOrId() )
    .prm( "from", currentSize )
    .prm( "to", newSize )
    .lineEnd();

    if( newSize > currentSize )
    {
        /* Size increase */
        neurons -> resize( newSize );

        for( int i = currentSize; i < newSize; i++ )
        {
            neurons -> setByIndex( i, newNeuron() );
        }
    }
    else
    {
        if( newSize < currentSize )
        {
            /* Size decrease */
            for( int i = newSize; i < currentSize; i++ )
            {
                auto n = neurons -> getByIndex( i );
                if( !n -> isNull())
                {
                    delete n;
                }
            }

            neurons -> resize( newSize );
        }
    }

    size = a;

    getLog().end();

    return this;
}



string Layer::getName()
{
   return name;
}



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



string Layer::getNameOrId()
{
   return name == "" ? id : name;
}




