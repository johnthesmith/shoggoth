#pragma once

/* Local libraries */
#include "../lib/payload.h"
#include "point3.h"
#include "camera.h"


using namespace std;



class Scene;

class ScenePayload : public Payload
{
    private:

        Scene* scene    = NULL;   /* Current scene for this payload */

        ScenePayload& defaultMouseEventInformation
        (
            Scene&,   /* Scene object */
            const string,
            const Point3d&,  /* MousePosition */
            const int
        );

    public:

        /*
            Transparent constructor
        */
        ScenePayload
        (
            Application*
        );



        /*
            Transparent destructor
        */
        ~ScenePayload();



        /*
            Creator
        */
        static ScenePayload* create
        (
            Application*
        );



        /*
            Destructor
        */
        void destroy();


        /**********************************************************************
        */


        /*
            Set scene value
        */
        ScenePayload* setScene
        (
            Scene*
        );



        /*
            Get scene value
        */
        Scene* getScene();



        /*
            Apply camera to scene
        */
        ScenePayload* applyCameraToScene
        (
            Camera*,    /* Camera object */
            Scene*      /* Scene object */
        );



        /*
            Apply screen to scene
        */
        ScenePayload* applyScreenToScene
        (
            Scene*      /* Scene object */
        );



        /*****************************************************************************
            Events
        */


        /*
            On activate event when payload set to scene
        */
        virtual void onActivate
        (
            Scene&  /* Scene object */
        );



        /*
            Draw method
        */
        virtual void onDraw
        (
            Scene*    /* Scene object */
        );



        /*
            Calc event method
        */
        virtual void onCalc
        (
            Scene*    /* Scene object */
        );



        /*
            On mouse left up event
        */
        virtual void onLeftUp
        (
            Scene&,     /* Scene object */
            const Point3d&,    /* Mouse position */
            const int         /* Key mode */
        );



        /*
            On mouse left down event
        */
        virtual void onLeftDown
        (
            Scene&,         /* Scene object */
            const Point3d&,  /* Mouse position */
            const int       /* Key mode */
        );



        /*
            On mouse left click event
        */
        virtual void onLeftClick
        (
            Scene&,         /* Scene object */
            const Point3d&,  /* Mouse position */
            const int       /* Key mode */
        );



        /*
            On mouse left double click event
        */
        virtual void onLeftDblClick
        (
            Scene& ,        /* Scene object */
            const Point3d&,  /* Mouse position */
            const int       /* Key mode */
        );



        /*
            Draw method
        */
        virtual void onRightUp
        (
            Scene& ,        /* Scene object */
            const Point3d&,  /* Mouse position */
            const int       /* Key mode */
        );



        /*
            Draw method
        */
        virtual void onRightDown
        (
            Scene&,         /* Scene object */
            const Point3d&,  /* Mouse position */
            const int       /* Key mode */
        );



        /*
            Draw method
        */
        virtual void onRightClick
        (
            Scene&,         /* Scene object */
            const Point3d&,  /* Mouse position */
            const int       /* Key mode */
        );



        /*
            Draw method
        */
        virtual void onRightDblClick
        (
            Scene&,         /* Scene object */
            const Point3d&,  /* Mouse position */
            const int       /* Key mode */
        );



        /*
            Mouse middle button up event
        */
        virtual void onMiddleUp
        (
            Scene&,         /* Scene object */
            const Point3d&,  /* Mouse position */
            const int       /* Key mode */
        );



        /*
            Mouse middle button down event
        */
        virtual void onMiddleDown
        (
            Scene&,         /* Scene object */
            const Point3d&,  /* Mouse position */
            const int       /* Key mode */
        );



        /*
            Mouse click middle button event
        */
        virtual void onMiddleClick
        (
            Scene&,   /* Scene object */
            const Point3d&,  /* Mouse position */
            const int       /* Key mode */
        );



        /*
            Mouse double click middle button event
        */
        virtual void onMiddleDblClick
        (
            Scene&,   /* Scene object */
            const Point3d&,  /* Mouse position */
            const int       /* Key mode */
        );



        /*
            Mouse wheel event
        */
        virtual void onMouseWheel
        (
            Scene&,         /* Scene object */
            const Point3d&
        );



        /*
            Mouse move event
        */
        virtual void onMouseMove
        (
            Scene&,             /* Scene object */
            const Point3d&
        );



        /*
            Mouse left drag begin
        */
        virtual bool onLeftDragBegin
        (
            Scene&,             /* Scene object */
            const Point3d&
        );



        /*
            Mouse left drag end
        */
        virtual void onLeftDragEnd
        (
            Scene&,             /* Scene object */
            const Point3d&
        );



        /*
            Mouse left drag
        */
        virtual void onLeftDrag
        (
            Scene&,             /* Scene object */
            const Point3d&
        );



        /*
            Mouse right drag begin
        */
        virtual void onRightDragBegin
        (
            Scene&,             /* Scene object */
            const Point3d&
        );



        /*
            Mouse right drag end
        */
        virtual void onRightDragEnd
        (
            Scene&,             /* Scene object */
            const Point3d&
        );



        /*
            Mouse right drag
        */
        virtual void onRightDrag
        (
            Scene&,             /* Scene object */
            const Point3d&
        );



        /*
            Keyboard up event
        */
        virtual void onKeyUp
        (
            Scene&,   /* Scene object */
            const int,      /* Key */
            const int,      /* Scan code*/
            const int       /* Mode */
        );



        /*
            Keyboard down event
        */
        virtual void onKeyDown
        (
            Scene&,   /* Scene object */
            const int,      /* Key */
            const int,      /* Scan code*/
            const int       /* Mode */
        );
};
