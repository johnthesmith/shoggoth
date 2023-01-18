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
    Main draw method
*/
void Form::onDraw
(
    Scene& aScene   /* Scene object */
)
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );

//    camera.shift( aScene.mouseDelta );
    applyCameraToScene( camera, aScene );

    aScene
    .clearColor()
    .drawAxisIdentity()
    .drawGreedIdentity();

//    aScene.getLog().trace();
//    LogPoints::write( aScene.getLog(), aScene.mousePos, "point" );
//    aScene.getLog().trace();
//    LogPoints::write( aScene.getLog(), aScene.mouseDelta, "delta" );
}



/*
    Keyboard up event
*/
void Form::onKeyUp
(
    Scene&    aScene, /* Scene object */
    const int       aKey,   /* Key */
    const int       aCode,  /* Scan code*/
    const int       aMode   /* Mode */
)
{
    switch( aCode )
    {
        case 9:
            aScene.setTerminate( true );
        break;
        case 54:
            if( aMode == 2 )
            {
                aScene.setTerminate( true );
            }
        break;
    }

    getLog()
    .trace( "Key up" )
    .prm( "key", aKey )
    .prm( "code", aCode )
    .prm( "mode", aMode )
    .lineEnd();
}

