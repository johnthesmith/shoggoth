/*
    Look at scene.h
*/

#include <iostream>
#include <string>
#include <unistd.h>         /* usleep */
#include <GL/glu.h>

/* Local libraries */
#include "utils.h"
#include "scene.h"

//#include "camera.h"
#include "matrix.h"
#include "scene_payload.h"
#include "log_points.h"
#include "moment.h"



using namespace std;



/*
    Constructor
*/
Scene::Scene
(
    Log& aLog
):
    log( aLog ) /* define the prived log from aLog argument */
{
    getLog().trace( "Create scene object" );
}



/*
    Destructor
*/
Scene::~Scene()
{
    getLog().trace( "Destory scene object" );
}



/*
    Return log
*/
Log& Scene::getLog()
{
    return log;
}



/*
    Initialize
*/
Scene& Scene::init
(
    int aWidth,     /* Window width */
    int aHeight,    /* Window height */
    string aTitle   /* Window title */
)
{
    glInit = glfwInit();
    if( isInit() )
    {
        log
        .trace( "glfl initialize" )
        .trace( "Create window" )
        .lineEnd();

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
        }
        else
        {
            log.warning( "glfw window create error" );
            setResult( "glfl_window_create_error" );
        }
    }
    else
    {
        log .warning( "glfw init error" );
        setResult( "glfl_init_error" );
    }

    return *this;
}



Scene& Scene::finit()
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
    return *this;
}



/*
    Main loop for scene
*/
Scene& Scene::loop()
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

    return *this;
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
            glViewport( viewport.left, viewport.top, viewport.width, viewport.height );
//            projectionMatrix.perspective( perspective, (float) viewport.width / (float) viewport.height, near, far );

            /* Projection matrrix load */
            glMatrixMode( GL_PROJECTION );
            glLoadIdentity();
            gluPerspective( 45, viewport.width / (float) viewport.height, near, far );
            glGetDoublev( GL_PROJECTION_MATRIX, (GLdouble*)&projectionMatrix );

            /* Modelview matrrix load */
            glMatrixMode( GL_MODELVIEW );
            glLoadMatrixd( (GLdouble*)&viewMatrix );

            /* Pulling event for keyboard and mouse*/
            glfwPollEvents();

            /* Payload draw event */
            payload -> onDraw( *this );

            /* Draw buffer to window */
            glfwSwapBuffers( win );

            auto code = glGetError();
            if( code != GL_NO_ERROR )
            {
                auto message = Scene::openglErrorToString( code );
                getLog().warning( "opengl error" ).prm( "code", code ).prm( "message", message );
                setResult( "opengl_error", message );
            }
        }
    }

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
        payload -> onCalc( *this );
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
    payload -> onMouseWheel( *this, Point3( aX, aY ));
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
        mouseCurrent.set( aX, aY, 0 );

        /* Mouse drag control */
        if( isMouseButton( MB_LEFT ) )
        {
            if( mouseLeftDrag )
            {
                payload -> onLeftDrag( *this, Point3( aX, aY ));
            }
            else
            {
                payload -> onLeftDragBegin( *this, Point3( aX, aY ));
                mouseLeftDrag = true;
            }
        }
        payload -> onMouseMove( *this, Point3( aX, aY ));
    }
    return *this;
}



/******************************************************************************
    Services
*/


/*
    Set payload
*/
Scene& Scene::setPayload
(
    ScenePayload& a
)
{
    payload = &a;
    payload -> setScene( *this );

    return *this;
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
Scene& Scene::begin
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
        case QUADS      : glBegin( GL_QUAD_STRIP ); break;
        case TRIANGLE   : glBegin( GL_TRIANGLES ); break;
        case TRIANGLES  : glBegin( GL_TRIANGLE_STRIP ); break;
        case FAN        : glBegin( GL_TRIANGLE_FAN ); break;
    }
    return *this;
}



Scene& Scene::end()
{
    glEnd();
    return *this;
}



/*
    Set color for canvas
*/
Scene& Scene::color
(
    const Rgba& a
)
{
    glColor4fv( (GLfloat*)&a );
    return *this;
}



/*
    Set vertex to canvas
*/
Scene& Scene::vertex
(
    const Point3& a
)
{
    glVertex3dv( (GLdouble*)&a );
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
        color( Rgba( 0,0,0,0.5 )); vertex( Point3( l, 0, -1 ));
        color( Rgba( 1,0,0,0.5 )); vertex( Point3( l, 0, 0 ));
        color( Rgba( 1,0,0,0.5 )); vertex( Point3( l, 0, 0 ));
        color( Rgba( 1,1,1,0.5 )); vertex( Point3( l, 0, 1 ));

        color( Rgba( 0,0,0,0.5 )); vertex( Point3( l, -1, 0));
        color( Rgba( 0,1,0,0.5 )); vertex( Point3( l, 0, 0 ));
        color( Rgba( 0,1,0,0.5 )); vertex( Point3( l, 0, 0 ));
        color( Rgba( 1,1,1,0.5 )); vertex( Point3( l, 1, 0 ));

        color( Rgba( 0,0,0,0.5 )); vertex( Point3( 0, l, -1));
        color( Rgba( 0,0,1,0.5 )); vertex( Point3( 0, l, 0 ));
        color( Rgba( 0,0,1,0.5 )); vertex( Point3( 0, l, 0 ));
        color( Rgba( 1,1,1,0.5 )); vertex( Point3( 0, l, 1 ));

    }
    end();
    return *this;
}



/*
    Draw axis from -1 to 1
*/
Scene& Scene::drawAxisIdentity()
{
    begin( LINE );
    {
        color( RGBA_WHITE ).vertex( VECTOR_3D_0 ).color( RGBA_GREEN ).vertex( VECTOR_3D_X );
        color( RGBA_WHITE ).vertex( VECTOR_3D_0 ).color( RGBA_BLUE ).vertex( VECTOR_3D_Y );
        color( RGBA_WHITE ).vertex( VECTOR_3D_0 ).color( RGBA_RED ).vertex( VECTOR_3D_Z );
    }
    end();
    return *this;
}



/*
    return view matrix reference
*/
Matrix4& Scene::getViewMatrixRef()
{
    return viewMatrix;
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
Point3 Scene::getScreenByWorld
(
    const Point3& a
)
{
    auto r = Point3();
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
    return r;
}



/*
    return world point by screen point
    https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluProject.xml
*/
Point3 Scene::getWorldByScreen
(
    const Point3& a
)
{
    auto r = Point3();
    gluUnProject
    (
        a.x, a.y, a.z,
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
Point3 Scene::getMouseCurrentScreen()
{
    return mouseCurrent;
}



/*
    Return mouse last position at screen
*/
Point3 Scene::getMouseLastScreen()
{
    return mouseLast;
}



/*
    Return mouse current position in world
*/
Point3 Scene::getMouseCurrentWorld()
{
    return getWorldByScreen( mouseCurrent );
}



/*
    Return mouse last position in world
*/
Point3 Scene::getMouseLastWorld()
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


