#include <iostream>

/* Local libraries */
#include "scene_payload.h"
#include "scene.h"
#include "rgba.h"
#include "log_points.h"
#include "draw_mode.h"



using namespace std;



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
void ScenePayload::onDraw
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
    Main calc method
*/
void ScenePayload::onCalc
(
    Scene& aScene   /* Scene object */
)
{
    aScene.getLog().trace( "Calculate event" );
}




/*
    Main draw method
*/
void ScenePayload::onLeftUp
(
    const Scene& aScene,          /* Scene object */
    const Point3& aMousePos,      /* MousePosition */
    const Point3& aMouseDelta,    /* MousePosition */
    const int aKeyMode
)
{
    defaultMouseEventInformation
    (
        aScene,
        "left up",
        aMousePos,
        aMouseDelta,
        aKeyMode
    );
}




/*
    Main draw method
*/
void ScenePayload::onLeftDown
(
    const Scene& aScene,          /* Scene object */
    const Point3& aMousePos,      /* MousePosition */
    const Point3& aMouseDelta,    /* MousePosition */
    const int aKeyMode
)
{
    defaultMouseEventInformation
    (
        aScene,
        "left down",
        aMousePos,
        aMouseDelta,
        aKeyMode
    );
}



/*
    Main draw method
*/
void ScenePayload::onLeftClick
(
    const Scene& aScene,          /* Scene object */
    const Point3& aMousePos,      /* MousePosition */
    const Point3& aMouseDelta,    /* MousePosition */
    const int aKeyMode
)
{
    defaultMouseEventInformation
    (
        aScene,
        "left click",
        aMousePos,
        aMouseDelta,
        aKeyMode
    );
}



/*
    Main draw method
*/
void ScenePayload::onLeftDblClick
(
    const Scene& aScene,          /* Scene object */
    const Point3& aMousePos,      /* MousePosition */
    const Point3& aMouseDelta,    /* MousePosition */
    const int aKeyMode
)
{
    defaultMouseEventInformation
    (
        aScene,
        "left double click",
        aMousePos,
        aMouseDelta,
        aKeyMode
    );
}



/*
    Main draw method
*/
void ScenePayload::onRightUp
(
    const Scene& aScene,          /* Scene object */
    const Point3& aMousePos,      /* MousePosition */
    const Point3& aMouseDelta,    /* MousePosition */
    const int aKeyMode
)
{
    defaultMouseEventInformation
    (
        aScene,
        "right up",
        aMousePos,
        aMouseDelta,
        aKeyMode
    );
}



/*
    Main draw method
*/
void ScenePayload::onRightDown
(
    const Scene& aScene,          /* Scene object */
    const Point3& aMousePos,      /* MousePosition */
    const Point3& aMouseDelta,    /* MousePosition */
    const int aKeyMode
)
{
    defaultMouseEventInformation
    (
        aScene,
        "right down",
        aMousePos,
        aMouseDelta,
        aKeyMode
    );
}



/*
    Main draw method
*/
void ScenePayload::onRightClick
(
    const Scene& aScene,          /* Scene object */
    const Point3& aMousePos,      /* MousePosition */
    const Point3& aMouseDelta,    /* MousePosition */
    const int aKeyMode
)
{
    defaultMouseEventInformation
    (
        aScene,
        "right click",
        aMousePos,
        aMouseDelta,
        aKeyMode
    );
}



/*
    Main draw method
*/
void ScenePayload::onRightDblClick

(
    const Scene& aScene,          /* Scene object */
    const Point3& aMousePos,      /* MousePosition */
    const Point3& aMouseDelta,    /* MousePosition */
    const int aKeyMode
)
{
    defaultMouseEventInformation
    (
        aScene,
        "right double click",
        aMousePos,
        aMouseDelta,
        aKeyMode
    );
}


/*
    Main draw method
*/
void ScenePayload::onMiddleUp
(
    const Scene& aScene,          /* Scene object */
    const Point3& aMousePos,      /* MousePosition */
    const Point3& aMouseDelta,    /* MousePosition */
    const int aKeyMode
)
{
    defaultMouseEventInformation
    (
        aScene,
        "middle up",
        aMousePos,
        aMouseDelta,
        aKeyMode
    );
}



/*
    Main draw method
*/
void ScenePayload::onMiddleDown
(
    const Scene& aScene,          /* Scene object */
    const Point3& aMousePos,      /* MousePosition */
    const Point3& aMouseDelta,    /* MousePosition */
    const int aKeyMode
)
{
    defaultMouseEventInformation
    (
        aScene,
        "middle down",
        aMousePos,
        aMouseDelta,
        aKeyMode
    );
}



/*
    Main draw method
*/
void ScenePayload::onMiddleClick
(
    const Scene& aScene,          /* Scene object */
    const Point3& aMousePos,      /* MousePosition */
    const Point3& aMouseDelta,    /* MousePosition */
    const int aKeyMode
)
{
    defaultMouseEventInformation
    (
        aScene,
        "middle click",
        aMousePos,
        aMouseDelta,
        aKeyMode
    );
}



/*
    Main draw method
*/
void ScenePayload::onMiddleDblClick
(
    const Scene& aScene,          /* Scene object */
    const Point3& aMousePos,      /* MousePosition */
    const Point3& aMouseDelta,    /* MousePosition */
    const int aKeyMode
)
{
    defaultMouseEventInformation
    (
        aScene,
        "middle double click",
        aMousePos,
        aMouseDelta,
        aKeyMode
    );
}



/*
    Mouse wheel event
*/
void ScenePayload::onMouseWheel
(
    const Scene& aScene,  /* Scene object */
    const double aX,      /* x scroll value */
    const double aY      /* y scroll value */
)
{
    getLog()
    .trace( "Mouse wheel" )
    .prm( "x", aX )
    .prm( "y", aY )
    .lineEnd();
}



/*
    Keyboard up event
*/
void ScenePayload::onKeyUp
(
    const Scene&    aScene, /* Scene object */
    const int       aKey,   /* Key */
    const int       aCode,  /* Scan code*/
    const int       aMode   /* Mode */
)
{
    getLog()
    .trace( "Key up" )
    .prm( "key", aKey )
    .prm( "code", aCode )
    .prm( "mode", aMode )
    .lineEnd();
}



/*
    Keyboard down event
*/
void ScenePayload::onKeyDown
(
    const Scene&    aScene, /* Scene object */
    const int       aKey,   /* Key */
    const int       aCode,  /* Scan code*/
    const int       aMode   /* Mode */
)
{
    getLog()
    .trace( "Key down" )
    .prm( "key", aKey )
    .prm( "code", aCode )
    .prm( "mode", aMode )
    .lineEnd();
}




/*
    Private methods
*/
ScenePayload& ScenePayload::defaultMouseEventInformation
(
    const Scene& aScene,          /* Scene object */
    const string aEvent,
    const Point3& aMousePos,      /* MousePosition */
    const Point3& aMouseDelta,    /* MousePosition */
    const int aKeyMode
)
{
    getLog()
    .trace( "Mouse event" )
    .prm( "name", aEvent )
    .prm( "mode", aKeyMode )
    .lineEnd();
    return *this;
}
