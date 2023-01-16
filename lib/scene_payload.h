#pragma once

/* Local libraries */
#include "payload.h"
//#include "log.h"
//#include "scene.h"
#include "point3.h"


using namespace std;



class Scene;

class ScenePayload : public Payload
{
    private:

        Scene* scene    = NULL;   /* Current scene for this payload */

        ScenePayload& defaultMouseEventInformation
        (
            const Scene&,   /* Scene object */
            const string,
            const Point3&,  /* MousePosition */
            const Point3&,  /* MousePosition */
            const int
        );

    public:

        /*
            Transparent constructor
        */
        ScenePayload
        (
            Log& aLog   /* Log */
        )
        : Payload( aLog ) /* Call parent constructor */
        {};


        /*
            Set scene value
        */
        ScenePayload& setScene
        (
            Scene&
        );

        /*
            Get scene value
        */
        Scene& getScene();


        /*
            Draw method
        */
        virtual void onDraw
        (
            Scene& aScene   /* Scene object */
        );



        /*
            On mouse left up event
        */
        virtual void onLeftUp
        (
            const Scene&,     /* Scene object */
            const Point3&,    /* Mouse position */
            const Point3&,    /* Mouse delta */
            const int         /* Key mode */
        );



        /*
            On mouse left down event
        */
        virtual void onLeftDown
        (
            const Scene& aScene,  /* Scene object */
            const Point3&,        /* Mouse position */
            const Point3&,        /* Mouse delta */
            const int             /* Key mode */
        );



        /*
            On mouse left click event
        */
        virtual void onLeftClick
        (
            const Scene& aScene,  /* Scene object */
            const Point3&,        /* Mouse position */
            const Point3&,        /* Mouse delta */
            const int             /* Key mode */
        );



        /*
            On mouse left double click event
        */
        virtual void onLeftDblClick
        (
            const Scene& aScene,  /* Scene object */
            const Point3&,        /* Mouse position */
            const Point3&,        /* Mouse delta */
            const int             /* Key mode */
        );



        /*
            Draw method
        */
        virtual void onRightUp
        (
            const Scene& aScene,  /* Scene object */
            const Point3&,        /* Mouse position */
            const Point3&,        /* Mouse delta */
            const int             /* Key mode */
        );



        /*
            Draw method
        */
        virtual void onRightDown
        (
            const Scene& aScene,  /* Scene object */
            const Point3&,        /* Mouse position */
            const Point3&,        /* Mouse delta */
            const int             /* Key mode */
        );



        /*
            Draw method
        */
        virtual void onRightClick
        (
            const Scene& aScene,  /* Scene object */
            const Point3&,        /* Mouse position */
            const Point3&,        /* Mouse delta */
            const int             /* Key mode */
        );



        /*
            Draw method
        */
        virtual void onRightDblClick
        (
            const Scene& aScene,  /* Scene object */
            const Point3&,        /* Mouse position */
            const Point3&,        /* Mouse delta */
            const int             /* Key mode */
        );



        /*
            Mouse middle button up event
        */
        virtual void onMiddleUp
        (
            const Scene& aScene,  /* Scene object */
            const Point3&,        /* Mouse position */
            const Point3&,        /* Mouse delta */
            const int             /* Key mode */
        );



        /*
            Mouse middle button down event
        */
        virtual void onMiddleDown
        (
            const Scene&,   /* Scene object */
            const Point3&,  /* Mouse position */
            const Point3&,  /* Mouse delta */
            const int       /* Key mode */
        );



        /*
            Mouse click middle button event
        */
        virtual void onMiddleClick
        (
            const Scene&,   /* Scene object */
            const Point3&,  /* Mouse position */
            const Point3&,  /* Mouse delta */
            const int       /* Key mode */
        );



        /*
            Mouse double click middle button event
        */
        virtual void onMiddleDblClick
        (
            const Scene&,   /* Scene object */
            const Point3&,  /* Mouse position */
            const Point3&,  /* Mouse delta */
            const int       /* Key mode */
        );



        /*
            Mouse wheel event
        */
        virtual void onMouseWheel
        (
            const Scene&,   /* Scene object */
            const double,   /* x scroll value */
            const double    /* y scroll value */
        );



        /*
            Keyboard up event
        */
        virtual void onKeyUp
        (
            const Scene&,   /* Scene object */
            const int,      /* Key */
            const int,      /* Scan code*/
            const int       /* Mode */
        );



        /*
            Keyboard down event
        */
        virtual void onKeyDown
        (
            const Scene&,   /* Scene object */
            const int,      /* Key */
            const int,      /* Scan code*/
            const int       /* Mode */
        );
};
