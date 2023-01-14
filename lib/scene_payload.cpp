#include <iostream>

/* Local libraries */
#include "scene_payload.h"
#include "scene.h"
#include "rgba.h"
#include "log_points.h"
#include "draw_mode.h"



using namespace std;



ScenePayload::ScenePayload
(
    Log& aLog
):
log( aLog ) /* define the prived log from aLog argument */
{
    getLog().trace( "Create payload" );
}



ScenePayload::~ScenePayload()
{
    getLog().trace( "Destroy payload" );
}


/*
    Set scene value
*/
ScenePayload& ScenePayload::setScene
(
    Scene& a
)
{
    scene = &a;
    return *this;
}



/*
    Get scene value
*/
Scene& ScenePayload::getScene()
{
    return *scene;
}



/*
    Main draw method
*/
void ScenePayload::draw
(
    Scene& aScene   /* Scene object */
)
{
    aScene
    .clearColor()
    .begin( LINE )
    .color( RGBA_GREEN ).vertex( VECTOR_3D_0 ).vertex( VECTOR_3D_X )
    .color( RGBA_BLUE ).vertex( VECTOR_3D_0 ).vertex( VECTOR_3D_Y )
    .color( RGBA_RED ).vertex( VECTOR_3D_0 ).vertex( VECTOR_3D_Z )
    .end()
    ;
}



/*
    Get scene value
*/
Log& ScenePayload::getLog()
{
    return log;
}
