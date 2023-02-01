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
    layer1 = Layer::create( getLog() ) -> setName( "Screen" ) -> setSize( Point3i( 64, 48, 1 ));
    layer2 = Layer::create( getLog() ) -> setName( "One" ) -> setSize( Point3i( 50, 50, 1 ));
    layer3 = Layer::create( getLog() ) -> setName( "Two" ) -> setSize( Point3i( 50, 50, 1 ));
    layer4 = Layer::create( getLog() ) -> setName( "Three" ) -> setSize( Point3i( 50, 50, 1 ));
    layer5 = Layer::create( getLog() ) -> setName( "For" ) -> setSize( Point3i( 50, 50, 1 ));

    layer1 -> setTarget( POINT_3D_X * 2 );
    layer2 -> setTarget( POINT_3D_X * 4 );

    layer1 -> connectTo( layer2 );
    layer2 -> connectTo( layer3 );
    layer3 -> connectTo( layer4 );
    layer4 -> connectTo( layer5 );
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
    .drawGreedIdentity()
    ;


    aScene
    .begin( LINE )
    .color( RGBA_RED ).vertex( POINT_3D_X ).vertex( aScene.getMouseCurrentWorld() )
    .color( RGBA_RED ).vertex( POINT_3D_Y ).vertex( aScene.getMouseCurrentWorld() )
    .color( RGBA_RED ).vertex( POINT_3D_Z ).vertex( aScene.getMouseCurrentWorld() )
    .end();


    /* Draw Layer */
    layer1 -> draw( aScene );
    layer2 -> draw( aScene );


    /* Switch to flat screen */
    applyScreenToScene( aScene );

    aScene
    .begin( LINE )
    .color( RGBA_GREEN ).vertex( POINT_3D_0 ).vertex( aScene.getMouseCurrentScreen() )
    .color( RGBA_GREEN ).vertex( POINT_3D_0 ).vertex( aScene.getMouseCurrentScreen() )
    .end();
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
    Mouse left drag
*/
void Form::onLeftDrag
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
