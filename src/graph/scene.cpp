/*
    Look at scene.h
*/

#include <iostream>
#include <string>
#include <unistd.h>         /* usleep */
#include <GL/glu.h>

/* Local libraries */
#include "../lib/utils.h"
#include "../lib/moment.h"

//#include "camera.h"
#include "scene.h"
#include "matrix.h"
#include "scene_payload.h"
#include "log_points.h"



using namespace std;



/*
    Constructor
*/
Scene::Scene
(
    Log* aLog
)
{
    log = aLog;
    getLog() -> trace( "Create scene object" );
    /* Create the font */
    font = Font::create( getLog() );
}



/*
    Destructor
*/
Scene::~Scene()
{
    font -> destroy();
    getLog() -> trace( "Destory scene object" );
}



/*
    Create new scene
*/
Scene* Scene::create
(
    Log* aLog
)
{
    return new Scene( aLog );
}



/*
    Scene self destroy
*/
void Scene::destroy()
{
    delete this;
}



/*
    Return log
*/
Log* Scene::getLog()
{
    return log;
}



/*
    Initialize
*/
Scene* Scene::init
(
    int aWidth,     /* Window width */
    int aHeight,    /* Window height */
    string aTitle   /* Window title */
)
{
    glInit = glfwInit();
    if( isInit() )
    {
        getLog()
        -> trace( "glfl initialize" )
        -> trace( "Create window" )
        -> lineEnd();

        /* Window create */
        win = glfwCreateWindow( aWidth, aHeight, aTitle.c_str(), NULL, NULL );
        if( isWindow() )
        {
            glfwMakeContextCurrent( win );

            /* Set user pointer for callback access */
            glfwSetWindowUserPointer( win, this );

            /*
                Set mouse button event in lambda
                https://www.glfw.org/docs/latest/group__input.html#ga571e45a030ae4061f746ed56cb76aede
            */
            glfwSetMouseButtonCallback
            (
                win,
                []
                (
                    GLFWwindow* aWin,
                    int aButton,
                    int aAction,
                    int aMods
                )
                {
                    /* Get scene object in lambda and call method */
                    ((Scene*)glfwGetWindowUserPointer( aWin ))
                    -> mouseEvent( aButton, aAction, aMods );
                }
            );

            /*
                Set mouse wheel event in lambda
                https://www.glfw.org/docs/latest/group__input.html#ga571e45a030ae4061f746ed56cb76aede
            */
            glfwSetScrollCallback
            (
                win,
                []
                (
                    GLFWwindow* aWin,
                    double aX,
                    double aY
                )
                {
                    /* Get scene object in lambda and call method */
                    ((Scene*)glfwGetWindowUserPointer( aWin ))
                    -> mouseWheelEvent( aX, aY );
                }
            );

            /*
                Set mouse move event in lambda
                https://www.glfw.org/docs/latest/group__input.html#gac1f879ab7435d54d4d79bb469fe225d7
            */
            glfwSetCursorPosCallback
            (
                win,
                []
                (
                    GLFWwindow* aWin,
                    double aX,
                    double aY
                )
                {
                    /* Get scene object in lambda and call method */
                    ((Scene*)glfwGetWindowUserPointer( aWin ))
                    -> mouseMoveEvent( aX, aY );
                }
            );

            /*
                Set keyboard event in lambda
                https://www.glfw.org/docs/3.3/group__input.html#ga1caf18159767e761185e49a3be019f8d
            */
            glfwSetKeyCallback
            (
                win,
                []
                (
                    GLFWwindow* aWin,
                    int aKey,
                    int aScancode,
                    int aAction,
                    int aMods
                )
                {
                    /* Get scene object in lambda */
                    ((Scene*)glfwGetWindowUserPointer( aWin ))
                    -> keyboardEvent( aKey, aScancode, aAction, aMods );
                }
            );

            /* Buuld the font */
            font -> build();
        }
        else
        {
            log -> warning( "glfw window create error" );
            setResult( "glfl_window_create_error" );
        }
    }
    else
    {
        log -> warning( "glfw init error" );
        setResult( "glfl_init_error" );
    }

    return this;
}



Scene* Scene::finit()
{
    /* destroy Window */
    if( isWindow() )
    {
        glfwDestroyWindow( win );
        win = NULL;
    }

    /* terminate opengl */
    if( glInit )
    {
        glfwTerminate();
        glInit = false;     /* Set initialize flag to false */
    }
    return this;
}



/*
    Main loop for scene
*/
Scene* Scene::loop()
{
    if( isInit() && isWindow() )
    {
        while
        (
            isOk() &&                       /* Scene isOk */
            !terminated  &&                 /* Not scene terminated */
            !glfwWindowShouldClose( win )   /* Window is not open */
        )
        {
            /* Run draw function */
            auto current = now();

            /* Draw frame */
            calcEvent();
            drawEvent();
            auto stopDraw = now();

            /* Calculate delta draw */
            auto delta = ( stopDraw - current );

            if( delta > 0 )
            {
                /* Calculate idel time */
                idle = 1000000 / fpsDrawLimit - delta;

                /* Sleep idle time */
                if( idle > 0 )
                {
                    usleep( idle );
                }
                else
                {
                    idle = 0;
                }

                /* Calculate fps */
                fps = 1000000 / ( stopDraw - current );
            }
        }
    }

    return this;
}



/*
    Window is not open
*/
bool Scene::windowClosed()
{
    return glfwWindowShouldClose( win );
}

/******************************************************************************
    Internal Events method
*/

/*
    Internal draw event method
    Call payload draw method
*/
Scene& Scene::drawEvent()
{
    if( isOk() && payload != NULL && isInit() )
    {
        /* Get window size */
        glfwGetFramebufferSize( win, &(viewport.width), &(viewport.height) );

        if( viewport.width > 0 && viewport.height > 0 && far != near )
        {
            /* Set opengl viewport default */
            glViewport
            (
                viewport.left,
                viewport.top,
                viewport.width,
                viewport.height
            );

//glEnable(GL_DEPTH_TEST);
//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//glEnable( GL_BLEND);
//glEnable( GL_LINE_SMOOTH);
//glHint( GL_LINE_SMOOTH_HINT, GL_NICEST);


            /* Pulling event for keyboard and mouse*/
            glfwPollEvents();
            /* Payload draw event */
            payload -> onDraw( this );

            /* Draw buffer to window */
            glfwSwapBuffers( win );

            auto code = glGetError();
            if( code != GL_NO_ERROR )
            {
                auto message = Scene::openglErrorToString( code );
                getLog()
                -> warning( "opengl error" )
                -> prm( "code", code )
                -> prm( "message", message );
                setResult( "opengl_error", message );
            }
        }
    }

    return *this;
}



/*
    World matrixes is loading to opengl
    Modelview matrix must prepared before thos operation
*/
Scene& Scene::switchToWorld()
{
    /* Projection matrrix load */
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 45, viewport.width / (float) viewport.height, near, far );
    glGetDoublev( GL_PROJECTION_MATRIX, (GLdouble*)&projectionMatrix );

    /* Modelview matrrix load */
    glMatrixMode( GL_MODELVIEW );
    glLoadMatrixd( (GLdouble*)&viewMatrix );

    return *this;
}



/*
    Ortho screen matrixes is loading to opengl
*/
Scene& Scene::switchToScreen()
{
    /* Projection matrrix load */
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho
    (
        viewport.left,
        viewport.left + viewport.width,
        viewport.top,
        viewport.top + viewport.height,
        0,
        1
    );

    /* Modelview matrrix load */
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    return *this;
}



/*
    Internal draw event method
    Call payload draw method
*/
Scene& Scene::calcEvent()
{
    if( isOk() && payload != NULL )
    {
        payload -> onCalc( this );
    }
    return *this;
}




/*
    Internal keyboard event
*/
Scene& Scene::keyboardEvent
(
    int aKey,
    int aScancode,
    int aAction,
    int aMods
)
{
    if( isOk() && payload != NULL && isInit() )
    {
        switch( aAction )
        {
            case 0:
                payload -> onKeyUp( *this, aKey, aScancode, aMods );
            break;
            case 1:
                payload -> onKeyDown( *this, aKey, aScancode, aMods );
            break;
        }
    }
    return *this;
}



/*
    Internal mouse event controller
    This method generate users event for mouse
*/
Scene& Scene::mouseEvent
(
    int aButton,
    int aAction,
    int aMods
)
{
    if( isOk() && payload != NULL && isInit() )
    {
        /* Read current moment */
        auto currentMoment = now();
        auto keyMode = aMods;

        /* Controller */
        switch( aButton )
        {
            case 0: /* Left button */
                switch( aAction )
                {
                    case 0: /* Left button Up */
                        payload -> onLeftUp( *this, mouseCurrent, keyMode );
                        if( mouseLeftDrag )
                        {
                            /* Left mouse drag end */
                            mouseLeftDrag = false;
                            payload -> onLeftDragEnd( *this, mouseCurrent );
                        }
                    break;
                    case 1: /* Left button Down */
                        payload -> onLeftDown( *this, mouseCurrent, keyMode );
                    break;
                }

                /* Click sequence change */
                if( currentMoment - lastLeftMouse < clickTimeoutMls * MILLISECOND )
                {
                    leftClickCount += 1;
                }
                else
                {
                    /* Stop click sequence */
                    leftClickCount = 0;
                }

                /* Click sequence controller */
                switch( leftClickCount )
                {
                    case 0: /* First down, begin of click */ break;
                    case 1: payload -> onLeftClick( *this, mouseCurrent, keyMode ); break;
                    case 2: /* Second down, begin of dbl click */ break;
                    case 3: payload -> onLeftDblClick( *this, mouseCurrent, keyMode); break;
                    case 4: leftClickCount = 0; break;
                }

                lastLeftMouse = currentMoment;
            break;

            case 1: /* Right button */
                switch( aAction )
                {
                    case 0: /* Right button Up */
                        payload -> onRightUp( *this, mouseCurrent, keyMode );
                        if( mouseRightDrag )
                        {
                            /* Left mouse drag end */
                            mouseRightDrag = false;
                            payload -> onRightDragEnd( *this, mouseCurrent );
                        }
                    break;
                    case 1: /* Right button Down */
                        payload -> onRightDown( *this, mouseCurrent, keyMode );
                    break;
                }

                /* Click sequence change */
                if( currentMoment - lastRightMouse < clickTimeoutMls * MILLISECOND )
                {
                    rightClickCount += 1;
                }
                else
                {
                    /* Stop click sequence */
                    rightClickCount = 0;
                }

                /* Click sequence controller */
                switch( rightClickCount )
                {
                    case 0: /* First down, begin of click */ break;
                    case 1: payload -> onRightClick( *this, mouseCurrent, keyMode ); break;
                    case 2: /* Second down, begin of dbl click */ break;
                    case 3: payload -> onRightDblClick( *this, mouseCurrent, keyMode); break;
                    case 4: rightClickCount = 0; break;
                }

                lastRightMouse = currentMoment;
            break;

            case 2: /* Middle button */
                switch( aAction )
                {
                    case 0: /* Middle button Up */
                        payload -> onMiddleUp( *this, mouseCurrent, keyMode );
                    break;
                    case 1: /* Middle button Down */
                        payload -> onMiddleDown( *this, mouseCurrent, keyMode );
                    break;
                }

                /* Click sequence change */
                if( currentMoment - lastMiddleMouse < clickTimeoutMls * MILLISECOND )
                {
                    middleClickCount += 1;
                }
                else
                {
                    /* Stop click sequence */
                    middleClickCount = 0;
                }

                /* Click sequence controller */
                switch( middleClickCount )
                {
                    case 0: /* First down, begin of click */ break;
                    case 1: payload -> onMiddleClick( *this, mouseCurrent, keyMode ); break;
                    case 2: /* Second down, begin of dbl click */ break;
                    case 3: payload -> onMiddleDblClick( *this, mouseCurrent, keyMode); break;
                    case 4: middleClickCount = 0; break;
                }

                lastMiddleMouse = currentMoment;
            break;
        }
        ;
    }
    return *this;
}



/*
    Internal mouse wheel event controller
    This method generate users event for mouse wheel
*/
Scene& Scene::mouseWheelEvent
(
    const double aX,
    const double aY
)
{
    payload -> onMouseWheel( *this, Point3d( aX, aY ));
    return *this;
}



/*
    Internal mouse move event controller
    This method generate users event for mouse wheel
*/
Scene& Scene::mouseMoveEvent
(
    const double aX,
    const double aY
)
{
    if( isOk() && payload != NULL && isInit() )
    {
        /* Calculate mouse delta position */
        mouseLast = mouseCurrent;
        mouseCurrent.set( aX, viewport.height - aY, 0 );

        /* Mouse left drag control */
        if( isMouseButton( MB_LEFT ) )
        {
            if( mouseLeftDrag )
            {
                payload -> onLeftDrag( *this, Point3d( aX, aY ));
            }
            else
            {
                payload -> onLeftDragBegin( *this, Point3d( aX, aY ));
                mouseLeftDrag = true;
            }
        }

        /* Mouse right drag control */
        if( isMouseButton( MB_RIGHT ) )
        {
            if( mouseRightDrag )
            {
                payload -> onRightDrag( *this, Point3d( aX, aY ));
            }
            else
            {
                payload -> onRightDragBegin( *this, Point3d( aX, aY ));
                mouseRightDrag = true;
            }
        }

        /* Nouse move event */
        payload -> onMouseMove( *this, Point3d( aX, aY ));
    }
    return *this;
}



/******************************************************************************
    Services
*/


/*
    Set payload
*/
Scene* Scene::setPayload
(
    ScenePayload* a
)
{
    payload = a;
    payload -> setScene( *this );

    if( isOk() && payload != NULL && isInit() )
    {
        payload -> onActivate( *this );
    }

    return this;
}



/*
    Get payload
*/
ScenePayload& Scene::getPayload()
{
    return *payload;
}



/*
    Return true if openGL is initialized
*/
bool Scene::isInit()
{
    return glInit;
}



/*
    Return true if window is initialized
*/
bool Scene::isWindow()
{
    return win != NULL;
}



/******************************************************************************
    Opengl API simple functions
*/

/*
    API commands
*/
Scene* Scene::begin
(
    const DrawMode a
)
{
    switch( a )
    {
        case POINT      : glBegin( GL_POINTS ); break;
        case LINE       : glBegin( GL_LINES ); break;
        case LINES      : glBegin( GL_LINE_STRIP ); break;
        case LOOP       : glBegin( GL_LINE_LOOP ); break;
        case QUAD       : glBegin( GL_QUADS ); break;
        case POLYGON    : glBegin( GL_POLYGON ); break;
        case QUADS      : glBegin( GL_QUAD_STRIP ); break;
        case TRIANGLE   : glBegin( GL_TRIANGLES ); break;
        case TRIANGLES  : glBegin( GL_TRIANGLE_STRIP ); break;
        case FAN        : glBegin( GL_TRIANGLE_FAN ); break;
    }
    return this;
}



Scene* Scene::end()
{
    glEnd();
    return this;
}



/*
    Set color for canvas
*/
Scene* Scene::color
(
    const Rgba& a
)
{
    glColor4fv( (GLfloat*)&a );
    return this;
}



/*
    Set vertex to canvas
*/
Scene* Scene::texture
(
    const Point2d& a
)
{
    glTexCoord2d( a.x, a.y );
    return this;
}



/*
    Set vertex to canvas
*/
Scene* Scene::vertex
(
    const Point2d& a
)
{
    glVertex2dv( (GLdouble*)&a );
    return this;
}



/*
    Set vertex to canvas
*/
Scene* Scene::vertex
(
    const Point3d& a
)
{
    glVertex3dv( (GLdouble*)&a );
    return this;
}



/*
    Set size of point
*/
Scene* Scene::setPointSize
(
    const float a
)
{
    glPointSize( a );
    return this;
}



/*
    Set
*/
Scene& Scene::setLineWidth
(
    const float a
)
{
    glLineWidth( a );
    return *this;
}



/******************************************************************************
    OpenGL complex functions
*/

/*
    Clear color
*/
Scene& Scene::clearColor()
{
    /* Очистка буффера */
    glClearColor
    (
        backgroundColor.red,
        backgroundColor.green,
        backgroundColor.blue,
        backgroundColor.alpha
    );
    glClear( GL_COLOR_BUFFER_BIT );
    return *this;
}



string Scene::openglErrorToString
(
    GLenum a
)
{
    switch( a )
    {
        default: return "Unknown opengl errror"; break;
        case GL_NO_ERROR: return "No error has been recorded. The value of this symbolic constant is guaranteed to be 0."; break;
        case GL_INVALID_ENUM: return "An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag."; break;
        case GL_INVALID_VALUE: return "A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag."; break;
        case GL_INVALID_OPERATION: return "The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag."; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.";
        case GL_OUT_OF_MEMORY: return "There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded."; break;
        case GL_STACK_UNDERFLOW: return "An attempt has been made to perform an operation that would cause an internal stack to underflow."; break;
        case GL_STACK_OVERFLOW: return "An attempt has been made to perform an operation that would cause an internal stack to overflow."; break;
    }
}



Scene& Scene::setTerminate
(
    bool a
)
{
    terminated = a;
    return *this;
}



/*
    Draw primitives
*/

/*
    Draw greed for all axisfrom -1 to 1
*/
Scene& Scene::drawGreedIdentity()
{
    begin( LINE );
    for( double l = -1; l<=1; l+=0.1 )
    {
        /* f(x) z */
        color( Rgba( 0,0,0,0.5 )); vertex( Point3d( l, 0, -1 ));
        color( Rgba( 1,0,0,0.5 )); vertex( Point3d( l, 0, 0 ));
        color( Rgba( 1,0,0,0.5 )); vertex( Point3d( l, 0, 0 ));
        color( Rgba( 1,1,1,0.5 )); vertex( Point3d( l, 0, 1 ));

        color( Rgba( 0,0,0,0.5 )); vertex( Point3d( l, -1, 0));
        color( Rgba( 0,1,0,0.5 )); vertex( Point3d( l, 0, 0 ));
        color( Rgba( 0,1,0,0.5 )); vertex( Point3d( l, 0, 0 ));
        color( Rgba( 1,1,1,0.5 )); vertex( Point3d( l, 1, 0 ));

        color( Rgba( 0,0,0,0.5 )); vertex( Point3d( 0, l, -1));
        color( Rgba( 0,0,1,0.5 )); vertex( Point3d( 0, l, 0 ));
        color( Rgba( 0,0,1,0.5 )); vertex( Point3d( 0, l, 0 ));
        color( Rgba( 1,1,1,0.5 )); vertex( Point3d( 0, l, 1 ));

    }
    end();
    return *this;
}



/*
    Draw qube
*/
Scene& Scene::sendQube
(
    Point3d& aCenter,
    Point3d& aSize
)
{
    auto p1 = Point3d( aCenter ).sub( aSize );
    auto p2 = Point3d( p1 ).add( 0.0, aSize.y * 2, 0.0 );
    auto p3 = Point3d( p2 ).add( aSize.x * 2, 0.0, 0.0 );
    auto p4 = Point3d( p1 ).add( aSize.x * 2, 0.0, 0.0 );

    auto p5 = Point3d( p1 ).add( 0.0, 0.0, aSize.z * 2 );
    auto p6 = Point3d( p2 ).add( 0.0, 0.0, aSize.z * 2 );
    auto p7 = Point3d( p3 ).add( 0.0, 0.0, aSize.z * 2 );
    auto p8 = Point3d( p4 ).add( 0.0, 0.0, aSize.z * 2 );

    vertex( p1 ); vertex( p2 ); vertex( p3 ); vertex( p4 );
    vertex( p4 ); vertex( p3 ); vertex( p7 ); vertex( p8 );
    vertex( p5 ); vertex( p8 ); vertex( p7 ); vertex( p6 );
    vertex( p1 ); vertex( p5 ); vertex( p6 ); vertex( p2 );
    vertex( p2 ); vertex( p6 ); vertex( p7 ); vertex( p3 );
    vertex( p1 ); vertex( p4 ); vertex( p8 ); vertex( p5 );

    return *this;
}



/*
    Draw qube
*/
Scene& Scene::sendRect
(
    Point3d& aTopLeft,
    Point3d& aBottomRight
)
{
    double mx = min( aTopLeft.x, aBottomRight.x );
    double my = min( aTopLeft.y, aBottomRight.y );
    double w = abs( aTopLeft.x - aBottomRight.x );
    double h = abs( aTopLeft.y - aBottomRight.y );
    vertex( Point3d( mx, my, 0));
    vertex( Point3d( mx + w, my, 0));
    vertex( Point3d( mx + w, my + h, 0));
    vertex( Point3d( mx, my + h, 0));
    return *this;
}



/*
    Draw quad
*/
Scene& Scene::sendRect
(
    const Rect2d& aRect
)
{
    Point2d p1, p2, p3, p4;

    aRect.calcCorners( p1, p2, p3, p4 );

    vertex( p1 );
    vertex( p2 );
    vertex( p3 );
    vertex( p4 );

    return *this;
}







/*
    Draw axis from -1 to 1
*/
Scene& Scene::drawAxisIdentity()
{
    /*
    begin( LINE );
    {
        color( RGBA_WHITE ).vertex( POINT_3D_0 ).color( RGBA_X ).vertex( POINT_3D_X );
        color( RGBA_WHITE ).vertex( POINT_3D_0 ).color( RGBA_Y ).vertex( POINT_3D_Y );
        color( RGBA_WHITE ).vertex( POINT_3D_0 ).color( RGBA_Z ).vertex( POINT_3D_Z );
    }
    */
    auto o = getScreenByWorld( POINT_3D_0);

    arrow( o, getScreenByWorld( POINT_3D_X ), POINT_3D_NZ, 3, 3, 3.0, 2.0, 0.0, 0.0, Rgba( RGBA_X ).setAlpha( 0 ), RGBA_X );
    arrow( o, getScreenByWorld( POINT_3D_Y ), POINT_3D_NZ, 3, 3, 3.0, 2.0, 0.0, 0.0, Rgba( RGBA_Y ).setAlpha( 0 ), RGBA_Y );
    arrow( o, getScreenByWorld( POINT_3D_Z ), POINT_3D_NZ, 3, 3, 3.0, 2.0, 0.0, 0.0, Rgba( RGBA_Z ).setAlpha( 0 ), RGBA_Z );

    return *this;
}



/*
    return view matrix reference
*/
Matrix4& Scene::getViewMatrixRef()
{
    return viewMatrix;
}



Scene& Scene::polygonMode
(
    PolygonMode a
)
{
    switch( a )
    {
        case POLYGON_LINE    : glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); break;
        case POLYGON_POINT   : glPolygonMode( GL_FRONT_AND_BACK, GL_POINT ); break;
        case POLYGON_FILL    : glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ); break;
    }
    return *this;
}





/*
    Return true if key is pressed
*/
bool Scene::isKey
(
    Key a
)
{
    return isWindow() ? glfwGetKey( win, a) == GLFW_PRESS : false;
}




/*
    Return true if mouse button is pressed
*/
bool Scene::isMouseButton
(
    MouseButton a
)
{
    return isWindow() ? glfwGetMouseButton( win, a ) == GLFW_PRESS : false;
}



/*
    return screen point by world point
    https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluUnProject.xml
*/
Point3d Scene::getScreenByWorld
(
    const Point3d& a
)
{
    auto r = Point3d();
    gluProject
    (
        a.x, a.y, a.z,
        (GLdouble*)&viewMatrix,
        (GLdouble*)&projectionMatrix,
        (GLint*)&viewport,
        (GLdouble*)&r.x,
        (GLdouble*)&r.y,
        (GLdouble*)&r.z
    );
    r.z = 0.0;
    return r;
}



/*
    return world point by screen point
    https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluProject.xml
*/
Point3d Scene::getWorldByScreen
(
    const Point3d& a
)
{
    auto r = Point3d();
    gluUnProject
    (
        a.x, viewport.height - a.y, a.z,
        (GLdouble*)&viewMatrix,
        (GLdouble*)&projectionMatrix,
        (GLint*)&viewport,
        (GLdouble*)&r.x,
        (GLdouble*)&r.y,
        (GLdouble*)&r.z
    );
    return r;
}




/*
    Return mouse current position at screen
*/
Point3d Scene::getMouseCurrentScreen()
{
    return mouseCurrent;
}



/*
    Return mouse last position at screen
*/
Point3d Scene::getMouseLastScreen()
{
    return mouseLast;
}



/*
    Return mouse current position in world
*/
Point3d Scene::getMouseCurrentWorld()
{
    return getWorldByScreen( mouseCurrent );
}



/*
    Return mouse last position in world
*/
Point3d Scene::getMouseLastWorld()
{
    return getWorldByScreen( mouseLast );
}



Scene& Scene::setFar
(
    const double a
)
{
    far = a;
    return *this;
}



/*
    Return far clipping
*/
double Scene::getFar()
{
    return far;
}



/*
    Set near clipping
*/
Scene& Scene::setNear
(
    const double a
)
{
    near = a;
    return *this;
}



/*
    Return near clipping
*/
double Scene::getNear()
{
    return near;
}




Ltwh& Scene::getViewport()
{
    return viewport;
}




/*
    Draw arrow
*/
Scene* Scene::arrow
(
    const Point3d&  aFrom,
    const Point3d&  aTo,
    const Point3d&  aTop,
    const double    aHeightFrom,
    const double    aHeightTo,
    const double    aArrowWidth,
    const double    aArrowHeight,
    const double    aSpaceFrom,
    const double    aSpaceTo,
    const Rgba&     aColorFrom,
    const Rgba&     aColorTo
)
{
    /* Build axis vectors */
    Point3d x = ( aTo - aFrom ).norm();
    Point3d y = ( aTop % x ).norm();
    Point3d z = ( y % x ).norm();

    /*
        Calculate points
           y
           ^
           .
           .                    * p4
           .    p6            p5|\
           .    *---------------* \
           .    |                  \ p3
     aFrom *. . b. . . > x     p*   * . . . * aTo
          .     |                  /
        .       *---------------* /
      z         p0            p1|/
                                * p2
    */
    auto b  = aFrom + x * aSpaceFrom;
    auto p3 = aTo - x * aSpaceTo;
    auto p  = p3 - x * ( aArrowWidth * aHeightTo );
    auto p1 = p - y * ( aHeightTo * 0.5 );
    auto p5 = p + y * ( aHeightTo * 0.5 );
    auto p0 = b - y * ( aHeightFrom * 0.5 );
    auto p6 = b + y * ( aHeightFrom * 0.5 );
    auto p2 = p - y * ( aArrowHeight * aHeightTo );
    auto p4 = p + y * ( aArrowHeight * aHeightTo );

    /* Draw points */
    begin( TRIANGLES );
    color( aColorFrom );
    vertex( p0 );
    vertex( p6 );
    color( aColorTo );
    vertex( p1 );
    vertex( p5 );
    vertex( p2 );
    vertex( p4 );
    vertex( p3 );
    end();

//    text( "HELLO", aHeightFrom, aFrom, x, y, HA_LEFT, BL_MIDDLE );

    return this;
}



Point3d Scene::getBack()
{
    return Point3d
    (
        viewMatrix.m[ M_CX ],
        viewMatrix.m[ M_CY ],
        viewMatrix.m[ M_CZ ]
    );
}



Point3d Scene::getRight()
{
    return Point3d
    (
        viewMatrix.m[ M_AX ],
        viewMatrix.m[ M_AY ],
        viewMatrix.m[ M_AZ ]
    );
}



Point3d Scene::getTop()
{
    return Point3d
    (
        viewMatrix.m[ M_BX ],
        viewMatrix.m[ M_BY ],
        viewMatrix.m[ M_BZ ]
    );
}



/*
    Scene enabled
*/
Scene* Scene::textureEnabled
(
    const bool a
)
{
    if( a )
    {
        glEnable( GL_TEXTURE_2D );
    }
    else
    {
        glDisable( GL_TEXTURE_2D );
    }
    return this;
}



/*
    Draw matrix
*/
Scene* Scene::drawMatrix
(
    const Point3d& aPos
)
{
    setTextPosition( aPos );
    setTextBaseLine( BASE_LINE_TOP );
    setTextHorisontalAlign( ALIGN_RIGHT );
    setTextRight( POINT_3D_RIGHT );
    setTextTop( POINT_3D_TOP );
    setTextHorisontalAlign( ALIGN_RIGHT );

    setTextPosition( Point3d( 100, getViewport().height - 30, 0 ));

    text( to_string( viewMatrix.m[ M_AX ] ) ); textTab();
    text( to_string( viewMatrix.m[ M_BX ] ) ); textTab();
    text( to_string( viewMatrix.m[ M_CX ] ) ); textTab();
    text( to_string( viewMatrix.m[ M_DX ] ) ); textCR();

    text( to_string( viewMatrix.m[ M_AY ] ) ); textTab();
    text( to_string( viewMatrix.m[ M_BY ] ) ); textTab();
    text( to_string( viewMatrix.m[ M_CY ] ) ); textTab();
    text( to_string( viewMatrix.m[ M_DY ] ) ); textCR();

    text( to_string( viewMatrix.m[ M_AZ ] ) ); textTab();
    text( to_string( viewMatrix.m[ M_BZ ] ) ); textTab();
    text( to_string( viewMatrix.m[ M_CZ ] ) ); textTab();
    text( to_string( viewMatrix.m[ M_DZ ] ) ); textCR();

    text( to_string( viewMatrix.m[ M_AW ] ) ); textTab();
    text( to_string( viewMatrix.m[ M_BW ] ) ); textTab();
    text( to_string( viewMatrix.m[ M_CW ] ) ); textTab();
    text( to_string( viewMatrix.m[ M_DW ] ) ); textCR();

    return this;
}



/******************************************************************************
    Text
*/



Font* Scene::getFont()
{
    return font;
}



Scene* Scene::text
(
    const string aString               /* Text */
)
{
    /* Apply texture */
    font -> getTexture() -> use();

    /* Enable texture */
    textureEnabled( true);

    Point3d textCursor =
    textPosition - textTop * ( textSize * textBaseLine ) + textRight * textShift;

    switch( textHorisontalAlign )
    {
        default:
        case ALIGN_LEFT:
        break;
        case ALIGN_RIGHT:
            textCursor = textCursor - textRight * ( font -> calcTextWidth( aString ) * textSize );
        break;
        case ALIGN_CENTER:
            textCursor = textCursor - textRight * ( font -> calcTextWidth( aString ) * textSize * 0.5 );
        break;
    }


    /*
        utf loop for drawing gliphs

           ^ y,aTop
           .
        p2 .    p3
           *----*-----*
           | G1 |  G2 |
           *----*-----* . . . > aRight
    textCursor         p4
    */

    begin( QUAD );

    Point3d p2 = textCursor + textTop * textSize;
    Point3d letterSpacing = textRight * font -> getLetterSpacing() * textSize;

    font -> utfLoop
    (
        aString,
        [
            &letterSpacing, &p2, &textCursor, this
        ]
        ( Gliph& aGliph )
        {
            auto width = ( aGliph.width <= 0 ? font -> getSpace() : aGliph.width );
            auto shift = textRight * ( textSize * width );
            textShift += ( textSize * width );

            auto p3 = p2 + shift;
            auto p4 = textCursor + shift;

            texture( aGliph.leftBottom );  vertex( textCursor );
            texture( aGliph.leftTop );     vertex( p2 );
            texture( aGliph.rightTop );    vertex( p3 );
            texture( aGliph.rightBottom ); vertex( p4 );

            p2 = p3 + letterSpacing;
            textCursor = p4 + letterSpacing;

            return false;
        }
    );
    end();

    textureEnabled();

    return this;
}



/*
    Text tab
*/
Scene* Scene::textTab
(
    const double aShift
)
{
    textShift = ( floor( textShift / aShift ) + 1 ) * aShift;
    return this;
}



/*
    Text cartridge return
*/
Scene* Scene::textCR
(
    const int aCout
)
{
    setTextPosition
    (
        getTextPosition() - textTop * ( textMaxSize == 0.0 ? textSize :  textMaxSize )
    );
    return this;
}



/*
    Set base line for text
*/
Scene* Scene::setTextBaseLine
(
    double a
)
{
    textBaseLine = a;
    return this;
}



double Scene::getTextBaseLine()
{
    return textBaseLine;
}



/*
    Set text horisontal aigment
    Using in text method
*/
Scene* Scene::setTextHorisontalAlign
(
    TextAlign a
)
{
    textHorisontalAlign = a;
    return this;
}



/*
    Return text horisontal aigment
    Using in text method
*/
TextAlign Scene::getTextHorisontalAlign()
{
    return textHorisontalAlign;
}



/*
    Set text top vector
    Using in text method
*/
Scene* Scene::setTextTop
(
    Point3d a
)
{
    textTop = a;
    return this;
}



/*
    Set text top vector from scene
    Using in text method
*/
Scene* Scene::setTextTop()
{
    textTop = getTop();
    return this;
}



/*
    Return text top
    Using in text method
*/
Point3d& Scene::getTextTop()
{
    return textTop;
}



/*
    Set text right vector
    Using in text method
*/
Scene* Scene::setTextRight
(
    Point3d a
)
{
    textRight = a;
    return this;
}



/*
    Set text right vector from scene
    Using in text method
*/
Scene* Scene::setTextRight()
{
    textRight = getRight();
    return this;
}



/*
    Return text right
    Using in text method
*/
Point3d& Scene::getTextRight()
{
    return textRight;
}



/*
    Set text position
    Using in text method
*/
Scene* Scene::setTextPosition
(
    const Point3d a
)
{
    textPosition    = a;
    textShift       = 0.0;
    textMaxSize     = 0.0;
    return this;
}



/*
    Return text position
    Using in text method
*/
Point3d& Scene::getTextPosition()
{
    return textPosition;
}



/*
    Set text size
    Using in text method
*/
Scene* Scene::setTextSize
(
    double a
)
{
    textSize = a;
    textMaxSize = max( textSize, textMaxSize );
    return this;
}



/*
    Return text size
    Using in text method
*/
double Scene::getTextSize()
{
    return textSize;
}





Scene* Scene::changeTextBaseLine()
{
    return this;
}

