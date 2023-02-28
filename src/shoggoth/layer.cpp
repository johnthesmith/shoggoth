#include "../lib/rnd.h"

#include "../graph/point3.h"

#include "func.h"
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



/*
    Return linear index of neuron by position
*/
int Layer::indexByPos
(
    const Point3i& a
)
{
    return a.x + a.y * size.x + a.z * size.x * size.y;
}



/*
    Prived method for the neuron creation
*/
Neuron* Layer::newNeuron()
{
    Neuron* result = new Neuron( this );
    result -> setValue( 0.0 );
    return result;
}



/*
    Connect all neurons from current layer to argument layer
    (i) -> (j)
*/
Layer* Layer::connectTo
(
    Layer*      aLayer,
    ConnectType aConnectType,
    BindType    aBindType,
    double      aMinWeight,
    double      aMaxWeight
)
{
    getLog()
    .begin( "Layer connecting" )
    .prm( "from", getNameOrId() )
    .prm( "to", aLayer -> getNameOrId() )
    .lineEnd();

    int cFrom = neurons -> getCount();
    int cTo = aLayer -> neurons -> getCount();

    BindList binds = BindList();

    switch( aConnectType )
    {
        case ONE_TO_ONE:
        {
            int maxIndex = max( cFrom, cTo );

            binds.resize( maxIndex );

            for( int i = 0; i < maxIndex; i++ )
            {
                int iFrom = (int) ((float) i / (float) maxIndex * cFrom ) ;
                int iTo = (int) ((float) i / (float) maxIndex * cTo );

                /* Create bind */
                Bind* bind = new Bind();

                /* Fill bind */
                bind
                -> setType( aBindType )
                -> setWeight( Rnd::get( aMinWeight, aMaxWeight ))
                -> setParent( neurons -> getByIndex( iFrom ))
                -> setChild( aLayer -> neurons -> getByIndex( iTo ));

                /* Store bind */
                binds.setByIndex( i, bind );

                neurons -> getByIndex( iFrom ) -> childrenBinds -> push( bind );
                aLayer -> neurons -> getByIndex( iTo ) -> parentBinds -> push( bind );
            }
        }
        break;

        case ALL_TO_ALL:
        {
            /* Create bind list */

            binds.resize( cFrom * cTo );

            for( int iFrom = 0; iFrom < cFrom; iFrom++ )
            {
                for( int iTo = 0; iTo < cTo; iTo++ )
                {
                    /* Create bind */
                    Bind* bind = new Bind();

                    /* Fill bind */
                    bind
                    -> setType( aBindType )
                    -> setWeight( Rnd::get( aMinWeight, aMaxWeight ))
                    -> setParent( neurons -> getByIndex( iFrom ))
                    -> setChild( aLayer -> neurons -> getByIndex( iTo ));

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
                aLayer -> neurons            /* for child layer */
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
        }
        break;
    }


    getLog().trace("").prm( "Binds count", binds.getCount() ).end();

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



Layer* Layer::switchShowBinds()
{
    switch( showBinds )
    {
        case BDM_HIDDEN: showBinds = BDM_WEIGHT; break;
        case BDM_WEIGHT: showBinds = BDM_TYPE; break;
        case BDM_TYPE: showBinds = BDM_HIDDEN; break;
    }
    return this;
}



Layer* Layer::switchShowLayer()
{
    showLayer = !showLayer;
    return this;
}



Point3i Layer::getSize()
{
    return size;
}



int Layer::getNeuronsCount()
{
    return neurons -> getCount();
}



BindDrawMode Layer::getShowBinds()
{
    return showBinds;
}



bool Layer::getShowLayer()
{
    return showLayer;
}


