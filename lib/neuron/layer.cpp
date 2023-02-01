#include "layer.h"
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
    auto p1 = Point3d( getTarget()).add( POINT_3D_Z_05 ).sub( POINT_3D_X_05 ).sub( POINT_3D_Y_05 );
    auto p2 = Point3d( p1 ).add( POINT_3D_Y );
    auto p3 = Point3d( p2 ).add( POINT_3D_X );
    auto p4 = Point3d( p1 ).add( POINT_3D_X );

    auto p5 = Point3d( p1 ).sub( POINT_3D_Z );
    auto p6 = Point3d( p2 ).sub( POINT_3D_Z );
    auto p7 = Point3d( p3 ).sub( POINT_3D_Z );
    auto p8 = Point3d( p4 ).sub( POINT_3D_Z );

    auto c = Rgba( 0.5, 0.7, 1.0, 0.3 );

     aScene
     .begin( QUAD )
     .color( c )
     .vertex( p1 ).vertex( p2 ).vertex( p3 ).vertex( p4 )
     .vertex( p4 ).vertex( p3 ).vertex( p7 ).vertex( p8 )
     .vertex( p5 ).vertex( p8 ).vertex( p7 ).vertex( p6 )
     .vertex( p1 ).vertex( p5 ).vertex( p6 ).vertex( p2 )
     .vertex( p2 ).vertex( p6 ).vertex( p7 ).vertex( p3 )
     .vertex( p1 ).vertex( p4 ).vertex( p8 ).vertex( p5 )
     .end();

glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

     aScene
     .begin( QUAD )
     .color( c )
     .vertex( p1 ).vertex( p2 ).vertex( p3 ).vertex( p4 )
     .vertex( p4 ).vertex( p3 ).vertex( p7 ).vertex( p8 )
     .vertex( p5 ).vertex( p8 ).vertex( p7 ).vertex( p6 )
     .vertex( p1 ).vertex( p5 ).vertex( p6 ).vertex( p2 )
     .vertex( p2 ).vertex( p6 ).vertex( p7 ).vertex( p3 )
     .vertex( p1 ).vertex( p4 ).vertex( p8 ).vertex( p5 )
     .end();

glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    return this;
}


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




