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
    const Point3& aPoint
)
{
}



/*
    Mouse left drag
*/
void Form::onLeftDrag
(
    Scene& aScene,      /* Scene object */
    const Point3& aPoint
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
    const Point3& aDelta
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

