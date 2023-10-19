#include "net_graph.h"

/* System */
#include <functional>
#include <iostream>

/* Graph libraries */
#include "../graph/chart.h"
#include "../graph/log_points.h"

/* Shoggot libraries */
#include "func.h"
#include "neuron.h"



/*
    Constructor
*/
NetGraph::NetGraph
(
    Application* a
)
: Net( a ) /* Call parent constructor */
{
    selected = NeuronList::create();
}



/*
    Destructor
*/
NetGraph::~NetGraph()
{
    selected -> destroy();
}



/*
    Create
*/
NetGraph* NetGraph:: create
(
    Application*    a
)
{
    return new NetGraph( a );
}



/*
    Draw layers
*/
NetGraph* NetGraph::drawLayers
(
    Scene* aScene,          /* Scene object */
    bool   aCalcScreenPos   /* Camera changed */
)
{
    getLayers() -> loop
    (
        [ this, aScene, aCalcScreenPos ]
        (
            void* iLayer
        ) -> bool
        {
            drawLayer( aScene, ( Layer* ) iLayer, aCalcScreenPos );
            return false;
        }
    );
    return this;
}




/*
    Draw layer
*/
NetGraph* NetGraph::drawLayer
(
    Scene*  aScene,
    Layer*  aLayer,
    bool    aCalcScreenPos
)
{
    /* Recalculate neurons position */
    aLayer -> neuronPointsCalc( false );

    /* Calculate scree point position */
    if( aCalcScreenPos )
    {
        neuronPointsScreenCalc( aScene, aLayer );
    }

    /* Get neurons conut */
    int neuronsCount = aLayer -> getNeuronsCount();

    /* Draw nurons point */
    aScene
    -> setPointSize( neuronDrawSize )
    -> begin( POINT );

    for( int i = 0; i < neuronsCount; i++ )
    {
        Rgba c;
        switch( neuronDrawMode )
        {
            case NDM_VALUE:
                c = Rgba( colorValueZ )
                .itpLin( colorValueP, aLayer -> getNeuronValue( i ));
            break;
            case NDM_ERROR:
                c = getErrorColor( aLayer -> getNeuronError( i ));
            break;
        }
        aScene
        -> color( c )
        -> vertex( aLayer -> getNeuronWorld( i ));
    }
    aScene -> end();


    if( aLayer -> getShowLayer() )
    {
        /* Draw layer center point */
        aScene
        -> setPointSize( 4 )
        -> begin( POINT )
        -> color( Rgba( RGBA_WHITE ))
        -> vertex( aLayer -> getEye() )
        -> end();

        /*  Draw layer box */
        aScene
        -> setLineWidth( 1 )
        -> polygonMode( POLYGON_LINE )
        .color( interfaceColorDark )
        -> begin( QUAD )
        -> sendQube( aLayer -> getEye(), aLayer -> getOuterBox() )
        .end()
        -> polygonMode( POLYGON_FILL )
        . begin( QUAD )
        -> sendQube( aLayer -> getEye(), aLayer -> getOuterBox() )
        .end();

        /* Draw layer name */

        auto textPoint = aLayer -> getEye() + POINT_3D_BOTTOM * aLayer -> getOuterBox().y;

        aScene
        -> color( Rgba( RGBA_WHITE ))
        -> setTextTop()
        -> setTextRight()
        -> setTextPosition( textPoint )
        -> setTextSize( 0.1 )
        -> setTextBaseLine( BASE_LINE_TOP )
        -> setTextHorisontalAlign( ALIGN_CENTER )
        -> text( aLayer -> getNameOrId() )
        -> setTextSize( 0.05 )
        -> textCR()
        -> color( Rgba( RGBA_RED ))
        -> text( "Error:" + to_string( aLayer -> getError()) );
    }

    return this;
}


/*
    Draw nerves
*/
NetGraph* NetGraph::drawNerves
(
    Scene* aScene /* Scene object */
)
{
    /* Draw nerves */
    getNerves() -> loop
    (
        [ this, &aScene ]
        ( void* iNerve )
        {
            drawNerve( aScene, ( Nerve* ) iNerve );
            return false;
        }
    );
    return this;
}



/*
    Draw layer
*/
NetGraph* NetGraph::drawNerve
(
    Scene*  aScene,
    Nerve*  aNerve
)
{
    /* Draw nerve binds */
    switch( showBinds )
    {
        case BDM_HIDDEN:
        break;
        case BDM_TYPE:
        case BDM_SIGNAL:
        case BDM_WEIGHT:
            aScene -> setLineWidth( 1 );
            aScene -> begin( LINE );

            auto c = aNerve -> getWeightsCount();

            for( int i = 0; i < c; i++ )
            {
                /* Retrive parent and child by weight index */
                auto iParent = aNerve -> getParentByWeightIndex( i );
                auto iChild = aNerve -> getChildByWeightIndex( i );

                /* Draw bind */
                auto color = Rgba();

                switch( showBinds )
                {
                    case BDM_TYPE:
                        switch( aNerve -> getBindType() )
                        {
                            case BT_VALUE:
                                color = RGBA_YELLOW;
                            break;
                            case BT_SAMPLE:
                                color = RGBA_MAGENTA;
                            break;
                            case BT_COMMAND:
                                color = RGBA_CYAN;
                            break;
                            case BT_ERROR_TO_VALUE:
                                color = RGBA_ORANGE;
                            break;
                        }
                    break;
                    case BDM_WEIGHT:
                        switch( neuronDrawMode )
                        {
                            case NDM_VALUE:
                                color = getBindValueColor
                                (
                                    aNerve -> getWeight( i )
                                );
                            break;
                            case NDM_ERROR:
                                color = getBindErrorColor
                                (
                                    aNerve -> getWeight( i )
                                );
                            break;
                        }
                    break;
                    case BDM_SIGNAL:
                        switch( neuronDrawMode )
                        {
                            case NDM_VALUE:
                                color = getBindValueColor
                                (
                                    aNerve -> getWeight( i )
                                    *
                                    aNerve
                                    -> getParent()
                                    -> getNeuronValue( iParent )
                                );
                            break;
                            case NDM_ERROR:
                                color = getBindErrorColor
                                (
                                    aNerve -> getWeight( i )
                                    *
                                    aNerve
                                    -> getChild()
                                    -> getNeuronError( iChild )
                                );
                            break;
                        }
                    break;
                }
                aScene
                -> color( color )
                -> vertex
                (
                    aNerve -> getParent() -> getNeuronWorld( iParent )
                )
                -> vertex
                (
                    aNerve -> getChild() -> getNeuronWorld( iChild )
                );
            }
            aScene -> end();
        break;
    }

    /* Draw nerve arrow */
    Rgba cFrom = getArrowColorByType( aNerve -> getBindType()).setAlpha( 0.0 );
    Rgba cTo = getArrowColorByType( aNerve -> getBindType()).setAlpha( 0.05 );

    aScene -> arrow
    (
        (( Nerve* ) aNerve ) -> getParent() -> getEye(),
        (( Nerve* ) aNerve ) -> getChild() -> getEye(),
        aScene -> getBack(),
        0.05,
        0.05,
        5.0, 1.0,
        0.1, 0.2,
        cFrom,
        cTo
    );

    return this;
}




NetGraph* NetGraph::neuronPointsScreenCalc
(
    Scene* aScene,  /* Scene object */
    Layer* aLayer
)
{
    getLog()
    -> begin( "Calculate neurons screen position" )
    -> prm( "Layer", aLayer -> getNameOrId() );

    auto c = aLayer -> getNeuronsCount();

    for( int i = 0; i < c; i ++ )
    {
        auto p = aScene
        -> getScreenByWorld( aLayer -> getNeuronWorld( i ) );
        aLayer -> setNeuronScreen( i, p );
    };

    getLog() -> end();

    return this;
}




/*
    Draw learning mode indicator
*/
NetGraph* NetGraph::drawLearningMode
(
    Scene* aScene
)
{
    if( getLearningMode() )
    {
        aScene
        -> setPointSize( 8 )
        -> color( RGBA_YELLOW )
        -> setTextBaseLine( BASE_LINE_TOP )
        -> setTextHorisontalAlign( ALIGN_LEFT )
        -> setTextRight( POINT_3D_RIGHT )
        -> setTextTop( POINT_3D_TOP )
        -> setTextPosition( Point3d( 0, aScene -> getViewport().height, 0 ))
        -> setTextSize( 20 )
        -> text( "Learning mode" );
    }

    return this;
}



/*
    Draw neuron chart
*/
NetGraph* NetGraph::drawNeuronChart
(
    Scene* aScene,
    NeuronList* aNeuronList
)
{
    Chart2d::create()
    -> setXMin( -2.0 )
    -> setXMax( 2.0 )
    -> setYMin( -2.0 )
    -> setYMax( 2.0 )
    -> setCenterSize( Point2d( 110,110 ), Point2d( 100, 100 ) )
    -> setBackColor( interfaceColorDark )
    -> setLineColor( interfaceColor )
    -> drawBack( aScene )
    -> setLineWeight( 1.0 )
    -> draw( aScene, FUNC_SIGMOID_LINE_ZERO_PLUS, 1.0 )
    -> draw( aScene, FUNC_SIGMOID_LINE_MINUS_PLUS, 1.0 )
    -> draw( aScene, FUNC_V_LINE, 1.0 )

    -> setLineWeight( 2.0 )
    -> setLineColor( RGBA_ORANGE )
    -> drawX( aScene, aNeuronList -> calcAvgValue() )
    -> setLineColor( RGBA_RED )
    -> drawX( aScene, aNeuronList -> calcAvgError() )
    -> setLineColor( RGBA_GREEN )
    -> drawX( aScene, avgWeightBySelect() )

    -> setLineColor( interfaceColor )
    -> setLineWeight( 1.0 )
    -> drawX( aScene, 0.0 )
    -> drawX( aScene, 1.0 )
    -> drawX( aScene, -1.0 )
    -> drawY( aScene, 0.0 )
    -> drawY( aScene, 1.0 )
    -> drawY( aScene, -1.0)
    -> destroy();

    return this;
}



/*
    Return neurons by screen rectangle
*/
NetGraph* NetGraph::getNeuronsByScreenRect
(
    NeuronList* aNeuronList,    /* Returned list */
    Point3d&    aTopLeft,       /* top left */
    Point3d&    aBottomRight    /* botom right */
)
{
    getLayers() -> loop
    (
        [ &aNeuronList, &aTopLeft, &aBottomRight ]
        (
            void* iLayer
        ) -> bool
        {
            (( Layer* ) iLayer ) -> getNeuronsByScreenRect
            (
                aNeuronList,
                aTopLeft,
                aBottomRight
            );
            return false;
        }
    );

    return this;
}



/*
    draw selected neurons
*/
NetGraph* NetGraph::drawSelectedNeurons
(
    Scene* aScene
)
{
    aScene
    -> color( interfaceColor )
    -> setPointSize( 6 )
    -> begin( POINT );
    selected -> loop
    (
        [ &aScene ]( Neuron* iNeuron ) -> bool
        {
            auto s = iNeuron -> getScreen();
            if( s.z < 1 )
            {
                aScene -> vertex( s.setZ( 0 ) );
            }
            return false;
        }
    );
    aScene -> end();

    return this;
}



/*
    Select neurons by screen rectangle
*/
NetGraph* NetGraph::selectNeuronsByRect
(
    Point3d aTopLeft,       /* Top left corner */
    Point3d aBottomRight    /* Bottom right corner */
)
{
    selected -> resize( 0 );
    getNeuronsByScreenRect( selected, aTopLeft, aBottomRight );
    return this;
}



/*
    Set neuron draw mode
*/
NetGraph* NetGraph::setNeuronDrawMode
(
    NeuronDrawMode a
)
{
    neuronDrawMode = a;
    return this;
}



/*
    Get neuron draw mode
*/
NeuronDrawMode NetGraph::getNeuronDrawMode()
{
    return neuronDrawMode;
}



/*
    Return list of selected neurons
*/
NeuronList* NetGraph::getSelectedNeurons()
{
    return selected;
}



Rgba NetGraph::getErrorColor
(
    const double a
)
{
    return
    a <= 0
    ? Rgba( colorErrorM ).itpLin( colorErrorZ, a + 1 )
    : Rgba( colorErrorZ ).itpLin( colorErrorP, a );
}



Rgba NetGraph::getBindErrorColor
(
    const double a
)
{
    return
    a <= 0
    ? Rgba( colorBindErrorM ).itpLin( colorBindErrorZ, a + 1 )
    : Rgba( colorBindErrorZ ).itpLin( colorBindErrorP, a );
}




Rgba NetGraph::getBindValueColor
(
    const double a
)
{
    return
    a <= 0
    ? Rgba( colorBindValueM ).itpLin( colorBindValueZ, a + 1 )
    : Rgba( colorBindValueZ ).itpLin( colorBindValueP, a );
}



NetGraph* NetGraph::switchShowBinds()
{
    switch( showBinds )
    {
        case BDM_HIDDEN: showBinds = BDM_WEIGHT; break;
        case BDM_WEIGHT: showBinds = BDM_SIGNAL; break;
        case BDM_SIGNAL: showBinds = BDM_TYPE; break;
        case BDM_TYPE: showBinds = BDM_HIDDEN; break;
    }

    int c = getLayers() -> getCount();
    for( int i = 0; i < c; i++ )
    {
        Layer* layer = ( Layer* ) ( getLayers() -> getByIndex( i ));
        layer -> setShowBinds( showBinds );
    }
    return this;
}



Rgba NetGraph::getArrowColorByType
(
    BindType aType
)
{
    Rgba result;
    switch( aType )
    {
        case BT_VALUE:
            result = RGBA_YELLOW;
        break;
        case BT_SAMPLE:
            result = RGBA_MAGENTA;
        break;
        case BT_COMMAND:
            result = RGBA_CYAN;
        break;
        case BT_ERROR_TO_VALUE:
            result = RGBA_ORANGE;
        break;
    }
    return result;
}



/*
    Return neuron by screen position
*/
NetGraph* NetGraph::getNeuronsByScreenPos
(
    NeuronList* aNeuronList,
    const Point3d& aPosition
)
{
    getLayers() -> loop
    (
        [ this, &aNeuronList, &aPosition ]
        (
            void* iLayer
        ) -> bool
        {
            (( Layer* ) iLayer ) -> getNeuronsByScreenPos
            (
                aNeuronList,
                aPosition,
                cursorRadius
            );
            return false;
        }
    );
    return this;
}



/*
    Return first selected neuron
*/
Neuron* NetGraph::getSelectedFirst()
{
    return ( Neuron* ) selected -> getFirst();
}



NetGraph* NetGraph::setSelected
(
    Neuron* a
)
{
    selected -> resize( 0 ) -> push( a );
    return this;
}



/*
    Set selected neurons
*/
NetGraph* NetGraph::setSelected
(
    Scene* aScene /* Scene object */
)
{
    selected -> resize( 0 );
    getNeuronsByScreenPos
    (
        selected,
        aScene -> getMouseCurrentScreen()
    );
    return this;
}



/*
    Add selected neurons
*/
NetGraph* NetGraph::addSelectedByCursor
(
    Scene* aScene /* Scene object */
)
{
    auto current = NeuronList::create();
    getNeuronsByScreenPos
    (
        current,
        aScene -> getMouseCurrentScreen()
    );
    selected -> merge( current );
    current -> destroy();
    return this;
}



/*
    Remove selected neurons by scene
*/
NetGraph* NetGraph::removeSelectedByCursor
(
    Scene* aScene /* Scene object */
)
{
    auto current = NeuronList::create();
    getNeuronsByScreenPos
    (
        current,
        aScene -> getMouseCurrentScreen()
    );
    selected -> remove( current );
    current -> destroy();
    return this;
}




/*
    Return screen radius
*/
double NetGraph::getCursorRadius()
{
    return cursorRadius;
}



/*
    Set screen radius
*/
NetGraph* NetGraph::setCursorRadius
(
    double a /* Pixel radius */
)
{
    cursorRadius = a;
    return this;
}



/*
    Return average weights for selected neurons
*/
double NetGraph::avgWeightBySelect()
{
    int total = 0;
    double sum = 0;

    nerveWeightLoop
    (
        getSelectedNeurons(),
        [
            &total, &sum
        ]
        (
            int aWeightIndex,
            Neuron* aNeuronFrom,
            Neuron* aNeuronTo,
            Nerve* aNerve
        )
        {
            total ++;
            sum += aNerve -> getWeight( aWeightIndex );
            return false;
        }
    );

    return total == 0.0 ? 0.0 : (double)sum / total;
}
