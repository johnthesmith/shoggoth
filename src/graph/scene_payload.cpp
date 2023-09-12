#include <iostream>

/* Local libraries */
#include "scene_payload.h"
#include "scene.h"
#include "rgba.h"
#include "log_points.h"
#include "draw_mode.h"



using namespace std;



/*
    Transparent constructor
*/
ScenePayload::ScenePayload
(
    Application* aApplication  /* Log */
)
: Payload( aApplication ) /* Call parent constructor */
{
}



ScenePayload::~ScenePayload()
{
}



/*
    Creator
*/
ScenePayload* ScenePayload::create
(
    Application* aApplication
)
{
    return new ScenePayload( aApplication );
}



/*
    Destructor
*/
void ScenePayload::destroy()
{
    delete this;
}



/******************************************************************************
    Methods
*/



/*
    Set scene value
*/
ScenePayload* ScenePayload::setScene
(
    Scene& a
)
{
    scene = &a;
    return this;
}



/*
    Get scene value
*/
Scene& ScenePayload::getScene()
{
    return *scene;
}



/*
    Apply camera to scene
*/
ScenePayload* ScenePayload::applyCameraToScene
(
    Camera& aCamera,    /* Camera object */
    Scene& aScene       /* Scene object */
)
{
    Matrix4& m = aScene.getViewMatrixRef();
    aCamera.setViewMatrixTo( m );
    aScene.switchToWorld();
    return this;
}



/*
    Apply screen to scene
*/
ScenePayload* ScenePayload::applyScreenToScene
(
    Scene& aScene       /* Scene object */
)
{
    aScene.switchToScreen();
    return this;
}




/*****************************************************************************
    Events
*/



/*
    On activate event
*/
void ScenePayload::onActivate
(
    Scene& aScene   /* Scene object */
)
{
    getLog() -> trace( "Scene activate" );
}



/*
    Main draw method
*/
void ScenePayload::onDraw
(
    Scene& aScene   /* Scene object */
)
{
    aScene.clearColor();

    aScene.begin( LINE );

    aScene.color( RGBA_GREEN )
    -> vertex( POINT_3D_0 )
    -> vertex( POINT_3D_X )
    -> color( RGBA_BLUE ) -> vertex( POINT_3D_0 ) -> vertex( POINT_3D_Y )
    -> color( RGBA_RED ) -> vertex( POINT_3D_0 ) -> vertex( POINT_3D_Z )
    -> end();
}



/*
    Main calc method
*/
void ScenePayload::onCalc
(
    Scene& aScene   /* Scene object */
)
{
//    aScene.getLog().trace( "Calculate event" );
}




/*
    Main draw method
*/
void ScenePayload::onLeftUp
(
    Scene& aScene,              /* Scene object */
    const Point3d& aMousePos,    /* MousePosition */
    const int aKeyMode
)
{
    defaultMouseEventInformation
    (
        aScene,
        "left up",
        aMousePos,
        aKeyMode
    );
}




/*
    Main draw method
*/
void ScenePayload::onLeftDown
(
    Scene& aScene,              /* Scene object */
    const Point3d& aMousePos,    /* MousePosition */
    const int aKeyMode
)
{
    defaultMouseEventInformation
    (
        aScene,
        "left down",
        aMousePos,
        aKeyMode
    );
}



/*
    Main draw method
*/
void ScenePayload::onLeftClick
(
    Scene& aScene,              /* Scene object */
    const Point3d& aMousePos,    /* MousePosition */
    const int aKeyMode
)
{
    defaultMouseEventInformation
    (
        aScene,
        "left click",
        aMousePos,
        aKeyMode
    );
}



/*
    Main draw method
*/
void ScenePayload::onLeftDblClick
(
    Scene& aScene,              /* Scene object */
    const Point3d& aMousePos,    /* MousePosition */
    const int aKeyMode
)
{
    defaultMouseEventInformation
    (
        aScene,
        "left double click",
        aMousePos,
        aKeyMode
    );
}



/*
    Main draw method
*/
void ScenePayload::onRightUp
(
    Scene& aScene,              /* Scene object */
    const Point3d& aMousePos,    /* MousePosition */
    const int aKeyMode
)
{
    defaultMouseEventInformation
    (
        aScene,
        "right up",
        aMousePos,
        aKeyMode
    );
}



/*
    Main draw method
*/
void ScenePayload::onRightDown
(
    Scene& aScene,              /* Scene object */
    const Point3d& aMousePos,    /* MousePosition */
    const int aKeyMode
)
{
    defaultMouseEventInformation
    (
        aScene,
        "right down",
        aMousePos,
        aKeyMode
    );
}



/*
    Main draw method
*/
void ScenePayload::onRightClick
(
    Scene& aScene,              /* Scene object */
    const Point3d& aMousePos,    /* MousePosition */
    const int aKeyMode
)
{
    defaultMouseEventInformation
    (
        aScene,
        "right click",
        aMousePos,
        aKeyMode
    );
}



/*
    Main draw method
*/
void ScenePayload::onRightDblClick

(
    Scene& aScene,              /* Scene object */
    const Point3d& aMousePos,    /* MousePosition */
    const int aKeyMode
)
{
    defaultMouseEventInformation
    (
        aScene,
        "right double click",
        aMousePos,
        aKeyMode
    );
}


/*
    Main draw method
*/
void ScenePayload::onMiddleUp
(
    Scene& aScene,              /* Scene object */
    const Point3d& aMousePos,    /* MousePosition */
    const int aKeyMode
)
{
    defaultMouseEventInformation
    (
        aScene,
        "middle up",
        aMousePos,
        aKeyMode
    );
}



/*
    Main draw method
*/
void ScenePayload::onMiddleDown
(
    Scene& aScene,              /* Scene object */
    const Point3d& aMousePos,    /* MousePosition */
    const int aKeyMode
)
{
    defaultMouseEventInformation
    (
        aScene,
        "middle down",
        aMousePos,
        aKeyMode
    );
}



/*
    Main draw method
*/
void ScenePayload::onMiddleClick
(
    Scene& aScene,              /* Scene object */
    const Point3d& aMousePos,    /* MousePosition */
    const int aKeyMode
)
{
    defaultMouseEventInformation
    (
        aScene,
        "middle click",
        aMousePos,
        aKeyMode
    );
}



/*
    Main draw method
*/
void ScenePayload::onMiddleDblClick
(
    Scene& aScene,              /* Scene object */
    const Point3d& aMousePos,    /* MousePosition */
    const int aKeyMode
)
{
    defaultMouseEventInformation
    (
        aScene,
        "middle double click",
        aMousePos,
        aKeyMode
    );
}



/*
    Mouse wheel event
*/
void ScenePayload::onMouseWheel
(
    Scene& aScene,      /* Scene object */
    const Point3d& aPoint
)
{
    getLog()
    -> trace( "Mouse wheel" )
    -> prm( "x", aPoint.x )
    -> prm( "y", aPoint.y )
    -> prm( "left shift", aScene.isKey( KEY_LEFT_SHIFT ))
    -> prm( "left control", aScene.isKey( KEY_LEFT_CONTROL ))
    -> prm( "left alt", aScene.isKey( KEY_LEFT_ALT ))
    -> lineEnd();
}



/*
    Mouse move event
*/
void ScenePayload::onMouseMove
(
    Scene& aScene,      /* Scene object */
    const Point3d& aPoint
)
{
    getLog()
    -> trace( "Mouse move" )
    -> prm( "x", aPoint.x )
    -> prm( "y", aPoint.y )
    -> prm( "left shift", aScene.isKey( KEY_LEFT_SHIFT ))
    -> prm( "left control", aScene.isKey( KEY_LEFT_CONTROL ))
    -> prm( "left alt", aScene.isKey( KEY_LEFT_ALT ))
    -> lineEnd();
}



/*
    Mouse left drag begin
*/
void ScenePayload::onLeftDragBegin
(
    Scene& aScene,      /* Scene object */
    const Point3d& aPoint
)
{
    getLog()
    -> trace( "Mouse left drag begin" )
    -> prm( "x", aPoint.x )
    -> prm( "y", aPoint.y )
    -> prm( "left shift", aScene.isKey( KEY_LEFT_SHIFT ))
    -> prm( "left control", aScene.isKey( KEY_LEFT_CONTROL ))
    -> prm( "left alt", aScene.isKey( KEY_LEFT_ALT ))
    -> lineEnd();
}


/*
    Mouse left drag end
*/
void ScenePayload::onLeftDragEnd
(
    Scene& aScene,      /* Scene object */
    const Point3d& aPoint
)
{
    getLog()
    -> trace( "Mouse left drag end" )
    -> prm( "x", aPoint.x )
    -> prm( "y", aPoint.y )
    -> prm( "left shift", aScene.isKey( KEY_LEFT_SHIFT ))
    -> prm( "left control", aScene.isKey( KEY_LEFT_CONTROL ))
    -> prm( "left alt", aScene.isKey( KEY_LEFT_ALT ))
    -> lineEnd();
}



/*
    Mouse left drag
*/
void ScenePayload::onLeftDrag
(
    Scene& aScene,      /* Scene object */
    const Point3d& aPoint
)
{
    getLog()
    -> trace( "Mouse left drag" )
    -> prm( "x", aPoint.x )
    -> prm( "y", aPoint.y )
    -> prm( "left shift", aScene.isKey( KEY_LEFT_SHIFT ))
    -> prm( "left control", aScene.isKey( KEY_LEFT_CONTROL ))
    -> prm( "left alt", aScene.isKey( KEY_LEFT_ALT ))
    -> lineEnd();
}



/*
    Mouse right drag begin
*/
void ScenePayload::onRightDragBegin
(
    Scene& aScene,      /* Scene object */
    const Point3d& aPoint
)
{
    getLog()
    -> trace( "Mouse right drag begin" )
    -> prm( "x", aPoint.x )
    -> prm( "y", aPoint.y )
    -> prm( "left shift", aScene.isKey( KEY_LEFT_SHIFT ))
    -> prm( "left control", aScene.isKey( KEY_LEFT_CONTROL ))
    -> prm( "left alt", aScene.isKey( KEY_LEFT_ALT ))
    -> lineEnd();
}


/*
    Mouse left drag end
*/
void ScenePayload::onRightDragEnd
(
    Scene& aScene,      /* Scene object */
    const Point3d& aPoint
)
{
    getLog()
    -> trace( "Mouse right drag end" )
    -> prm( "x", aPoint.x )
    -> prm( "y", aPoint.y )
    -> prm( "left shift", aScene.isKey( KEY_LEFT_SHIFT ))
    -> prm( "left control", aScene.isKey( KEY_LEFT_CONTROL ))
    -> prm( "left alt", aScene.isKey( KEY_LEFT_ALT ))
    -> lineEnd();
}


/*
    Mouse left drag
*/
void ScenePayload::onRightDrag
(
    Scene& aScene,      /* Scene object */
    const Point3d& aPoint
)
{
    getLog()
    -> trace( "Mouse right drag" )
    -> prm( "x", aPoint.x )
    -> prm( "y", aPoint.y )
    -> prm( "left shift", aScene.isKey( KEY_LEFT_SHIFT ))
    -> prm( "left control", aScene.isKey( KEY_LEFT_CONTROL ))
    -> prm( "left alt", aScene.isKey( KEY_LEFT_ALT ))
    -> lineEnd();
}



/*
    Keyboard up event
*/
void ScenePayload::onKeyUp
(
    Scene&      aScene, /* Scene object */
    const int   aKey,   /* Key */
    const int   aCode,  /* Scan code*/
    const int   aMode   /* Mode */
)
{
    getLog()
    -> trace( "Key up" )
    -> prm( "key", aKey )
    -> prm( "code", aCode )
    -> prm( "mode", aMode )
    -> lineEnd();
}



/*
    Keyboard down event
*/
void ScenePayload::onKeyDown
(
    Scene&      aScene, /* Scene object */
    const int   aKey,   /* Key */
    const int   aCode,  /* Scan code*/
    const int   aMode   /* Mode */
)
{
    getLog()
    -> trace( "Key down" )
    -> prm( "key", aKey )
    -> prm( "code", aCode )
    -> prm( "mode", aMode )
    -> lineEnd();
}




/*
    Private methods
*/
ScenePayload& ScenePayload::defaultMouseEventInformation
(
    Scene& aScene,              /* Scene object */
    const string aEvent,
    const Point3d& aMousePos,    /* MousePosition */
    const int aKeyMode
)
{
    getLog()
    -> trace( "Mouse event" )
    -> prm( "name", aEvent )
    -> prm( "mode", aKeyMode )
    -> lineEnd();
    return *this;
}
