#include <iostream>



/* Local libraries */
#include "../lib/graph/scene_payload.h"
#include "../lib/graph/scene.h"
#include "../lib/graph/rgba.h"
#include "../lib/graph/log_points.h"
#include "../lib/graph/draw_mode.h"
#include "../lib/graph/chart.h"
#include "../lib/graph/rect2d.h"

#include "../lib/shoggoth/neuron.h"
#include "../lib/shoggoth/func.h"

#include "../lib/rnd.h"

/* User libraries */
#include "form.h"



using namespace std;



/*
    Constructor
*/
Form::Form
(
    Log& aLog   /* Log */
)
: ScenePayload( aLog ) /* Call parent constructor */
{
    net = Net::create( &getLog() );

    layer1 = net -> createLayer( "Screen" )     -> setSize( Point3i( 10, 10, 1 )) -> setLayerType( LT_RECEPTOR );
    layer2 = net -> createLayer( "Left1" )      -> setSize( Point3i( 5, 5, 1 ));
    layer3 = net -> createLayer( "Right1" )     -> setSize( Point3i( 5, 5, 1 ));
    layer4 = net -> createLayer( "Left2" )      -> setSize( Point3i( 5, 5, 1 ));
    layer5 = net -> createLayer( "Right2" )     -> setSize( Point3i( 5, 5, 1 ));
    layer6 = net -> createLayer( "Left3" )      -> setSize( Point3i( 5, 5, 1 ));
    layer7 = net -> createLayer( "Right3" )     -> setSize( Point3i( 5, 5, 1 ));
    layer8 = net -> createLayer( "ResultLeft" ) -> setSize( Point3i( 2, 1, 1 )) -> setLayerType( LT_RESULT );
    layer9 = net -> createLayer( "ResultRight" )-> setSize( Point3i( 2, 1, 1 )) -> setLayerType( LT_RESULT );

    layer1 -> setTarget( POINT_3D_Z * 0 );
    layer2 -> setTarget( POINT_3D_Z * 2 - POINT_3D_X );
    layer3 -> setTarget( POINT_3D_Z * 2 + POINT_3D_X );
    layer4 -> setTarget( POINT_3D_Z * 3 - POINT_3D_X );
    layer5 -> setTarget( POINT_3D_Z * 3 + POINT_3D_X );
    layer6 -> setTarget( POINT_3D_Z * 4 - POINT_3D_X );
    layer7 -> setTarget( POINT_3D_Z * 4 + POINT_3D_X );
    layer8 -> setTarget( POINT_3D_Z * 5 - POINT_3D_X );
    layer9 -> setTarget( POINT_3D_Z * 5 + POINT_3D_X );

    layer1 -> connectTo( layer2 );
    layer1 -> connectTo( layer3 );

    layer2 -> connectTo( layer4 );
    layer3 -> connectTo( layer5 );

    layer4 -> connectTo( layer6 );
    layer5 -> connectTo( layer7 );

    layer6 -> connectTo( layer8 );
    layer7 -> connectTo( layer9 );
}



/*
    Destructor
*/
Form::~Form()
{
    net -> destroy();
}



/*
    Creator
*/
Form* Form::create
(
    Log& aLog
)
{
    return new Form( aLog );
}



/*
    Destructor
*/
void Form::destroy()
{
    delete this;
}



/******************************************************************************
    Methods
*/



/*
    Activate payload
*/
void Form::onActivate
(
    Scene& aScene   /* Scene object */
)
{
    aScene.setFar( 1000.0 );
}



/*
    Main calc method
*/
void Form::onCalc
(
    Scene& aScene   /* Scene object */
)
{
    net -> calc( &aScene );
}



/*
    Main draw method
*/
void Form::onDraw
(
    Scene& aScene   /* Scene object */
)
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );


    applyCameraToScene( camera, aScene );

    aScene
    .clearColor()
    .drawAxisIdentity()
    ;


//    aScene
//    .begin( LINE )
//    .color( RGBA_RED ).vertex( POINT_3D_X ).vertex( aScene.getMouseCurrentWorld() )
//    .color( RGBA_RED ).vertex( POINT_3D_Y ).vertex( aScene.getMouseCurrentWorld() )
//    .color( RGBA_RED ).vertex( POINT_3D_Z ).vertex( aScene.getMouseCurrentWorld() )
//    .end();


    /* Draw Layers */
    net -> draw( &aScene, camera.getChanged() );
    camera.setChanged( false );  /* Reset camera changed */


    /*
        Switch to flat screen
    */
    applyScreenToScene( aScene );

    aScene
    .setPointSize( 4 )
    .begin( POINT )
    .color( RGBA_X ).vertex( aScene.getScreenByWorld( POINT_3D_X ))
    .color( RGBA_Y ).vertex( aScene.getScreenByWorld( POINT_3D_Y ))
    .color( RGBA_Z ).vertex( aScene.getScreenByWorld( POINT_3D_Z ))
    .end();


    /*
        Draw neuron chart
    */
    Neuron* neuron = net -> getSelected();
    if( neuron != NULL )
    {

        aScene
        .color( interfaceColor )
        .setLineWidth( 3 )
        .begin( LOOP )
        .sendRect
        (
            Rect2d().setCenterSize
            (
                neuron -> getScreenPoint(),
                Point2d( 10.0, 10.0 )
            )
        )
        .end();

        /* Draw chart */
        Chart2d::create()
        -> setXMin( -1.0 )
        -> setXMax( 2.0 )
        -> setYMin( -1.0 )
        -> setYMax( 2.0 )
        -> setCenterSize( Point2d( 110,110 ), Point2d( 100, 100 ) )
        -> setBackColor( interfaceColorDark )
        -> setLineColor( interfaceColor )
        -> drawBack( &aScene )
        -> draw( &aScene, FUNC_SIGMOID, neuron -> getLayer() -> getSensivity() )
        -> draw( &aScene, FUNC_SIGMOID_DERIVATIVE, neuron -> getLayer() -> getSensivity() )

        -> drawX( &aScene, neuron -> getValue(), RGBA_ORANGE )
        -> drawX( &aScene, neuron -> getError(), RGBA_RED )
        -> drawX( &aScene, neuron -> getLearningValue(), RGBA_MAGENTA )

        -> drawX( &aScene, 0.0, interfaceColor )
        -> drawX( &aScene, 1.0, interfaceColor )
        -> drawY( &aScene, 0.0, interfaceColor )
        -> drawY( &aScene, 1.0, interfaceColor )
        -> destroy();
    }


    if( selectTopLeft != selectBottomRight )
    {
        /* Draw selection rect */
        aScene
        .color( interfaceColorDark )
        .setLineWidth( 2 )
        .begin( QUAD )
        .sendRect( selectTopLeft, selectBottomRight )
        .end()
        .color( interfaceColor )
        .setLineWidth( 1 )
        .begin( LOOP )
        .sendRect( selectTopLeft, selectBottomRight )
        .end()
        ;

        /* Draw selected neurons */
        NeuronList* list = NeuronList::create();
        net -> getNeuronsByScreenRect( list, selectTopLeft, selectBottomRight );

        aScene
        .color( interfaceColor )
        .setPointSize( 4 )
        .begin( POINT );
        list -> loop
        (
            [ &aScene ]( Neuron* neuron ) -> bool
            {
                aScene.vertex( neuron -> getScreenPoint() );
                return false;
            }
        );
        aScene.end();

        list -> destroy();
    }
}



/*
    Keyboard up event
*/
void Form::onKeyUp
(
    Scene&      aScene, /* Scene object */
    const int   aKey,   /* Key */
    const int   aCode,  /* Scan code*/
    const int   aMode   /* Mode */
)
{
    switch( aKey )
    {
        case KEY_ESCAPE:
            aScene.setTerminate( true );
        break;
        case KEY_LEFT_CONTROL:
            camera.setEyeLock( false );
        break;
        case KEY_LEFT_SHIFT:
            camera.setTargetLock( false );
        break;
        case KEY_F1:
            net -> setNeuronDrawMode( NDM_VALUE );
        break;
        case KEY_F2:
            net -> setNeuronDrawMode( NDM_ERROR );
        break;
    }
}



/*
    Keyboard up event
*/
void Form::onKeyDown
(
    Scene&      aScene, /* Scene object */
    const int   aKey,   /* Key */
    const int   aCode,  /* Scan code*/
    const int   aMode   /* Mode */
)
{
    switch( aKey )
    {
        case KEY_LEFT_CONTROL:
            camera.setEyeLock( true );
        break;
        case KEY_LEFT_SHIFT:
            camera.setTargetLock( true );
        break;
    }
}



void Form::onMouseMove
(
    Scene& aScene,      /* Scene object */
    const Point3d& aPoint
)
{
}



/*
    Mouse left drag begin
*/
void Form::onLeftDragBegin
(
    Scene& aScene,      /* Scene object */
    const Point3d& aPoint
)
{
    selectTopLeft = aScene.getMouseCurrentScreen();
    selectBottomRight = selectTopLeft;
}



/*
    Mouse left drag begin
*/
void Form::onLeftDragEnd
(
    Scene& aScene,      /* Scene object */
    const Point3d& aPoint
)
{
    selectTopLeft = POINT_3D_0;
    selectBottomRight = POINT_3D_0;
}



/*
    Mouse left drag
*/
void Form::onLeftDrag
(
    Scene& aScene,      /* Scene object */
    const Point3d& aPoint
)
{
    selectBottomRight = aScene.getMouseCurrentScreen();
}



/*
    Mouse rigth drag
*/
void Form::onRightDrag
(
    Scene& aScene,      /* Scene object */
    const Point3d& aPoint
)
{
    double k = camera.getGaze().magn() / aScene.getNear();
    Point3d p = (aScene.getMouseLastWorld() - aScene.getMouseCurrentWorld()) * k;
    camera.shift( p );
}



/*
    Mouse wheel event
*/
void Form::onMouseWheel
(
    Scene& aScene,      /* Scene object */
    const Point3d& aDelta
)
{
    Neuron* neuron = net -> getSelected();
    if(  neuron != NULL )
    {
        switch( neuron -> getLayer() -> getLayerType())
        {

            case LT_RECEPTOR:
                /* Neuron value Control*/
                neuron -> setValue
                (
                    neuron -> getValue() + aDelta.y *
                    ( aScene.isKey( KEY_LEFT_SHIFT ) ? 0.01 : 0.1 )
                );
            break;
            case LT_RESULT:
                /* Neyron learning value control */
                neuron -> setLearningValue
                (
                    neuron -> getLearningValue() + aDelta.y *
                    ( aScene.isKey( KEY_LEFT_SHIFT ) ? 0.01 : 0.1 )
                );
            break;
        }
    }
    else
    {
        /* Camera control*/
        bool rotation = false;

        if( aScene.isKey( KEY_LEFT_CONTROL ))
        {
            camera.rotateEye( camera.getRight(), aDelta.y * 0.1 );
            rotation = true;
        }

        if( aScene.isKey( KEY_LEFT_SHIFT ))
        {
            camera.rotateEye( camera.getTop(), aDelta.y * 0.1 );
            rotation = true;
        }

        if( aScene.isKey( KEY_LEFT_ALT ))
        {
            camera.rotateTop( camera.getFront(), aDelta.y * 0.1 );
            rotation = true;
        }

        if( !rotation )
        {
            camera.zoom( aDelta.y > 0 ? 0.9 : 1.1 );
        }
    }
}



/*
    On mouse left click event
*/
void Form::onLeftClick
(
    Scene& aScene,          /* Scene object */
    const Point3d& aMouse,  /* Mouse position */
    const int aMode         /* Key mode */
)
{
    if( net -> setSelected( aScene ) -> getSelected() != NULL )
    {
//        exit(0);
    }
}



/*
    On mouse left click event
*/
void Form::onLeftDblClick
(
    Scene& aScene,          /* Scene object */
    const Point3d& aMouse,  /* Mouse position */
    const int aMode         /* Key mode */
)
{
    if( net -> setSelected( aScene ) -> getSelected() != NULL )
    {
        camera.moveTarget( net -> getSelected() -> getWorldPoint() );
    }
}
