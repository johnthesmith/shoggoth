#include "layer_ui.h"
#include "limb_ui.h"
#include "neuron_list_ui.h"
#include "neuron_ui.h"




/*
    Constructor
*/
LayerUi::LayerUi
(
    Limb* aLimb,
    string aId
)
:Layer( aLimb, aId )
{
    object = Object::create();
    getLog() -> trace( "... it was a layer ui" );
}



/*
    Destructor
*/
LayerUi::~LayerUi()
{
    delete object;
}



/*
    Creator
*/
LayerUi* LayerUi::create
(
    Limb* aLimb,
    string aId
)
{
    return new LayerUi( aLimb, aId );
}



/******************************************************************************
    Data plans work
*/



/*
    Clear screen points for all neurons
*/
LayerUi* LayerUi::clearScreen()
{
    getLimb() -> lock();
    memset( (void*) screen, 0, getCount() * sizeof( Point3d ));
    getLimb() -> unlock();
    return this;
}



/*
    Clear world for all neurons
*/
LayerUi* LayerUi::clearWorld()
{
    getLimb() -> lock();
    memset( (void*) world, 0, getCount() * sizeof( Point3d ));
    getLimb() -> unlock();
    return this;
}



/*
    Clear selected flag for all neurons
*/
LayerUi* LayerUi::clearSelected()
{
    getLimb() -> lock();
    memset( (void*) selected, 0, getCount() * sizeof( bool ));
    getLimb() -> unlock();
    return this;
}



/******************************************************************************c
    Calculations
*/



/*
    Calculate neuron position at world
*/
LayerUi* LayerUi::neuronPointsCalc
(
    bool aStrict
)
{
    if( aStrict || !pointsRecalc )
    {
        auto s = getSize();
        /* Calculate box */
        auto box = Point3d
        (
            drawSize.x == 0 ? ( s.x - 1 ) * neuronDrawBox : drawSize.x,
            drawSize.y == 0 ? ( s.y - 1 ) * neuronDrawBox : drawSize.y,
            drawSize.z == 0 ? ( s.z - 1 ) * neuronDrawBox : drawSize.z
        );

        /* Draw layer box */
        outerBox = Point3d().set( box ).scale( 0.5 ).add( borderSize );
        getLog() -> begin( "Calculate neurons world position" )-> prm( "Layer", getNameOrId() );

        /* Claculate step in world for neurons */
        auto step = box / ( s - POINT_3I_I );
        /* Calculate ege in the world and start point in the world */
        Point3d ege = box * -0.5 + object -> getEye();
        Point3d p = ege;

        int i = 0;

        getLimb() -> lock();
        for( int z = 0; z < s.z; z++ )
        {
            for( int y = 0; y < s.y; y++ )
            {
                for( int x = 0; x < s.x; x++ )
                {
                    setNeuronWorld( i, p );
                    p.x += step.x;
                    i++;
                }
                p.y += step.y;
                p.x = ege.x;
            }
            p.z += step.z;
            p.y = ege.y;
        }
        getLimb() -> unlock();

        getLog() -> end();

        setPointsRecalc( true );
    }

    return this;
}



/***********************************************************************
    Setters and getters
*/



LimbUi* LayerUi::getLimb()
{
    return ( LimbUi* ) Layer::getLimb();
}



/*
    Set neurons count and reallocate plans
*/
LayerUi* LayerUi::setSize
(
    /* New size of ui layer */
    Point3i aSize
)
{
    getLimb() -> lock();
    if( aSize != getSize() )
    {
        Layer::setSize( aSize );

        /* Create UI plans */
        worldCreate();
        screenCreate();
        selectedCreate();
    }
    getLimb() -> unlock();

    /*
        Set neurons recalculation task if draw size was changed
        Itsd no good. Following must calculated not for setCount but for setSize
    */
    setPointsRecalc( false );

    return this;
}



/*
    Resize world plan
*/
LayerUi* LayerUi::worldCreate()
{
    getLimb() -> lock();
    /* Delete old plan */
    if( world != NULL )
    {
        delete [] world;
    }

    /*Create new plan */
    world = new Point3d[ getCount() ];
    getLimb() -> unlock();

    /* Clear world position */
    clearWorld();
    /* Recalculate neuron points */
    neuronPointsCalc( true );

    getLog() -> trace( "World plan created" ) -> prm( "count", getCount() );

    return this;
}



/*
    Resize screen plan
*/
LayerUi* LayerUi::screenCreate()
{
    getLimb() -> lock();
    /* Delete old plan */
    if( screen != NULL )
    {
        delete [] screen;
    }

    /*Create new plan */
    screen = new Point3d[ getCount() ];
    getLimb() -> unlock();

    clearScreen();

    return this;
}


/*
    Resize selected plan
*/
LayerUi* LayerUi::selectedCreate()
{
    getLimb() -> lock();

    /* Delete old plan */
    if( selected != NULL )
    {
        delete [] selected;
    }

    /*Create new plan */
    selected = new bool[ getCount() ];
    getLimb() -> unlock();

    clearSelected();

    return this;
}



/*
    Set draw size for layer
*/
LayerUi* LayerUi::setDrawSize
(
    ParamList* a
)
{
    auto jsonPosition = a -> getObject( "drawSize" );
    if( jsonPosition != NULL )
    {
        setDrawSize
        (
            Point3d
            (
                jsonPosition -> getDouble( 0 ),
                jsonPosition -> getDouble( 1 ),
                jsonPosition -> getDouble( 2 )
            )
        );

        /* Set neurons recalculation task if draw size was changed */
        setPointsRecalc( false );
    }

    return this;
}



Point3d& LayerUi::getDrawSize()
{
    return drawSize;
}



/*
    Set dimations size
*/
LayerUi* LayerUi::setPosition
(
    ParamList* a
)
{
    auto jsonPosition = a -> getObject( "position" );
    if( jsonPosition != NULL )
    {
        object -> setEye
        (
            Point3d
            (
                jsonPosition -> getDouble( 0 ),
                jsonPosition -> getDouble( 1 ),
                jsonPosition -> getDouble( 2 )
            )
        );

        /* Set neurons recalculation task if position was changed */
        if(  object -> getChanged() )
        {
            setPointsRecalc( false );
        }
    }
    return this;
}



LayerUi* LayerUi::setNeuronDrawBox
(
    const double a
)
{
    neuronDrawBox = a;
    return this;
}



LayerUi* LayerUi::setNeuronDrawSize
(
    const double a
)
{
    neuronDrawSize = a;
    return this;
}



LayerUi* LayerUi::setBorderSize
(
    const Point3d& a
)
{
    borderSize = a;
    return this;
}



LayerUi* LayerUi::setDrawSize
(
    const Point3d& a
)
{
    if( drawSize != a )
    {
        drawSize = a;
        object -> setChanged( true );
    }
    return this;
}



LayerUi* LayerUi::setPointsRecalc
(
    bool a
)
{
    pointsRecalc = a;
    return this;
}



/*
    Return list of neurons in screen rect
*/
LayerUi* LayerUi::getNeuronsByScreenRect
(
    NeuronListUi* aList,
    Point3d& aTopLeft,      /* Top left point */
    Point3d& aBottomRight   /* Bottom right point */
)
{
    auto cThis = getCount();
    auto list = new int[ cThis ];
    int iBuffer = 0;

    /* Coolect indexes to list */
    getLimb() -> lock();
    for( int i = 0; i < cThis; i++ )
    {
        if
        (
            getNeuronScreen( i ).testRectXY( aTopLeft, aBottomRight )
        )
        {
            list[ iBuffer ] = i;
            iBuffer++;
        }
    }
    getLimb() -> unlock();

    int c = aList -> getCount();
    /* Resize the rusult buffer */
    aList -> resize( c + iBuffer );

    for( int i = 0; i < iBuffer; i++ )
    {
        aList -> setByIndex( c + i, NeuronUi::create( this, list[ i ]));
    }

    delete [] list;

    return this;
}



/*
    Return list of neurons around  the screen position
*/
LayerUi* LayerUi::getNeuronsByScreenPos
(
    NeuronListUi*   aList,
    const Point3d&  aPosition,
    const int       aScreenRadius
)
{
    auto cThis = getCount();
    auto list = new int[ cThis ];
    int iBuffer = 0;

    /* Coolect indexes to list */
    getLimb() -> lock();
    for( int i = 0; i < cThis; i++ )
    {
        auto s = getNeuronScreen( i );
        if
        (
            s.z < 1.0 &&
            s.distXY( aPosition ) < aScreenRadius
        )
        {
            list[ iBuffer ] = i;
            iBuffer++;
        }
    }
    getLimb() -> unlock();

    int c = aList -> getCount();

    /* Resize the rusult buffer */
    aList -> resize( c + iBuffer );
    for( int i = 0; i < iBuffer; i++ )
    {
        aList -> setByIndex
        (
            c + i,
            NeuronUi::create( this, list[ i ])
        );
    }

    delete [] list;

    return this;
}



LayerUi* LayerUi::switchShowLayer()
{
    showLayer = !showLayer;
    return this;
}



BindDrawMode LayerUi::getShowBinds()
{
    return showBinds;
}



LayerUi* LayerUi::setShowBinds
(
    BindDrawMode a
)
{
    showBinds = a;
    return this;
}



bool LayerUi::getShowLayer()
{
    return showLayer;
}



/*
    Return outer box size
*/
Point3d& LayerUi::getOuterBox()
{
    return outerBox;
}



/**********************************************************************
    Neurons setters and getters
*/


/*
    Set neuron world position
*/
LayerUi* LayerUi::setNeuronWorld
(
    int aIndex,           /* Index of neuron */
    Point3d& aValue       /* Value */
)
{
    if( world != NULL )
    {
        if( aIndex >= 0 && aIndex < getCount() )
        {
            world[ aIndex ] = aValue;
        }
        else
        {
            setResult( "IndexWorldOutOfRangeForSet" );
        }
    }
    else
    {
        setResult( "NeuronWorldArrayNotDefinedForSet" );
    }
    return this;
}



/*
    Return neuron world position or default value
*/
Point3d LayerUi::getNeuronWorld
(
    int aIndex  /* Index of neuron */
)
{
    auto result = POINT_3D_0;

    if( world != NULL )
    {
        if( aIndex >= 0 && aIndex < getCount() )
        {
            result = world[ aIndex ];
        }
        else
        {
            setResult( "IndexWorldPointsOutOfRangeForGet" );
        }
    }
    else
    {
        setResult( "WorldPointsNotDefinedforForGet" );
    }
    return result;
}



/*
    Set neuron screen position
*/
LayerUi* LayerUi::setNeuronScreen
(
    int aIndex,          /* Index of neuron */
    Point3d& aValue      /* Value */
)
{
    if( screen != NULL )
    {
        if( aIndex >= 0 && aIndex < getCount() )
        {
            screen[ aIndex ] = aValue;
        }
        else
        {
            setResult( "IndexWorldPointsOutOfRangeForSet" );
        }
    }
    else
    {
        setResult( "WorldPointsNotDefinedforForSet" );
    }

    return this;
}



/*
    Return neuron screen position or default value
*/
Point3d LayerUi::getNeuronScreen
(
    int aIndex  /* Index of neuron */
)
{
    auto result = POINT_3D_0;

    if( screen != NULL )
    {
        if( aIndex >= 0 && aIndex < getCount() )
        {
            result = screen[ aIndex ];
        }
        else
        {
            setResult( "IndexScreenPointsOutOfRangeForGet" );
        }
    }
    else
    {
        setResult( "ScreenPointsNotDefinedforForGet" );
    }

    return result;
}




/*
    Set neuron selected
*/
LayerUi* LayerUi::setNeuronSelected
(
    int aIndex,     /* Index of neuron */
    bool aValue     /* Value */
)
{
    if( selected != NULL )
    {
        if( aIndex >= 0 && aIndex < getCount() )
        {
            selected[ aIndex ] = aValue;
        }
        else
        {
            setResult( "IndexScreenPointsOutOfRangeForSet" );
        }
    }
    else
    {
        setResult( "ScreenPointsNotDefinedForSet" );
    }

    return this;
}



/*
    Return neuron selected or default value
*/
Point3d LayerUi::getNeuronSelected
(
    int aIndex     /* Index of neuron */
)
{
    auto result = false;

    if( selected != NULL )
    {
        if( aIndex >= 0 && aIndex < getCount() )
        {
            result = selected[ aIndex ];
        }
        else
        {
            setResult( "IndexSelectedOutOfRangeForGet" );
        }
    }
    else
    {
        setResult( "SelectedNotDefinedForGet" );
    }

    return result;
}




Object* LayerUi::getObject()
{
    return object;
}
