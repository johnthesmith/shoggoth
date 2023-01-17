#pragma once

#include <GLFW/glfw3.h>     /* https://www.glfw.org/docs/latest/input_guide.html */
#include <string>

/* Local libraries */
#include "result.h"
#include "log.h"
#include "rgba.h"
#include "point3.h"
#include "point4.h"
#include "matrix.h"
#include "draw_mode.h"
//#include "scene_payload.h"


using namespace std;



/* Predeclaration ScenePayload from scene_payload.h */
class ScenePayload;
class Camera;



/*
    Key mode
*/
enum KeyMode
{
    ALT,
    CONTROL,
    SHIFT
};



class Scene : public Result
{
    private:

        /* States */
        Log& log;                               /* Log object defined at constructor */
        ScenePayload*   payload     = NULL;     /* Payload */
        GLFWwindow*     win         = NULL;     /* Window handle */
        bool            glInit      = false;    /* opengl init flag */
        int             fps         = 0;        /* FPS */
        int             idle        = 0;        /* Idle time in microseconds */
        bool            terminated  = false;    /* Terminated flag */
        int             width       = 0;        /* Scene window width */
        int             height      = 0;        /* Scene window height */
        float           ratio       = 0;        /* Radio window width / height */

        long long       lastLeftMouse       = 0;
        long long       lastRightMouse      = 0;
        long long       lastMiddleMouse     = 0;
        short           leftClickCount      = 0;
        short           rightClickCount     = 0;
        short           middleClickCount    = 0;

        /* Settings */
        int         fpsDrawLimit            = 10;       /* FPS limit */
        int         fpsCalcLimit            = 10;       /* FPS limit */

    public:

        int clickTimeoutMls     = 200;                          /* Click and double click timeoud in milliseconds */
        Point3  mousePos        = Point3( 0.0, 0.0, 0.0 );      /* Mouse cursor position */
        Point3  mouseDelta      = Point3( 0.0, 0.0, 0.0 );      /* Mouse cursor position delta */
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
            ScenePayload&       /* Argument for setting */
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
            const Point3&
        );



        /*
            Apply camera settings to matrix
        */
        Scene& cameraApply
        (
            const Camera&
        );



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
};

