#include "limb_ui.h"
#include "stdio.h"

/* System */
#include <functional>
#include <iostream>
#include <sstream>
#include <fstream>

/* Graph libraries */
#include "../../../../../lib/graph/chart.h"
#include "../../../../../lib/graph/log_points.h"
#include "../../../../../lib/core/math.h"
#include "../../../../../lib/core/str.h"

/* Shoggot libraries */
#include "../func.h"
#include "neuron_list_ui.h"
#include "neuron_ui.h"
#include "layer_ui.h"



/*
    Constructor
*/
LimbUi::LimbUi
(
    Net* aNet
)
/* Call parent constructor */
: Limb
(
    aNet -> getLogManager()
)
{
    net = aNet;
    selected = NeuronListUi::create();
}



/*
    Destructor
*/
LimbUi::~LimbUi()
{
    selected -> destroy();
}



/*
    Create
*/
LimbUi* LimbUi:: create
(
    Net* aNet
)
{
    return new LimbUi( aNet );
}



/*
    Draw layers
*/
LimbUi* LimbUi::drawLayers
(
    Scene* aScene,          /* Scene object */
    bool   aCalcScreenPos   /* Camera changed */
)
{
    getLayerList() -> loop
    (
        [ this, aScene, aCalcScreenPos ]
        (
            void* iLayer
        ) -> bool
        {
            drawLayer( aScene, ( LayerUi* ) iLayer, aCalcScreenPos );
            return false;
        }
    );
    return this;
}




/*
    Draw layer
*/
LimbUi* LimbUi::drawLayer
(
    Scene*      aScene,
    LayerUi*    aLayer,
    bool        aCalcScreenPos
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
    int neuronsCount = aLayer -> getCount();

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
        -> vertex( aLayer -> getObject() -> getEye() )
        -> end();

        /*  Draw layer box */
        aScene
        -> setLineWidth( 1 )
        -> polygonMode( POLYGON_LINE )
        .color( interfaceColorDark )
        -> begin( QUAD )
        -> sendQube( aLayer -> getObject() -> getEye(), aLayer -> getOuterBox() )
        .end()
        -> polygonMode( POLYGON_FILL )
        . begin( QUAD )
        -> sendQube( aLayer -> getObject() -> getEye(), aLayer -> getOuterBox() )
        .end();

        /* Draw layer name */

        auto textPoint = aLayer -> getObject() -> getEye()
        + POINT_3D_BOTTOM * aLayer -> getOuterBox().y;

        /* Draw labels */
        aScene
        -> color( Rgba( RGBA_WHITE ))
        -> setTextTop()
        -> setTextRight()
        -> setTextPosition( textPoint )
        -> setTextSize( 0.1 )
        -> setTextBaseLine( BASE_LINE_TOP )
        -> setTextHorisontalAlign( ALIGN_CENTER )

        /* Layer Name or Id */
        -> text( aLayer -> getNameOrId() )
        -> textCR( 1 )

        /* Layer function name for fornt and backward calculation */
        -> setTextSize( 0.05 )
        -> color( Rgba( RGBA_GREY ))
        -> text
        (
            neuronFuncToStr( aLayer -> getFrontFunc()) + "/" +
            neuronFuncToStr( aLayer -> getBackFunc())
        )

        /* Layer error and value */
        -> textCR( 1 )
        -> color( Rgba( RGBA_RED ))
        -> text( "Error:" + to_string( aLayer -> calcSumError()) )
        -> textCR( 1 )
        -> color( Rgba( RGBA_ORANGE ))
        -> text( "Value:" + to_string( aLayer -> calcSumValue()) )
        ;
    }

    return this;
}


/*
    Draw nerves
*/
LimbUi* LimbUi::drawNerves
(
    Scene* aScene /* Scene object */
)
{
    /* Draw nerves */
    getNerveList() -> loop
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
LimbUi* LimbUi::drawNerve
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
                    case BDM_HIDDEN:
                        /* The binds is invisible */
                    break;
                    case BDM_TYPE:
                        switch( aNerve -> getBindType() )
                        {
                            case BT_ALL:
                                color = RGBA_RED;
                            break;
                            case BT_ADD:
                                color = RGBA_CYAN;
                            break;
                            case BT_MUL:
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
                    ((LayerUi*) ( aNerve -> getParent() )) -> getNeuronWorld( iParent )
                )
                -> vertex
                (
                    ((LayerUi*) (aNerve -> getChild() )) -> getNeuronWorld( iChild )
                );
            }
            aScene -> end();
        break;
    }

    /* Draw nerve arrow */
    Rgba cFrom = getArrowColorByType( aNerve -> getBindType()).setAlpha( 0.1 );
    Rgba cTo = getArrowColorByType( aNerve -> getBindType()).setAlpha( 0.2 );

    aScene -> arrow
    (
        (( LayerUi* ) ( aNerve -> getParent() )) -> getObject() -> getEye(),
        (( LayerUi* ) ( aNerve -> getChild() )) -> getObject() -> getEye(),
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




LimbUi* LimbUi::neuronPointsScreenCalc
(
    Scene* aScene,  /* Scene object */
    LayerUi* aLayer
)
{
    getLog()
    -> begin( "Calculate neurons screen position" )
    -> prm( "Layer", aLayer -> getNameOrId() );

    auto c = aLayer -> getCount();


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
    Draw neuron chart
*/
LimbUi* LimbUi::drawNeuronChart
(
    Scene* aScene,
    NeuronListUi* aNeuronList
)
{
    Chart2d::create( aScene )
    -> setXMin( -6.0 )
    -> setXMax( 5.0 )
    -> setYMin( -1.0 )
    -> setYMax( 1.0 )
    -> setCenterSize( Point2d( 310,110 ), Point2d( 300, 100 ) )
    -> setBackColor( interfaceColorDark )
    -> drawBack()
    -> setLineWeight( 2.0 )
    -> setLineColor( RGBA_MAGENTA )
    -> draw( *(aNeuronList -> getByIndex( 0 ) -> getLayer() -> getFrontFunc()) )
    -> setLineColor( RGBA_CYAN )
    -> draw( *(aNeuronList -> getByIndex( 0 ) -> getLayer() -> getBackFunc()) )
    -> setLineWeight( 3.0 )
    -> setLineColor( RGBA_ORANGE )
    -> drawX( aNeuronList -> calcAvgValue(), true, -1 )
    -> setLineColor( RGBA_RED )
    -> drawX( aNeuronList -> calcAvgError(), true, -2 )
    -> setLineColor( RGBA_GREEN )
    -> drawX( avgWeightBySelect(), true, -3 )
    -> setLineColor( interfaceColor )
    -> setLineWeight( 1.0 )
    -> drawX( 0.0 )
    -> drawX( 1.0 )
    -> drawX( -1.0 )
    -> drawY( 0.0 )
    -> drawY( 1.0 )
    -> drawY( -1.0)
    -> destroy();

    return this;
}



/*
    Return neurons by screen rectangle
*/
LimbUi* LimbUi::getNeuronsByScreenRect
(
    NeuronListUi* aNeuronList,    /* Returned list */
    Point3d&    aTopLeft,       /* top left */
    Point3d&    aBottomRight    /* botom right */
)
{
    getLayerList() -> loop
    (
        [ &aNeuronList, &aTopLeft, &aBottomRight ]
        (
            void* iLayer
        ) -> bool
        {
            (( LayerUi* ) iLayer ) -> getNeuronsByScreenRect
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
LimbUi* LimbUi::drawSelectedNeurons
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
        [ &aScene ]( NeuronUi* iNeuron ) -> bool
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
LimbUi* LimbUi::selectNeuronsByRect
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
LimbUi* LimbUi::setNeuronDrawMode
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
NeuronDrawMode LimbUi::getNeuronDrawMode()
{
    return neuronDrawMode;
}



/*
    Return list of selected neurons
*/
NeuronListUi* LimbUi::getSelectedNeurons()
{
    return selected;
}



Rgba LimbUi::getErrorColor
(
    const double a
)
{
    return
    a <= 0
    ? Rgba( colorErrorM ).itpLin( colorErrorZ, a + 1 )
    : Rgba( colorErrorZ ).itpLin( colorErrorP, a );
}



Rgba LimbUi::getBindErrorColor
(
    const double a
)
{
    return
    a <= 0
    ? Rgba( colorBindErrorM ).itpLin( colorBindErrorZ, a + 1 )
    : Rgba( colorBindErrorZ ).itpLin( colorBindErrorP, a );
}




Rgba LimbUi::getBindValueColor
(
    const double a
)
{
    return
    a <= 0
    ? Rgba( colorBindValueM ).itpLin( colorBindValueZ, a + 1 )
    : Rgba( colorBindValueZ ).itpLin( colorBindValueP, a );
}



LimbUi* LimbUi::switchShowBinds()
{
    switch( showBinds )
    {
        case BDM_HIDDEN: showBinds = BDM_WEIGHT; break;
        case BDM_WEIGHT: showBinds = BDM_SIGNAL; break;
        case BDM_SIGNAL: showBinds = BDM_TYPE; break;
        case BDM_TYPE: showBinds = BDM_HIDDEN; break;
    }

    int c = getLayerList() -> getCount();
    for( int i = 0; i < c; i++ )
    {
        auto layer = ( LayerUi* ) ( getLayerList() -> getByIndex( i ));
        layer -> setShowBinds( showBinds );
    }
    return this;
}



Rgba LimbUi::getArrowColorByType
(
    BindType aType
)
{
    Rgba result;
    switch( aType )
    {
        case BT_ALL:
            result = RGBA_RED;
        break;
        case BT_ADD:
            result = RGBA_CYAN;
        break;
        case BT_MUL:
            result = RGBA_ORANGE;
        break;
    }
    return result;
}



/*
    Return neuron by screen position
*/
LimbUi* LimbUi::getNeuronsByScreenPos
(
    NeuronListUi* aNeuronList,
    const Point3d& aPosition
)
{
    getLayerList() -> loop
    (
        [ this, &aNeuronList, &aPosition ]
        (
            void* iLayer
        ) -> bool
        {
            (( LayerUi* ) iLayer ) -> getNeuronsByScreenPos
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
NeuronUi* LimbUi::getSelectedFirst()
{
    return ( NeuronUi* ) selected -> getFirst();
}



/*
    Put the first Selected neuron in to WeightsExchange for sent to server
*/
LimbUi* LimbUi::selectedToNet()
{
    net -> lock();

    /* Clear all neurons for current client id */
    net -> getWeightsExchange() -> purgeByClientId();

    auto first = getSelectedFirst();
    if( first != NULL )
    {
        net
        -> getWeightsExchange()
        -> add( first -> getLayer() -> getId(), first -> getIndex() );
    }
    net -> unlock();

    return this;
}



LimbUi* LimbUi::setSelected
(
    NeuronUi* a
)
{
    selected -> resize( 0 ) -> push( a );
    selectedToNet();
    return this;
}



/*
    Set selected neurons
*/
LimbUi* LimbUi::setSelected
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

    selectedToNet();

    return this;
}



/*
    Add selected neurons
*/
LimbUi* LimbUi::addSelectedByCursor
(
    Scene* aScene /* Scene object */
)
{
    auto current = NeuronListUi::create();
    getNeuronsByScreenPos
    (
        current,
        aScene -> getMouseCurrentScreen()
    );
    selected -> merge( current );
    current -> destroy();

    selectedToNet();

    return this;
}



/*
    Remove selected neurons by scene
*/
LimbUi* LimbUi::removeSelectedByCursor
(
    Scene* aScene /* Scene object */
)
{
    auto current = NeuronListUi::create();
    getNeuronsByScreenPos
    (
        current,
        aScene -> getMouseCurrentScreen()
    );
    selected -> remove( current );
    current -> destroy();

    selectedToNet();

    return this;
}




/*
    Return screen radius
*/
double LimbUi::getCursorRadius()
{
    return cursorRadius;
}



/*
    Set screen radius
*/
LimbUi* LimbUi::setCursorRadius
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
double LimbUi::avgWeightBySelect()
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
            NeuronUi* aNeuronFrom,
            NeuronUi* aNeuronTo,
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



LimbUi* LimbUi::nerveWeightLoop
(
    NeuronListUi* a,
    IndexWeightLambda aCallback
)
{
    auto nerves = getNerveList();
    a -> loop
    (
        [ this, &a, &aCallback, &nerves ]
        (
            NeuronUi* fromNeuron
        )
        {
            a -> loop
            (
                [ this, &fromNeuron, &aCallback, &nerves ]
                (
                    NeuronUi* toNeuron
                )
                {
                    auto fromLayer = fromNeuron -> getLayer();
                    auto toLayer = toNeuron -> getLayer();

                    auto foundedNerves = NerveList::create( getLogManager() );
                    nerves -> selectByLayers
                    (
                        fromLayer,
                        toLayer,
                        foundedNerves
                    );

                    foundedNerves -> loop
                    (
                        [
                            &fromNeuron,
                            &toNeuron,
                            &aCallback
                        ]
                        (
                            void* aItem
                        )
                        {
                            auto nerve = (Nerve*)aItem;
                            auto indexFrom = fromNeuron -> getIndex();
                            auto indexTo = toNeuron -> getIndex();
                            auto weightIndex = nerve -> getIndexByNeuronsIndex
                            (
                                indexFrom,  /* in parent layer */
                                indexTo     /* in child layer */
                            );
                            if( weightIndex >= 0 )
                            {
                                aCallback
                                (
                                    weightIndex,
                                    fromNeuron,
                                    toNeuron,
                                    nerve
                                );
                            }
                            return false;
                        }
                    );

                    foundedNerves -> destroy();
                    return false;
                }
            );
            return false;
        }
    );
    return this;
}



LimbUi* LimbUi::switchShowLayer()
{
    LayerList* layers = getLayerList();
    int c = layers -> getCount();
    for( int i = 0; i < c; i++ )
    {
        auto layer = ( LayerUi* ) layers -> getByIndex( i );
        layer -> switchShowLayer();
    }
    return this;
}



/*
    Create new layer for this limb and copy parameters from source layer.
    This method is an overridden method of the Limb class.
*/
Layer* LimbUi::copyLayerFrom
(
    Layer* aLayerFrom
)
{
    auto result =
    LayerUi::create( this, aLayerFrom -> getId() )
    -> setFrontFunc( aLayerFrom -> getFrontFunc() )
    -> setBackFunc( aLayerFrom -> getBackFunc() );

    return ( Layer* )result;
}



/*
    Return net object
*/
Net* LimbUi::getNet()
{
    return net;
}



/*
    Configuration postprocessing
*/
void LimbUi::onAfterReconfig
(
    ParamList*  aConfig
)
{
    getLayerList() -> loop
    (
        [ this, &aConfig ]
        (
            void* aLayer
        )
        {
            auto layer = (LayerUi*) aLayer;
            auto configLayer = aConfig -> getObject
            (
                Path { "layers", layer -> getId() }
            );

            if( configLayer != NULL )
            {
                layer -> setSize( configLayer );
                layer -> setDrawSize( configLayer );
                layer
                -> setPosition( configLayer )
                -> getObject()
                -> setTop( POINT_3D_Y )
                -> setTarget( POINT_3D_X );
            }
            else
            {
                getLog() -> error( "NetAndConfigNotConsistent" ) -> lineEnd();
            }

            return false;
        }
    );
}




LimbUi* LimbUi::dumpXY
(
    int aZ,             /* Z layer */
    ofstream &aFile,     /* File stream */
    LayerUi* aLayer,
    function <double ( int )> aCallback
)
{
    auto pos = Point3i( 0, 0, aZ );

    auto size = aLayer -> getSize();

    for( pos.y = 0; pos.y < size.y ; pos.y ++ )
    {
        for( pos.x = 0; pos.x < size.x; pos.x++ )
        {
            auto index = aLayer ->  indexByPos( pos );
            auto arg = aCallback( index );
            auto absArg = abs( arg );
            aFile
            << ( absArg < EPSILON_D ? INK_WHITE : ( arg > 0.0 ? INK_RED : INK_BLUE ))
            << strAlign( to_string( absArg ), ALIGN_RIGHT, 8 )
            << INK_DEFAULT
            << " ";
        }
        aFile << endl;
    }

    return this;
}



/*
    dump weights in to file
*/
LimbUi* LimbUi::dumpWeights
(
    LayerUi* aLayer,
    int aNeuronIndex,
    LayerUi* aLayerLink,
    string aType,
    char* aBuffer,
    size_t aSize
)
{
    auto pos = Point3i();
    auto size = aLayerLink -> getSize();
    for( pos.z = 0; pos.z < size.z; pos.z++ )
    {
        /* Create file name */
        stringstream s;
        s
        << aLayer -> getId()
        << "_"
        << aType
        << "_"
        << aLayerLink -> getId()
        << "_z:"
        << pos.z
        << ".txt";

        auto file = net -> getWeightsPath( s.str() );

        if( checkPath( getPath( file )))
        {
            ofstream f;
            f.open( file );
            if( f.is_open() )
            {
                f
                << "layer: " << aLayer -> getId() << endl
                << "index: " << aNeuronIndex << endl
                << "value: " << aLayer -> getNeuronValue( aNeuronIndex ) << endl
                << "error: " << aLayer -> getNeuronError( aNeuronIndex ) << endl
                ;

                f << endl << "Weights" << endl;
                dumpXY
                (
                    pos.z, f, aLayerLink,
                    [ &aBuffer ]( int aIndex )
                    {
                        return *((double*) &aBuffer[ aIndex * NEURON_WEIGHT_SIZE ]);
                    }
                );

                /* Parent or child values */

                f << endl << ( aType == "parent" ? "Parent" : "Child" ) << " values" << endl;
                dumpXY
                (
                    pos.z, f, aLayerLink,
                    [ &aLayerLink ]( int aIndex )
                    {
                        return aLayerLink -> getNeuronValue( aIndex );
                    }
                );

                /* Parent or child values */
                f << endl << ( aType == "parent" ? "Parent" : "Child" ) << " errors" << endl;
                dumpXY
                (
                    pos.z, f, aLayerLink,
                    [ &aLayerLink ]( int aIndex )
                    {
                        return aLayerLink -> getNeuronError( aIndex );
                    }
                );

                /* Incoming and outcoming values */
                f << endl << ( aType == "parent" ? "Incoming" : "Outcoming" ) << " values" << endl;
                dumpXY
                (
                    pos.z, f, aLayerLink,
                    [ &aType, &aBuffer, &aLayerLink, &aLayer, &aNeuronIndex ]( int aIndex )
                    {
                        return
                        (
                            aType == "parent"
                            ? aLayerLink -> getNeuronValue( aIndex )
                            : aLayer -> getNeuronValue( aNeuronIndex )
                        ) *
                        *((double*) &aBuffer[ aIndex * NEURON_WEIGHT_SIZE ]);
                    }
                );

                f.close();
            }
        }
    }

    return this;
}



LimbUi* LimbUi::dumpWeightsExchange()
{
    net -> lock();

    net -> getWeightsExchange() -> loop
    (
        [ this ]
        ( Param* item )
        {
            if( item -> isObject() )
            {
                auto layerId = item -> getObject() -> getString( "layerId" );
                auto neuronIndex = item -> getObject() -> getInt( "neuronIndex" );
                auto children = item -> getObject() -> getObject( "children" );
                auto parents = item -> getObject() -> getObject( "parents" );
                auto layer = (LayerUi*) getLayerList() -> getById( layerId );

                if( layer != NULL)
                {
                    /* Dump children */
                    if( children != NULL )
                    {
                        children -> loop
                        (
                            [ this, &layer, &neuronIndex ]
                            ( Param* child )
                            {
                                auto childLayer = ( LayerUi* ) getLayerList()
                                -> getById( child -> getName() );

                                if( childLayer != NULL )
                                {
                                    dumpWeights
                                    (
                                        layer,
                                        neuronIndex,
                                        childLayer,
                                        "child",
                                        child -> getValue(),
                                        child -> getSize()
                                    );
                                }
                                return false;
                            }
                        );
                    }

                    /* Dump parents */
                    if( parents != NULL )
                    {
                        parents -> loop
                        (
                            [ this, &layer, &neuronIndex]
                            ( Param* parent )
                            {
                                auto parentLayer = ( LayerUi* ) getLayerList()
                                -> getById( parent -> getName() );

                                if( parentLayer != NULL )
                                {
                                    dumpWeights
                                    (
                                        layer,
                                        neuronIndex,
                                        parentLayer,
                                        "parent",
                                        parent -> getValue(),
                                        parent -> getSize()
                                    );
                                }
                                return false;
                            }
                        );
                    }
                }

            }
            return false;
        }
    );

    net -> unlock();
    return this;
}
