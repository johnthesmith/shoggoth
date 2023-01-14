#include <iostream>

/* Local libraries */
#include "../lib/scene_payload.h"
#include "../lib/scene.h"
#include "../lib/rgba.h"
#include "../lib/log_points.h"
#include "../lib/draw_mode.h"



using namespace std;



//Form::Form
//(
//    Log& aLog
//)
//: ScenePayload( aLog ) /* Call parent constructor */
//{}
//



/*
    Main draw method
*/
void Form::draw
(
    Scene& aScene   /* Scene object */
)
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );

    aScene.mouseDelta;

    aScene
    .cameraLocation( Point3( 1,1,1 ), Point3( 0,0,0 ), VECTOR_3D_Y )
    .cameraApply()

    .clearColor()
    .begin( LINE )
    .color( RGBA_GREEN ).vertex( VECTOR_3D_0 ).vertex( VECTOR_3D_X )
    .color( RGBA_BLUE ).vertex( VECTOR_3D_0 ).vertex( VECTOR_3D_Y )
    .color( RGBA_RED ).vertex( VECTOR_3D_0 ).vertex( VECTOR_3D_Z )
    .end()
    ;

    aScene.begin( LINE );
    for( double l = -1; l<=1; l+=0.1 )
    {
        aScene
        .color( Rgba( 1,0,0,0.5 ))
        .vertex( Point3( l, 0, -1 ))
        .vertex( Point3( l, 0, 1 ))
        .vertex( Point3( -1, 0, l ))
        .vertex( Point3( 1, 0, l ))

        .color( Rgba( 0,1,0,0.5 ))
        .vertex( Point3( 0, l, -1 ))
        .vertex( Point3( 0, l, 1 ))
        .vertex( Point3( 0, -1, l ))
        .vertex( Point3( 0, 1, l ))

        .color( Rgba( 0,0,1,0.5 ))
        .vertex( Point3( -1, l, 0 ))
        .vertex( Point3( 1, l, 0 ))
        .vertex( Point3( l, -1, 0 ))
        .vertex( Point3( l, 1, 0 ));
    }
    aScene.end();

//    aScene.getLog().trace();
//    LogPoints::write( aScene.getLog(), aScene.mousePos, "point" );
//    aScene.getLog().trace();
//    LogPoints::write( aScene.getLog(), aScene.mouseDelta, "delta" );
}

