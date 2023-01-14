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


        /* Camera settings */
        Point3          eye         = VECTOR_3D_I;
        Point3          top         = VECTOR_3D_Y;
        Point3          target      = VECTOR_3D_0;

        /* Settings */
        int         fpsLimit    = 10;       /* FPS limit */

    public:

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
            Draw method
        */
        Scene& draw();

        /*
            Main loop for scene
        */
        Scene& loop();



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
        Scene& cameraApply();


        /*
            Set camera to Eye position. Camera look at Target.
        */
        Scene& cameraLocation
        (
            const Point3&,  /* Eye camera position */
            const Point3&,  /* Target for camera gaze */
            const Point3&   /* Top of camera */
        );



        /*
            Shift camera
        */
        Scene& shift
        (
            const Point3*   /* Delta */
        );


        static string openglErrorToString( GLenum );
};











//// постараюсь описать вопрос:
//
//class Scene{...}; // класс сцены
//
//class Payload
//{
//    private:
//        ScenePayload* payload; ... // <—— вот тут надо что то другое
//    public:
//    /*
//        Set payload
//    */
//    Scene& setPayload
//    (
//        ScenePayload&       /* Argument for setting */
//    );
//};
//
//class UserPayload : public Payload {...} ; // это юзерский код наследник Payload
//
//
//// далее main
//
//UserPayload form = UserPayload(); // создали юзерский модуль
//Scene().setPayload( form ).run(); // создали сцену, установили ей юдерский модуль, запустили.
//
//// в методе Scene::run мне надо вызывать из UserPayload метод draw
//
//// как?
