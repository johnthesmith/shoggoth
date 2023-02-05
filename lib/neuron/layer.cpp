#include "../log_points.h"
#include "../point3.h"
#include "../rnd.h"

#include "layer.h"
#include "neuron.h"




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
    /* Create neurons point list */
    points = new Points3d();
    id = Rnd::getUuid();
}



/*
    Internal destructor
*/
Layer::~Layer()
{
    /* Destroy neurons */
    setSize();

    /* Destroy the list of points for neurons */
    delete points;

    /* Destroy the list of neurons */
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



int Layer::indexByPos
(
    const Point3i& a
)
{
    return a.x + a.y * size.x + a.z * size.x * size.y;
}



/*
    New neoron method
*/
Neuron* Layer::newNeuron()
{
    Neuron* result = new Neuron();
    result -> setLayer( this );
    result -> setValue( Rnd::get( 0.0, 1.0 ) );
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
    auto si = neurons -> getCount();
    for( int i = 0; i < si; i++ )
    {
        Neuron* iNeuron = neurons -> getByIndex( i );
        iNeuron -> addChildren( a -> neurons, 0 ); /* TODO rnd min max */
    }

//    /* Create parnet binds */
//    auto sj = a -> neurons -> getCount();
//    for( int j = 0; j < sj; j++ )
//    {
//        Neuron* jNeuron = a -> neurons -> getByIndex( j );
//        jNeuron -> addParents( *neurons );
//    }

    getLog().end();

    return this;
}



Layer* Layer::neuronPointsCalc()
{
    if( pointsRecalc )
    {
        getLog().begin( "Calculate neurons position" ).prm( "Layer", getNameOrId() );

        /* Calculate box */
        auto box = Point3d
        (
            drawSize.x == 0 ? ( size.x - 1 ) * neuronDrawBox : drawSize.x,
            drawSize.y == 0 ? ( size.y - 1 ) * neuronDrawBox : drawSize.y,
            drawSize.z == 0 ? ( size.z - 1 ) * neuronDrawBox : drawSize.z
        );

        /* Claculate step in world for neurons */
        auto step = box / ( size - POINT_3I_I );

        /* Calculate ege in the world and start point in the world */
        Point3d ege = box * -0.5 + getTarget();
        Point3d p = ege;

        int i = 0;

        for( int z = 0; z < size.z; z++ )
        {
            for( int y = 0; y < size.y; y++ )
            {
                for( int x = 0; x < size.x; x++ )
                {
                    Neuron* n = neurons -> getByIndex( i );
                    points -> setByIndex( i, p );

                    p.x += step.x;
                    i++;
                }
                p.y += step.y;
                p.x = ege.x;
            }
            p.z += step.z;
            p.y = ege.y;
        }

        /* */
        setChanged( false );

        getLog().end();
    }


    return this;
}



Layer* Layer::draw
(
    Scene& aScene
)
{
    if( getChanged() )
    {
        neuronPointsCalc();
    }

    /* Draw center point */
    aScene
    .setPointSize( 4 )
    .begin( POINT )
    .color( Rgba( RGBA_WHITE ))
    .vertex( getTarget())
    .end();

    /* Calculate box */
    auto box = Point3d
    (
        drawSize.x == 0 ? ( size.x - 1 ) * neuronDrawBox : drawSize.x,
        drawSize.y == 0 ? ( size.y - 1 ) * neuronDrawBox : drawSize.y,
        drawSize.z == 0 ? ( size.z - 1 ) * neuronDrawBox : drawSize.z
    );

    auto step = box / ( size - POINT_3I_I );


    Point3d ege = box * -0.5 + getTarget();
    Point3d p = ege;


    if( pointsRecalc )
    {
        int currentSize = neurons -> getCount();

        /* Draw nurons point */
        aScene
        .setPointSize( neuronDrawSize )
        .begin( POINT );
        for( int i = 0; i < currentSize; i++ )
        {
            Neuron* n = neurons -> getByIndex( i );
            aScene.color( Rgba( 1.0, 0.7, 0.0, n -> getValue() ));
            aScene.vertex( points -> items[ i ] );
        }
        aScene.end();

        /* Draw neuron links */
        aScene.color( Rgba( 1.0, 1.0, 1.0, 0.1 ));

        aScene.begin( LINE );
        for( int i = 0; i < currentSize; i++ )
        {
            Neuron* iNeuron = neurons -> getByIndex( i );
            auto countChildren = iNeuron -> children.getCount();
            for( int c = 0; c < countChildren; c++ )
            {
                Neuron* cNeuron = iNeuron -> children.getByIndex( c );
                /* Draw bind */
                aScene
                .vertex( points -> items[ i ] )
                .vertex( cNeuron -> getLayer().points -> items[ c ] );
            }
        }
        aScene.end();

    }


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
    auto currentSize = neurons -> getCount();
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
        points -> resize( newSize );

        /* Neurons create */
        for( int i = currentSize; i < newSize; i++ )
        {
            auto nNeuron = newNeuron();
            neurons -> setByIndex( i, nNeuron );
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
            points -> resize( newSize );
        }
    }

    size = a;

    /* Recalculate neuron points */
    neuronPointsCalc();

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



Layer* Layer::setNeuronDrawBox
(
    const double a
)
{
    neuronDrawBox = a;
    return this;
}



Layer* Layer::setNeuronDrawSize
(
    const double a
)
{
    neuronDrawSize = a;
    return this;
}



Layer* Layer::setBorderSize
(
    const Point3d& a
)
{
    borderSize = a;
    return this;
}



Layer* Layer::setDrawSize
(
    const Point3d& a
)
{
    drawSize = a;
    return this;
}



Layer* Layer::setPointsRecalc
(
    bool a
)
{
    pointsRecalc = a;
    neuronPointsCalc();
    return this;
}



