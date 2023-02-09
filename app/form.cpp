#include <iostream>

/* Local libraries */
#include "../lib/scene_payload.h"
#include "../lib/scene.h"
#include "../lib/rgba.h"
#include "../lib/log_points.h"
#include "../lib/draw_mode.h"

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
    layer1 = Layer::create( getLog() ) -> setName( "Screen" )   -> setSize( Point3i( 20, 20, 1 ));
    layer2 = Layer::create( getLog() ) -> setName( "Left1" )    -> setSize( Point3i( 5, 5, 1 ));
    layer3 = Layer::create( getLog() ) -> setName( "Right1" )   -> setSize( Point3i( 5, 5, 1 ));
    layer4 = Layer::create( getLog() ) -> setName( "Left2" )    -> setSize( Point3i( 5, 5, 1 ));
    layer5 = Layer::create( getLog() ) -> setName( "Right2" )   -> setSize( Point3i( 5, 5, 1 ));
    layer6 = Layer::create( getLog() ) -> setName( "Left3" )    -> setSize( Point3i( 5, 5, 1 ));
    layer7 = Layer::create( getLog() ) -> setName( "Right3" )   -> setSize( Point3i( 5, 5, 1 ));
    layer8 = Layer::create( getLog() ) -> setName( "ResultLeft" )  -> setSize( Point3i( 2, 1, 1 ));
    layer9 = Layer::create( getLog() ) -> setName( "ResultRight" )  -> setSize( Point3i( 2, 1, 1 ));


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
    layer1 -> destroy();
    layer2 -> destroy();
    layer3 -> destroy();
    layer4 -> destroy();
    layer5 -> destroy();
    layer6 -> destroy();
    layer7 -> destroy();
    layer8 -> destroy();
    layer9 -> destroy();
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
    layer1 -> calc( aScene );
    layer2 -> calc( aScene );
    layer3 -> calc( aScene );
    layer4 -> calc( aScene );
    layer5 -> calc( aScene );
    layer6 -> calc( aScene );
    layer7 -> calc( aScene );
    layer8 -> calc( aScene );
    layer9 -> calc( aScene );
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
//    .drawGreedIdentity()
    ;


//    aScene
//    .begin( LINE )
//    .color( RGBA_RED ).vertex( POINT_3D_X ).vertex( aScene.getMouseCurrentWorld() )
//    .color( RGBA_RED ).vertex( POINT_3D_Y ).vertex( aScene.getMouseCurrentWorld() )
//    .color( RGBA_RED ).vertex( POINT_3D_Z ).vertex( aScene.getMouseCurrentWorld() )
//    .end();


    /* Draw Layer */
    layer1 -> draw( aScene );
    layer2 -> draw( aScene );
    layer3 -> draw( aScene );
    layer4 -> draw( aScene );
    layer5 -> draw( aScene );
    layer6 -> draw( aScene );
    layer7 -> draw( aScene );
    layer8 -> draw( aScene );
    layer9 -> draw( aScene );


    /* Switch to flat screen */
    applyScreenToScene( aScene );

    if( selectTopLeft != selectBottomRight )
    {
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
    camera.shift
    (
        (
            (aScene.getMouseLastWorld() - aScene.getMouseCurrentWorld()) * k
        )
    );
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
