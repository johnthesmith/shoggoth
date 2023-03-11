#include <iostream>

/* File read */
#include <streambuf>
#include <sstream>
#include <fstream>



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
#include "../lib/moment.h"
#include "../json/json.h"

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

    layerRetina = net -> createLayer( "Retina" ) -> setId( "Retina" ) -> setSize( Point3i( 20, 20, 1 ));
    layerSample = net -> createLayer( "Sample" ) -> setSize( Point3i( 10, 1, 1 ));
    layerCortex1 = net -> createLayer( "Cortex1" ) -> setSize( Point3i( 10, 10, 1 ));
    layerCortex2 = net -> createLayer( "Cortex2" ) -> setSize( Point3i( 10, 10, 1 ));
    layerCortex3 = net -> createLayer( "Cortex3" ) -> setSize( Point3i( 10, 10, 1 ));
    layerResult = net -> createLayer( "Result" ) -> setSize( Point3i( 10, 1, 1 ));

    layerRetina -> setEye( POINT_3D_Z * 0 );
    layerCortex1 -> setEye( POINT_3D_Z * 1 );
    layerCortex2 -> setEye( POINT_3D_Z * 2 );
    layerCortex3 -> setEye( POINT_3D_Z * 3 );
    layerResult -> setEye( POINT_3D_Z * 4 );
    layerSample -> setEye( POINT_3D_Z * 5 );

    layerRetina -> connectTo( layerCortex1, ALL_TO_ALL, BT_VALUE, -0.5, 0.5);
    layerCortex1 -> connectTo( layerCortex2, ALL_TO_ALL, BT_VALUE, -0.5, 0.5);
    layerCortex2 -> connectTo( layerCortex3, ALL_TO_ALL, BT_VALUE, -0.5, 0.5);
    layerCortex3 -> connectTo( layerResult, ALL_TO_ALL, BT_VALUE, -0.5, 0.5);
    layerSample -> connectTo( layerResult, ONE_TO_ONE, BT_SAMPLE,  1.0, 1.0 );
    layerSample -> connectTo( layerResult, ALL_TO_ALL, BT_COMMAND,  1.0, 1.0 );
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
    /* Read config */
    auto nowMoment = now();
    if( lastConfigRead + SECOND < nowMoment )
    {
        auto t = ifstream( "net.json" );
        if( t.is_open() )
        {
            /* Read file */
            stringstream b;
            b << t.rdbuf();

            /* Parsing file */
            auto json = Json::create();
            json -> fromString( b.str() );

            if( json -> isOk())
            {
                /* Config apply */
                net -> applyConfig( json);
            }
            else
            {
                /* Config error */
                getLog()
                .warning( "Config error" )
                .prm( "message", json -> getMessage() );
            }
            json -> destroy();
        }

        /* Set last moment */
        lastConfigRead = nowMoment;
    }

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
    layerRetina -> neurons -> loop
    (
        []( Neuron* neuron )
        {
            neuron -> setValue( Rnd::get( 0.0, 1.0 ) );
            return false;
        }
    );
    Rnd::restoreSeed();

    for( int i = 0; i < layerSample -> neurons -> getCount(); i ++ )
    {
        layerSample -> neurons -> getByIndex( i ) -> setValue( i+49 == a ? 1.0 : 0.0 );
    }
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
        /* Neuron value Control*/
        neuron -> setValue
        (
            neuron -> getValue() + aDelta.y *
            ( aScene.isKey( KEY_LEFT_SHIFT ) ? 0.01 : 0.1 )
        );
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
