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

    aScene.mouseDelta;

    aScene
    .cameraLocation( Point3( 1,1,1 ), Point3( 0,0,0 ), VECTOR_3D_Y )
//    .cameraApply()

    .clearColor()
    .drawAxisIdentity()
    .drawGreedIdentity();

//    aScene.getLog().trace();
//    LogPoints::write( aScene.getLog(), aScene.mousePos, "point" );
//    aScene.getLog().trace();
//    LogPoints::write( aScene.getLog(), aScene.mouseDelta, "delta" );
}

