#include <iostream>



/* Local libraries */
#include "../graph/scene_payload.h"
#include "../graph/scene.h"
#include "../graph/rgba.h"
#include "../graph/log_points.h"
#include "../graph/draw_mode.h"
#include "../graph/chart.h"
#include "../graph/rect2d.h"

#include "../shoggoth/neuron.h"
#include "../shoggoth/func.h"

#include "../lib/rnd.h"
#include "../math.h"

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
    net = NetGraph::create( &getLog() );

    layer1 = net -> createLayer( "Screen" )     -> setSize( Point3i( 10, 10, 1 )) -> setLayerType( LT_RECEPTOR );
    layer2 = net -> createLayer( "Left1" )      -> setSize( Point3i( 4, 4, 4 ));
    layer3 = net -> createLayer( "Right1" )     -> setSize( Point3i( 4, 4, 4 ));
    layer4 = net -> createLayer( "Left2" )      -> setSize( Point3i( 4, 4, 4 ));
    layer5 = net -> createLayer( "Right2" )     -> setSize( Point3i( 4, 4, 4 ));
    layer6 = net -> createLayer( "Left3" )      -> setSize( Point3i( 4, 4, 4 ));
    layer7 = net -> createLayer( "Right3" )     -> setSize( Point3i( 4, 4, 4 ));
    layer8 = net -> createLayer( "ResultLeft" ) -> setSize( Point3i( 2, 1, 1 )) -> setLayerType( LT_RESULT );
    layer9 = net -> createLayer( "ResultRight" )-> setSize( Point3i( 2, 1, 1 )) -> setLayerType( LT_RESULT );
    layer10 = net -> createLayer( "ResultAll" )-> setSize( Point3i( 9, 1, 1 )) -> setLayerType( LT_RESULT );

    layer1 -> setTarget( POINT_3D_Z * 0 );
    layer2 -> setTarget( POINT_3D_Z * 2 - POINT_3D_X );
    layer3 -> setTarget( POINT_3D_Z * 2 + POINT_3D_X );
    layer4 -> setTarget( POINT_3D_Z * 3 - POINT_3D_X );
    layer5 -> setTarget( POINT_3D_Z * 3 + POINT_3D_X );
    layer6 -> setTarget( POINT_3D_Z * 4 - POINT_3D_X );
    layer7 -> setTarget( POINT_3D_Z * 4 + POINT_3D_X );
    layer8 -> setTarget( POINT_3D_Z * 5 - POINT_3D_X );
    layer9 -> setTarget( POINT_3D_Z * 5 + POINT_3D_X );
    layer10 -> setTarget( POINT_3D_Z * 5 + POINT_3D_Y + 2 );

    layer1 -> connectTo( layer2 );
    layer1 -> connectTo( layer3 );

    layer2 -> connectTo( layer4 );
    layer3 -> connectTo( layer5 );

    layer4 -> connectTo( layer6 );
    layer5 -> connectTo( layer7 );

    layer6 -> connectTo( layer8 );
    layer7 -> connectTo( layer9 );

    layer6 -> connectTo( layer10 );

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


//    aScene
//    .setPointSize( 4 )
//    .begin( POINT )
//    .color( RGBA_X );
//    for( double err = -3; err < 3; err += 0.1 )
//    {
//        for( double vw = 0; vw <= 1; vw += 0.1 )
//        {
//            auto f0 = FUNC_SIGMOID_LINE_MINUS_PLUS( err, 1.0 ) * 0.1;
//            auto f1 = FUNC_SIGMOID_LINE_MINUS_PLUS( err, 1.0 );
//            auto r = f0 + ( f1 - f0 ) * vw;
//            aScene.vertex( Point3d( err, vw, r ));
//       }
//    }
//    aScene.end();


    aScene
    .clearColor()
    .drawAxisIdentity()
    ;


    /* Draw Layers */
    net -> drawLayers( &aScene, camera.getChanged() );

    /* Reset camera changed */
    camera.setChanged( false );

    /*
        Switch to flat screen
    */
    applyScreenToScene( aScene );

    /* Draw Learning mode */
    net -> drawLearningMode( &aScene );

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

        net -> drawNeuronChart( &aScene, neuron );
    }


    net -> drawSelectedNeurons( &aScene );


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
    }
}



Form* Form::help()
{
    getLog()
    .info( "----" )
    .info( "Help" )
    .info( "----" )
    .info( "h - help" )
    .info( "v - draw value of neurons" )
    .info( "e - draw error of neurons" )
    .info( "b - switch show/hide binds" )
    .info( "c - switch show/hide layer cover" )
    .info( "l - switch on/off learning mode" )
    .info( "r - set random values for selected neurons" )
    .lineEnd()
    ;
    return this;
}



Form* Form::fillScreen
(
    int a
)
{
    Rnd::storeSeed( a );
    layer1 -> neurons -> loop
    (
        []( Neuron* neuron )
        {
            neuron -> setValue( Rnd::get( 0.0, 1.0 ) );
            return false;
        }
    );
    Rnd::restoreSeed();

    layer8 -> neurons -> getByIndex( 0 ) -> setLearningValue( 1 - a % 2 );
    layer8 -> neurons -> getByIndex( 1 ) -> setLearningValue( a % 2 );

    return this;
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
        case KEY_1:
        case KEY_2:
        case KEY_3:
        case KEY_4:
        case KEY_5:
        case KEY_6:
        case KEY_7:
        case KEY_8:
        case KEY_9:
            fillScreen( aKey );
        break;

        case KEY_H:
            help();
        break;
        case KEY_R:
            net -> getSelectedNeurons() -> loop
            (
                []( Neuron* neuron )
                {
                    neuron -> setValue( Rnd::get( 0.0, 1.0 ) );
                    return false;
                }
            );
        break;
        case KEY_V:
            net -> setNeuronDrawMode( NDM_VALUE );
        break;
        case KEY_E:
            net -> setNeuronDrawMode( NDM_ERROR );
        break;
        case KEY_B:
            net -> switchShowBinds();
        break;
        case KEY_C:
            net -> switchShowLayer();
        break;
        case KEY_L:
            net -> switchLearningMode();
        break;
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
    net -> selectNeuronsByRect( selectTopLeft, selectBottomRight );
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
