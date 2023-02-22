#pragma once

#include <GLFW/glfw3.h>     /* https://www.glfw.org/docs/latest/input_guide.html */
#include <string>
#include <math.h>

/* Local libraries */
#include "../lib/result.h"
#include "../lib/log.h"

/* Graphics libraries */
#include "rgba.h"
#include "point3.h"
#include "point2d.h"
#include "rect2d.h"
#include "point4d.h"
#include "matrix.h"
#include "draw_mode.h"
#include "scene_key.h"


using namespace std;



/* Predeclaration */
class ScenePayload;
//class Camera;



/*
    Key mode
*/
enum KeyMode
{
    ALT,
    CONTROL,
    SHIFT
};



struct Ltwh
{
    int left = 0;
    int top = 0;
    int width = 0;
    int height = 0;
};



class Scene : public Result
{
    private:

        /* States */
        Log&            log;                            /* Log object defined at constructor */
        ScenePayload*   payload             = NULL;     /* Payload */
        GLFWwindow*     win                 = NULL;     /* Window handle */
        bool            glInit              = false;    /* opengl init flag */
        int             fps                 = 0;        /* FPS */
        int             idle                = 0;        /* Idle time in microseconds */
        bool            terminated          = false;    /* Terminated flag */
        float           ratio               = 0;        /* Radio window width / height */

        long long       lastLeftMouse       = 0;
        long long       lastRightMouse      = 0;
        long long       lastMiddleMouse     = 0;
        short           leftClickCount      = 0;
        short           rightClickCount     = 0;
        short           middleClickCount    = 0;
        bool            mouseLeftDrag       = false;
        bool            mouseRightDrag      = false;

        Point3d         mouseCurrent        = POINT_3D_0;  /* Current mouse cursor position */
        Point3d         mouseLast           = POINT_3D_0;  /* Last mouse cursot position */

        Matrix4         projectionMatrix;
        Matrix4         viewMatrix;
        Ltwh            viewport;           /* Viewport */

        /* Settings */
        int         fpsDrawLimit            = 50;       /* FPS limit */
        int         fpsCalcLimit            = 10;       /* FPS limit */
        double      near            = 0.1;
        double      far             = 3.0;
        double      perspective     = M_PI * 0.5;   /* Perspective at rad*/

    public:
        int clickTimeoutMls     = 200;                          /* Click and double click timeoud in milliseconds */
        Rgba    backgroundColor = Rgba( 0.2, 0.2, 0.2, 1.0 );   /* Bakcgound color */

        /*
            Constructor
        */
        Scene
        (
            Log& /* Log object */
        );

        /*
            Destructor
        */
        ~Scene();

        /*
            Return log
        */
        Log& getLog();

        /*
            Initialize window
        */
        Scene& init
        (
            int = 640,          /* Window width */
            int = 480,          /* Window height */
            string = "scene"    /* Window title */
        );

        /*
            Finalize opengl and window
        */
        Scene& finit();

        /*
            Main loop for scene
        */
        Scene& loop();


        /******************************************************************************
            Internal Events method
        */

        /*
            Internal draw method
        */
        Scene& drawEvent();

        /*
            Internal calc method
        */
        Scene& calcEvent();


        /*
            Internal keyboard event
        */
        Scene& keyboardEvent
        (
            int,
            int,
            int,
            int
        );

        /*
            Internal mouse event
        */
        Scene& mouseEvent
        (
            int,
            int,
            int
        );



        /*
            Internal mouse wheel event
        */
        Scene& mouseWheelEvent
        (
            const double, /* x offset */
            const double  /* y offset */
        );




        /*
            Internal mouse move event
        */
        Scene& mouseMoveEvent
        (
            const double, /* x offset */
            const double  /* y offset */
        );




        /******************************************************************************
            Services
        */

        /*
            Get payload
        */
        ScenePayload& getPayload();



        /*
            Set payload
        */
        Scene& setPayload
        (
            ScenePayload*       /* Argument for setting */
        );



        /*
            Return true if openGL is initialized
        */
        bool isInit();



        /*
            Return true if openGL is initialized
        */
        bool isWindow();



        /*
            Clear color
        */
        Scene& clearColor();


        Scene& begin
        (
            const DrawMode
        );

        Scene& end();



        /*
            Set color
        */
        Scene& color
        (
            const Rgba&
        );



        /*
            Set vertex to canvas
        */
        Scene& vertex
        (
            const Point2d&
        );



        /*
            Set vertex to canvas
        */
        Scene& vertex
        (
            const Point3d&
        );


        /*
            Set size of point
        */
        Scene& setPointSize
        (
            const float /* Point size at pixels */
        );



        /*
            Set width of the line
        */
        Scene& setLineWidth
        (
            const float /* Point size at pixels */
        );



        /**********************************************************************
        */

        static string openglErrorToString( GLenum );



        /*
            Set Terminate signal for main loop
            Main loop will be terminated with setTerminate( true );
        */
        Scene& setTerminate
        (
            bool
        );


        Scene& drawGreedIdentity();
        Scene& drawAxisIdentity();


        /*
            Draw qube
        */
        Scene& sendQube
        (
            Point3d&,    /* Qube center */
            Point3d&
        );



        /*
            Draw qube
        */
        Scene& sendRect
        (
            Point3d&,   /* left top */
            Point3d&    /* bottom right */
        );



        /*
            Draw quad
        */
        Scene& sendRect
        (
            const Rect2d&
        );



        Scene& polygonMode
        (
            PolygonMode
        );


        /*
            return view matrix reference
        */
        Matrix4& getViewMatrixRef();


        /*
            Return true if key is pressed
        */
        bool isKey
        (
            Key /* Key name from scene_key.h*/
        );



        /*
            Return true if mouse bugtton is pressed
        */
        bool isMouseButton
        (
            MouseButton /* Key name from scene_key.h*/
        );



        /*
            Return screen point by world point
        */
        Point3d getScreenByWorld
        (
            const Point3d&
        );



        /*
            Return world point by screen point
        */
        Point3d getWorldByScreen
        (
            const Point3d&
        );



        /*
            Return mouse current position at screen
        */
        Point3d getMouseCurrentScreen();



        /*
            Return mouse last position at screen
        */
        Point3d getMouseLastScreen();



        /*
            Return mouse current position in world
        */
        Point3d getMouseCurrentWorld();



        /*
            Return mouse last position in world
        */
        Point3d getMouseLastWorld();



        /*
            Set far clipping
        */
        Scene& setFar
        (
            const double /* value */
        );



        /*
            Return far clipping
        */
        double getFar();



        /*
            Set near clipping
        */
        Scene& setNear
        (
            const double /* value */
        );



        /*
            Return near clipping
        */
        double getNear();



        Scene& switchToWorld();
        Scene& switchToScreen();



        Ltwh& getViewport();
};



// https://forum.sources.ru/index.php?showtopic=242739
//glLoadMatrixd( (GLdouble*)&projectionMatrinx );
//glGetDoublev( GL_PROJECTION_MATRIX, (GLdouble*)&projectionMatrix );
