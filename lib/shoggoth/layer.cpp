#include "../rnd.h"

#include "../graph/point3.h"
#include "../graph/log_points.h"

#include "layer.h"
#include "neuron.h"
#include "func.h"




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

    id = Rnd::getUuid();
}



/*
    Internal destructor
*/
Layer::~Layer()
{
    /* Destroy neurons */
    setSize();

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
    Neuron* result = new Neuron( this );
    result -> setValue( 0.0 );
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

    /* Create bind list */
    int cFrom = neurons -> getCount();
    int cTo = a -> neurons -> getCount();

    BindList binds = BindList();
    binds.resize( cFrom * cTo );

    for( int iFrom = 0; iFrom < cFrom; iFrom++ )
    {
        for( int iTo = 0; iTo < cTo; iTo++ )
        {
            /* Create bind */
            Bind* bind = new Bind();

            /* Fill bind */
            bind
            -> setWeight( Rnd::get( -0.9, 0.9 ))
            -> setParent( neurons -> getByIndex( iFrom ))
            -> setChild( a -> neurons -> getByIndex( iTo ));

            /* Store bind */
            binds.setByIndex( iFrom * cTo + iTo, bind );
        }
    }

    /* Resize children list in parent layer */
    for( int iFrom = 0; iFrom < cFrom; iFrom++ )
    {
        neurons                 /* for this layer */
        -> getByIndex( iFrom )  /* get the next neuron */
        -> childrenBinds        /* and him children list */
        -> expand( cTo );       /* expand to count of childrens */
    }

    /* Resize parents list in child layer */
    for( int iTo = 0; iTo < cTo; iTo++ )
    {
        a -> neurons            /* for child layer */
        -> getByIndex( iTo )    /* get the next neuron */
        -> parentBinds          /* and him parents list */
        -> expand( cFrom );     /* expand to count of parents */
    }

    /* Run at binds */
    int c = binds.getCount();
    for( int i = 0; i < c; i++ )
    {
        Bind* iBind = binds.getByIndex( i );

        Neuron* parent = iBind -> getParent();
        Neuron* child = iBind -> getChild();

        int parentChildIndex = parent -> childrenBinds -> getCount() - cTo + i % cTo;
        int childParentIndex = child -> parentBinds -> getCount() - cFrom + i / cTo;

        parent -> childrenBinds -> setByIndex( parentChildIndex, iBind );
        child -> parentBinds -> setByIndex( childParentIndex, iBind );
    }

    getLog().end();

    return this;
}



/*
    Calculate neuron position at world
*/
Layer* Layer::neuronPointsCalc()
{
    if( pointsRecalc )
    {
        getLog().begin( "Calculate neurons world position" ).prm( "Layer", getNameOrId() );

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
                    n -> setWorldPoint( p );
                    p.x += step.x;
                    i++;
                }
                p.y += step.y;
                p.x = ege.x;
            }
            p.z += step.z;
            p.y = ege.y;
        }

        getLog().end();
    }
    return this;
}



/*
    Recalculate screeen position
*/

Layer* Layer::neuronPointsScreenCalc
(
    Scene* aScene /* Scene object */
)
{
    getLog().begin( "Calculate neurons screen position" ).prm( "Layer", getNameOrId() );

    /* Get count of points in the layer */
    int neuronsCount = neurons -> getCount();

    neurons -> loop
    (
        [ aScene ]
        (
            Neuron* neuron
        ) -> bool
        {
            auto p = aScene -> getScreenByWorld( neuron -> getWorldPoint());
            neuron -> setScreenPoint( p );
            return false;
        }
    );


    getLog().end();

    return this;
}



Layer* Layer::calcValue
(
    bool aLoopParity
)
{
    bool calculatedValueFinish = true;

    /* Calculate neurons */
    neurons -> loop
    (
        [ &calculatedValueFinish, &aLoopParity ]( Neuron* neuron ) -> bool
        {
            if( neuron -> getLoopParityValue() != aLoopParity )
            {
                /* Calc neuron */
                neuron -> calcValue( aLoopParity );
                /* Accumulate layer finish value */
                calculatedValueFinish
                = calculatedValueFinish
                && ( neuron -> getLoopParityValue() == aLoopParity );
            }
            return false;
        }
    );

    /* Fixing the loop parity for layer */
    if( calculatedValueFinish )
    {
        loopParityValue = aLoopParity;
    }


    /* Calculate 3d */
    if( getChanged() )
    {
        neuronPointsCalc();
        setChanged( false );
    }

    return this;
}



/*
    Calculate neurons error for learning
*/
Layer* Layer::calcError
(
    bool aLoopParity
)
{
    bool calculatedErrorFinish = true;

    /* Calculate neurons */
    neurons -> loop
    (
        [ &calculatedErrorFinish, &aLoopParity ]( Neuron* neuron ) -> bool
        {
            if( neuron -> getLoopParityError() != aLoopParity )
            {
                /* Calc neuron */
                neuron -> calcError( aLoopParity );
                /* Accumulate layer finish value */
                calculatedErrorFinish
                = calculatedErrorFinish
                && ( neuron -> getLoopParityError() == aLoopParity );
            }
            return false;
        }
    );

    /* Fixing the loop parity for layer */
    if( calculatedErrorFinish )
    {
        loopParityError = aLoopParity;
    }

    return this;
}



/*
    Calculate learning
*/
Layer* Layer::learning()
{
    /* Calculate neurons */
    neurons -> loop
    (
        []( Neuron* neuron ) -> bool
        {
            /* Calc neuron */
            neuron -> learning();
            return false;
        }
    );

    return this;
}




Layer* Layer::draw
(
    Scene* aScene,
    bool calcScreenPos
)
{
    /* Calculate scree point position */
    if( calcScreenPos )
    {
        neuronPointsScreenCalc( aScene );
    }

    /* Calculate box */
    auto box = Point3d
    (
        drawSize.x == 0 ? ( size.x - 1 ) * neuronDrawBox : drawSize.x,
        drawSize.y == 0 ? ( size.y - 1 ) * neuronDrawBox : drawSize.y,
        drawSize.z == 0 ? ( size.z - 1 ) * neuronDrawBox : drawSize.z
    );

    /* Calculate step between neurons */
    auto step = box / ( size - POINT_3I_I );

    /* Calculate bound box for layer */
    Point3d ege = box * -0.5 + getTarget();
    Point3d p = ege;

    /* Draw nurons point */
    aScene
    -> setPointSize( neuronDrawSize )
    .begin( POINT );
    int currentSize = neurons -> getCount();
    for( int i = 0; i < currentSize; i++ )
    {
        Neuron* n = neurons -> getByIndex( i );
        Rgba c;
        switch( neuronDrawMode )
        {
            case NDM_VALUE:
                c = Rgba( colorValue0 ).itpLin( colorValue1, n -> getValue() );
            break;
            case NDM_ERROR:
                if( layerType != LT_RECEPTOR )
                {
                    c = Rgba( colorError0 ).itpLin( colorError1, n -> getError() );
                }
                else
                {
                    c = Rgba( colorValue0 ).itpLin( colorValue1, n -> getValue() );
                }
            break;
        }
        aScene -> color( c );
        aScene -> vertex( n -> getWorldPoint() );
    }
    aScene -> end();

    /* Draw neuron links */
    aScene -> setLineWidth( 1 );
    aScene -> begin( LINE );
    for( int i = 0; i < currentSize; i++ )
    {
        Neuron* iNeuron = neurons -> getByIndex( i );
        auto countChildren = iNeuron -> childrenBinds -> getCount();
        for( int c = 0; c < countChildren; c++ )
        {
            Bind* bind = iNeuron -> childrenBinds -> getByIndex( c );
            Neuron* cNeuron = bind -> getChild();

            /* Draw bind */

            auto w = bind -> getWeight();

            aScene
            -> color( Rgba( 0, 1, 0, w * 0.1 ))
            .vertex( iNeuron -> getWorldPoint() )
            .vertex( cNeuron -> getWorldPoint() )
            ;
        }
    }
    aScene -> end();

    /* Draw layer center point */
    aScene
    -> setPointSize( 4 )
    .begin( POINT )
    .color( Rgba( RGBA_WHITE ))
    .vertex( getTarget())
    .end();


    /* Draw layer box */
    auto outerBox = Point3d().set( box ).scale( 0.5 ).add( borderSize );

    /* Define color of neuron */
    Rgba c;
    switch( layerType )
    {
        case LT_RECEPTOR: c = colorLayerTypeReceptor; break;
        case LT_CORTEX: c = colorLayerTypeCortex; break;
        case LT_RESULT: c = colorLayerTypeResult; break;
    }

    aScene
    -> setLineWidth( 1 )
    .polygonMode( POLYGON_LINE )
    .color( c )
    .begin( QUAD )
    .sendQube( getTarget(), outerBox )
    .end()
    .polygonMode( POLYGON_FILL )
    .begin( QUAD )
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

        /* Neurons create */
        for( int i = currentSize; i < newSize; i++ )
        {
            neurons -> setByIndex( i, newNeuron());
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



/*
    Return true if neurons error changed in calcError method
*/
bool Layer::getErrorChange()
{
    return errorChange;
}




/******************************************************************************
    Setters and getters
*/



/*
    Set sensivity of layer
*/
Layer* Layer::setSensivity
(
    const double a
)
{
    sensivity = a;
    return this;
}



/*
    Get sensivity of layer
*/
double Layer::getSensivity()
{
    return sensivity;
}



/*
    Return parity of loop for current layer
*/
bool Layer::getLoopParityValue()
{
    return loopParityValue;
}



/*
    Return parity of loop for current layer
*/
bool Layer::getLoopParityError()
{
    return loopParityError;
}



/*
    Return list of neurons in screen rect
*/
Layer* Layer::getNeuronsByScreenRect
(
    NeuronList* aList,
    Point3d& aTopLeft,      /* Top left point */
    Point3d& aBottomRight   /* Bottom right point */
)
{
    NeuronList* buffer = NeuronList::create();
    buffer -> resize( neurons -> getCount() );

    int iBuffer = 0;

    neurons -> loop
    (
        [ buffer, &aTopLeft, &aBottomRight, &iBuffer ]
        (
            Neuron* neuron
        ) -> bool
        {
            if( neuron -> getScreenPoint().testRectXY( aTopLeft, aBottomRight ))
            {
                buffer -> setByIndex( iBuffer, neuron );
                iBuffer++;
            }
            return false;
        }
    );

    buffer -> resize( iBuffer );
    aList -> add( buffer );
    buffer -> destroy();

    return this;
}



/*
    Return list of neurons around  the screen position
*/
Layer* Layer::getNeuronsByScreenPos
(
    NeuronList* aList,
    const Point3d& aPosition
)
{
    NeuronList* buffer = NeuronList::create();
    buffer -> resize( neurons -> getCount() );

    int iBuffer = 0;

    neurons -> loop
    (
        [ buffer, &aPosition, &iBuffer ]
        (
            Neuron* neuron
        ) -> bool
        {
            if( neuron -> getScreenPoint().distXY( aPosition ) < 10 )
            {
                buffer -> setByIndex( iBuffer, neuron );
                iBuffer++;
            }
            return false;
        }
    );

    buffer -> resize( iBuffer );
    aList -> add( buffer );
    buffer -> destroy();

    return this;
}



Layer* Layer::setNeuronDrawMode
(
    const NeuronDrawMode a
)
{
    neuronDrawMode = a;

    return this;
}



NeuronDrawMode Layer::getNeuronDrawMode()
{
    return neuronDrawMode;
}



Layer* Layer::setLayerType
(
    const LayerType a
)
{
    layerType = a;
    return this;
}



LayerType Layer::getLayerType()
{
    return layerType;
}
