#include <iostream>
#include <string>
#include <unistd.h>         /* usleep */

/* Local libraries */
#include "utils.h"
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
    string title    /* Window title */
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
        win = glfwCreateWindow( aWidth, aHeight, "Hello World", NULL, NULL );
        if( isWindow() )
        {
            glfwMakeContextCurrent( win );

            /* Set user pointer for callback access */
            glfwSetWindowUserPointer( win, this );

            /* Set keyboard event */
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
                    Scene& scene = *(Scene*)glfwGetWindowUserPointer( aWin );

                    scene.getLog()
                    .info( "" )
                    .prm( "key", aKey )
                    .prm( "scancode", aScancode );
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
    Draw method
    Call payload draw method
*/
Scene& Scene::draw()
{
    if( payload != NULL && isInit() )
    {
        payload -> draw( *this );
        /* Draw buffer to window */
        glfwSwapBuffers( win );
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
            !terminated  &&                 /* Not scene terminated */
            !glfwWindowShouldClose( win )   /* Window is not open */
        )
        {
            /* Run draw function */
            auto startDraw = now();

            /* Get window size */
            glfwGetFramebufferSize( win, &width, &height);
            /* Set opengl viewport default */
            glViewport( 0, 0, width, height );
            /* Calculate ratio */
            if( height > 0 )
            {
                ratio = width / (float) height;
            }
            else
            {
                ratio = 0;
            }


            /* Pulling event for keyboard and mouse*/
            glfwPollEvents();

            /* Get cursor positions */
            double xpos;
            double ypos;
            glfwGetCursorPos( win, &xpos, &ypos);
            mouseDelta.set( mousePos );
            mousePos.set( xpos, ypos, 0 );
            mouseDelta.subFrom( mousePos );

            /* Draw frame */
            draw();
            auto stopDraw = now();

            /* Calculate dalta draw */
            auto delta = ( stopDraw - startDraw );

            if( delta > 0 )
            {
                /* Calculate idel time */
                idle = 1000000 / fpsLimit - delta;

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
                fps = 1000000 / ( stopDraw - startDraw );
            }
        }
    }

    return *this;
}



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



/*
    Service methods
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
        case POINT  : glBegin( GL_POINTS ); break;
        case LINE   : glBegin( GL_LINES ); break;
        case LINES  : glBegin( GL_LINE_STRIP ); break;
    }

    auto e = glGetError();
    if( e != GL_NO_ERROR )
    {
        getLog().warning( "glBegin" ).prm( "message", Scene::openglErrorToString( e ) );
    }
    return *this;
}



Scene& Scene::end()
{
    glEnd();

//    auto e = glGetError();
//    if( e != GL_NO_ERROR )
//    {
//        getLog().warning( "glEnd" ).prm( "message", Scene::openglErrorToString( e ) );
//    }
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



Scene& Scene::cameraApply()
{
    Point3 gaze = ( target - eye ).norm();
    auto rotate = Matrix4().look( gaze, top );
    auto translate = Matrix4().shift( eye.toPoint4() );

LogPoints::write( getLog(), rotate, "" );

    auto view = Matrix4().dot( rotate, translate );

	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum (-1, 1, -1, 1, -1, 1);
//    glMultMatrixd( (GLdouble*)&translate );
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    glLoadMatrixd( (GLdouble*)&view );

    return *this;
}



Scene& Scene::cameraLocation
(
    const Point3& aEye,
    const Point3& aTarget,
    const Point3& aTop
)
{
    eye = aEye;
    target = aTarget;
    top = aTop;

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
